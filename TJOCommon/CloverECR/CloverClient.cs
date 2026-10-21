using System;
using System.Collections.Generic;
using System.Configuration;
using System.Globalization;
using System.IO;
using System.Net;
using System.Text;
using System.Threading;
using System.Web.Script.Serialization;

namespace Alberdi.TJOCommon.CloverECR
{
    // Copiado de CloverIntegracion\src\CloverECR.ClientePos (CloverClient.cs) y adaptado a C# 3 / VS2008.
    // Cambios respecto del original: namespace, tipos genericos explicitos en Esperar y log en LogCloverECR.txt.

    /// <summary>
    /// Sin respuesta de CloverECR (red caída, servidor apagado, timeout).
    /// </summary>
    public class CloverEcrSinConexionException : Exception
    {
        public CloverEcrSinConexionException(string mensaje, Exception inner) : base(mensaje, inner) { }
    }

    /// <summary>
    /// Cliente de CloverECR para la caja (.NET Framework 3.5): HttpWebRequest + JavaScriptSerializer, todo sincrónico.
    /// El POS nunca espera a Clover: los POST responden 202 al instante y el resultado se consulta con polling,
    /// igual que con PrismaECR. Los errores de CloverECR vuelven como objeto con Estado = "ERROR" y HttpStatus;
    /// solo se lanza excepción si CloverECR no responde.
    /// </summary>
    public class CloverClient
    {
        private const string Prefijo = "api/v1/pos/clover/";

        private static readonly object lockLog = new object();

        private readonly string _baseUrl;
        private readonly string _apiKey;
        private readonly JavaScriptSerializer _json = new JavaScriptSerializer { MaxJsonLength = 4 * 1024 * 1024 };

        /// <summary>Timeout de cada llamada HTTP (ms). CloverECR responde rápido: nunca espera a Clover.</summary>
        public int TimeoutMs { get; set; }
        /// <summary>Intervalo del polling (ms), como en PrismaECR.</summary>
        public int IntervaloPollingMs { get; set; }
        /// <summary>
        /// Espera máxima de una operación. Debe cubrir el timeout del Flex (~2 min 20 s, con el botón Reintentar)
        /// más la verificación de CloverECR (hasta 3 min, prorrogable).
        /// </summary>
        public int EsperaMaximaMs { get; set; }
        /// <summary>Consultas de estado seguidas sin respuesta que se toleran antes de rendirse.</summary>
        public int FallosToleradosPolling { get; set; }

        public CloverClient(string baseUrl, string apiKey)
        {
            if (string.IsNullOrEmpty(baseUrl)) throw new ArgumentException("Falta la URL de CloverECR.", "baseUrl");
            if (string.IsNullOrEmpty(apiKey) || apiKey.Trim().Length == 0) throw new ArgumentException("Falta la API key de la caja.", "apiKey");
            _baseUrl = baseUrl.TrimEnd('/') + "/";
            _apiKey = apiKey.Trim();
            TimeoutMs = 15000;
            IntervaloPollingMs = 2000;
            EsperaMaximaMs = 10 * 60 * 1000;
            FallosToleradosPolling = 15;
        }

        /// <summary>
        /// Lee appSettings: CloverEcrUrl (app.config de la sucursal) y CloverApiKey (ApiKeyCaja.config de la caja,
        /// incluido con &lt;appSettings file="ApiKeyCaja.config"&gt;). Opcional: CloverTimeoutMs.
        /// </summary>
        public static CloverClient DesdeConfiguracion()
        {
            string url = ConfigurationManager.AppSettings["CloverEcrUrl"];
            string key = ConfigurationManager.AppSettings["CloverApiKey"];
            if (string.IsNullOrEmpty(url))
                throw new ConfigurationErrorsException("Falta CloverEcrUrl en la configuración de la caja.");
            if (string.IsNullOrEmpty(key))
                throw new ConfigurationErrorsException("Falta CloverApiKey: revise que exista ApiKeyCaja.config al lado del ejecutable.");
            CloverClient cliente = new CloverClient(url, key);
            int timeout;
            if (int.TryParse(ConfigurationManager.AppSettings["CloverTimeoutMs"], out timeout) && timeout > 0)
                cliente.TimeoutMs = timeout;
            return cliente;
        }

        // ───────────────────────────── Ventas ─────────────────────────────

        /// <summary>
        /// POST /pagos. 202 = enviada al Flex (consultar con ConsultarPago). Si CloverECR no responde y el pedido traía
        /// EcrTransactionId, se busca en las transacciones del día por si la venta llegó a crearse (no se reenvía a ciegas).
        /// </summary>
        public RespuestaPos IniciarPago(PagoRequest pago)
        {
            DateTime inicio = DateTime.Now;
            try
            {
                return Enviar<RespuestaPos>("POST", "pagos", pago);
            }
            catch (CloverEcrSinConexionException)
            {
                if (string.IsNullOrEmpty(pago.EcrTransactionId))
                    throw;
                Transaccion recuperada = BuscarVentaPorTicket(pago.EcrTransactionId, inicio);
                if (recuperada == null)
                    throw;
                EscribeLog("IniciarPago: respuesta perdida, recuperada por ticket " + pago.EcrTransactionId + " -> " + recuperada.IdOperacion, "CAJA");
                RespuestaPos r = new RespuestaPos();
                r.IdOperacion = recuperada.IdOperacion;
                r.Estado = recuperada.Estado;
                r.Mensaje = "Respuesta perdida: operación recuperada por el número de ticket.";
                r.HttpStatus = 202;
                return r;
            }
        }

        public EstadoPago ConsultarPago(string idOperacion)
        {
            return Enviar<EstadoPago>("GET", "pagos/" + Uri.EscapeDataString(idOperacion), null);
        }

        /// <summary>PUT /pagos/{id}/cancelaciones: solo antes de que el cliente presente la tarjeta. El resultado llega por el polling.</summary>
        public RespuestaPos CancelarPago(string idOperacion)
        {
            return Enviar<RespuestaPos>("PUT", "pagos/" + Uri.EscapeDataString(idOperacion) + "/cancelaciones", null);
        }

        /// <summary>
        /// Venta completa: inicia y consulta hasta un estado final. "cancelar" se evalúa en cada vuelta (p. ej. el cajero
        /// apretó ESC): pide la cancelación una sola vez y sigue consultando hasta el resultado. "alCambiar" avisa cada
        /// cambio de estado (para mostrar "Esperando al cliente...", "Verificando..."). "alIniciar" recibe el
        /// IdOperacion apenas CloverECR acepta la venta, para poder guardarlo aunque después se corte la espera.
        /// </summary>
        public EstadoPago Pagar(PagoRequest pago, Func<bool> cancelar, Action<EstadoPago> alCambiar, Action<string> alIniciar)
        {
            RespuestaPos inicio = IniciarPago(pago);
            if (!inicio.Aceptada)
            {
                EstadoPago rechazo = new EstadoPago();
                rechazo.IdOperacion = inicio.IdOperacion;
                rechazo.Estado = inicio.Estado;
                rechazo.Mensaje = inicio.Mensaje;
                rechazo.HttpStatus = inicio.HttpStatus;
                return rechazo;
            }
            if (alIniciar != null) alIniciar(inicio.IdOperacion);

            bool cancelacionPedida = false;
            string idOperacion = inicio.IdOperacion;
            return Esperar<EstadoPago>(
                delegate { return ConsultarPago(idOperacion); },
                delegate
                {
                    if (cancelacionPedida || cancelar == null || !cancelar()) return;
                    cancelacionPedida = true;
                    CancelarPago(idOperacion);
                },
                alCambiar);
        }

        /// <summary>Sigue consultando una venta ya iniciada (p. ej. la que quedó en VERIFICANDO al vencer la espera).</summary>
        public EstadoPago EsperarPago(string idOperacion, Action<EstadoPago> alCambiar)
        {
            return Esperar<EstadoPago>(delegate { return ConsultarPago(idOperacion); }, null, alCambiar);
        }

        // ──────────────────────── Anulaciones y devoluciones ────────────────────────

        /// <summary>GET /anulaciones/{claveBusq}: datos de la venta (200) o su estado si ya fue anulada/devuelta (409).</summary>
        public VentaParaReversion BuscarVenta(string claveBusq)
        {
            return Enviar<VentaParaReversion>("GET", "anulaciones/" + Uri.EscapeDataString(claveBusq), null);
        }

        public RespuestaPos IniciarAnulacion(string claveBusq, string numeroCajero)
        {
            Dictionary<string, object> cuerpo = new Dictionary<string, object>();
            cuerpo.Add("numeroCajero", numeroCajero);
            return Enviar<RespuestaPos>("POST", "anulaciones/" + Uri.EscapeDataString(claveBusq), cuerpo);
        }

        /// <summary>monto null = todo lo que queda. Total con el lote abierto: CloverECR la hace como anulación.</summary>
        public RespuestaPos IniciarDevolucion(string claveBusq, decimal? monto, string numeroCajero)
        {
            Dictionary<string, object> cuerpo = new Dictionary<string, object>();
            cuerpo.Add("monto", monto);
            cuerpo.Add("numeroCajero", numeroCajero);
            return Enviar<RespuestaPos>("POST", "devoluciones/" + Uri.EscapeDataString(claveBusq), cuerpo);
        }

        public EstadoOperacion ConsultarOperacion(string idOperacion)
        {
            return Enviar<EstadoOperacion>("GET", "operaciones/" + Uri.EscapeDataString(idOperacion), null);
        }

        public EstadoOperacion Anular(string claveBusq, string numeroCajero, Action<EstadoOperacion> alCambiar)
        {
            return EsperarOperacion(IniciarAnulacion(claveBusq, numeroCajero), alCambiar);
        }

        public EstadoOperacion Devolver(string claveBusq, decimal? monto, string numeroCajero, Action<EstadoOperacion> alCambiar)
        {
            return EsperarOperacion(IniciarDevolucion(claveBusq, monto, numeroCajero), alCambiar);
        }

        /// <summary>POST /recibos/{id}: reimprime el recibo de una venta, anulación o devolución en el Flex de esta caja.</summary>
        public RespuestaPos Reimprimir(string idOperacion)
        {
            return Enviar<RespuestaPos>("POST", "recibos/" + Uri.EscapeDataString(idOperacion), null);
        }

        // ───────────────────────────── Consultas ─────────────────────────────

        /// <summary>GET /resumen: totales del día de la caja (reemplaza al cierre: Clover cierra el lote solo).</summary>
        public Resumen ObtenerResumen(DateTime fecha)
        {
            return Enviar<Resumen>("GET", "resumen?fecha=" + fecha.ToString("yyyy-MM-dd", CultureInfo.InvariantCulture), null);
        }

        public List<Transaccion> ObtenerTransacciones(DateTime fecha)
        {
            Crudo r = EnviarCrudo("GET", "transacciones?fecha=" + fecha.ToString("yyyy-MM-dd", CultureInfo.InvariantCulture), null);
            return r.Status == 200 ? _json.Deserialize<List<Transaccion>>(r.Texto) : new List<Transaccion>();
        }

        // ───────────────────────────── Infraestructura ─────────────────────────────

        private EstadoOperacion EsperarOperacion(RespuestaPos inicio, Action<EstadoOperacion> alCambiar)
        {
            if (!inicio.Aceptada)
            {
                EstadoOperacion rechazo = new EstadoOperacion();
                rechazo.IdOperacion = inicio.IdOperacion;
                rechazo.Estado = inicio.Estado;
                rechazo.Mensaje = inicio.Mensaje;
                rechazo.HttpStatus = inicio.HttpStatus;
                return rechazo;
            }
            string idOperacion = inicio.IdOperacion;
            return Esperar<EstadoOperacion>(delegate { return ConsultarOperacion(idOperacion); }, null, alCambiar);
        }

        // Polling hasta estado final. Tolera cortes sueltos (la operación sigue en CloverECR aunque la caja no lo vea)
        private T Esperar<T>(Func<T> consultar, Action enCadaVuelta, Action<T> alCambiar) where T : RespuestaPos
        {
            DateTime limite = DateTime.Now.AddMilliseconds(EsperaMaximaMs);
            string ultimoEstado = null;
            T ultimo = null;
            int fallos = 0;
            while (true)
            {
                if (enCadaVuelta != null) enCadaVuelta();
                try
                {
                    ultimo = consultar();
                    fallos = 0;
                }
                catch (CloverEcrSinConexionException)
                {
                    if (++fallos > FallosToleradosPolling) throw;
                }

                if (ultimo != null)
                {
                    if (ultimo.Estado != ultimoEstado && alCambiar != null) alCambiar(ultimo);
                    ultimoEstado = ultimo.Estado;
                    // 404 u otro error del polling: no tiene sentido seguir
                    if (Estados.EsFinal(ultimo.Estado) || ultimo.HttpStatus >= 400) return ultimo;
                }
                if (DateTime.Now >= limite)
                {
                    if (ultimo == null) throw new CloverEcrSinConexionException("CloverECR no respondió durante la espera.", null);
                    return ultimo;   // sigue PENDIENTE/VERIFICANDO: consultar más tarde con el IdOperacion
                }
                Thread.Sleep(IntervaloPollingMs);
            }
        }

        // Venta de hoy con ese ticket, creada después de iniciar el pedido
        private Transaccion BuscarVentaPorTicket(string ecrTransactionId, DateTime desde)
        {
            try
            {
                Transaccion encontrada = null;
                foreach (Transaccion t in ObtenerTransacciones(desde.Date))
                {
                    DateTime fecha;
                    if (t.TipoOperacion == "VENTA" && t.EcrTransactionId == ecrTransactionId &&
                        DateTime.TryParse(t.Fecha, CultureInfo.InvariantCulture, DateTimeStyles.None, out fecha) && fecha >= desde.AddMinutes(-1))
                        encontrada = t;   // vienen ordenadas por fecha: queda la última
                }
                return encontrada;
            }
            catch (CloverEcrSinConexionException)
            {
                return null;
            }
        }

        private T Enviar<T>(string metodo, string ruta, object cuerpo) where T : RespuestaPos, new()
        {
            Crudo r = EnviarCrudo(metodo, ruta, cuerpo);
            T resultado;
            try
            {
                resultado = string.IsNullOrEmpty(r.Texto) ? new T() : _json.Deserialize<T>(r.Texto);
            }
            catch (ArgumentException)
            {
                resultado = new T();
                resultado.Mensaje = "Respuesta inválida de CloverECR (HTTP " + r.Status + ").";
            }
            catch (InvalidOperationException)
            {
                resultado = new T();
                resultado.Mensaje = "Respuesta inválida de CloverECR (HTTP " + r.Status + ").";
            }
            if (resultado == null) resultado = new T();
            resultado.HttpStatus = r.Status;
            if (r.Status >= 400 && string.IsNullOrEmpty(resultado.Estado)) resultado.Estado = Estados.Error;
            return resultado;
        }

        private sealed class Crudo
        {
            public int Status;
            public string Texto;
        }

        private Crudo EnviarCrudo(string metodo, string ruta, object cuerpo)
        {
            HttpWebRequest req = (HttpWebRequest)WebRequest.Create(_baseUrl + Prefijo + ruta);
            req.Method = metodo;
            req.Accept = "application/json";
            req.Timeout = TimeoutMs;
            req.ReadWriteTimeout = TimeoutMs;
            req.KeepAlive = false;
            req.ServicePoint.Expect100Continue = false;
            req.Headers["X-Api-Key"] = _apiKey;

            string json = null;
            try
            {
                if (cuerpo != null || metodo == "POST" || metodo == "PUT")
                {
                    json = cuerpo == null ? string.Empty : _json.Serialize(cuerpo);
                    byte[] bytes = Encoding.UTF8.GetBytes(json);
                    req.ContentType = "application/json; charset=utf-8";
                    req.ContentLength = bytes.Length;
                    using (Stream s = req.GetRequestStream())
                        s.Write(bytes, 0, bytes.Length);
                }
                EscribeLog(metodo + " " + ruta + (string.IsNullOrEmpty(json) ? "" : " " + json), "CAJA=>CLOVERECR");

                using (HttpWebResponse resp = (HttpWebResponse)req.GetResponse())
                {
                    Crudo ok = new Crudo();
                    ok.Status = (int)resp.StatusCode;
                    ok.Texto = Leer(resp);
                    EscribeLog(metodo + " " + ruta + " HTTP " + ok.Status + " " + ok.Texto, "CAJA<=CLOVERECR");
                    return ok;
                }
            }
            catch (WebException ex)
            {
                // 4xx/5xx llegan como excepción: el cuerpo trae { estado: "ERROR", mensaje }
                HttpWebResponse resp = ex.Response as HttpWebResponse;
                if (resp == null)
                {
                    EscribeLog(metodo + " " + ruta + " SIN RESPUESTA (" + ex.Status + "): " + ex.Message, "CAJA<=CLOVERECR");
                    throw new CloverEcrSinConexionException("CloverECR no respondió (" + ex.Status + "): " + ex.Message, ex);
                }
                using (resp)
                {
                    Crudo error = new Crudo();
                    error.Status = (int)resp.StatusCode;
                    error.Texto = Leer(resp);
                    EscribeLog(metodo + " " + ruta + " HTTP " + error.Status + " " + error.Texto, "CAJA<=CLOVERECR");
                    return error;
                }
            }
        }

        private static string Leer(HttpWebResponse resp)
        {
            using (StreamReader sr = new StreamReader(resp.GetResponseStream(), Encoding.UTF8))
                return sr.ReadToEnd();
        }

        // Mismo formato que LogPrismaECR.txt (OperaPrismaECR.EscribeLog). La API key nunca se loguea.
        public static void EscribeLog(string line, string sentido)
        {
            try
            {
                lock (lockLog)
                {
                    using (StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogCloverECR.txt"))
                        writer.WriteLine(String.Format("{0}= {1}: '{2}'", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), sentido, line));
                }
            }
            catch (Exception)
            {
                // el log no puede cortar un cobro
            }
        }

        // Separador con titulo al empezar cada operacion, como OperaPrismaECR.EscribeLog(titulo)
        public static void EscribeLog(string titulo)
        {
            try
            {
                lock (lockLog)
                {
                    using (StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogCloverECR.txt"))
                    {
                        writer.WriteLine("-------------------------------------------------------------------");
                        writer.WriteLine(String.Format("{0}= {1}", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), titulo.ToUpper()));
                    }
                }
            }
            catch (Exception)
            {
            }
        }
    }
}

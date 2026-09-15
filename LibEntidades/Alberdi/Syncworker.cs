using System;
using System.IO;
using System.Net;
using System.Threading;
using System.Configuration;
using Newtonsoft.Json;
using RestSharp;

namespace LibEntidades.Alberdi
{
    /// <summary>
    /// Hilo de fondo que consume el buffer SQLite local y envía los tickets
    /// pendientes al endpoint central de ventas.
    ///
    /// Uso típico (al iniciar la aplicación POS):
    ///
    ///     SyncWorker worker = new SyncWorker();
    ///     worker.Iniciar();
    ///
    /// Al cerrar la aplicación:
    ///
    ///     worker.Detener();
    ///
    /// El worker NO bloquea el hilo principal: corre como IsBackground = true,
    /// por lo que se detiene automáticamente cuando cierra el proceso.
    /// </summary>
    public class SyncWorker
    {
        // ----------------------------------------------------------------
        //  Configuración (puede exponerse como propiedades si se prefiere)
        // ----------------------------------------------------------------

        /// <summary>Intervalo entre cada ciclo de sincronización.</summary>
        private static TimeSpan INTERVALO_NORMAL  = TimeSpan.FromSeconds(30);

        /// <summary>
        /// Intervalo extendido que se usa cuando el último ciclo detectó
        /// un error de red, para no bombardear un endpoint caído.
        /// </summary>
        private static TimeSpan INTERVALO_BACKOFF = TimeSpan.FromSeconds(120);

        /// <summary>Tamaño del lote por ciclo.</summary>
        private const int LOTE = 50;

        /// <summary>
        /// Fallos consecutivos tolerados dentro de un mismo ciclo antes de
        /// abandonarlo. Un solo ticket problemático ya no corta el lote (así
        /// no bloquea la cola), pero si fallan varios seguidos el endpoint
        /// está en mal estado y conviene esperar al próximo ciclo.
        /// </summary>
        private const int MAX_FALLOS_SEGUIDOS = 5;

        /// <summary>
        /// Timeout del POST, en milisegundos. Antes eran 5000, demasiado
        /// justo: si el webapi tarda un poco más, el POS aborta un envío que
        /// el servidor SÍ procesó y lo reintenta, duplicando el comprobante.
        /// Subirlo achica la ventana pero NO la elimina (ver
        /// ClasificarErrorTransporte).
        /// </summary>
        private const int TIMEOUT_MS = 15000;

        /// <summary>
        /// Archivo donde se deja constancia de los tickets que agotaron los
        /// reintentos. Antes pasaban a ERROR_PERMANENTE en silencio.
        /// </summary>
        private const string ARCHIVO_ALERTA = "TicketsNoSincronizados.txt";

        /// <summary>
        /// Intentos fallidos (timeout o 5xx) a partir de los cuales se deja un aviso
        /// en ARCHIVO_ALERTA. El ticket NO se descarta: se sigue reintentando cada
        /// 15 minutos. Con la espera creciente, el aviso llega unos 90 minutos
        /// después del primer fallo.
        /// </summary>
        private const int INTENTOS_AVISO = 10;

        /// <summary>
        /// Días que se conservan en ticketsync.db los tickets ya sincronizados,
        /// por si hay que reenviar o revisar un comprobante.
        /// </summary>
        private const int DIAS_CONSERVAR_SINCRONIZADOS = 30;

        // ----------------------------------------------------------------
        //  Estado interno
        // ----------------------------------------------------------------

        private TicketSyncRepository _repo;
        private Thread   _thread;
        private volatile bool _corriendo;
        private bool _hayErrorRed;   // flag para alternar intervalo
        private DateTime _ultimaLimpieza = DateTime.MinValue;   // ver LimpiarSiCorresponde

        // ----------------------------------------------------------------
        //  Constructor
        // ----------------------------------------------------------------

        /// <summary>
        /// Crea el worker listo para arrancar.
        /// El mismo dbPath que se le pasa a TicketSyncRepository.
        /// </summary>
        public SyncWorker()
        {
            _repo = new TicketSyncRepository("ticketsync.db");
        }

        // ----------------------------------------------------------------
        //  Control del ciclo de vida
        // ----------------------------------------------------------------

        public void Iniciar()
        {
            if (_corriendo) return;
            _corriendo = true;
            _thread = new Thread(Loop)
            {
                IsBackground = true,
                Name = "SyncWorker"
            };
            _thread.Start();
            Loging.EscribeMensaje("SyncWorker iniciado.");
        }

        public void Detener()
        {
            _corriendo = false;
            Loging.EscribeMensaje("SyncWorker detenido.");
        }

        // ----------------------------------------------------------------
        //  Loop principal
        // ----------------------------------------------------------------

        private void Loop()
        {
            while (_corriendo)
            {
                try
                {
                    LimpiarSiCorresponde();
                }
                catch (Exception ex)
                {
                    // Nunca debe impedir que se sincronice.
                    Loging.EscribeExcepcion("SyncWorker.LimpiarSiCorresponde", ex);
                }

                try
                {
                    ProcesarLote();
                }
                catch (Exception ex)
                {
                    Loging.EscribeExcepcion("SyncWorker.Loop", ex);
                }

                // Si tuvimos error de red esperamos más tiempo antes del reintento
                TimeSpan espera = _hayErrorRed ? INTERVALO_BACKOFF : INTERVALO_NORMAL;
                Thread.Sleep(espera);
            }
        }

        /// <summary>
        /// Borra del buffer los tickets SINCRONIZADO con más de
        /// DIAS_CONSERVAR_SINCRONIZADOS días. Corre en el primer ciclo y
        /// después una vez por día: antes no se llamaba nunca y ticketsync.db
        /// crecía sin límite. PENDIENTE y ERROR_PERMANENTE no se tocan.
        /// </summary>
        private void LimpiarSiCorresponde()
        {
            if (_ultimaLimpieza.Date == DateTime.Today)
                return;

            // Se marca antes de borrar: si falla, se reintenta mañana y no
            // en cada ciclo de 30 segundos.
            _ultimaLimpieza = DateTime.Now;

            int borrados = _repo.LimpiarSincronizados(DIAS_CONSERVAR_SINCRONIZADOS);
            Loging.EscribeMensaje(string.Format(
                "SyncWorker: limpieza del buffer, {0} ticket(s) sincronizado(s) de mas de {1} dias borrado(s).",
                borrados, DIAS_CONSERVAR_SINCRONIZADOS));
        }

        // ----------------------------------------------------------------
        //  Procesamiento de un lote
        // ----------------------------------------------------------------

        private void ProcesarLote()
        {
            var pendientes = _repo.ObtenerPendientes(LOTE);
            if (pendientes.Count == 0)
            {
                _hayErrorRed = false;
                return;
            }

            Loging.EscribeMensaje(
                string.Format("SyncWorker: procesando {0} ticket(s) pendiente(s).",
                pendientes.Count));

            int fallosSeguidos = 0;

            foreach (TicketPendiente item in pendientes)
            {
                if (!_corriendo) break;   // respetar detención durante el lote

                try
                {
                    EnviarAlEndpoint(item.Payload);

                    // Éxito
                    _repo.MarcarSincronizado(item.Id);
                    _hayErrorRed   = false;
                    fallosSeguidos = 0;

                    Loging.EscribeMensaje(
                        string.Format("SyncWorker: ticket {0} sincronizado OK.", item.Seq));
                }
                catch (SyncErrorConexionException ex)
                {
                    // No se pudo establecer la conexión: el endpoint está caído
                    // o inalcanzable. El servidor NUNCA vio el ticket, así que
                    // el reintento es inofensivo. Como el problema es de red y
                    // no de este ticket, sí corresponde cortar el lote.
                    // No cuenta como intento del ticket: antes sí, y con el
                    // webapi caído unos 20 minutos (10 ciclos de 120 s) el más
                    // antiguo pasaba a ERROR_PERMANENTE y no se enviaba nunca más.
                    // La espera la da INTERVALO_BACKOFF.
                    Loging.EscribeMensaje(string.Format(
                        "SyncWorker: ticket {0} sin conexion con el webapi, se reintenta en el proximo ciclo - {1}",
                        item.Seq, ex.Message));
                    _hayErrorRed = true;
                    break;
                }
                catch (SyncErrorPermanenteException ex)
                {
                    // El servidor rechazó el dato (4xx): no sirve reintentar.
                    // Va derecho a ERROR_PERMANENTE -- antes se marcaba con
                    // MarcarError, que lo dejaba PENDIENTE y lo hacía reintentar
                    // otras 9 veces pese al comentario.
                    _repo.MarcarErrorPermanente(item.Id, item.Intentos + 1, ex.Message);
                    Loging.EscribeMensaje(
                        string.Format("SyncWorker: ticket {0} ERROR PERMANENTE - {1}",
                        item.Seq, ex.Message));
                    AlertarNoSincronizado(item, ex.Message, false);
                    // Seguimos con el siguiente; este no bloqueará el lote
                }
                catch (Exception ex)
                {
                    // Timeout o 5xx: el problema es de ESTE ticket (o de esta
                    // respuesta), no de la red. Seguimos con los demás para que
                    // uno solo no bloquee a toda la cola que tiene detrás.
                    string tipo = (ex is SyncErrorTimeoutException)
                        ? "timeout (el webapi pudo haberlo grabado igual)"
                        : "error transitorio";

                    RegistrarFallo(item, ex.Message, tipo);

                    fallosSeguidos++;
                    if (fallosSeguidos >= MAX_FALLOS_SEGUIDOS)
                    {
                        _hayErrorRed = true;
                        Loging.EscribeMensaje(string.Format(
                            "SyncWorker: {0} fallos seguidos, se abandona el ciclo.",
                            fallosSeguidos));
                        break;
                    }
                }
            }
        }

        // ----------------------------------------------------------------
        //  Registro de fallos
        // ----------------------------------------------------------------

        /// <summary>
        /// Contabiliza el intento fallido y programa el próximo (el ticket sigue
        /// PENDIENTE). Al llegar a INTENTOS_AVISO deja constancia una sola vez.
        /// </summary>
        private void RegistrarFallo(TicketPendiente item, string detalle, string tipo)
        {
            _repo.MarcarError(item.Id, item.Intentos + 1, detalle);

            Loging.EscribeMensaje(string.Format(
                "SyncWorker: ticket {0} {1} (intento {2}, proximo en {3} s) - {4}",
                item.Seq, tipo, item.Intentos + 1,
                (int)TicketSyncRepository.EsperaReintento(item.Intentos + 1).TotalSeconds, detalle));

            if (item.Intentos + 1 == INTENTOS_AVISO)
                AlertarNoSincronizado(item, detalle, true);
        }

        /// <summary>
        /// Deja constancia visible de un ticket con problemas: nadie consulta el
        /// SQLite, y sin este aviso el ticket podía faltar en el webapi en silencio.
        /// sigueReintentando = false: el servidor lo rechazó (4xx) y queda como
        /// ERROR_PERMANENTE (LimpiarSincronizados no lo borra). true: lleva
        /// INTENTOS_AVISO fallos transitorios y se sigue reintentando.
        /// </summary>
        private void AlertarNoSincronizado(TicketPendiente item, string detalle, bool sigueReintentando)
        {
            if (sigueReintentando)
                Loging.EscribeMensaje(string.Format(
                    "SyncWorker: *** ATENCION *** el ticket {0} lleva {1} intentos sin " +
                    "sincronizar; se sigue reintentando cada 15 minutos. Ver {2}",
                    item.Seq, item.Intentos + 1, ARCHIVO_ALERTA));
            else
                Loging.EscribeMensaje(string.Format(
                    "SyncWorker: *** ATENCION *** el ticket {0} NO se sincronizo y no se " +
                    "reintenta mas. Queda como ERROR_PERMANENTE en ticketsync.db; ver {1}",
                    item.Seq, ARCHIVO_ALERTA));

            try
            {
                string ruta = Path.Combine(
                    AppDomain.CurrentDomain.BaseDirectory, ARCHIVO_ALERTA);

                using (StreamWriter w = new StreamWriter(ruta, true))
                {
                    w.WriteLine("-------------------------------------------------------------------");
                    w.WriteLine(string.Format(
                        sigueReintentando
                            ? "{0} - ticket {1} sin sincronizar tras {2} intento(s), SE SIGUE REINTENTANDO"
                            : "{0} - ticket {1} NO sincronizado tras {2} intento(s)",
                        DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"),
                        item.Seq, item.Intentos + 1));
                    w.WriteLine("Ultimo error: " + detalle);
                    w.WriteLine("Payload:");
                    w.WriteLine(item.Payload);
                }
            }
            catch (Exception ex)
            {
                Loging.EscribeExcepcion("SyncWorker.AlertarNoSincronizado", ex);
            }
        }

        // ----------------------------------------------------------------
        //  Envío al endpoint (mismo estilo que ClienteComprobante)
        // ----------------------------------------------------------------

        /// <summary>
        /// Envía el payload JSON al endpoint central.
        /// Lanza SyncErrorPermanenteException si el servidor devuelve 4xx.
        /// Lanza SyncErrorConexionException si no se pudo conectar.
        /// Lanza SyncErrorTimeoutException si venció el timeout.
        /// Lanza Exception normal ante 5xx u otro código inesperado.
        /// </summary>
        private void EnviarAlEndpoint(string jsonPayload)
        {
            string ip      = ConfigurationManager.AppSettings["ipapiventas"];
            string port    = ConfigurationManager.AppSettings["portapiventas"];
            string url     = string.Format("http://{0}:{1}/api/ventas/comprobante", ip, port);

            RestClient  cliente  = new RestClient(url);
            RestRequest request  = new RestRequest();
            request.Timeout          = TIMEOUT_MS;
            // Timeout solo cubre hasta que llegan los headers; sin este otro,
            // una respuesta que se arrastra puede colgar el hilo mucho más
            // (el 12/08 un POST tardó 59s con Timeout puesto en 5s).
            request.ReadWriteTimeout = TIMEOUT_MS;
            request.Method       = Method.POST;
            request.AddHeader("Content-Type", "application/json");
            request.AddParameter("application/json", jsonPayload, ParameterType.RequestBody);

            IRestResponse rta = cliente.Execute(request);

            if (rta.ResponseStatus != ResponseStatus.Completed || rta.ErrorException != null)
                throw ClasificarErrorTransporte(rta);

            int status = (int)rta.StatusCode;

            if (status == 200 || status == 201)
                return;   // OK

            // 409 Conflict = el webapi ya tiene un comprobante con este Seq (índice
            // único). Es el caso del reintento tras un timeout en el que el server
            // SÍ lo había grabado: para el POS está sincronizado, no es un error.
            if (status == 409)
            {
                Loging.EscribeMensaje(
                    "SyncWorker: el webapi informa que el comprobante ya existia (409); se da por sincronizado.");
                return;
            }

            if (status >= 400 && status < 500)
            {
                // El servidor rechazó el comprobante: no sirve reintentar
                throw new SyncErrorPermanenteException(
                    string.Format("HTTP {0}: {1}", status, rta.Content));
            }

            // 5xx u otro código inesperado → error transitorio
            throw new Exception(
                string.Format("HTTP {0}: {1}", status, rta.Content));
        }

        /// <summary>
        /// Traduce un fallo de transporte a la excepción que corresponda.
        ///
        /// La distinción importa: si no se llegó a establecer la conexión, el
        /// servidor nunca vio el ticket y reintentar es inofensivo. Un timeout,
        /// en cambio, es AMBIGUO -- la respuesta no volvió a tiempo, pero el
        /// webapi puede haberlo procesado igual, y entonces cada reintento
        /// inserta otra copia. Esa duplicación no se puede cerrar desde el POS:
        /// requiere que el endpoint sea idempotente por Seq (índice único).
        /// </summary>
        private Exception ClasificarErrorTransporte(IRestResponse rta)
        {
            Exception ex   = rta.ErrorException;
            string mensaje = (ex != null) ? ex.Message : "sin respuesta del endpoint";

            if (rta.ResponseStatus == ResponseStatus.TimedOut)
                return new SyncErrorTimeoutException(mensaje);

            WebException wex = ex as WebException;
            if (wex != null)
            {
                switch (wex.Status)
                {
                    case WebExceptionStatus.Timeout:
                        return new SyncErrorTimeoutException(mensaje);

                    // El pedido nunca salió: no hay riesgo de duplicado
                    case WebExceptionStatus.ConnectFailure:
                    case WebExceptionStatus.NameResolutionFailure:
                    case WebExceptionStatus.ProxyNameResolutionFailure:
                        return new SyncErrorConexionException(mensaje);
                }
            }

            return (ex != null) ? ex : new Exception(mensaje);
        }
    }

    // ----------------------------------------------------------------
    //  Excepciones internas para clasificar el fallo
    // ----------------------------------------------------------------

    /// <summary>
    /// Se lanza cuando el endpoint responde 4xx (dato inválido / rechazado).
    /// En ese caso no tiene sentido reintentar: se pasa a ERROR_PERMANENTE.
    /// </summary>
    public class SyncErrorPermanenteException : Exception
    {
        public SyncErrorPermanenteException(string mensaje) : base(mensaje) { }
    }

    /// <summary>
    /// No se pudo establecer la conexión con el endpoint (caído, apagado, IP
    /// mal configurada). El servidor no recibió nada: reintentar es seguro.
    /// </summary>
    public class SyncErrorConexionException : Exception
    {
        public SyncErrorConexionException(string mensaje) : base(mensaje) { }
    }

    /// <summary>
    /// Venció el timeout esperando la respuesta. AMBIGUO: el webapi puede
    /// haber grabado el comprobante igual. Se reintenta, pero queda anotado.
    /// </summary>
    public class SyncErrorTimeoutException : Exception
    {
        public SyncErrorTimeoutException(string mensaje) : base(mensaje) { }
    }
}

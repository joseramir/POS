using System;
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

        // ----------------------------------------------------------------
        //  Estado interno
        // ----------------------------------------------------------------

        private TicketSyncRepository _repo;
        private Thread   _thread;
        private volatile bool _corriendo;
        private bool _hayErrorRed;   // flag para alternar intervalo

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

            foreach (TicketPendiente item in pendientes)
            {
                if (!_corriendo) break;   // respetar detención durante el lote

                try
                {
                    EnviarAlEndpoint(item.Payload);

                    // Éxito
                    _repo.MarcarSincronizado(item.Id);
                    _hayErrorRed = false;

                    Loging.EscribeMensaje(
                        string.Format("SyncWorker: ticket {0} sincronizado OK.", item.Seq));
                }
                catch (SyncErrorPermanenteException ex)
                {
                    // El servidor rechazó el dato (4xx): no sirve reintentar
                    _repo.MarcarError(item.Id, item.Intentos + 1, ex.Message);
                    Loging.EscribeMensaje(
                        string.Format("SyncWorker: ticket {0} ERROR PERMANENTE - {1}",
                        item.Seq, ex.Message));
                    // Seguimos con el siguiente; este no bloqueará el lote
                }
                catch (Exception ex)
                {
                    // Error de red / timeout / 5xx: reintentar más tarde
                    _repo.MarcarError(item.Id, item.Intentos + 1, ex.Message);
                    _hayErrorRed = true;
                    Loging.EscribeMensaje(
                        string.Format("SyncWorker: ticket {0} error transitorio - {1}",
                        item.Seq, ex.Message));

                    // Cortar el lote: si hay error de red no tiene sentido
                    // intentar los siguientes en este ciclo
                    break;
                }
            }
        }

        // ----------------------------------------------------------------
        //  Envío al endpoint (mismo estilo que ClienteComprobante)
        // ----------------------------------------------------------------

        /// <summary>
        /// Envía el payload JSON al endpoint central.
        /// Lanza SyncErrorPermanenteException si el servidor devuelve 4xx.
        /// Lanza Exception normal si hay error de red o 5xx.
        /// </summary>
        private void EnviarAlEndpoint(string jsonPayload)
        {
            string ip      = ConfigurationManager.AppSettings["ipapiventas"];
            string port    = ConfigurationManager.AppSettings["portapiventas"];
            string url     = string.Format("http://{0}:{1}/api/ventas/comprobante", ip, port);

            RestClient  cliente  = new RestClient(url);
            RestRequest request  = new RestRequest();
            request.Timeout      = 5000;      // 5 seg — el worker no bloquea la caja
            request.Method       = Method.POST;
            request.AddHeader("Content-Type", "application/json");
            request.AddParameter("application/json", jsonPayload, ParameterType.RequestBody);

            IRestResponse rta = cliente.Execute(request);

            if (rta.ErrorException != null)
                throw rta.ErrorException;

            int status = (int)rta.StatusCode;

            if (status == 200 || status == 201)
                return;   // OK

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
    }

    // ----------------------------------------------------------------
    //  Excepción interna para distinguir errores permanentes
    // ----------------------------------------------------------------

    /// <summary>
    /// Se lanza cuando el endpoint responde 4xx (dato inválido / rechazado).
    /// En ese caso no tiene sentido reintentar: se pasa a ERROR_PERMANENTE.
    /// </summary>
    public class SyncErrorPermanenteException : Exception
    {
        public SyncErrorPermanenteException(string mensaje) : base(mensaje) { }
    }
}
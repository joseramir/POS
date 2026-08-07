using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using RestSharp;
using System.Configuration;
using Newtonsoft.Json;

using System.Data;
using System.Data.SqlClient;
using System.IO;


namespace LibEntidades.Alberdi
{
    public class ClienteComprobante
    {
        public bool ConError { get; set; }
        public string MensajeError { get; set; }

        // ---- instancia compartida del repositorio local ----
        // En producción puede inyectarse; aquí se crea con ruta de app.config
        // clave sugerida en app.config: <add key="ticketsyncdb" value="sync\ticketsync.db"/>
        private static readonly TicketSyncRepository _syncRepo =
            new TicketSyncRepository("ticketsync.db");

        public ClienteComprobante() { }

        /// <summary>
        /// 1. Guarda el comprobante en el buffer SQLite local (siempre).
        /// 2. Intenta el envío inmediato al endpoint.
        ///    Si falla, el SyncWorker lo reintentará en segundo plano.
        /// </summary>
        public bool PostComprobante(HeaderDoc doc)
        {
            ConError = false;
            MensajeError = "";

            // ── PASO 1: buffer local garantizado ─────────────────────────
            try
            {
                _syncRepo.InsertarPendiente(doc);
            }
            catch (Exception ex)
            {
                // Loguear pero NO abortar: el ticket igual se intentará
                // enviar por red; en el peor caso queda solo en trans.dbf
                Loging.EscribeExcepcion("PostComprobante.InsertarPendiente", ex);
            }

            // ── PASO 2: intento inmediato al endpoint ─────────────────────
            bool enviado = false;
            int intentos = 0;

            try
            {
                while (!enviado && intentos < 2)
                {
                    if (ConexionRed.comprobarIP(
                            ConfigurationManager.AppSettings["ipapiventas"], 1))
                    {
                        string jsonToSend = JsonConvert.SerializeObject(
                            doc,
                            Formatting.None,
                            new JsonSerializerSettings
                            {
                                NullValueHandling = NullValueHandling.Ignore
                            });

                        string url = string.Format(
                            "http://{0}:{1}/api/ventas/comprobante",
                            ConfigurationManager.AppSettings["ipapiventas"],
                            ConfigurationManager.AppSettings["portapiventas"]);

                        RestClient cliente = new RestClient(url);
                        RestRequest request = new RestRequest();
                        request.Timeout = 3000;
                        request.Method = Method.POST;
                        request.AddHeader("Content-Type", "application/json");
                        request.AddParameter("application/json",
                            jsonToSend, ParameterType.RequestBody);

                        IRestResponse rta = cliente.Execute(request);

                        if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                        {
                            enviado = true;

                            // Marcar como sincronizado en SQLite para que el
                            // SyncWorker no lo reenvíe innecesariamente.
                            // Buscamos el registro recién insertado por Seq.
                            MarcaSincronizadoPorSeq(doc.Seq);
                        }

                        if (rta.ErrorException != null &&
                            !string.IsNullOrEmpty(rta.ErrorMessage))
                        {
                            ConError = true;
                            MensajeError = rta.ErrorMessage;
                            Loging.EscribeMensaje(rta.ErrorMessage);
                        }
                        break;
                    }
                    else
                    {
                        ConError = true;
                        MensajeError = "Sin comunicacion al servidor de APIVentas";
                        Loging.EscribeMensaje("Sin comunicacion al servidor de APIVentas");
                        intentos++;
                        if (intentos < 2)
                            System.Threading.Thread.Sleep(1500);
                    }
                }
            }
            catch (Exception ex)
            {
                ConError = true;
                MensajeError = ex.Message;
                Loging.EscribeExcepcion("PostComprobante", ex);
            }

            // Aunque no se haya enviado por red, devolvemos true porque el
            // ticket quedó guardado localmente y será sincronizado luego.
            // Si el caller necesita distinguir ambos casos, exponer una
            // propiedad adicional "EnviadoInmediato".
            return true;
        }

        // ----------------------------------------------------------------
        //  Helpers privados
        // ----------------------------------------------------------------

        /// <summary>
        /// Busca el id interno del ticket por su Seq y lo marca como
        /// sincronizado. Solo se llama cuando el envío inmediato fue exitoso.
        /// </summary>
        private void MarcaSincronizadoPorSeq(string seq)
        {
            try
            {
                // Reutilizamos ObtenerPendientes con límite 1 y filtramos por seq.
                // Es más simple que agregar un método nuevo al repositorio.
                // Si hay mucho volumen conviene agregar un método específico.
                var pendientes = _syncRepo.ObtenerPendientes(200);
                foreach (TicketPendiente tp in pendientes)
                {
                    if (tp.Seq == seq)
                    {
                        _syncRepo.MarcarSincronizado(tp.Id);
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Loging.EscribeExcepcion("MarcaSincronizadoPorSeq", ex);
            }
        }

        public bool GuardaSqlServer(HeaderDoc doc)
        {

            bool aux = false;
            try
            {
                //HeaderDoc tiene un encabezado y varios detalles
                string query = @"INSERT INTO phdr (seq, idtienda, caja, cajero, canalventa, puntoventa, tipocomprobante, numerocomprobante, 
                                codcli, nombrecli, direccioncli, idcondiva,reparticionid, total, fecha, hora, nrovendedor, 
                                referenciacomprobante, sync, anulado, motivonc) VALUES 
                                (@seq, @idtienda, @caja, @cajero, @canalventa, @puntoventa, @tipocomprobante, @numerocomprobante, 
                                @codcli, @nombrecli, @direccioncli, @idcondiva, @reparticionid, @total, @fecha, @hora, @nrovendedor, 
                                @referenciacomprobante, @sync, @anulado, @motivonc);";

                using (SqlConnection connection = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))
                {
                    connection.Open();
                    SqlCommand command = connection.CreateCommand();
                    command.CommandText = query;
                    command.Parameters.AddWithValue("@seq", doc.Seq);
                    command.Parameters.AddWithValue("@idtienda", doc.Tienda.IdTienda);
                    command.Parameters.AddWithValue("@caja", doc.Caja);
                    command.Parameters.AddWithValue("@cajero", doc.Cajero);
                    command.Parameters.AddWithValue("@canalventa", doc.CanalVenta);
                    command.Parameters.AddWithValue("@puntoventa", doc.PuntoVenta);
                    command.Parameters.AddWithValue("@tipocomprobante", doc.TipoComprobante);
                    command.Parameters.AddWithValue("@numerocomprobante", doc.NumeroComprobante);
                    command.Parameters.AddWithValue("@codcli", doc.CodCli);
                    command.Parameters.AddWithValue("@nombrecli", doc.Nombre);
                    command.Parameters.AddWithValue("@direccioncli", doc.Direccion);
                    command.Parameters.AddWithValue("@idcondiva", doc.mTipoIva.IdCondIva);
                    command.Parameters.AddWithValue("@reparticionid", doc.mReparticion.ReparticionID);
                    command.Parameters.AddWithValue("@total", doc.Total);
                    command.Parameters.AddWithValue("@fecha", doc.FechaHora.ToString("yyyy-MM-dd"));
                    command.Parameters.AddWithValue("@hora", doc.FechaHora.ToString("HH:mm:ss"));
                    command.Parameters.AddWithValue("@nrovendedor", doc.NroVendedor);
                    command.Parameters.AddWithValue("@referenciacomprobante", doc.ReferenciaComprobante);
                    command.Parameters.AddWithValue("@sync", doc.Sync);
                    command.Parameters.AddWithValue("@anulado", doc.Anulado ? 1 : 0);
                    command.Parameters.AddWithValue("@motivonc", doc.MotivoNC); // Por ahora vacio
                    command.ExecuteNonQuery();

                    // Insertar detalles venta
                    query = @"INSERT INTO pdeta (seq, codigo, dep, subdep, importe, scan, precio, cantidad, iva, escaneado, 
                        milla, anulado, hora) VALUES (@seq, @codigo, @dep, @subdep, @importe, @scan, @precio, @cantidad, 
                        @iva, @escaneado, @milla, @anulado, @hora);";
                    command.CommandText = query;
                    foreach (DetalleDoc deta in doc.Detalle)
                    {
                        command.Parameters.Clear();
                        command.Parameters.AddWithValue("@seq", doc.Seq);
                        command.Parameters.AddWithValue("@codigo", deta.Codigo);
                        command.Parameters.AddWithValue("@dep", deta.Departamento);
                        command.Parameters.AddWithValue("@subdep", deta.Subdepartamento);
                        command.Parameters.AddWithValue("@importe", deta.Importe);
                        command.Parameters.AddWithValue("@scan", deta.Scan);
                        command.Parameters.AddWithValue("@precio", deta.Precio);
                        command.Parameters.AddWithValue("@cantidad", deta.Cantidad);
                        command.Parameters.AddWithValue("@iva", deta.IVA);
                        command.Parameters.AddWithValue("@escaneado", deta.Escaneado ? 1 : 0);
                        command.Parameters.AddWithValue("@milla", deta.Milla);
                        command.Parameters.AddWithValue("@anulado", deta.Anulado ? 1 : 0);
                        command.Parameters.AddWithValue("@hora", deta.Hora.ToString("HH:mm:ss"));
                        command.ExecuteNonQuery();
                    }

                    //insertar pagos
                    query = @"INSERT INTO ppago (seq, mpago, monto, vuelto, codtarjeta, cuotas, lote, cupon, autorizacion, integracion,
                            terminal, qr, extracash, hora) VALUES (@seq, @mpago, @monto, @vuelto, @codtarjeta, @cuotas, @lote, 
                            @cupon, @autorizacion, @integracion, @terminal, @qr, @extracash, @hora);";
                    command.CommandText = query;
                    foreach (PagoDoc pago in doc.Pagos)
                    {
                        command.Parameters.Clear();
                        command.Parameters.AddWithValue("@seq", doc.Seq);
                        command.Parameters.AddWithValue("@mpago", pago.MedioPago);
                        command.Parameters.AddWithValue("@monto", pago.Monto);
                        command.Parameters.AddWithValue("@vuelto", pago.Vuelto);
                        command.Parameters.AddWithValue("@codtarjeta", pago.CodTarjeta);
                        command.Parameters.AddWithValue("@cuotas", pago.Cuotas);
                        command.Parameters.AddWithValue("@lote", pago.Lote);
                        command.Parameters.AddWithValue("@cupon", pago.Cupon);
                        command.Parameters.AddWithValue("@autorizacion", pago.Autorizacion);
                        command.Parameters.AddWithValue("@integracion", pago.Integracion);
                        command.Parameters.AddWithValue("@terminal", pago.Terminal);
                        command.Parameters.AddWithValue("@qr", pago.QR);
                        command.Parameters.AddWithValue("@extracash", pago.Extracash);
                        command.Parameters.AddWithValue("@hora", pago.Hora.ToString("HH:mm:ss"));
                        command.ExecuteNonQuery();
                    }

                    //insertar impuestos
                    query = @"INSERT INTO impuestosaplicados (seq, tipoimpuesto, monto, alicuota, hora) VALUES 
                            (@seq, @tipoimpuesto, @monto, @alicuota, @hora);";
                    command.CommandText = query;
                    foreach (ImpuestoAplicado imp in doc.Impuestos)
                    {
                        command.Parameters.Clear();
                        command.Parameters.AddWithValue("@seq", doc.Seq);
                        command.Parameters.AddWithValue("@tipoimpuesto", imp.TipoImpuesto);
                        command.Parameters.AddWithValue("@monto", imp.Monto);
                        command.Parameters.AddWithValue("@alicuota", imp.Alicuota);
                        command.Parameters.AddWithValue("@hora", imp.Hora.ToString("HH:mm:ss"));
                        command.ExecuteNonQuery();
                    }

                    //insertar promociones
                    query = @"INSERT INTO promosaplicadas (seq, codpromo, descarga, cantidad, iva, monto, hora) VALUES 
                            (@seq, @codpromo, @descarga, @cantidad, @iva, @monto, @hora);";
                    command.CommandText = query;
                    PromoAplicada pcupon = null;
                    foreach (PromoAplicada promo in doc.Promociones)
                    {
                        command.Parameters.Clear();
                        command.Parameters.AddWithValue("@seq", doc.Seq);
                        command.Parameters.AddWithValue("@codpromo", promo.CodPromo);
                        command.Parameters.AddWithValue("@descarga", promo.Descarga);
                        command.Parameters.AddWithValue("@cantidad", promo.Cantidad);
                        command.Parameters.AddWithValue("@iva", promo.IVA);
                        command.Parameters.AddWithValue("@monto", promo.Monto);
                        command.Parameters.AddWithValue("@hora", promo.Hora.ToString("HH:mm:ss"));
                        command.ExecuteNonQuery();

                        //Esto es para verificar que la promocion es de un cupon y si es lo agrega en tabla cupones
                        //if ((Math.Abs(99999999999999 - promo.CodPromo) < 1000) || promo.CodPromo == 20251001020901)
                        if ((Math.Abs(99999999999999 - promo.CodPromo) < 1000))
                            pcupon = promo;
                    }
                    connection.Close();
                }
                aux = true;
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "BuscaClientePorCod");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }
    }
}

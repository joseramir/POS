using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


using System.Configuration;
using Newtonsoft.Json;
using RestSharp;
using System.IO;


namespace Alberdi.TJOCommon.ComunicaMercadoPago
{
    public class OperaQrCashout
    {
        private const int MAX_INTENTOS = 6;//NUMERO MAXIMO DE INTENTOS ANTES DE DEJAR DE REINTENTAS, PASADO ESTE LIMITE SE CONSIDERA QUE NO RESPONDE EL WEB

        public bool ConError { get; set; }
        public string MensajeError { get; set; }
        public int CantIntentos { get; set; }


        public RtaOrdenCaja CrearOrdenCashout(OrdenCashout peticion)
        {
            RtaOrdenCaja RespuestaVenta = null;
            this.ConError = false;

            string urlCreaOrden = "http://" + ConfigurationManager.AppSettings["ipsmartpointmp"] + ":" + ConfigurationManager.AppSettings["portsmartpointmp"];

            urlCreaOrden += "/api/AlberdiQr/" + (peticion.Cashout > 0 ? "OrdenCajaQRCashOut" : "OrdenCajaQR");

            string jsonToSend = Newtonsoft.Json.JsonConvert.SerializeObject(peticion, Formatting.Indented, new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore });
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlCreaOrden);
                RestRequest request = new RestRequest();
                request.Method = Method.POST;
                request.AddHeader("Content-Type", "application/json");
                request.AddParameter("application/json", jsonToSend, ParameterType.RequestBody);

                IRestResponse rta = cliente.Execute(request);

                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<RtaOrdenCaja>(rta.Content.ToString());
                }
                else
                {
                    this.ConError = true;
                    if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = "No se encontro terminal para la caja y tienda indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "No se pudo crear la Orden en BD WebService";
                    }
                    else
                    {
                        this.MensajeError = "No se pudo crear la orden. Verificar conexiones de red y servidor.";
                    }
                    this.EscribeLog("No se pudo crear la orden, verificar WEBService", "CAJA=>WEB");
                }
            }
            catch (Exception ex)
            {
                RespuestaVenta = null;
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaVenta;
        }

        public RtaOrdenCaja ConsultarStatusOrden(string external_reference)
        {
            RtaOrdenCaja RespuestaVenta = null;
            this.ConError = false;
            string urlWebhook = "http://" + ConfigurationManager.AppSettings["ipsmartpointmp"] + ":" + ConfigurationManager.AppSettings["portsmartpointmp"] +
                "/api/AlberdiQr/ConsuCaja?external_reference=" + external_reference;
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlWebhook);
                RestRequest reqWebhok = new RestRequest();
                reqWebhok.Method = Method.GET;
                reqWebhok.AddHeader("Content-Type", "application/json");
                IRestResponse rtaWhebhook = cliente.Execute(reqWebhok);
                if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<RtaOrdenCaja>(rtaWhebhook.Content.ToString());
                }
                else
                {
                    if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "Se requiere el envio de 'External_reference'";
                    }
                    else if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        if (rtaWhebhook.Content.StartsWith("\"PROCESANDO"))
                            return RespuestaVenta;
                        else
                            MensajeError = "No se encontro una Orden con la referencia indicada";
                    }
                    else if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "No se pudo crear la Orden en BD WebService";
                    }
                    else
                    {
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    ConError = true;
                    RespuestaVenta = null;
                    //if (CantIntentos >= MAX_INTENTOS)
                    //{
                    //    ConError = true;
                    //    MensajeError = "Maximo intentos alcanzado";
                    //}
                }

            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaVenta;
        }


        public RtaOrdenCaja ConsultarStatusOrdenMercadoPago(string external_reference)
        {
            RtaOrdenCaja RespuestaVenta = null;
            this.ConError = false;
            string urlWebhook = "http://" + ConfigurationManager.AppSettings["ipsmartpointmp"] + ":" + ConfigurationManager.AppSettings["portsmartpointmp"] +
                "/api/AlberdiQr/ConsuMP?external_reference=" + external_reference;
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlWebhook);
                RestRequest reqWebhok = new RestRequest();
                reqWebhok.Method = Method.GET;
                reqWebhok.AddHeader("Content-Type", "application/json");
                IRestResponse rtaWhebhook = cliente.Execute(reqWebhok);
                if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<RtaOrdenCaja>(rtaWhebhook.Content.ToString());
                }
                else
                {
                    if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "SIN AUTORIZAR order in Mercado Pago.";
                    }
                    else if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        if (rtaWhebhook.Content.StartsWith("\"PROCESANDO"))
                            return RespuestaVenta;
                        else
                            MensajeError = "No se encontro una Orden con la referencia indicada";
                    }
                    else if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        ConError = true;
                        MensajeError = "Error Interno. An error occurred while processing your request.";
                    }
                    else
                    {
                        ConError = true;
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    RespuestaVenta = null;
                }

            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaVenta;
        }

        //Cancela una Orden. Generalmente cuando se presiona la tecla ESCAPE.
        public RtaOrdenCaja CancelarOrden(RtaOrdenCaja peticion)
        {
            RtaOrdenCaja RespuestaCancela = null;
            this.ConError = false;
            string urlWebhook = "http://" + ConfigurationManager.AppSettings["ipsmartpointmp"] + ":" + ConfigurationManager.AppSettings["portsmartpointmp"] +
                "/api/AlberdiQr/CancelarOrden";
            string jsonToSend = Newtonsoft.Json.JsonConvert.SerializeObject(peticion, Formatting.None, new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore });
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlWebhook);
                RestRequest reqWebhok = new RestRequest();
                reqWebhok.Method = Method.POST;
                reqWebhok.AddHeader("Content-Type", "application/json");
                reqWebhok.AddParameter("application/json", jsonToSend, ParameterType.RequestBody);
                IRestResponse rtaWhebhook = cliente.Execute(reqWebhok);
                if (rtaWhebhook.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RespuestaCancela = Newtonsoft.Json.JsonConvert.DeserializeObject<RtaOrdenCaja>(rtaWhebhook.Content.ToString());
                }
                else
                {
                    ConError = true;
                    MensajeError = "No se pudo Cancelar. Anular en la Terminal";
                }
            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
                EscribeLog(ex.Message);
            }
            return RespuestaCancela;
        }


        public void EscribeLog(string line, string sentido)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogQrCashout.txt");
            writer.WriteLine(String.Format("{0}= {1}: '{2}'", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), sentido, line));
            writer.Close();
        }

        public void EscribeLog(string titulo)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogQrCashout.txt");
            writer.WriteLine("-------------------------------------------------------------------");
            writer.WriteLine(String.Format("{0}= {1}", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), titulo.ToUpper()));
            writer.Close();
        }
        
    }
}

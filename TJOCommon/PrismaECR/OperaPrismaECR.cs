using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Configuration;
using Newtonsoft.Json;
using RestSharp;
using System.IO;


namespace Alberdi.TJOCommon.PrismaECR
{
    public class OperaPrismaECR
    {        

        public bool ConError { get; set; }
        public string MensajeError { get; set; }
        public int CantIntentos { get; set; }

        //Crea la intencion de pago en Web API que a su vez lo envia a Payway
        public PosPaymentResponse CreaIntencionPago(PosPaymentRequest peticion)
        {
            PosPaymentResponse RespuestaVenta = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                    "/api/v1/pos/pagos";
            string jsonToSend = Newtonsoft.Json.JsonConvert.SerializeObject(peticion, Formatting.None, new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore });
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.POST;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                reqPrismaEcr.AddParameter("application/json", jsonToSend, ParameterType.RequestBody);
                this.EscribeLog("CreaIntencionPago: " + jsonToSend, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("CreaIntencionPago: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    //RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                }
                else
                {
                    this.ConError = true;
                    if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se encontro terminal para la caja y tienda indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = RespuestaVenta.Mensaje; 
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se pudo crear la Orden en BD WebService";                        
                    }
                    else
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se pudo crear la orden. Verificar conexiones de red y servidor.";
                    }
                    this.EscribeLog("No se pudo crear la orden, " + MensajeError, "CAJA=>WEBAPI");
                    RespuestaVenta = null;
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

        //Consulta a Web API si un pago ya esta autorizado, Web API consulta en su BD Local
        public PosPaymentResponse ConsultarStatusPago(string IdOperacionPayway)
        {
            PosPaymentResponse RespuestaVenta = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                "/api/v1/pos/pagos/" + IdOperacionPayway;
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.GET;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                this.EscribeLog("ConsultarStatusPago: " + IdOperacionPayway, "CAJA=>WEBAPI");
                IRestResponse rtaPago = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("ConsultarStatusPago: " + rtaPago.Content.ToString(), "CAJA<=WEBAPI");
                if (rtaPago.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rtaPago.Content.ToString());
                }
                else
                {
                    if (rtaPago.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "Se requiere el envio de 'External_reference'";
                    }
                    else if (rtaPago.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = "No se encontro una Orden con la referencia indicada";
                    }
                    else if (rtaPago.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "No se pudo crear la Orden en BD WebService";
                    }
                    else
                    {
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    ConError = true;
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

        //Cancela la intencion de pago, se usa cuando la intencion no esta siendo procesada
        public PosPaymentResponse CancelarIntencionPago(string IdOperacionPayway)
        {
            PosPaymentResponse RespuestaVenta = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                    "/api/v1/pos/pagos/" + IdOperacionPayway + "/cancelaciones";            
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.PUT;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                this.EscribeLog("CancelarIntencionPago: " + IdOperacionPayway, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("CancelarIntencionPago: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    //RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                }
                else
                {
                    this.ConError = true;
                    if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se encontro terminal para la caja y tienda indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = RespuestaVenta.Mensaje;
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se pudo crear la Orden en BD WebService";                        
                    }
                    else
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se pudo crear la orden. Verificar conexiones de red y servidor.";
                    }
                    this.EscribeLog("No se pudo cancelar la intencion, " + MensajeError, "CAJA=>WEB");
                    RespuestaVenta = null;
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

        //Consulta a Web API por una operacion aprobada usando el LOTE, CUPON y AUTORIZACION
        public PosPaymentResponse buscaTransaccionAnula(string claveBusq)
        {
            PosPaymentResponse RespuestaVenta = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                    "api/v1/pos/anulaciones/" + claveBusq;
            
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.GET;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                this.EscribeLog("buscaTransaccionAnula: " + claveBusq, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("buscaTransaccionAnula: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    //RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                }
                else
                {
                    this.ConError = true;
                    if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se encontro terminal para la caja y tienda indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = RespuestaVenta.Mensaje;
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se pudo crear la Orden en BD WebService";                        
                    }
                    else
                    {
                        MensajeError = RespuestaVenta.Mensaje; //"No se pudo crear la orden. Verificar conexiones de red y servidor.";
                    }
                    this.EscribeLog("No se pudo encontrar, " + MensajeError, "CAJA=>WEB");
                    RespuestaVenta = null;
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

        //Crea una intencion de Anulacion de Pago en Web API que a su vez lo hace en Payway
        public PosAnulacionResponse CreaIntencionAnula(string claveBusq)
        {
            PosAnulacionResponse RespuestaIntencion = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                    "/api/v1/pos/anulaciones/" + claveBusq;
            
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.POST;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                this.EscribeLog("CreaIntencionAnula: " + claveBusq, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("CreaIntencionAnula: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                RespuestaIntencion = Newtonsoft.Json.JsonConvert.DeserializeObject<PosAnulacionResponse>(rta.Content.ToString());
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    //RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                }
                else
                {
                    this.ConError = true;
                    if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = RespuestaIntencion.Mensaje; //"No se encontro terminal para la caja y tienda indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = RespuestaIntencion.Mensaje;
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = RespuestaIntencion.Mensaje; //"No se pudo crear la Orden en BD WebService";                        
                    }
                    else
                    {
                        MensajeError = RespuestaIntencion.Mensaje; //"No se pudo crear la orden. Verificar conexiones de red y servidor.";
                    }
                    this.EscribeLog("No se pudo crear la orden, " + MensajeError, "CAJA=>WEB");
                    RespuestaIntencion = null;
                }
            }
            catch (Exception ex)
            {
                RespuestaIntencion = null;
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaIntencion;
        }

        //Consulta por el Estado de una anulacion previamente creada
        public PosAnulacionResponse ConsultarStatusAnulacion(string idOperacionPayway)
        {
            PosAnulacionResponse RespuestaConsulta = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                "/api/v1/pos/anulaciones/" + idOperacionPayway + "/estado";
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.GET;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                this.EscribeLog("ConsultarStatusAnulacion: " + idOperacionPayway, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("ConsultarStatusAnulacion: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RespuestaConsulta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosAnulacionResponse>(rta.Content.ToString());
                }
                else
                {
                    if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "Se requiere el envio de 'idOperacionPayway'";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = "No se encontro una Orden con la referencia indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "No se pudo crear la Orden en BD WebService";
                    }
                    else
                    {
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    ConError = true;
                    RespuestaConsulta = null;
                }

            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaConsulta;
        }

        //Cancela la intencion de pago, se usa cuando la intencion no esta siendo procesada
        public PosAnulacionResponse CancelarIntencionAnulacion(string idOperacionPayway)
        {
            PosAnulacionResponse RespuestaCancela = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                    "/api/v1/pos/anulaciones/" + idOperacionPayway + "/cancelaciones";
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.PUT;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                this.EscribeLog("CancelarIntencionAnulacion: " + idOperacionPayway, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("CancelarIntencionAnulacion: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                RespuestaCancela = Newtonsoft.Json.JsonConvert.DeserializeObject<PosAnulacionResponse>(rta.Content.ToString());
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    //RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                }
                else
                {
                    this.ConError = true;
                    if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = RespuestaCancela.Mensaje; //"No se encontro terminal para la caja y tienda indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = RespuestaCancela.Mensaje;
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = RespuestaCancela.Mensaje; //"No se pudo crear la Orden en BD WebService";                        
                    }
                    else
                    {
                        MensajeError = RespuestaCancela.Mensaje; //"No se pudo crear la orden. Verificar conexiones de red y servidor.";
                    }
                    this.EscribeLog("No se pudo cancelar la intencion, " + MensajeError, "CAJA=>WEB");
                    RespuestaCancela = null;
                }
            }
            catch (Exception ex)
            {
                RespuestaCancela = null;
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaCancela;
        }

        /**********************************/
        public PosCierreResponse CreaIntencionCierre(int numSucursal, int numCaja)
        {
            PosCierreRequest peticion = new PosCierreRequest() { NumeroSucursal = numSucursal, NumeroCaja = numCaja };
            PosCierreResponse RespuestaCierre = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                    "/api/v1/pos/cierres";
            string jsonToSend = Newtonsoft.Json.JsonConvert.SerializeObject(peticion, Formatting.None, new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore });
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.POST;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                reqPrismaEcr.AddParameter("application/json", jsonToSend, ParameterType.RequestBody);
                this.EscribeLog("CreaIntencionCierre: " + jsonToSend, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("CreaIntencionCierre: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                RespuestaCierre = Newtonsoft.Json.JsonConvert.DeserializeObject<PosCierreResponse>(rta.Content.ToString());
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    //RespuestaVenta = Newtonsoft.Json.JsonConvert.DeserializeObject<PosPaymentResponse>(rta.Content.ToString());
                }
                else
                {
                    this.ConError = true;
                    if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = RespuestaCierre.Mensaje; //"No se encontro terminal para la caja y tienda indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = RespuestaCierre.Mensaje;
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = RespuestaCierre.Mensaje; //"No se pudo crear la Orden en BD WebService";                        
                    }
                    else
                    {
                        MensajeError = RespuestaCierre.Mensaje; //"No se pudo crear la orden. Verificar conexiones de red y servidor.";
                    }
                    this.EscribeLog("No se pudo crear la orden, " + MensajeError, "CAJA=>WEB");
                    RespuestaCierre = null;
                }
            }
            catch (Exception ex)
            {
                RespuestaCierre = null;
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaCierre;
        }

        //Consulta por el Estado de una anulacion previamente creada
        public PosCierreResponse ConsultarStatusCierre(string idOperacionPayway)
        {
            PosCierreResponse RespuestaCierre = null;
            this.ConError = false;
            this.MensajeError = "";

            string urlApiPrismaEcr = "http://" + ConfigurationManager.AppSettings["ipapiprismaecr"] + ":" + ConfigurationManager.AppSettings["portapiprismaecr"] +
                "/api/v1/pos/cierres/" + idOperacionPayway;
            try
            {
                CantIntentos++;
                RestClient cliente = new RestClient(urlApiPrismaEcr);
                RestRequest reqPrismaEcr = new RestRequest();
                reqPrismaEcr.Method = Method.GET;
                reqPrismaEcr.AddHeader("Content-Type", "application/json");
                this.EscribeLog("ConsultarStatusCierre: " + idOperacionPayway, "CAJA=>WEBAPI");
                IRestResponse rta = cliente.Execute(reqPrismaEcr);
                this.EscribeLog("ConsultarStatusCierre: " + rta.Content.ToString(), "CAJA<=WEBAPI");
                if (rta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RespuestaCierre = Newtonsoft.Json.JsonConvert.DeserializeObject<PosCierreResponse>(rta.Content.ToString());
                }
                else
                {
                    if (rta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "Se requiere el envio de 'idOperacionPayway'";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = "No se encontro una Orden con la referencia indicada";
                    }
                    else if (rta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "No se pudo crear la Orden en BD WebService";
                    }
                    else
                    {
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    ConError = true;
                    RespuestaCierre = null;
                }

            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
            }
            return RespuestaCierre;
        }


        public void EscribeLog(string line, string sentido)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogPrismaECR.txt");
            writer.WriteLine(String.Format("{0}= {1}: '{2}'", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), sentido, line));
            writer.Close();
        }

        public void EscribeLog(string titulo)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogPrismaECR.txt");
            writer.WriteLine("-------------------------------------------------------------------");
            writer.WriteLine(String.Format("{0}= {1}", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), titulo.ToUpper()));
            writer.Close();
        }
    }
}

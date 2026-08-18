using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using RestSharp;
using System.Configuration;
using Newtonsoft.Json;
using System.IO;


namespace LibEntidades.Alberdi.Parking
{
    public class ClienteParking
    {
        public bool ConError { get; set; }
        public string MensajeError { get; set; }

        public ClienteParking()
        {

        }

        public DatosParking GetDatosParking(int idParking)
        {
            DatosParking aux = null;

            this.ConError = false;
            //prueba con ngrok 
            //string urlParking = ConfigurationManager.AppSettings["ipapiparking"] + "/estadia/" + idParking;
            //LINK SERVIDOR 192.168.0.22
            string urlParking = "http://" + ConfigurationManager.AppSettings["ipapiparking"] + ":" + ConfigurationManager.AppSettings["portapiparking"] +"/estadia/" + idParking;
            
            try
            {
                //La siguiente linea no funciona en ejecucion sí compila
                System.Net.ServicePointManager.SecurityProtocol = (System.Net.SecurityProtocolType)3072;// System.Net.SecurityProtocolType.Tls12;
                RestClient cliente = new RestClient(urlParking);
                RestRequest reqParking = new RestRequest();
                reqParking.Method = Method.GET;
                reqParking.AddHeader("Content-Type", "application/json");
                reqParking.AddHeader("X-API-Key", ConfigurationManager.AppSettings["apikeyparking"]);
                EscribeLog(string.Format("Solicito el idParking: {0}", idParking), "CAJA=>WEBPARK");
                IRestResponse respuesta = cliente.Execute(reqParking);
                //System.Windows.Forms.MessageBox.Show(urlParking + "\n" + respuesta.Content.ToString());
                //string respuestaS = "[  { \"Cantidad\": 1,  \"Codigo\": 66519, \"Descripcion\": \"Tarifa Estacionamiento - Particular Auto\",  \"MontoTotal\": 5000.0, \"PrecioUnitario\": 5000.0  }]";
                if (respuesta.StatusCode == System.Net.HttpStatusCode.OK)
                //if (true)
                {
                    DatosParking[] data = Newtonsoft.Json.JsonConvert.DeserializeObject<DatosParking[]>(respuesta.Content.ToString());
                    //DatosParking[] data = Newtonsoft.Json.JsonConvert.DeserializeObject<DatosParking[]>(respuestaS);
                    if (data != null && data.Length > 0)
                    {
                        aux = data[0];
                        EscribeLog(string.Format("Recibido OK - {0}", aux.ToString()), "CAJA<=WEBPARK");
                    }
                    else
                    {
                        this.ConError = true;
                        this.MensajeError = "No se encontraron datos de parking para el ID indicado.";
                        EscribeLog("No se encontraron datos de parking para el ID indicado.", "CAJA<=WEBPARK");
                    }
                    //aux = Newtonsoft.Json.JsonConvert.DeserializeObject<ListParking>(respuestaS);
                }
                else
                {
                    if (respuesta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "BadRequest-Algun dato enviado esta mal";
                    }
                    else if (respuesta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = "NotFound- No se encontro una Orden con la IDPARKING indicada";
                    }
                    else if (respuesta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "InternalServerError - Error interno de servidor API Parking";
                    }
                    else
                    {
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    EscribeLog(MensajeError, "CAJA<=WEBPARK");
                    ConError = true;
                }

            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
                EscribeLog(string.Format("ERROR - Mensaje: {0}, Excepcion: {1}", ex.Message, ex.StackTrace));
            }
            return aux;
        }

        //public bool PostDatosParking(int idParking, List<PagoDoc> lista_pagos,int caja)
        public bool PostDatosParking(ParkingPost datos)
        {
            bool aux = false;
            this.ConError = false;

            //ParkingPost datos = new ParkingPost() { id_estadia = idParking, estado = false, pagos = lista_pagos ,caja=caja};

            //string urlParking = ConfigurationManager.AppSettings["ipapiparking"] + "/actualizar-estado";
            string urlParking = "http://" + ConfigurationManager.AppSettings["ipapiparking"] + ":" + ConfigurationManager.AppSettings["portapiparking"] + "/actualizar-estado";
            try
            {
                //La siguiente linea no funciona en ejecucion sí compila
                System.Net.ServicePointManager.SecurityProtocol = (System.Net.SecurityProtocolType)3072;// System.Net.SecurityProtocolType.Tls12;
                string jsonToSend = Newtonsoft.Json.JsonConvert.SerializeObject(datos, Formatting.None, new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore });
                RestClient cliente = new RestClient(urlParking);
                RestRequest request = new RestRequest();
                request.Timeout = 3000;
                request.Method = Method.POST;
                request.AddHeader("Content-Type", "application/json");
                //Agrego API KEY
                request.AddHeader("X-API-Key", ConfigurationManager.AppSettings["apikeyparking"]);
                request.AddParameter("application/json", jsonToSend, ParameterType.RequestBody);

                EscribeLog(string.Format("POST Parking: {0}", datos.ToString()), "CAJA=>WEBPARK");
                IRestResponse respuesta = cliente.Execute(request);
                if (respuesta.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    aux = true;
                    EscribeLog("Recibo OK POST Parking", "CAJA=>WEBPARK");
                }
                else
                {
                    if (respuesta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "BadRequest - Se requiere el envio de 'External_reference'";
                    }
                    else if (respuesta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = "NotFound - No se encontro una Orden con la referencia indicada";
                    }
                    else if (respuesta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "InternalServerError - Error interno de servidor API Parking";
                    }
                    else
                    {
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    EscribeLog(MensajeError, "CAJA<=WEBPARK");
                    ConError = true;
                }

            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
                EscribeLog(string.Format("ERROR - Mensaje: {0}, Excepcion: {1}", ex.Message, ex.StackTrace));
            }
            return aux;
        }


        public AutorizacionCierre ConsultaCierreRealizado()
        {
            AutorizacionCierre aux = null;

            this.ConError = false;
            string urlParking = "http://" + ConfigurationManager.AppSettings["ipapiparking"] + ":" + ConfigurationManager.AppSettings["portapiparking"] + "/autorizacion-cierre-caja";

            try
            {
                //La siguiente linea no funciona en ejecucion sí compila
                System.Net.ServicePointManager.SecurityProtocol = (System.Net.SecurityProtocolType)3072;// System.Net.SecurityProtocolType.Tls12;
                //System.Net.ServicePointManager.SecurityProtocol = System.Net.SecurityProtocolType.Tls;
                RestClient cliente = new RestClient(urlParking);
                RestRequest reqParking = new RestRequest();
                reqParking.Method = Method.GET;
                reqParking.AddHeader("Content-Type", "application/json");
                reqParking.AddHeader("X-API-Key", ConfigurationManager.AppSettings["apikeyparking"]);
                EscribeLog("Consulto estado del cierre", "CAJA=>WEBPARK");
                IRestResponse respuesta = cliente.Execute(reqParking);
                if (respuesta.StatusCode == System.Net.HttpStatusCode.OK)                
                {
                    AutorizacionCierre data = Newtonsoft.Json.JsonConvert.DeserializeObject<AutorizacionCierre>(respuesta.Content.ToString());
                    if (data != null)
                    {
                        aux = data;
                        EscribeLog(string.Format("Recibido OK - {0}", aux.ToString()), "CAJA<=WEBPARK");
                    }
                    else
                    {
                        this.ConError = true;
                        this.MensajeError = "No se pudo consultar el Cierre parking.";
                        EscribeLog("No se encontraron datos de parking para el ID indicado.", "CAJA<=WEBPARK");
                    }
                    
                }
                else
                {
                    if (respuesta.StatusCode == System.Net.HttpStatusCode.BadRequest)
                    {
                        MensajeError = "BadRequest-Algun dato enviado esta mal";
                    }
                    else if (respuesta.StatusCode == System.Net.HttpStatusCode.NotFound)
                    {
                        MensajeError = "NotFound- No se encontro una Orden con la IDPARKING indicada";
                    }
                    else if (respuesta.StatusCode == System.Net.HttpStatusCode.InternalServerError)
                    {
                        MensajeError = "InternalServerError - Error interno de servidor API Parking";
                    }
                    else
                    {
                        MensajeError = "Verificar la comunicacion con el servidor.";
                    }
                    EscribeLog(MensajeError, "CAJA<=WEBPARK");
                    ConError = true;
                }

            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = ex.Message;
                EscribeLog(string.Format("ERROR - Mensaje: {0}, Excepcion: {1}", ex.Message, ex.StackTrace));
            }
            return aux;
        }


        public void EscribeLog(string line, string sentido)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogParking.txt");
            writer.WriteLine(String.Format("{0}= {1}: '{2}'", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), sentido, line));
            writer.Close();
        }

        public void EscribeLog(string titulo)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogParking.txt");
            writer.WriteLine("-------------------------------------------------------------------");
            writer.WriteLine(String.Format("{0}= {1}", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), titulo.ToUpper()));
            writer.Close();
        }
    }
}
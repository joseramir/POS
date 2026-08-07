using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;
using System.IO.Ports;
using System.Configuration;
using System.Threading;


namespace Alberdi.TJOCommon.Prisma
{
    public class OperarTrxPrisma : IDisposable
    {
        private const char ENQ = '\x05';
        private const char ACK = '\x06';
        private const char NAK = '\x15';

        private const int TSLEEP = 500;

        private const double TSEND = 5000;
        //private const double TAUTOR = 90000;

        private string rtaport = "";

        private bool canSend;
        private int numStep = 0;
        private bool continuar = true;
        private bool EsUnaVenta = false;

        private SerialPort sp;
        ProtocoloTrxPrisma mtx = new ProtocoloTrxPrisma();
        RespuestaBase rtaBase = null;

        public string MensajeError { get; set; }
        public bool ConError { get; set; }

        private System.Timers.Timer tmrSend;
        //private System.Timers.Timer tmrAuto;
        private bool TimeSendExced;
        //private bool TimeAutoExced;
        private Dictionary<int, string> aequiv;

        private DateTime timeInicio;

        public bool Cancelado = false;

        public OperarTrxPrisma(int portNewland)
        {
            sp = new SerialPort();
            //sp.PortName = ConfigurationManager.AppSettings["COMPrisma"];
            sp.PortName = "COM" + (portNewland + 1).ToString();
            sp.BaudRate = 19200;
            sp.DataBits = 8;
            sp.Parity = Parity.None;
            sp.StopBits = StopBits.One;

            sp.WriteTimeout = 500;
            sp.ReadTimeout = 500;

            sp.DataReceived += Sp_DataReceived;
            tmrSend = new System.Timers.Timer(TSEND);
            tmrSend.Elapsed += new System.Timers.ElapsedEventHandler(tmrSend_Elapsed);
            //tmrAuto = new System.Timers.Timer(TAUTOR);
            //tmrAuto.Elapsed += new System.Timers.ElapsedEventHandler(tmrAct_Elapsed);
            aequiv = new Dictionary<int, string>();
            aequiv.Add(0, "<NUL>");
            aequiv.Add(1, "<SOH>");
            aequiv.Add(2, "<STX>");
            aequiv.Add(3, "<ETX>");
            aequiv.Add(4, "<EOT>");
            aequiv.Add(5, "<ENQ>");
            aequiv.Add(6, "<ACK>");
            aequiv.Add(7, "<BEL>");
            aequiv.Add(8, "<BS>");
            aequiv.Add(9, "<HT>");
            aequiv.Add(10, "<LF>");
            aequiv.Add(11, "<VT>");
            aequiv.Add(12, "<FF>");
            aequiv.Add(13, "<CR>");
            aequiv.Add(14, "<SO>");
            aequiv.Add(15, "<SI>");
            aequiv.Add(16, "<DLE>");
            aequiv.Add(17, "<DC1>");
            aequiv.Add(18, "<DC2>");
            aequiv.Add(19, "<DC3>");
            aequiv.Add(20, "<DC4>");
            aequiv.Add(21, "<NAK>");
            aequiv.Add(22, "<SIN>");
            aequiv.Add(23, "<ETB>");
            aequiv.Add(24, "<CAN>");
            aequiv.Add(25, "<EM>");
            aequiv.Add(26, "<SUB>");
            aequiv.Add(27, "<ESC>");
            aequiv.Add(28, "<FS>");
            aequiv.Add(29, "<GS>");
            aequiv.Add(30, "<RS>");
            aequiv.Add(31, "<US>");
            aequiv.Add(127, "<DEL>");
            Cancelado = false;
        }

        //void InicTmrAuto()
        //{
        //    tmrAuto.Enabled = true;
        //    TimeAutoExced = false;
        //}

        //void FinaTmrAuto()
        //{
        //    tmrAuto.Enabled = false;
        //    TimeAutoExced = false;
        //}

        //void tmrAct_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        //{
        //    tmrAuto.Enabled = false;
        //    TimeAutoExced = true;
        //}

        void InicTmrSend()
        {
            tmrSend.Enabled = true;
            TimeSendExced = false;
        }

        void FinaTmrSend()
        {
            tmrSend.Enabled = false;
            TimeSendExced = false;
        }

        void tmrSend_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            tmrSend.Enabled = false;
            TimeSendExced = true;
        }

        private void Sp_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            try
            {
                rtaport = sp.ReadExisting();
                EscribeLog(rtaport, "VER==>PC");
                if (!string.IsNullOrEmpty(rtaport))
                {
                    if (numStep == 1)
                    {
                        FinaTmrSend();
                        if (rtaport.Length == 1)
                        {
                            if (Convert.ToChar(rtaport[0]) == ACK)
                            {
                                continuar = Convert.ToChar(rtaport[0]) == ACK;
                                canSend = true;
                                numStep = 2;
                            }
                            else
                            {
                                if (Convert.ToChar(rtaport[0]) == NAK)
                                {
                                    ConError = true;
                                    MensajeError = "NEWLAND OCUPADO EN OPERACION, VERIFICAR LA PANTALLA DEL EQUIPO";
                                }
                                continuar = false;
                            }
                        }
                        else
                            continuar = false;
                    }
                    //espero la confirmacion de recibido del Verifone
                    else if (numStep == 3)
                    {
                        FinaTmrSend();
                        if (rtaport.Length == 1)
                        {
                            if (Convert.ToChar(rtaport[0]) == ACK)
                            {
                                continuar = Convert.ToChar(rtaport[0]) == ACK;
                                numStep = 4;
                            }
                            else
                                continuar = false;
                        }
                        //puede ocurrir que venga ACK con mensaje de error
                        else
                        {
                            //en este caso hay que enviar ACK de respueta recibida y decodificar mensaje error
                            numStep = 5;
                            canSend = true;
                            //hay que decodificar el mensaje de error
                            ConError = true;
                        }

                    }
                    //es cuando tomo bien el comando. por ejemplo en la venta lo que se espera que se apruebe
                    else if (numStep == 4)
                    {
                        //FinaTmrAuto();
                        //Agregado para validar el tiempo de respuesta en operaciones de Compra tiene menor que 15 segundos. Es decir el envio de ACK
                        timeInicio = DateTime.Now;
                        numStep = 5;
                        canSend = true;
                        //falta decodificar la respuesta                    
                        //if (rtaport.Substring(4, 3) == "000")
                        if (rtaport.Substring(4, 1) == "0")
                            rtaBase = mtx.ResolverRespuesta(rtaport.Substring(1));
                        else
                        {
                            //Este primer error se refiere a Comunicacion entre el Punto de Venta y el Verifone
                            ConError = true;
                            MensajeError = mtx.ResolverError(rtaport.Substring(1));
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                EscribeLog("Linea Excepcion Sp_DataReceived 1");
                ConError = true;
                MensajeError = "Error al recibir respuesta. Hay que Verificar Protocolo Comunicacion.";
            }
        }

        private void Comunicar(string comando)
        {
            try
            {
                Cancelado = false;
                sp.Open();
                sp.DiscardInBuffer();
                sp.DiscardOutBuffer();

                numStep = 0;
                canSend = true;
                continuar = true;
                ConError = false;

                while (continuar && !Cancelado)
                {
                    try
                    {
                        if (canSend)
                        {
                            //arranca la comunicacion
                            if (numStep == 0)
                            {
                                EscribeLog(ENQ.ToString(), "PC==>VER");
                                sp.WriteLine(ENQ.ToString());
                                canSend = false;
                                numStep = 1;
                                InicTmrSend();
                            }
                            //recien ahora puedo escribir el comando a enviar
                            else if (numStep == 2)
                            {
                                EscribeLog(comando, "PC==>VER");
                                sp.WriteLine(comando);
                                canSend = false;
                                numStep = 3;
                                //InicTmrAuto();
                            }
                            //este es el paso final envia la confirmacion de recibido al verifone
                            else if (numStep == 5)
                            {
                                Thread.Sleep(TSLEEP);
                                EscribeLog(ACK.ToString(), "PC==>VER");
                                sp.WriteLine(ACK.ToString());
                                continuar = false;
                                InicTmrSend();
                            }
                        }
                        if (TimeSendExced)// || TimeAutoExced)
                        {
                            ConError = true;
                            MensajeError = "No se recibio respuesta en tiempor esperado.";
                            break;
                        }
                        Thread.Sleep(TSLEEP);

                        if (Cancelado)
                        {
                            ConError = true;
                            MensajeError = "Cancelado por usuario. Ver pantalla Newland!!!";
                            EscribeLog(MensajeError);
                            break;
                        }
                    }
                    catch (Exception)
                    {
                        EscribeLog("Linea Excepcion Comunicar 1");
                        ConError = true;
                        MensajeError = "No se pudo escribir en puerto COM/USB";
                        break;
                    }
                }
                sp.Close();
            }
            catch (Exception)
            {
                EscribeLog("Linea Excepcion Comunicar 2");
                ConError = true;
                MensajeError = "Verificar la conexion al puerto COM/USB"; ;
            }
        }

        public string HacerTest()
        {
            string comando = mtx.GetTest();
            EscribeLog("Hace comando Test");
            Comunicar(comando);
            if (!ConError)
            {
                RtaTest arta = (RtaTest)rtaBase;
                return arta.Respuesta;
            }
            return "";
        }

        public RtaVenta HacerVenta(TrxPrisma trx)
        {
            RtaVenta aux = null;
            string comando = mtx.GetMensajeVta(trx);
            try
            {
                EscribeLog("Hace comando Venta");
                EsUnaVenta = true;
                Comunicar(comando);
                //hasta aqui no hay error en la comunicacion
                if (!ConError)
                {
                    aux = (RtaVenta)rtaBase;
                    //controla si la operacion fue autorizada
                    if (aux.CodigoRespuesta != 0)
                    {
                        ConError = true;
                        MensajeError = aux.MensajeRespuesta;
                    }
                    AccesoDb.GuardarTransaccion(trx, aux);
                }
            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = "";
                EscribeLog("ERROR EX: " + ex.Message + "\n" + ex.StackTrace);
            }
            finally
            {
                EsUnaVenta = false;
            }
            return aux;
        }

        public RtaVenta HacerVentaExtraCash(TrxPrisma trx)
        {
            RtaVenta aux = null;
            string comando = mtx.GetMensajeVtaExtraCash(trx);
            try
            {
                EscribeLog("Hace comando Venta con Extracash");
                EsUnaVenta = true;
                Comunicar(comando);
                //hasta aqui no hay error en la comunicacion
                if (!ConError)
                {
                    aux = (RtaVenta)rtaBase;
                    //controla si la operacion fue autorizada
                    if (aux.CodigoRespuesta != 0)
                    {
                        ConError = true;
                        MensajeError = aux.MensajeRespuesta;
                    }
                    AccesoDb.GuardarTransaccion(trx, aux);
                }
            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = "";
                EscribeLog("ERROR EX: " + ex.Message + "\n" + ex.StackTrace);
            }
            finally { EsUnaVenta = false; }
            return aux;
        }

        public RtaQrModo HacerVentaQrModo(TrxPrisma trx)
        {
            RtaQrModo aux = null;
            string comando = mtx.GetMensajeQrModo(trx);
            try
            {
                EscribeLog("Hace comando Venta con QR");
                EsUnaVenta = true;
                Comunicar(comando);
                //hasta aqui no hay error en la comunicacion
                if (!ConError)
                {
                    aux = (RtaQrModo)rtaBase;
                    trx.CodigoTarjeta = aux.CodigoTarjeta;
                    //controla si la operacion fue autorizada
                    if (aux.CodigoRespuesta != 0)
                    {
                        ConError = true;
                        MensajeError = aux.MensajeRespuesta;
                    }
                    AccesoDb.GuardarTransaccionQr(trx, aux);
                }
            }
            catch (Exception ex)
            {
                this.ConError = true;
                this.MensajeError = "";
                EscribeLog("ERROR EX: " + ex.Message + "\n" + ex.StackTrace);
            }
            finally { EsUnaVenta = false; }
            return aux;
        }

        public void HacerDevolAnul(ref TrxPrisma trxOrig, ref RtaVenta rtaOrig)
        {
            if (AccesoDb.GetTransaccion(ref trxOrig, ref rtaOrig))
            {
                if (trxOrig.Estado == 0)
                {
                    string comando = "";
                    if (rtaOrig.Cerrado)//se hace devolucion
                    {
                        EscribeLog("Hace comando Devolucion");
                        comando = mtx.GetMensajeDevolucion(trxOrig, rtaOrig);
                    }
                    else
                    {
                        EscribeLog("Hace comando Anulacion");
                        comando = mtx.GetMensajeAnulacion(rtaOrig.NumeroCupon, trxOrig.CodigoTarjeta);
                    }
                    Comunicar(comando);
                    if (!ConError)
                    {
                        RtaVenta mrta = (RtaVenta)rtaBase;
                        if (mrta.CodigoRespuesta == 0)
                        {
                            if (rtaOrig.Cerrado)//Devol
                                trxOrig.Estado = 2;
                            else
                                trxOrig.Estado = 1;//Anulacion
                            AccesoDb.WriteEstadoTransaccion(trxOrig);
                            //Como la operacion esta aprobada se agrega un nuevo registro de la operacion
                            //con datos de transaccion original, cambia el estado porque esta aprobado
                            trxOrig.Estado = 3;//como registro de anulacion
                            AccesoDb.GuardarTransaccion(trxOrig, mrta);
                        }
                        else
                            MensajeError = "No se pudo anular/devol la operacion\n" + mrta.MensajeRespuesta;
                    }
                }
                else
                {
                    ConError = true;
                    MensajeError = "La transaccion ya se anulo o devolvio, segun registros";
                }
            }
            else
            {
                ConError = true;
                MensajeError = "No se encontro la operacion indicada";
            }
        }

        public void HacerCierre()
        {
            string comando = mtx.GetCierre();
            EscribeLog("Hace comando Cierre de lote");
            Comunicar(comando);
            //hasta aqui no hay error en la comunicacion
            if (!ConError)
            {
                RtaCierre mrta = (RtaCierre)rtaBase;
                AccesoDb.WriteCierre();
            }

        }

        public void ObtenerTarjetas()
        {
            string comando = "";
            int i = 0;
            AccesoDb.LimpiarTarjetas();
            EscribeLog("Hace comando Obtiene Tarjetas");
            ConError = false;
            while (!ConError)
            {
                comando = mtx.GetTablaTarjetas(i);
                Comunicar(comando);
                if (!ConError)
                {
                    RtaTablaTarjeta mrta = (RtaTablaTarjeta)rtaBase;
                    AccesoDb.AgregarTarjeta(mrta);
                }
                i++;
            }
        }

        public void ObtenerPlanes()
        {
            string comando = "";
            int i = 0;
            AccesoDb.LimpiarPlanes();
            EscribeLog("Hace comando Obtiene planes");
            ConError = false;
            while (!ConError)
            {
                comando = mtx.GetTablaPlanes(i);
                Comunicar(comando);
                if (!ConError)
                {
                    RtaTablaPlan mrta = (RtaTablaPlan)rtaBase;
                    AccesoDb.AgregarPlan(mrta);
                }
                i++;
            }
        }

        public RtaTrans ObtenerUltimaTrans()
        {
            RtaTrans mrta = null;
            string comando = mtx.GetUltimaOperacion();
            EscribeLog("Hace Comando Obtiene ultima operacion");
            Comunicar(comando);
            //hasta aqui no hay error en la comunicacion
            if (!ConError)
            {
                mrta = (RtaTrans)rtaBase;
            }
            return mrta;
        }

        public void ObtenerUltCierre()
        {
            string comando = "";
            int i = 0;
            ConError = false;
            MensajeError = "";
            EscribeLog("Hace comando Obtener ultimo Cierre");
            while (!ConError)
            {
                comando = mtx.GetUltCierre(i);
                Comunicar(comando);
                if (!ConError)
                {
                    RtaUltCierre mrta = (RtaUltCierre)rtaBase;
                    //AccesoDb.AgregarTarjeta(mrta);
                }
                i++;
            }
        }

        public void ReimpreUltTrans()
        {
            string comando = mtx.ReimpreUltTrans();
            EscribeLog("Hace comando Reimprime ultima transaccion");
            Comunicar(comando);
        }

        public void ReimpreUltCierre()
        {
            string comando = mtx.ReimpreUltCierre();
            EscribeLog("Hace comando Reimprime ultimo Cierre");
            Comunicar(comando);
        }

        public void Dispose()
        {
            Dispose(true);

            // Use SupressFinalize in case a subclass 
            // of this type implements a finalizer.
            GC.SuppressFinalize(this);
        }
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // Clear all property values that maybe have been set
                // when the class was instantiated
                sp = null;
                mtx = null;
                rtaBase = null;
                tmrSend = null;
                //tmrAuto = null;
            }
            // Indicate that the instance has been disposed.            
        }

        private string ToHexa(string texto)
        {
            StringBuilder cadena = new StringBuilder();
            int k = 0;
            foreach (char letra in texto)
            {
                // Get the integral value of the character.
                cadena.Append(string.Format("{0:X2}", Convert.ToInt32(letra)));
                if (k < (texto.Length - 1))
                    cadena.Append(" ");
                k++;
            }
            return cadena.ToString();
        }

        private string ToAscii(string texto)
        {
            StringBuilder cadena = new StringBuilder();
            foreach (char letra in texto)
            {
                // Get the integral value of the character.
                int value = Convert.ToInt32(letra);
                // Convert the decimal value to a hexadecimal value in string form.
                if (aequiv.ContainsKey(value))
                {
                    string unc = "";
                    aequiv.TryGetValue(value, out unc);
                    cadena.Append(unc);
                }
                else
                    cadena.Append(letra);
            }
            return cadena.ToString();
        }

        private void EscribeLog(string line, string sentido)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogPrisma.txt");
            writer.WriteLine(String.Format("{0}= {1}: '{2}'", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), sentido, ToHexa(line)));
            writer.WriteLine(String.Format("{0}= {1}: '{2}'", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), sentido, ToAscii(line)));
            writer.Close();
        }

        private void EscribeLog(string titulo)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogPrisma.txt");
            writer.WriteLine("-------------------------------------------------------------------");
            writer.WriteLine(String.Format("{0}= {1}", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), titulo.ToUpper()));
            writer.Close();
        }
    }
}

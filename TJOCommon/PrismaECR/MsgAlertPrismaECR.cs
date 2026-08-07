using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Threading;

namespace Alberdi.TJOCommon.PrismaECR
{
    public delegate void ActualizaMsgDelegate(string tit1, string tit2);

    public delegate void WaitCierre();


    public partial class MsgAlertPrismaECR : Form
    {
        ActualizaMsgDelegate actualizaMsg;

        //WaitVenta waitFunc;
        WaitCierre waitUnCierre;

        PosPaymentRequest peticion;

        private const int MAX_INTENTOS = 60;//NUMERO MAXIMO DE INTENTOS ANTES DE DEJAR DE REINTENTAR

        public PosPaymentResponse RespuestaVenta = null;
        public PosAnulacionResponse RespuestaAnulacion = null;
        public PosCierreResponse RespuestaCierre = null;
        public bool ConError;
        public string MensajeError;
        bool SePuedeCancelar;
        bool TerminoHilo;

        bool enPago;
        bool enAnulacion;
        bool enCierre;

        bool salirProcesando;

        OperaPrismaECR Realizar = new OperaPrismaECR();

        public MsgAlertPrismaECR()
        {
            InitializeComponent();
            this.KeyPreview = true; // FIX: necesario para que PreviewKeyDown reciba Escape
            ConError = false;
            MensajeError = "";
            SePuedeCancelar = false;
            lblCancelar.Visible = false;
            TerminoHilo = false;

            salirProcesando = true;
            peticion = null;
            enPago = enAnulacion = enCierre = false;
        }

        public MsgAlertPrismaECR(PosPaymentRequest request)
        {
            InitializeComponent();
            this.KeyPreview = true; // FIX: necesario para que PreviewKeyDown reciba Escape
            ConError = false;
            MensajeError = "";
            SePuedeCancelar = false;
            lblCancelar.Visible = false;
            TerminoHilo = false;

            salirProcesando = true;
            peticion = request;
            enPago = enAnulacion = enCierre = false;
        }

        private void ActualizaTitulos(string tit1, string tit2)
        {
            if (InvokeRequired)
            {
                Invoke(new ActualizaMsgDelegate(ActualizaTitulos), new string[] { tit1, tit2 });
            }
            else
            {
                label1.Text = tit1;
                label2.Text = tit2;
            }
        }

        public void ShowMsgProcesaVenta(string tit1, string tit2)
        {
            label1.Text = tit1;
            label2.Text = tit2;

            enPago = enAnulacion = enCierre = false;

            this.Refresh();

            lblCancelar.Visible = false;

            tmrSePuedeCancelar.Enabled = true;
            tmrControlCierre.Enabled = true;

            ConError = false;
            MensajeError = "";

            RespuestaVenta = null;
            salirProcesando = false;
            Thread hilo = new System.Threading.Thread(() =>
            {
                try
                {
                    this.RespuestaVenta = Realizar.CreaIntencionPago(peticion);
                    //La Intencion Fue Creada
                    if (RespuestaVenta != null && RespuestaVenta.Estado == "PENDIENTE")
                    {
                        enPago = true;
                        int itera = 0;
                        TerminoHilo = false;
                        salirProcesando = false;
                        while (true && !salirProcesando)
                        {
                            PosPaymentResponse rtaCons = Realizar.ConsultarStatusPago(RespuestaVenta.IdOperacionPayway);
                            if (Realizar.ConError)
                            {
                                this.ConError = true;
                                MensajeError = Realizar.MensajeError;
                                salirProcesando = TerminoHilo = true;
                                break;
                            }
                            if (rtaCons != null)
                            {

                                this.RespuestaVenta = rtaCons;
                                if (rtaCons.Estado == "APROBADA")
                                {
                                    ActualizaTitulos("Operacion APROBADA", "");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                                else if (rtaCons.Estado == "PENDIENTE")
                                    ActualizaTitulos("Operacion FALTA APROBAR", "Ver la pantalla de Newland");
                                else if (rtaCons.Estado == "RECHAZADA" || rtaCons.Estado == "ANULADA")
                                {
                                    ActualizaTitulos("FALLO APROBACION", "");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                            }
                            if (TerminoHilo)
                                salirProcesando = true;
                            itera++;
                            ActualizaTitulos("Operacion NO Apobada Todavia", "Consultando Operacion, numero consulta = " + itera.ToString());
                            rtaCons = null;
                            //Con 60 iteraciones son aprox 2 minutos que no recibe respuesta y se cancela por timeout
                            if (itera == MAX_INTENTOS)
                            {
                                this.ConError = true;
                                this.MensajeError = "Tiempo excedido para procesar la operacion.";
                                ActualizaTitulos("Tiempo excedido..", "CANCELANDO intencion Pago");
                                Realizar.CancelarIntencionPago(RespuestaVenta.IdOperacionPayway);
                                TerminoHilo = salirProcesando = true;
                                break;
                            }
                            Thread.Sleep(2000);
                        }
                    }
                    else
                    {
                        this.ConError = true;
                        this.MensajeError = "No se pudo crear Intencion Pago " + Realizar.MensajeError;
                        TerminoHilo = salirProcesando = true;
                    }
                }
                catch (Exception ex)
                {
                    this.ConError = true;
                    this.MensajeError = ex.Message;
                    Realizar.EscribeLog("Error: " + ex.Message);
                    Realizar.EscribeLog("Error: " + ex.StackTrace);
                    //MessageBox.Show("Ocurrio el error: " + ex.Message);
                }

            });
            hilo.Start();
            this.ShowDialog();
            hilo.Join();
            tmrFin.Enabled = true;
        }

        public void ShowMsgProcesaAnulacion(string tit1, string tit2, string claveBusq)
        {
            label1.Text = tit1;
            label2.Text = tit2;

            enPago = enAnulacion = enCierre = false;

            this.Refresh();

            lblCancelar.Visible = false;

            tmrSePuedeCancelar.Enabled = true;
            tmrControlCierre.Enabled = true;

            ConError = false;
            MensajeError = "";

            RespuestaAnulacion = null;
            salirProcesando = false;
            Thread hilo = new System.Threading.Thread(() =>
            {
                try
                {
                    PosAnulacionResponse anulCreado = Realizar.CreaIntencionAnula(claveBusq);
                    //La Orden Fue Creada
                    if (anulCreado != null && anulCreado.Estado == "ANULACION_PENDIENTE")
                    {
                        enAnulacion = true;
                        int itera = 0;
                        TerminoHilo = false;
                        salirProcesando = false;
                        while (true && !salirProcesando)
                        {
                            PosAnulacionResponse rtaCons = Realizar.ConsultarStatusAnulacion(anulCreado.IdOperacionPayway);
                            if (Realizar.ConError)
                            {
                                this.ConError = true;
                                MensajeError = Realizar.MensajeError;
                                salirProcesando = TerminoHilo = true;
                                break;
                            }
                            if (rtaCons != null)
                            {
                                this.RespuestaAnulacion = rtaCons;
                                if (rtaCons.Estado == "ANULADA")
                                {
                                    ActualizaTitulos("Anulación APROBADA", "");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                                else if (rtaCons.Estado == "PENDIENTE")
                                    ActualizaTitulos("Anulación FALTA APROBAR", "Ver la pantalla de Newland");
                                else if (rtaCons.Estado == "RECHAZADA")
                                {
                                    ActualizaTitulos("FALLO ANULACION", "");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                            }
                            if (TerminoHilo)
                                salirProcesando = true;
                            itera++;
                            ActualizaTitulos("Operacion NO Anulada Todavia", "Consultando Anulación, numero consulta = " + itera.ToString());
                            rtaCons = null;
                            //Agrego validacion para que salga a los 2 minutos aprox
                            if (itera == MAX_INTENTOS)
                            {
                                this.ConError = true;
                                this.MensajeError = "Tiempo excedido para procesar la Anulación.";
                                Realizar.CancelarIntencionAnulacion(RespuestaAnulacion.IdOperacionPayway);
                                TerminoHilo = salirProcesando = true;
                                break;
                            }
                            Thread.Sleep(2000);
                        }
                    }
                    else
                    {
                        this.ConError = true;
                        this.MensajeError = Realizar.MensajeError;
                        TerminoHilo = salirProcesando = true;
                    }
                }
                catch (Exception ex)
                {
                    this.ConError = true;
                    this.MensajeError = ex.Message;
                    Realizar.EscribeLog("Error: " + ex.Message);
                    Realizar.EscribeLog("Error: " + ex.StackTrace);
                    //MessageBox.Show("Ocurrio el error: " + ex.Message);
                }

            });
            hilo.Start();
            this.ShowDialog();
            hilo.Join();
            tmrFin.Enabled = true;
        }

        public void ShowMsgProcesaCierre(string tit1, string tit2, int numSucursal, int numCaja)
        {
            label1.Text = tit1;
            label2.Text = tit2;

            enPago = enAnulacion = enCierre = false;

            this.Refresh();

            lblCancelar.Visible = false;

            tmrSePuedeCancelar.Enabled = true;
            tmrControlCierre.Enabled = true;

            ConError = false;
            MensajeError = "";

            RespuestaCierre = null;
            salirProcesando = false;
            Thread hilo = new System.Threading.Thread(() =>
            {
                try
                {
                    PosCierreResponse anulCreado = Realizar.CreaIntencionCierre(numSucursal, numCaja);
                    //La Intencion Fue Creada
                    if (anulCreado != null && anulCreado.SettlementId != null)
                    {
                        enCierre = true;
                        int itera = 0;
                        TerminoHilo = false;
                        salirProcesando = false;
                        while (true && !salirProcesando)
                        {
                            PosCierreResponse rtaCons = Realizar.ConsultarStatusCierre(anulCreado.SettlementId);
                            if (Realizar.ConError)
                            {
                                this.ConError = true;
                                MensajeError = Realizar.MensajeError;
                                salirProcesando = TerminoHilo = true;
                                break;
                            }
                            if (rtaCons != null)
                            {
                                this.RespuestaCierre = rtaCons;
                                if (rtaCons.Estado == "CONFIRMADO")
                                {
                                    ActualizaTitulos("Cierre FINALIZADO", "");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                                //else if (rtaCons.Estado == "PENDIENTE")
                                //    ActualizaTitulos("Anulación FALTA APROBAR", "Ver la pantalla de Newland");
                                else if (rtaCons.Estado == "RECHAZADO" || rtaCons.Estado == "ERROR")
                                {
                                    ActualizaTitulos("FALLO ANULACION", "");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                            }
                            if (TerminoHilo)
                                salirProcesando = true;
                            itera++;
                            ActualizaTitulos("Cierre de lotes NO terminado Todavia", "Consultando Cierre, numero consulta = " + itera.ToString());
                            rtaCons = null;
                            //Agrego validacion para al minuto salga de intentar con el webhook y consulte en MercadoPago
                            if (itera == MAX_INTENTOS)
                            {
                                this.ConError = true;
                                this.MensajeError = "Tiempo excedido para procesar Cierre de Lotes.";
                                TerminoHilo = salirProcesando = true;
                                break;
                            }
                            Thread.Sleep(2000);
                        }
                    }
                    else
                    {
                        this.ConError = true;
                        this.MensajeError = Realizar.MensajeError;
                        TerminoHilo = salirProcesando = true;
                    }
                }
                catch (Exception ex)
                {
                    this.ConError = true;
                    this.MensajeError = ex.Message;
                    Realizar.EscribeLog("Error: " + ex.Message);
                    Realizar.EscribeLog("Error: " + ex.StackTrace);
                    //MessageBox.Show("Ocurrio el error: " + ex.Message);
                }

            });
            hilo.Start();
            this.ShowDialog();
            hilo.Join();
            tmrFin.Enabled = true;
        }

        private void MsgAlertPrismaECR_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            // FIX: KeyPreview = true en el constructor es requisito para que este evento funcione.
            if (e.KeyData == Keys.Escape && SePuedeCancelar && !salirProcesando)
            {
                this.ConError = false;
                SePuedeCancelar = false; // Evita doble disparo si el usuario presiona Escape dos veces

                // FIX: La cancelacion se mueve a un hilo de fondo para no bloquear el UI thread.
                // Llamar a CancelarIntencionPago/Anulacion directamente en el UI thread congela
                // el formulario durante la llamada de red.
                Thread hiloCancelacion = new System.Threading.Thread(() =>
                {
                    try
                    {
                        if (enPago)
                        {
                            if (this.RespuestaVenta != null && RespuestaVenta.Estado == "PENDIENTE")
                            {
                                // Senalamos al loop principal que debe detenerse
                                salirProcesando = TerminoHilo = true;
                                ActualizaTitulos("CANCELANDO ORDEN", "CREADA " + RespuestaVenta.IdOperacionPayway);
                                PosPaymentResponse rtaCons = Realizar.CancelarIntencionPago(this.RespuestaVenta.IdOperacionPayway);
                                if (Realizar.ConError)
                                {
                                    this.ConError = true;
                                    this.MensajeError = Realizar.MensajeError;
                                    // FIX: NO resetear salirProcesando/TerminoHilo a false aqui.
                                    // Hacerlo reactivaba el loop del hilo principal, causando comportamiento
                                    // impredecible. El hilo ya fue senalado a detenerse; dejarlo terminar.
                                    ActualizaTitulos("No se pudo cancelar la Intencion", "");
                                }
                                else
                                {
                                    ActualizaTitulos("Finalizando Cancelar Orden", "");
                                    enPago = false;
                                }
                            }
                        }
                        else if (enAnulacion)
                        {
                            if (this.RespuestaAnulacion != null && RespuestaAnulacion.Estado == "ANULACION_PENDIENTE")
                            {
                                salirProcesando = TerminoHilo = true;
                                ActualizaTitulos("Anulando Pago", "Aprobado " + RespuestaAnulacion.IdOperacionPayway);

                                PosAnulacionResponse rtaCons = Realizar.CancelarIntencionAnulacion(this.RespuestaAnulacion.IdOperacionPayway);
                                if (Realizar.ConError)
                                {
                                    this.ConError = true;
                                    this.MensajeError = Realizar.MensajeError;
                                    // FIX: idem que el caso de pago: no resetear las flags.
                                    ActualizaTitulos("No se pudo cancelar la Intencion", "");
                                }
                                else
                                {
                                    ActualizaTitulos("Finalizando Cancelar Orden", "");
                                    enAnulacion = false;
                                }
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        this.ConError = true;
                        this.MensajeError = ex.Message;
                        Realizar.EscribeLog("Error al cancelar con Escape: " + ex.Message);
                        // FIX: MessageBox desde un hilo de fondo puede causar problemas en WinForms.
                        // Se usa Invoke para mostrarlo en el UI thread de forma segura.
                        if (InvokeRequired)
                            Invoke(new Action(() => MessageBox.Show("Ocurrio el error: " + ex.Message)));
                        else
                            MessageBox.Show("Ocurrio el error: " + ex.Message);
                    }
                });
                hiloCancelacion.IsBackground = true;
                hiloCancelacion.Start();
            }
        }

        private void tmrFin_Tick(object sender, EventArgs e)
        {
            tmrFin.Enabled = false;
            this.Close();
        }

        private void tmrSePuedeCancelar_Tick(object sender, EventArgs e)
        {
            tmrSePuedeCancelar.Enabled = false;
            SePuedeCancelar = true;
            lblCancelar.Visible = true;
        }

        private void tmrControlCierre_Tick(object sender, EventArgs e)
        {
            if (TerminoHilo && salirProcesando)
            {
                tmrControlCierre.Enabled = false;
                tmrFin.Enabled = true;
            }
        }

    }
}
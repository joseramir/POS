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
        // Pago que no se pudo cancelar: se sigue hasta 5 minutos desde la solicitud. Los cajeros
        // reinician la caja si la espera se alarga, y en produccion Payway confirmo estos casos
        // entre 65 y 287 s (99 % de las ventas antes de 91 s). El Gateway sigue la operacion
        // hasta 10 minutos: si se aprueba despues, queda APROBADA alla.
        private const int SEG_MAX_SEGUIMIENTO_PAGO = 300;
        // Errores seguidos de la consulta de estado del pago antes de abandonar
        private const int MAX_ERRORES_CONSULTA_PAGO = 5;

        // ESC durante un pago: lo atiende el hilo de consulta (cancela y, si no se puede, sigue consultando)
        volatile bool pedidoCancelarPago = false;
        AutoResetEvent despertarPago = new AutoResetEvent(false);

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

        private void MarcaSeguimiento()
        {
            if (InvokeRequired)
            {
                Invoke(new Action(MarcaSeguimiento));
            }
            else
            {
                tmrSePuedeCancelar.Enabled = false;
                SePuedeCancelar = false;
                lblCancelar.Visible = false;
                label1.ForeColor = Color.Red;
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
            pedidoCancelarPago = false;
            despertarPago.Reset();
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
                        int erroresSeguidos = 0;
                        // En seguimiento: no se pudo cancelar (el cliente ya opera la terminal) y se
                        // espera el resultado final. Hasta el 01/11/2026 la caja abandonaba aca y el
                        // cajero reintentaba: el pago anterior se aprobaba igual y quedaba sin venta.
                        bool enSeguimiento = false;
                        string idOperacion = RespuestaVenta.IdOperacionPayway;
                        DateTime inicio = DateTime.Now;
                        TerminoHilo = false;
                        salirProcesando = false;
                        while (true && !salirProcesando)
                        {
                            PosPaymentResponse rtaCons = Realizar.ConsultarStatusPago(idOperacion);
                            if (Realizar.ConError)
                            {
                                // Un corte de red no deja la caja libre para cobrar de nuevo mientras
                                // el pago puede seguir en curso: se toleran varios errores seguidos
                                erroresSeguidos++;
                                Realizar.EscribeLog("ConsultarStatusPago con error (" + erroresSeguidos.ToString() + " seguidos): " + Realizar.MensajeError, "CAJA");
                                if (erroresSeguidos >= MAX_ERRORES_CONSULTA_PAGO)
                                {
                                    this.ConError = true;
                                    MensajeError = Realizar.MensajeError + "\nNo se pudo confirmar el resultado del pago. Verificar en la terminal antes de volver a cobrar.";
                                    salirProcesando = TerminoHilo = true;
                                    break;
                                }
                            }
                            else
                                erroresSeguidos = 0;
                            if (rtaCons != null)
                            {

                                this.RespuestaVenta = rtaCons;
                                if (rtaCons.Estado == "APROBADA")
                                {
                                    // Tambien si se habia pedido cancelar: el cliente fue cobrado, la venta se registra
                                    if (enSeguimiento)
                                        Realizar.EscribeLog("Pago " + idOperacion + " APROBADO despues de un pedido de cancelacion que no se pudo hacer", "CAJA");
                                    this.ConError = false;
                                    this.MensajeError = "";
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
                                else if (rtaCons.Estado == "ERROR")
                                {
                                    // PrismaECR no pudo confirmar el resultado con Payway: no se sabe si cobro
                                    ActualizaTitulos("RESULTADO DESCONOCIDO", "Verificar antes de volver a cobrar");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                            }
                            itera++;
                            if (enSeguimiento)
                            {
                                int quedan = SEG_MAX_SEGUIMIENTO_PAGO - (int)(DateTime.Now - inicio).TotalSeconds;
                                ActualizaTitulos("NO REINICIE LA CAJA", "Esperando la terminal, quedan " + Math.Max(quedan, 0).ToString() + " s");
                                if ((DateTime.Now - inicio).TotalSeconds >= SEG_MAX_SEGUIMIENTO_PAGO)
                                {
                                    this.ConError = true;
                                    this.MensajeError = "No se pudo confirmar el resultado del pago (" + idOperacion + ").\nVerificar en la terminal antes de volver a cobrar.";
                                    Realizar.EscribeLog("Pago " + idOperacion + " sin estado final despues de " + SEG_MAX_SEGUIMIENTO_PAGO.ToString() + " s", "CAJA");
                                    ActualizaTitulos("RESULTADO DESCONOCIDO", "Verificar antes de volver a cobrar");
                                    TerminoHilo = salirProcesando = true;
                                    break;
                                }
                            }
                            else
                            {
                                ActualizaTitulos("Operacion NO Apobada Todavia", "Consultando Operacion, numero consulta = " + itera.ToString());
                                // Con 60 iteraciones son aprox 2 minutos (o ESC del cajero): se intenta cancelar
                                bool porTiempo = (itera >= MAX_INTENTOS);
                                if (porTiempo || pedidoCancelarPago)
                                {
                                    ActualizaTitulos(porTiempo ? "Tiempo excedido.." : "CANCELANDO ORDEN", "CANCELANDO intencion Pago " + idOperacion);
                                    Realizar.CancelarIntencionPago(idOperacion);
                                    if (!Realizar.ConError)
                                    {
                                        // Cancelada: el cliente no llego a operar la terminal
                                        this.ConError = porTiempo;
                                        this.MensajeError = porTiempo ? "Tiempo excedido para procesar la operacion." : "";
                                        ActualizaTitulos("Finalizando Cancelar Orden", "");
                                        TerminoHilo = salirProcesando = true;
                                        break;
                                    }
                                    // No se pudo cancelar (409 del Gateway, o error): el pago puede
                                    // aprobarse igual, se sigue consultando hasta el estado final
                                    Realizar.EscribeLog("No se pudo cancelar el pago " + idOperacion + " (" + (porTiempo ? "tiempo excedido" : "ESC") + "), se sigue consultando: " + Realizar.MensajeError, "CAJA");
                                    enSeguimiento = true;
                                    itera = 0;
                                    // En seguimiento ESC no hace nada: se saca el "ESC para cancelar"
                                    // y el aviso de no reiniciar va en rojo
                                    MarcaSeguimiento();
                                }
                            }
                            rtaCons = null;
                            despertarPago.WaitOne(2000, false);
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
                                else if (rtaCons.Estado == "ERROR")
                                {
                                    // PrismaECR no pudo confirmar el resultado con Payway: no se sabe si anulo
                                    ActualizaTitulos("RESULTADO DESCONOCIDO", "Verificar antes de reintentar");
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

                if (enPago)
                {
                    // El hilo de consulta hace la cancelacion: si Payway no la acepta (el cliente ya
                    // opera la terminal) sigue esperando el resultado en vez de dejar la caja libre
                    // para cobrar de nuevo.
                    if (this.RespuestaVenta != null && RespuestaVenta.Estado == "PENDIENTE")
                    {
                        ActualizaTitulos("CANCELANDO ORDEN", "CREADA " + RespuestaVenta.IdOperacionPayway);
                        pedidoCancelarPago = true;
                        despertarPago.Set();
                    }
                    return;
                }

                // FIX: La cancelacion se mueve a un hilo de fondo para no bloquear el UI thread.
                // Llamar a CancelarIntencionPago/Anulacion directamente en el UI thread congela
                // el formulario durante la llamada de red.
                Thread hiloCancelacion = new System.Threading.Thread(() =>
                {
                    try
                    {
                        if (enAnulacion)
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
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;


using System.Threading;
using Newtonsoft.Json;
using RestSharp;
using System.Configuration;


namespace Alberdi.TJOCommon.ComunicaMercadoPago
{
    public partial class MsgCashoutMp : Form
    {
        ActualizaMsgDelegate actualizaMsg;

        //WaitVenta waitFunc;
        WaitCierre waitUnCierre;

        OrdenCashout peticion;

        public RtaOrdenCaja RespuestaVenta = null;
        public bool ConError;
        public string MensajeError;
        bool SePuedeCancelar;
        bool TerminoHilo;

        bool salirProcesando;

        OperaQrCashout Realizar = new OperaQrCashout();

        public MsgCashoutMp()
        {
            InitializeComponent();
            ConError = false;
            MensajeError = "";
            SePuedeCancelar = false;
            lblCancelar.Visible = false;
            TerminoHilo = false;

            salirProcesando = true;
        }

        public MsgCashoutMp(OrdenCashout request)
        {
            InitializeComponent();
            peticion = request;

            ConError = false;
            MensajeError = "";
            SePuedeCancelar = false;
            lblCancelar.Visible = false;
            TerminoHilo = false;

            salirProcesando = true;
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
                    this.RespuestaVenta = Realizar.CrearOrdenCashout(peticion);
                    //La Orden Fue Creada
                    if (RespuestaVenta != null && RespuestaVenta.Estado == 2)
                    {
                        int itera = 0;
                        TerminoHilo = false;
                        salirProcesando = false;
                        while (true && !salirProcesando)
                        {
                            RtaOrdenCaja rtaCons = Realizar.ConsultarStatusOrden(RespuestaVenta.ExternalReference);
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
                                if (rtaCons.Estado == 3)
                                    ActualizaTitulos("Operacion APROBADA", "");
                                else if (rtaCons.Estado == 6)
                                    ActualizaTitulos("Operacion FALTA APROBAR", "Ver la pantalla de SMARTPOINT");
                                else
                                    ActualizaTitulos("FALLO APROBACION", "");
                                TerminoHilo = salirProcesando = true;
                                break;
                            }
                            if (TerminoHilo)
                                salirProcesando = true;
                            itera++;
                            ActualizaTitulos("Operacion NO Apobada Todavia", "Consultando Operacion, numero consulta = " + itera.ToString());
                            rtaCons = null;
                            //Agrego validacion para al minuto salga de intentar con el webhook y consulte en MercadoPago
                            if (itera == 60)
                                break;
                            Thread.Sleep(2000);
                        }
                        ////Consulta la operacion en MercadoPago
                        //if (!TerminoHilo && !salirProcesando)
                        //{
                        //    itera = 0;
                        //    while (true && !salirProcesando)
                        //    {
                        //        RtaOrdenCaja rtaCons = Realizar.ConsultarStatusOrdenMercadoPago(RespuestaVenta.ExternalReference);
                        //        if (Realizar.ConError)
                        //        {
                        //            this.ConError = true;
                        //            MensajeError = Realizar.MensajeError;
                        //            salirProcesando = TerminoHilo = true;
                        //            break;
                        //        }
                        //        if (rtaCons != null)
                        //        {

                        //            this.RespuestaVenta = rtaCons;
                        //            if (rtaCons.Estado == 3)
                        //                ActualizaTitulos("Operacion APROBADA", "");
                        //            else
                        //                ActualizaTitulos("FALLO APROBACION", "");
                        //            TerminoHilo = salirProcesando = true;
                        //            break;
                        //        }
                        //        if (TerminoHilo)
                        //            salirProcesando = true;
                        //        itera++;
                        //        ActualizaTitulos("Operacion NO Apobada Todavia", "Consultando Operacion MercadoPago, numero consulta = " + itera.ToString());
                        //        rtaCons = null;
                        //        //Agrego validacion para al minuto salga de intentar con el webhook y consulte en MercadoPago
                        //        if (itera == 120)
                        //            break;
                        //        Thread.Sleep(2000);
                        //    }
                        //}
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


        private void MsgCashoutMp_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyData == Keys.Escape && SePuedeCancelar && !salirProcesando)
            {
                this.ConError = false;
                //Cancela la orden creada
                //Thread hilo = new System.Threading.Thread(() =>
                //{
                try
                {
                    if (this.RespuestaVenta != null && this.RespuestaVenta.Estado == 2)
                    {
                        salirProcesando = TerminoHilo = true;
                        ActualizaTitulos("CANCELANDO ORDEN", "CREADA " + RespuestaVenta.IdOrdenMercadoPago);
                        RtaOrdenCaja rtaCons = Realizar.CancelarOrden(this.RespuestaVenta);
                        if (Realizar.ConError)
                        {
                            this.ConError = true;
                            this.MensajeError = Realizar.MensajeError;
                        }
                        ActualizaTitulos("Finalizando Cancelar Orden", "");
                        //if (rtaCons != null && rtaCons.Estado != 5)
                        //{
                        //    this.ConError = true;
                        //    this.MensajeError = Realizar.MensajeError;
                        //}
                    }

                }
                catch (Exception ex)
                {
                    this.ConError = true;
                    this.MensajeError = ex.Message;
                    MessageBox.Show("Ocurrio el error: " + ex.Message);
                }

                //});
                //hilo.Start();
                //this.ShowDialog();
                //hilo.Join();
                //tmrFin.Enabled = true;                
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

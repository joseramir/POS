using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Threading;
using Alberdi.TJOCommon.Prisma;

namespace Alberdi.TJOCommon.Prisma
{
    public delegate void ActualizaMsgDelegate(string tit1, string tit2);

    //System.Action<TrxPrisma, RtaVenta> waitFunc;
    public delegate RtaVenta WaitVenta(TrxPrisma transx);
    public delegate void WaitCierre();

    public partial class MsgAlertPrisma : Form
    {        
        //public bool Cancelado;

        ActualizaMsgDelegate actualizaMsg;

        WaitVenta waitFunc;
        WaitCierre waitUnCierre;

        TrxPrisma loTrx;
        
        OperarTrxPrisma hacer;

        public RtaVenta RespuestaVenta = null;
        public bool ConError;
        public string MensajeError;
        bool SePuedeCancelar;
        bool TerminoHilo;

        //public MsgAlertPrisma(OperarTrxPrisma popera)
        public MsgAlertPrisma(TrxPrisma paramTrx, int COM_NEWLAND)
        {
            InitializeComponent();
            
            //Cancelado = false;
            loTrx = paramTrx;            
            hacer = new OperarTrxPrisma(COM_NEWLAND);
            ConError = false;
            MensajeError = "";
            SePuedeCancelar = false;
            lblCancelar.Visible = false;
            TerminoHilo = false;
        }

        public MsgAlertPrisma(int COM_NEWLAND)
        {
            InitializeComponent();

            //Cancelado = false;
            loTrx = null;
            hacer = new OperarTrxPrisma(COM_NEWLAND);
            ConError = false;
            MensajeError = "";
            SePuedeCancelar = false;
            lblCancelar.Visible = false;
            TerminoHilo = false;
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
            Thread hilo = new System.Threading.Thread(() =>
            {                
                if (loTrx.CodigoTarjeta != "QRQ")
                {
                    if (loTrx.MontoExtraCash > 0)
                        waitFunc = hacer.HacerVentaExtraCash;
                    else
                        waitFunc = hacer.HacerVenta;
                }
                else
                    waitFunc = hacer.HacerVentaQrModo;
                IAsyncResult iar = waitFunc.BeginInvoke(loTrx, null, null);
                while (!iar.IsCompleted && !hacer.Cancelado)
                    Thread.Sleep(1000);

                ConError = hacer.ConError;
                MensajeError = hacer.MensajeError;

                if (!hacer.Cancelado)
                {
                    RespuestaVenta = waitFunc.EndInvoke(iar);
                    TerminoHilo = true;
                }

            });
            hilo.Start();
            this.ShowDialog();
            hilo.Join();
            
        }

        public void ShowMsgHacerCierreLotes(string tit1, string tit2)
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
            Thread hilo = new System.Threading.Thread(() =>
            {
                waitUnCierre = hacer.HacerCierre;

                IAsyncResult iar = waitUnCierre.BeginInvoke(null, null);
                while (!iar.IsCompleted && !hacer.Cancelado)
                    Thread.Sleep(1000);

                ConError = hacer.ConError;
                MensajeError = hacer.MensajeError;

                if (!hacer.Cancelado)
                {
                    waitUnCierre.EndInvoke(iar);
                    TerminoHilo = true;
                }

            });
            hilo.Start();
            this.ShowDialog();
            hilo.Join();

        }

        private void MsgAlertPrisma_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyData == Keys.Escape && SePuedeCancelar)
            {
                //Cancelado = true;
                if (hacer != null)
                    hacer.Cancelado = true;
                tmrFin.Enabled = true;
                //lopera.Cancelado = true;
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
            if (TerminoHilo)
            {
                tmrControlCierre.Enabled = false;
                tmrFin.Enabled = true;
            }
        }

    }
}

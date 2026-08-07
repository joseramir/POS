using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Configuration;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Web.Script.Serialization;

using System.Threading;

namespace Controles
{
    public delegate void ActualizaMsgDelegate(string tit1, string tit2);

    public partial class MsgAlert : Form
    {
        bool TerminoEjecucion;
        bool Cancelado;
        public int Intentos { get; set; }

        public PeticionCaja mipet { get; set; }
        
        ActualizaMsgDelegate actualizaMsg;

        public MsgAlert()
        {
            InitializeComponent();
            TerminoEjecucion = false;
            Cancelado = false;
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
        public void ShowMsgCrea(string tit1, string tit2)
        {
            label1.Text = tit1;
            label2.Text = tit2;
            this.Refresh();
            TerminoEjecucion = false;

            
            Thread my = new Thread(ProcesarMp);
            my.Start();
            tmrPasa.Enabled = true;
            this.ShowDialog();            
        }

        private void MsgAlert_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyData == Keys.Escape)
            {
                Cancelado = true;
            }
        }

        private void ProcesarMp()
        {
            TerminoEjecucion = false;

            MpCrearOrden();
            if (!Cancelado)
            {
                if (mipet.estado == 2)
                {
                    int primerosIntentos = 3;//Consulta a sitio web
                    int i = 0;
                    ActualizaTitulos(string.Format("MercadoPago: {0}", mipet.idoper), "Orden Creada el Cliente puede pagar");
                    while (i < primerosIntentos)
                    {
                        MpConsultaOrden();
                        if (mipet.estado == 7)
                            break;
                        else
                        {
                            if (Cancelado)
                                break;
                        }
                        i++;
                    }
                    if (!Cancelado)
                    {
                        if (mipet.estado < 7)
                        {
                            i = 0;
                            //intentos = 60;
                            ActualizaTitulos(string.Format("MercadoPago: {0}", mipet.idoper), "Orden Creada el Cliente puede pagar");
                            while (i < Intentos)
                            {
                                MpConsultaPagoAMp();
                                if (mipet.estado == 7)
                                    break;
                                else
                                    if (Cancelado)
                                        break;
                                i++;
                            }
                        }
                    }
                }
                else
                    ActualizaTitulos("No se pudo crear orden", "Verificar Servidor");
            }
            if (Cancelado && ((mipet.estado > 1 && mipet.estado < 7) || mipet.estado == 9))
            {
                ActualizaTitulos("Cancelando Orden", "");
                MpCancelarOrden();
            }
            TerminoEjecucion = true;
            //tmrFin.Enabled = true;
        }



        public void MpCrearOrden()
        {            
            try
            {
                if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 15))
                {                    
                    JavaScriptSerializer ser = new JavaScriptSerializer();
                    string salida = ser.Serialize(mipet);

                    TcpClient cliente = new TcpClient();
                    var rescli = cliente.BeginConnect(ConfigurationManager.AppSettings["ipgatwaymp"], int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]), null, null);
                    rescli.AsyncWaitHandle.WaitOne(TimeSpan.FromSeconds(10));
                    if (cliente.Connected)
                    {
                        //cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
                        NetworkStream output = cliente.GetStream();
                        BinaryWriter writer = new BinaryWriter(output);
                        BinaryReader reader = new BinaryReader(output);
                        output.Flush();
                        writer.Flush();                        
                        writer.Write(salida);
                        string rta = reader.ReadString();
                        if (rta == "OK")
                        {
                            mipet.seq = reader.ReadInt32();
                            mipet.estado = reader.ReadInt32();
                            mipet.idmp = reader.ReadString();
                            mipet.idoper = reader.ReadString();
                        }
                        reader.Close();
                        writer.Close();
                        output.Close();
                    }
                    //cliente.Close();
                    cliente.EndConnect(rescli);
                }
                

            }
            catch (System.Exception ex)
            {
                bool cual = false;
                Loging.EscribeExcepcion("MpCOrden", ex);
            }            
        }

        public void MpConsultaOrden()
        {
            System.Threading.Thread.Sleep(5000);
            try
            {
                if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 15))
                {
                    mipet.estado = 3;

                    JavaScriptSerializer ser = new JavaScriptSerializer();
                    string salida = ser.Serialize(mipet);

                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(salida);
                    string rta = reader.ReadString();
                    if (rta == "OK")
                    {
                        mipet.estado = reader.ReadInt32();
                        mipet.payment_id = reader.ReadInt64();
                    }
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }

            }
            catch (System.Exception ex)
            {
                Loging.EscribeExcepcion("MpConsultaOrden", ex);
            }                        
        }

        public void MpConsultaPagoAMp()
        {
            System.Threading.Thread.Sleep(5000);
            try
            {
                if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 15))
                {
                    mipet.estado = 9;

                    JavaScriptSerializer ser = new JavaScriptSerializer();
                    string salida = ser.Serialize(mipet);

                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(salida);
                    string rta = reader.ReadString();
                    if (rta == "OK")
                    {
                        mipet.estado = reader.ReadInt32();
                        mipet.payment_id = reader.ReadInt64();
                    }
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }

            }
            catch (System.Exception ex)
            {
                Loging.EscribeExcepcion("MpConsultaPagoAMp", ex);
            }            
        }
        
        public void MpCancelarOrden()
        {
            System.Threading.Thread.Sleep(5000);
            try
            {
                if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 15))
                {
                    mipet.estado = 13;

                    JavaScriptSerializer ser = new JavaScriptSerializer();
                    string salida = ser.Serialize(mipet);

                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(salida);
                    string rta = reader.ReadString();
                    if (rta == "OK")
                    {
                        mipet.estado = reader.ReadInt32();
                    }
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }

            }
            catch (System.Exception ex)
            {
                Loging.EscribeExcepcion("MpCancelarOrden", ex);
            }            
        }

        private void tmrFin_Tick(object sender, EventArgs e)
        {
            tmrFin.Enabled = false;
            this.Close();
        }

        private void tmrPasa_Tick(object sender, EventArgs e)
        {
            if (TerminoEjecucion)
            {
                tmrPasa.Enabled = false;
                tmrFin.Enabled = true;
            }
        }

        
    }
}

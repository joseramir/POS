using System;
using System.Net;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net.NetworkInformation;

namespace Prominente
{
    public partial class MainForm : Form
    {
        /// <summary>
        /// Poner a TRUE solamente durante la depuracion del modulo. Cuando esta en el modo de 'emular':
        /// - No se hace ping a la URL del servicio. SIEMPRE lo da por bueno.
        /// - Se activa el boton de 'probar', que nos permite generar una transaccion de prueba.
        /// - A 4 de cada 5 requerimientos, le reponde como que todo esta OK. Al restante le indica 'fallo',
        ///   todo esto de manera automatica, sin consultar al servicio de prominente.
        /// </summary>
        bool emular = true;

        /// <summary>
        /// Constructor
        /// </summary>
        public MainForm()
        {
            InitializeComponent();

            sRojo.Left = sVerde.Left = sAmarillo.Left;
            ColorSemaforo = SemColor.Amarillo;
            timer.Interval = 2000;
            timer.Enabled = true;
            timer.Start();

            if (emular) lbEmu.Visible = true;
        }

        /// <summary>
        /// Permite conocer/cambiar el color del semaforo en pantalla.
        /// </summary>
        public SemColor ColorSemaforo
        {
            get
            {
                if (sRojo.Visible) return SemColor.Rojo;
                if (sAmarillo.Visible) return SemColor.Amarillo;
                return SemColor.Verde;
            }
            set
            {
                sRojo.Visible = sVerde.Visible = sAmarillo.Visible = false;
                switch (value)
                {
                    case SemColor.Rojo:
                        sRojo.Visible = true;
                        break;

                    case SemColor.Amarillo:
                        sAmarillo.Visible = true;
                        break;

                    default:
                        sVerde.Visible = true;
                        break;
                }
                Application.DoEvents();
            }
        }

        /// <summary>
        /// Permite mostrar un mensaje en el listbox al efecto.
        /// </summary>
        /// <param name="msg"></param>
        public void Report(string msg)
        {
            int index = lb.Items.Add(msg);
            lb.SelectedIndex = index;
            Application.DoEvents();
        }

        /// <summary>
        /// Evento de pulsacion del boton 'Ping'
        /// </summary>
        /// <param name="sender">no se usa</param>
        /// <param name="e">no se usa</param>
        private void btPing_Click(object sender, EventArgs e)
        {
            ColorSemaforo = SemColor.Amarillo;
            DoPing();
        }

        /// <summary>
        /// Efectua un ping fisico a la direccion url que se le pasa
        /// </summary>
        /// <param name="uri">La direccion del server al cual se le efectuara un ping</param>
        private void PerformPing(Uri uri)
        {
            IPAddress addr = null;
            if (char.IsDigit(uri.DnsSafeHost[0]))
            {
                string[] parts = uri.DnsSafeHost.Split('.');
                byte[] bparts = new byte[4];
                bparts[0] = byte.Parse(parts[0]);
                bparts[1] = byte.Parse(parts[1]);
                bparts[2] = byte.Parse(parts[2]);
                bparts[3] = byte.Parse(parts[3]);
                addr = new IPAddress(bparts);
            }
            else
            {
                // Si se trata de el nombre de un servidor, resuelve el nombre
                var host = Dns.GetHostEntry(uri.DnsSafeHost);
                addr = host.AddressList[0];
            }

            Ping ping = new Ping();
            PingOptions options = new PingOptions();
            options.DontFragment = true;

            // Crea un buffer de 32 bytes de datos
            string data = "Ping desde BITNET - PruebaPrueba";
            byte[] buffer = Encoding.ASCII.GetBytes(data);
            int timeout = 5000;
            PingReply reply = ping.Send(addr, timeout, buffer, options);
            if (reply.Status != IPStatus.Success)
                throw new Exception("El SERVER no está en línea");
        }

        /// <summary>
        /// Hace un ping (o lo emula, si corresponde), ajustando la presentacion de la form como corresponda.
        /// </summary>
        private void DoPing()
        {
            timer.Stop();
            Report("Probando las comunicaciones...");
            btPing.Enabled = false;
            Uri uri = new Uri(Prominente.Properties.Settings.Default.URL);
            try
            {
                if (!emular)
                    PerformPing(uri);

                ColorSemaforo = SemColor.Verde;
                timer.Interval = 60000;
                Report("Comunicaciones OK");                
            }
            catch (Exception ex)
            {
                Report("Error de PING: " + ex.Message);
                ColorSemaforo = SemColor.Rojo;
                timer.Interval = 5000;
            }
            finally
            {
                btPing.Enabled = true;
                timer.Start();
            }
        }

        /// <summary>
        /// Evento de tick del timer que se usa para generar pings cada tango.
        /// </summary>
        /// <param name="sender">no se usa</param>
        /// <param name="e">no se usa</param>
        private void timer_Tick(object sender, EventArgs e)
        {
            DoPing();
        }
    }

    /// <summary>
    /// Posibles colores del semaforo.
    /// </summary>
    public enum SemColor { Rojo, Verde, Amarillo }
}

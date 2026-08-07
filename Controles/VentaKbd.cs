using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Pos.Desktop
{
    public partial class VentaKbd : UserControl
    {
        public const int ANCHO_CERRADO = 268;

        int anchoAbierto;

        public static int TiempoAbierto { get; set; }

        public bool Colapsable { get; set; }

        public VentaKbd()
        {
            InitializeComponent();

            // Por defecto, el teclado NO ES colapsable.
            this.Colapsable = (TiempoAbierto > 0);

            // Carga la definicion de los botones.
            KbdLogger.Log("Inicializando 'VentaKbd'");
            if (File.Exists("pos\\TouchKeyboard.def"))
            {
                foreach(string line in File.ReadAllLines("pos\\TouchKeyboard.def"))
                {
                    string l = line.Trim();
                    if (l.Length == 0 || l[0] == '\'') continue;

                    string[] parts = l.Split(',');
                    KbdLogger.Log(parts);

                    if (l.Length != 0 && !l.StartsWith("\'") && parts.Length == 3)
                    {
                        try
                        {
                            int k = int.Parse(parts[0]);
                            int assig = int.Parse(parts[1]);
                            string func = "k" + k.ToString();
                            foreach (Control ctl in Controls)
                            {
                                if (func == (string)ctl.Tag)
                                {
                                    ctl.Text = parts[2].Trim();
                                    ctl.Tag = assig.ToString();
                                    KbdLogger.Log(ctl);

                                    break;
                                }
                            }
                        }
                        catch(Exception ex)
                        { 
                            KbdLogger.Log(string.Format("\tERROR al ASIGNAR '{0}' ({1})", ex.Message, ex.GetType().ToString()));
                        }
                    }
                }
            }
        }

        private void button_Click(object sender, EventArgs e)
        {
            this.timer.Enabled = false;
            this.timer.Enabled = this.Colapsable && (this.Width != ANCHO_CERRADO);
            if (ClickEnBoton != null)
            {
                Button boton = (Button)sender;
                KbdLogger.Log(string.Format("Pulsacion. Name='{0}', Tag='{1}', Text='{2}'", boton.Name, boton.Tag, boton.Text));
                try
                {
                    ClickEnBoton(uint.Parse((string)boton.Tag));
                }                
                catch(Exception ex)
                { 
                    KbdLogger.Log(string.Format("\tERROR al PULSAR '{0}' ({1})", ex.Message, ex.GetType().ToString()));
                }
            }
        }

        public event BotonClick ClickEnBoton;

        private void VentaKbd_Load(object sender, EventArgs e)
        {
            if (this.Colapsable)
            {
                anchoAbierto = this.Width;
                this.Width = ANCHO_CERRADO;
                this.agrandar.Visible = true;
            }
        }

        private void agrandar_Click(object sender, EventArgs e)
        {
            this.Width = anchoAbierto;
            this.agrandar.Visible = false;
            if (TiempoAbierto > 0)
            {
                this.timer.Interval = TiempoAbierto;
                this.timer.Enabled = true;
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            this.timer.Enabled = false;
            this.Width = ANCHO_CERRADO;
            this.agrandar.Visible = true;
        }
    }
}

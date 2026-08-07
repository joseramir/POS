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
    public delegate void BotonClick(uint tecla);

    public partial class NumKbd : UserControl
    {
        Rectangle originalButton1Rect;
        Rectangle originalButton2Rect;
        Rectangle originalButton3Rect;
        float fontScale = 1.2f;
        float originalFontSize;

        public NumKbd()
        {
            InitializeComponent();
            originalButton1Rect = new Rectangle(button1.Location, button1.Size);
            originalButton2Rect = new Rectangle(button2.Location, button1.Size);
            originalButton3Rect = new Rectangle(button3.Location, button1.Size);

            originalFontSize = button1.Font.Size;

            KbdLogger.Log("Inicializando 'NumKbd'");

            if (File.Exists("pos\\TouchKeyboard.def"))
            {
                foreach (string line in File.ReadAllLines("pos\\TouchKeyboard.def"))
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
                        catch (Exception ex)
                        {
                            KbdLogger.Log(string.Format("\tERROR al ASIGNAR '{0}' ({1})", ex.Message, ex.GetType().ToString()));
                        }
                    }
                }
            }
        }

        private void button_Click(object sender, EventArgs e)
        {
            if (ClickEnBoton != null)
            {
                Button boton = (Button)sender;
                KbdLogger.Log(string.Format("Pulsacion. Name='{0}', Tag='{1}', Text='{2}'", boton.Name, boton.Tag, boton.Text));
                try
                {
                    uint code = uint.Parse((string)boton.Tag);
                    ClickEnBoton(code);
                }
                catch (Exception ex)
                {
                    KbdLogger.Log(string.Format("\tERROR al PULSAR '{0}' ({1})", ex.Message, ex.GetType().ToString()));
                }
            }
        }

        public event BotonClick ClickEnBoton;

        private void NumKbd_Resize(object sender, EventArgs e)
        {
            ResizeChildrenControls();
        }
        
        private void ResizeChildrenControls()
        {
            resizeControl(button1, originalButton1Rect, originalFontSize);
            resizeControl(button2, originalButton1Rect, originalFontSize);
            resizeControl(button3, originalButton1Rect, originalFontSize);
        }

        private void resizeControl(Control control, Rectangle originalControlRect, float originalFontSize)
        {
            //float xRatio = (float)this.ClientRectangle.Width / (float)originalControlRect.Width;
            //float yRatio = (float)this.ClientRectangle.Height / (float)originalControlRect.Height;

            //float newX = originalControlRect.Location.X * xRatio;
            //float newY = originalControlRect.Location.Y * yRatio;

            //control.Location = new Point((int)newX, (int)newY);
            //control.Width = (int)(originalControlRect.Width * xRatio);
            //control.Height = (int)(originalControlRect.Height * yRatio);

            //float ratio = xRatio;
            //if (xRatio >= yRatio)
            //    ratio = yRatio;

            //float newFontSize = originalFontSize * ratio * fontScale;
            //Font newFont = new Font(control.Font.FontFamily, newFontSize);
            //control.Font = newFont;
        }
        
    }
}

using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Controles
{
    public partial class BaseDialog : Form
    {
        public static int TipoPantalla { get; set; }

        bool extEnabled;

        protected Control tb, lastCtl;

        protected bool borrar;

        protected List<Control> TabControls = new List<Control>();

        public bool CardReaderEnabled { get; set; }

        public BaseDialog()
        {
            InitializeComponent();

   			// Pone el 'TopMost' de la form, pero solo si no estamos en modo debug.
			string sd = System.Environment.GetEnvironmentVariable("SYSDEBUG");
			this.TopMost = (sd != null && sd.ToLower() == "on")? false: true;

            CardReaderEnabled = false;
            this.AcceptButton = this.btOk;
            this.CancelButton = this.cancel;
            this.Load += new EventHandler(BaseDialog_Load);
            this.ControlAdded += new ControlEventHandler(BaseDialog_ControlAdded);
        }

        void BaseDialog_ControlAdded(object sender, ControlEventArgs e)
        {
            Colorize(e.Control, 1);
        }

        Color[] backColors = new Color[] { Color.WhiteSmoke, Color.LightCyan, Color.LightGray, Color.Gray };

        protected virtual void Colorize(Control ctl, int backIndex)
        {
            if (ctl is Label)
            {
                ctl.ForeColor = Color.Black;
            }
            else if (ctl is TextBox)
            {
                TextBox tb = (TextBox)ctl;
                tb.BackColor = Color.White;
                tb.ForeColor = Color.Black;
            }
            else if (ctl is Button)
            {
                ctl.BackColor = Color.DarkGray;
                ctl.ForeColor = Color.White;
            }
            else if (ctl is Panel)
            {
                ctl.BackColor = backColors[backIndex];
                ColorizeControls(ctl.Controls, backIndex + 1);
            }
        }

        void ColorizeControls(Control.ControlCollection controls, int backIndex)
        {
            if (backIndex >= backColors.Length) backIndex = backColors.Length - 1;
            foreach (Control ctl in controls)
                Colorize(ctl, backIndex);
        }

        void BaseDialog_Load(object sender, EventArgs e)
        {
            // Cambia los colores de los componentes, si corresponde.
            if(BaseDialog.TipoPantalla == 3)
            {
                this.SuspendLayout();
                this.BackColor = Color.White;
                ColorizeControls(Controls, 0);
                this.ResumeLayout();
            }

            // Establece el control con el foco.
            int focused = -1, index = 0;
            foreach (Control ctl in TabControls)
            {
                if (ctl.Enabled)
                {
                    focused = index;
                    break;
                }
                index++;
            }

            if (focused >= 0)
                SetFocusTo(TabControls[0]);
            else
                SetFocusTo(null);
        }

        protected virtual void Cancel_Click(object sender, EventArgs e)
        {
            if (this.cancel.Enabled)
            {
                this.DialogResult = DialogResult.Cancel;
                this.Close();
            }
        }

        public bool ExtendedKeyboardEnabled
        {
            get { return extEnabled; }
            set
            {
                extEnabled = value;
                this.b0.Visible =
                    this.b1.Visible =
                    this.b2.Visible =
                    this.b3.Visible =
                    this.b4.Visible =
                    this.b5.Visible =
                    this.b6.Visible =
                    this.b7.Visible =
                    this.b8.Visible =
                    this.b9.Visible =
                    this.bBorra.Visible =
                    this.bEnter.Visible = extEnabled;
            }
        }

        public bool ClickIfButton()
        {
            if (tb != null)
            {
                Button b = tb as Button;
                if (b != null)
                {
                    b.PerformClick();
                    return true;
                }
                else
                {
                    ComboBox cb = tb as ComboBox;
                    if (cb != null)
                    {
                        ProcessTab(1);
                        return true;
                    }
                }
            }
            return false;
        }

        public virtual bool ProcessHotKey(Keys key)
        {
            if (key == Keys.Left)
            {
                if (tb != null && tb is TextBox)
                {
                    string text = tb.Text.Trim();
                    if (text.Length > 0)
                    {
                        tb.Text = text.Substring(0, text.Length - 1);
                        TextBox _tb = (TextBox)tb;
                        _tb.SelectionStart = text.Length - 1;
                        _tb.SelectionLength = 1;
                    }
                }
                else if (tb != null && tb is MaskedTextBox)
                {
                    MaskedTextBox _tb = (MaskedTextBox)tb;
                    _tb.TextMaskFormat = MaskFormat.ExcludePromptAndLiterals;                        
                    string text = _tb.Text.Trim();
                    _tb.TextMaskFormat = MaskFormat.IncludePromptAndLiterals;
                    if (text.Length > 0)
                    {
                        //int selec = text.EndsWith("/") ? 2 : 1;
                        int selec = 1;
                        tb.Text = text.Substring(0, text.Length - selec);
                        //_tb.SelectionStart = text.Length - selec;
                        //_tb.SelectionLength = 1;
                    }
                    
                }
                return true;
            }
            return false;
        }

        protected virtual void Boton_Click(object sender, EventArgs e)
        {
            if (tb != null && tb is TextBox)
            {
                Button btn = (Button)sender;
                string k = (string)btn.Tag;
                if (borrar)
                {
                    tb.Text = (k == ".") ? "0." : k;
                    borrar = false;
                }
                else
                {
                    if (k != "." || (k == "." && !tb.Text.Contains(".")))
                        tb.Text += k;
                }
            }
            else if (tb != null && tb is MaskedTextBox){
                Button btn = (Button)sender;
                string k = (string)btn.Tag;

                MaskedTextBox _tb = (MaskedTextBox)tb;
                _tb.TextMaskFormat = MaskFormat.ExcludePromptAndLiterals;
                string text = _tb.Text.Trim();
                _tb.TextMaskFormat = MaskFormat.IncludePromptAndLiterals;
                
                _tb.Text = text + k;
                
            }
        }

        protected virtual void bBorra_Click(object sender, EventArgs e)
        {
            if (tb != null && tb is TextBox)
            {
                if (tb.Text.Length > 0)
                {
                    if (borrar) borrar = false;
                    tb.Text = tb.Text.Substring(0, tb.Text.Length - 1);
                }
            }
            else if (tb != null && tb is MaskedTextBox)
            {
                MaskedTextBox _tb = (MaskedTextBox)tb;
                _tb.TextMaskFormat = MaskFormat.ExcludePromptAndLiterals;
                string text = _tb.Text.Trim();
                _tb.TextMaskFormat = MaskFormat.IncludePromptAndLiterals;
                if (text.Length > 0)
                {                    
                    int selec = 1;
                    tb.Text = text.Substring(0, text.Length - selec);                    
                }
            }
        }

        protected virtual void bEnter_Click(object sender, EventArgs e)
        {
            ProcessTab(1);
        }

        public void ProcessTab(int dir)
        {
            // Busca el indice del objeto actualmente seleccionado.
            int selIdx = -1;
            for (int i = 0; i < TabControls.Count; i++)
            {
                if ((object)TabControls[i] == (object)tb)
                {
                    selIdx = i;
                    break;
                }
            }
            
            if(selIdx >= 0)
            {
                int nextIdx = selIdx + dir;
                while(true)
                {
                    if(nextIdx < 0) nextIdx = TabControls.Count - 1;
                    if(nextIdx >= TabControls.Count) nextIdx = 0;
                    if(nextIdx == selIdx)
                    {
                        SetFocusTo(null);
                        return;
                    }
                    if(TabControls[nextIdx].Enabled) break;
                    nextIdx += dir;
                }

                SetFocusTo(TabControls[nextIdx]);
                return;
            }
            else
                SetFocusTo(null);

            // Si el item actualmente seleccionado no esta en nuestra lista...
            SetFocusTo(null);
        }

        protected virtual void SetFocusTo(Control ctl)
        {
            if (ctl == null)
            {
                flagIzq.Visible = flagDer.Visible = false;
                return;
            }
            
            tb = ctl;
            ctl.Focus();
            if (ctl != lastCtl)
            {
                lastCtl = ctl;
                borrar = true;
            }

            // Reposiciona las banderas indicadoras.
            Point loc = ctl.Parent.PointToScreen(ctl.Location);
            loc.Y += ((ctl.Height > flagIzq.Height) ?
                        (ctl.Height - flagIzq.Height) / 2 :
                        0);
            loc.X -= flagIzq.Width + 2;
            flagIzq.Location = flagIzq.Parent.PointToClient(loc);

            loc.X += ctl.Width + flagDer.Width + 2;
            flagDer.Location = flagDer.Parent.PointToClient(loc);
            flagIzq.BringToFront();
            flagDer.BringToFront();
            flagIzq.Visible = flagDer.Visible = true;

            if (!ctl.Enabled) ProcessTab(1);
        }

        protected virtual void btOk_Click(object sender, EventArgs e)
        {
            if (this.btOk.Enabled)
            {
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
            else
                SetFocusTo(tb);
        }

        private void BaseDialog_Shown(object sender, EventArgs e)
        {
            if (TabControls.Count != 0)
                SetFocusTo(TabControls[0]);
        }

        #region Soporte para el Lector de Tarjetas Magneticas

        public StringBuilder cardData = null;
        protected string cardNumber, cardHolder, cardAdicData, track_1; //, serviceCode;
        protected int cardVenc;

        int timerCR_Count;

        public virtual void StartCardReader()
        {
            if (cardData == null)
                cardData = new StringBuilder();
            else
                cardData.Length = 0;

            // Habilita el timer de proteccion.
            timerCR_Count = 0;
            timerCR.Enabled = true;
        }

        public virtual bool CardReaderDigit(char ch)
        {
            if (timerCR_Count++ >= 3) return false;
            cardData.Append(ch);
            timerCR_Count = 0;
            return true;
        }

        public virtual void EndCardReader()
        {
            string card = cardData.ToString();           
            //card = "%B4517650551447683&VILLAFA/ES-RICARDO FEDER &120912110000        00118000000_Ñ4517650551447683¡12091211180000000000_";
            //card = "%B4517650551447683^VILLAFA/ES-RICARDO FEDER `120912110000        00118000000?;4517650551447683=12091211180000000000?";
            //card = "%B4517650560147506^VILLAFA&ES/RICARDO FEDER ^140412110000        00648000000?;4517650560147506=14041216480000000000?";
            cardNumber = cardHolder = cardAdicData = "";// = serviceCode = "";
            cardVenc = 0;

            // Separa las tracks y las partes de las tracks que nos interesan.
            //cambiado
            //if (card.StartsWith("%B") && card.Length > 16)
            if (card.StartsWith("%") && card.Length > 16)
            {
                if (!card.StartsWith("%B"))
                    card = "%B" + card.Substring(1, card.Length - 1);
                // Decodifica los sentinel y otros caracteres de control usados.
                char questionMark = card[card.Length - 1];
                char sentinel = 'x';
                char separator = 'x';
                int extrCount = 0;
                for (int i = card.Length - 2; i > 0; i--)
                {
                    if (sentinel == 'x' && card[i] == questionMark)
                    {
                        sentinel = card[i + 1];
                        break;
                    }
                    else if (!char.IsDigit(card[i]))
                    {
                        extrCount++;
                        if (extrCount == 2)
                        {
                            sentinel = card[i];
                            break;
                        }
                    }
                }
                for (int i = 4; i < card.Length; i++)
                {
                    if (!char.IsDigit(card[i]))
                    {
                        separator = card[i];
                        break;
                    }
                }

                // Separa las tracks
                if (separator != 'x' && sentinel != 'x')
                {
                    string[] tracks = card.Split(sentinel);
                    if (tracks.Length == 2)
                    {
                        cardAdicData = tracks[1].Trim();
                        cardAdicData = cardAdicData.Substring(0, cardAdicData.Length - 1);
                        for (int i = 0; i < cardAdicData.Length; i++)
                            if (!char.IsDigit(cardAdicData[i]))
                                cardAdicData = cardAdicData.Substring(0, i) + "=" + cardAdicData.Substring(i + 1);

                        if (cardAdicData.Length > 0)
                        {
                            string[] parts = tracks[0].Split(separator);
                            if (tracks[0].Length > 2)
                                track_1 = tracks[0].Length > 76 ? tracks[0].Substring(1, 76) : tracks[0].Substring(1, tracks[0].Length - 2);
                            else
                                track_1 = string.Empty;
                            
                            cardNumber = parts[0].Substring(2);
                            if (parts.Length > 1)
                                cardHolder = parts[1].Trim();
                            if (parts.Length > 2)
                                if (parts[2].Length > 3)
                                {
                                    cardVenc = int.Parse(parts[2].Substring(2, 2)) * 100 + int.Parse(parts[2].Substring(0, 2));
                                    //serviceCode = parts[2].Substring(4, 3);
                                }
                                else
                                {
                                    string cadena = tracks[1];
                                    char mseparador = ' ';
                                    for (int j = 0; j < cadena.Length; j++)
                                    {
                                        if (!char.IsDigit(cadena[j]) && (sentinel != cadena[j]))
                                        {
                                            mseparador = cadena[j];
                                            break;
                                        }
                                    }
                                    if (mseparador != ' ')
                                    {
                                        parts = cadena.Split(mseparador);
                                        if (parts.Length > 1)
                                        {
                                            cardVenc = int.Parse(parts[1].Substring(2, 2)) * 100 + int.Parse(parts[1].Substring(0, 2));
                                            //serviceCode = parts[1].Substring(4, 3);
                                        }
                                    }
                                }
                        }
                    }

                    // Hace un dump de los datos de la tarjeta
                    StreamWriter sw = File.CreateText("cardtrac.txt");
                    //StreamWriter sw = File.AppendText("cardtrac.txt");

                    sw.WriteLine(DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"));
                    sw.WriteLine("[" + card + "]\n");
                    
                    //foreach (char ch in card)
                    //    sw.WriteLine("\t'{0}' = {1}", ch, (int)ch);

                    sw.WriteLine("\nTracks={0}, Separator='{1}', Sentinel='{2}'", tracks.Length, separator, sentinel);
                    int trackCount = 1;
                    foreach (string track in tracks)
                        sw.WriteLine("  Track {0} = '{1}'", trackCount++, track);
                    sw.WriteLine("\nResultado:");
                    sw.WriteLine("  cardNumber = '{0}'", cardNumber);
                    sw.WriteLine("  cardHolder = '{0}'", cardHolder);
                    sw.WriteLine("    cardVenc = '{0}'", cardVenc);
                    sw.WriteLine("    adicData = '{0}'", cardAdicData);
                    //sw.WriteLine(" ServiceCode = '{0}'", serviceCode);
                    sw.Close();
                }


                /*
                string[] tracks = card.Split('^');
                if (tracks.Length == 1)
                    tracks = card.Split(card[18]);

                cardNumber = tracks[0].Substring(2);
                if (tracks.Length > 1)
                {
                    cardHolder = tracks[1].Trim();
                    if (tracks.Length > 2)
                    {
                        string[] parts = tracks[2].Split('?');
                        if (parts.Length == 1) parts = tracks[2].Split('ñ');
                        cardVenc = int.Parse(parts[0].Substring(2, 2)) * 100 + int.Parse(parts[0].Substring(0, 2));
                        if (parts.Length > 1 && parts[1].Trim().Length > 0)
                            cardAdicData = parts[1].Substring(1);
                        else
                            cardAdicData = string.Empty;
                    }
                    %B6042015252251801^VILLAFA&EditorAttribute RICARDO FEDERI  ^17111212210000000000?;6042015252251801=17111212210000000000?

                }
            */
            }
        }

        // El timer se agrega para que no quede la rutina del teclado bloqueada cuando, por un problema del lector,
        // no se lee la banda magnetica completa.
        private void timerCR_Tick(object sender, EventArgs e)
        {
            timerCR_Count++;
            if (timerCR_Count >= 3)
            {
            }

        }

        public void txtSoloNumeros_KeyPress(object sender, KeyPressEventArgs e)
        {
			if (char.IsLetter(e.KeyChar))
            {
                e.Handled = true;
            }
			else if (char.IsDigit(e.KeyChar))
            {
                e.Handled = false;
            }
			else if (char.IsControl(e.KeyChar))
            {
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
        }

        public void txtMonto_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (char.IsLetter(e.KeyChar))
            {
                e.Handled = true;
            }
			else if (char.IsDigit(e.KeyChar) )
            {
                e.Handled = false;
            }
			else if (e.KeyChar == '.')
			{
				TextBox auxtext = (TextBox)(sender);
				e.Handled = auxtext.Text.Contains(".");
			}
			else if (Char.IsControl(e.KeyChar))
            {
                e.Handled = false;
            }
            else
            {
                e.Handled = true;
            }
        }

        #endregion

    }
}

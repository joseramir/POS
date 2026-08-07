namespace Controles
{
    partial class BaseDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BaseDialog));
            this.panel = new System.Windows.Forms.Panel();
            this.flagDer = new System.Windows.Forms.PictureBox();
            this.flagIzq = new System.Windows.Forms.PictureBox();
            this.bEnter = new System.Windows.Forms.Button();
            this.cancel = new System.Windows.Forms.Button();
            this.btOk = new System.Windows.Forms.Button();
            this.bBorra = new System.Windows.Forms.Button();
            this.b0 = new System.Windows.Forms.Button();
            this.b3 = new System.Windows.Forms.Button();
            this.b2 = new System.Windows.Forms.Button();
            this.b1 = new System.Windows.Forms.Button();
            this.b6 = new System.Windows.Forms.Button();
            this.b5 = new System.Windows.Forms.Button();
            this.b4 = new System.Windows.Forms.Button();
            this.b9 = new System.Windows.Forms.Button();
            this.b8 = new System.Windows.Forms.Button();
            this.b7 = new System.Windows.Forms.Button();
            this.panel3 = new System.Windows.Forms.Panel();
            this.Icono = new System.Windows.Forms.PictureBox();
            this.Titulo = new System.Windows.Forms.Label();
            this.timerCR = new System.Windows.Forms.Timer(this.components);
            this.panel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.flagDer)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.flagIzq)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Icono)).BeginInit();
            this.SuspendLayout();
            // 
            // panel
            // 
            this.panel.BackColor = System.Drawing.Color.DarkSeaGreen;
            this.panel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel.Controls.Add(this.flagDer);
            this.panel.Controls.Add(this.flagIzq);
            this.panel.Controls.Add(this.bEnter);
            this.panel.Controls.Add(this.cancel);
            this.panel.Controls.Add(this.btOk);
            this.panel.Controls.Add(this.bBorra);
            this.panel.Controls.Add(this.b0);
            this.panel.Controls.Add(this.b3);
            this.panel.Controls.Add(this.b2);
            this.panel.Controls.Add(this.b1);
            this.panel.Controls.Add(this.b6);
            this.panel.Controls.Add(this.b5);
            this.panel.Controls.Add(this.b4);
            this.panel.Controls.Add(this.b9);
            this.panel.Controls.Add(this.b8);
            this.panel.Controls.Add(this.b7);
            this.panel.Controls.Add(this.panel3);
            this.panel.Controls.Add(this.Icono);
            this.panel.Controls.Add(this.Titulo);
            this.panel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel.Location = new System.Drawing.Point(0, 0);
            this.panel.Name = "panel";
            this.panel.Size = new System.Drawing.Size(658, 434);
            this.panel.TabIndex = 24;
            // 
            // flagDer
            // 
            this.flagDer.BackColor = System.Drawing.Color.Transparent;
            this.flagDer.Image = global::Controles.Properties.Resources.stop;
            this.flagDer.Location = new System.Drawing.Point(499, 293);
            this.flagDer.Name = "flagDer";
            this.flagDer.Size = new System.Drawing.Size(16, 16);
            this.flagDer.TabIndex = 38;
            this.flagDer.TabStop = false;
            this.flagDer.Visible = false;
            // 
            // flagIzq
            // 
            this.flagIzq.BackColor = System.Drawing.Color.Transparent;
            this.flagIzq.Image = global::Controles.Properties.Resources.stop;
            this.flagIzq.Location = new System.Drawing.Point(466, 287);
            this.flagIzq.Name = "flagIzq";
            this.flagIzq.Size = new System.Drawing.Size(16, 16);
            this.flagIzq.TabIndex = 37;
            this.flagIzq.TabStop = false;
            this.flagIzq.Visible = false;
            // 
            // bEnter
            // 
            this.bEnter.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.bEnter.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.bEnter.ForeColor = System.Drawing.Color.Navy;
            this.bEnter.Image = ((System.Drawing.Image)(resources.GetObject("bEnter.Image")));
            this.bEnter.Location = new System.Drawing.Point(578, 233);
            this.bEnter.Name = "bEnter";
            this.bEnter.Size = new System.Drawing.Size(50, 48);
            this.bEnter.TabIndex = 36;
            this.bEnter.Tag = "21248";
            this.bEnter.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.bEnter.UseVisualStyleBackColor = false;
            this.bEnter.Click += new System.EventHandler(this.bEnter_Click);
            // 
            // cancel
            // 
            this.cancel.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.cancel.Font = new System.Drawing.Font("Tahoma", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cancel.ForeColor = System.Drawing.Color.Navy;
            this.cancel.Image = ((System.Drawing.Image)(resources.GetObject("cancel.Image")));
            this.cancel.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.cancel.Location = new System.Drawing.Point(466, 371);
            this.cancel.Name = "cancel";
            this.cancel.Size = new System.Drawing.Size(162, 48);
            this.cancel.TabIndex = 35;
            this.cancel.Tag = "21248";
            this.cancel.Text = "Escape";
            this.cancel.UseVisualStyleBackColor = false;
            this.cancel.Click += new System.EventHandler(this.Cancel_Click);
            // 
            // btOk
            // 
            this.btOk.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.btOk.Font = new System.Drawing.Font("Tahoma", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btOk.ForeColor = System.Drawing.Color.Navy;
            this.btOk.Image = ((System.Drawing.Image)(resources.GetObject("btOk.Image")));
            this.btOk.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.btOk.Location = new System.Drawing.Point(466, 317);
            this.btOk.Name = "btOk";
            this.btOk.Size = new System.Drawing.Size(162, 48);
            this.btOk.TabIndex = 34;
            this.btOk.Tag = "21248";
            this.btOk.Text = "Ok";
            this.btOk.UseVisualStyleBackColor = false;
            this.btOk.Click += new System.EventHandler(this.btOk_Click);
            // 
            // bBorra
            // 
            this.bBorra.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.bBorra.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.bBorra.ForeColor = System.Drawing.Color.Navy;
            this.bBorra.Image = ((System.Drawing.Image)(resources.GetObject("bBorra.Image")));
            this.bBorra.Location = new System.Drawing.Point(522, 233);
            this.bBorra.Name = "bBorra";
            this.bBorra.Size = new System.Drawing.Size(50, 48);
            this.bBorra.TabIndex = 33;
            this.bBorra.Tag = "21248";
            this.bBorra.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.bBorra.UseVisualStyleBackColor = false;
            this.bBorra.Click += new System.EventHandler(this.bBorra_Click);
            // 
            // b0
            // 
            this.b0.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b0.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b0.ForeColor = System.Drawing.Color.Navy;
            this.b0.Image = ((System.Drawing.Image)(resources.GetObject("b0.Image")));
            this.b0.Location = new System.Drawing.Point(466, 233);
            this.b0.Name = "b0";
            this.b0.Size = new System.Drawing.Size(50, 48);
            this.b0.TabIndex = 31;
            this.b0.Tag = "0";
            this.b0.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b0.UseVisualStyleBackColor = false;
            this.b0.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b3
            // 
            this.b3.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b3.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b3.ForeColor = System.Drawing.Color.Navy;
            this.b3.Image = ((System.Drawing.Image)(resources.GetObject("b3.Image")));
            this.b3.Location = new System.Drawing.Point(578, 181);
            this.b3.Name = "b3";
            this.b3.Size = new System.Drawing.Size(50, 48);
            this.b3.TabIndex = 30;
            this.b3.Tag = "3";
            this.b3.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b3.UseVisualStyleBackColor = false;
            this.b3.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b2
            // 
            this.b2.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b2.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b2.ForeColor = System.Drawing.Color.Navy;
            this.b2.Image = ((System.Drawing.Image)(resources.GetObject("b2.Image")));
            this.b2.Location = new System.Drawing.Point(522, 181);
            this.b2.Name = "b2";
            this.b2.Size = new System.Drawing.Size(50, 48);
            this.b2.TabIndex = 29;
            this.b2.Tag = "2";
            this.b2.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b2.UseVisualStyleBackColor = false;
            this.b2.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b1
            // 
            this.b1.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b1.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b1.ForeColor = System.Drawing.Color.Navy;
            this.b1.Image = ((System.Drawing.Image)(resources.GetObject("b1.Image")));
            this.b1.Location = new System.Drawing.Point(466, 181);
            this.b1.Name = "b1";
            this.b1.Size = new System.Drawing.Size(50, 48);
            this.b1.TabIndex = 28;
            this.b1.Tag = "1";
            this.b1.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b1.UseVisualStyleBackColor = false;
            this.b1.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b6
            // 
            this.b6.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b6.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b6.ForeColor = System.Drawing.Color.Navy;
            this.b6.Image = ((System.Drawing.Image)(resources.GetObject("b6.Image")));
            this.b6.Location = new System.Drawing.Point(578, 129);
            this.b6.Name = "b6";
            this.b6.Size = new System.Drawing.Size(50, 48);
            this.b6.TabIndex = 27;
            this.b6.Tag = "6";
            this.b6.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b6.UseVisualStyleBackColor = false;
            this.b6.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b5
            // 
            this.b5.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b5.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b5.ForeColor = System.Drawing.Color.Navy;
            this.b5.Image = ((System.Drawing.Image)(resources.GetObject("b5.Image")));
            this.b5.Location = new System.Drawing.Point(522, 129);
            this.b5.Name = "b5";
            this.b5.Size = new System.Drawing.Size(50, 48);
            this.b5.TabIndex = 26;
            this.b5.Tag = "5";
            this.b5.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b5.UseVisualStyleBackColor = false;
            this.b5.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b4
            // 
            this.b4.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b4.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b4.ForeColor = System.Drawing.Color.Navy;
            this.b4.Image = ((System.Drawing.Image)(resources.GetObject("b4.Image")));
            this.b4.Location = new System.Drawing.Point(466, 129);
            this.b4.Name = "b4";
            this.b4.Size = new System.Drawing.Size(50, 48);
            this.b4.TabIndex = 25;
            this.b4.Tag = "4";
            this.b4.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b4.UseVisualStyleBackColor = false;
            this.b4.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b9
            // 
            this.b9.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b9.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b9.ForeColor = System.Drawing.Color.Navy;
            this.b9.Image = ((System.Drawing.Image)(resources.GetObject("b9.Image")));
            this.b9.Location = new System.Drawing.Point(578, 77);
            this.b9.Name = "b9";
            this.b9.Size = new System.Drawing.Size(50, 48);
            this.b9.TabIndex = 24;
            this.b9.Tag = "9";
            this.b9.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b9.UseVisualStyleBackColor = false;
            this.b9.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b8
            // 
            this.b8.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b8.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b8.ForeColor = System.Drawing.Color.Navy;
            this.b8.Image = ((System.Drawing.Image)(resources.GetObject("b8.Image")));
            this.b8.Location = new System.Drawing.Point(522, 77);
            this.b8.Name = "b8";
            this.b8.Size = new System.Drawing.Size(50, 48);
            this.b8.TabIndex = 23;
            this.b8.Tag = "8";
            this.b8.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b8.UseVisualStyleBackColor = false;
            this.b8.Click += new System.EventHandler(this.Boton_Click);
            // 
            // b7
            // 
            this.b7.BackColor = System.Drawing.Color.PaleGoldenrod;
            this.b7.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.b7.ForeColor = System.Drawing.Color.Navy;
            this.b7.Image = ((System.Drawing.Image)(resources.GetObject("b7.Image")));
            this.b7.Location = new System.Drawing.Point(466, 77);
            this.b7.Name = "b7";
            this.b7.Size = new System.Drawing.Size(50, 48);
            this.b7.TabIndex = 22;
            this.b7.Tag = "7";
            this.b7.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
            this.b7.UseVisualStyleBackColor = false;
            this.b7.Click += new System.EventHandler(this.Boton_Click);
            // 
            // panel3
            // 
            this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel3.Location = new System.Drawing.Point(2, 55);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(650, 4);
            this.panel3.TabIndex = 20;
            // 
            // Icono
            // 
            this.Icono.Image = ((System.Drawing.Image)(resources.GetObject("Icono.Image")));
            this.Icono.Location = new System.Drawing.Point(19, 10);
            this.Icono.Name = "Icono";
            this.Icono.Size = new System.Drawing.Size(37, 37);
            this.Icono.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.Icono.TabIndex = 19;
            this.Icono.TabStop = false;
            // 
            // Titulo
            // 
            this.Titulo.AutoSize = true;
            this.Titulo.Font = new System.Drawing.Font("Tahoma", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Titulo.Location = new System.Drawing.Point(62, 10);
            this.Titulo.Name = "Titulo";
            this.Titulo.Size = new System.Drawing.Size(342, 23);
            this.Titulo.TabIndex = 15;
            this.Titulo.Text = "Elegir Cliente (o Cuenta Corriente)";
            // 
            // timerCR
            // 
            this.timerCR.Interval = 500;
            this.timerCR.Tick += new System.EventHandler(this.timerCR_Tick);
            // 
            // BaseDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(658, 434);
            this.Controls.Add(this.panel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "BaseDialog";
            this.Text = "BaseDialog";
            this.Shown += new System.EventHandler(this.BaseDialog_Shown);
            this.panel.ResumeLayout(false);
            this.panel.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.flagDer)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.flagIzq)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Icono)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        protected System.Windows.Forms.Panel panel;
        protected System.Windows.Forms.Button cancel;
        protected System.Windows.Forms.Button btOk;
        protected System.Windows.Forms.Button bBorra;
        protected System.Windows.Forms.Button b0;
        protected System.Windows.Forms.Button b2;
        protected System.Windows.Forms.Button b1;
        protected System.Windows.Forms.Button b5;
        protected System.Windows.Forms.Button b4;
        protected System.Windows.Forms.Button b8;
        protected System.Windows.Forms.Button b7;
        protected System.Windows.Forms.Panel panel3;
        protected System.Windows.Forms.PictureBox Icono;
        protected System.Windows.Forms.Label Titulo;
        protected System.Windows.Forms.Button bEnter;
        protected System.Windows.Forms.Button b3;
        protected System.Windows.Forms.Button b6;
        protected System.Windows.Forms.Button b9;
        private System.Windows.Forms.PictureBox flagIzq;
        private System.Windows.Forms.PictureBox flagDer;
        private System.Windows.Forms.Timer timerCR;
    }
}
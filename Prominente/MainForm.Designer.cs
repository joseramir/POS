namespace Prominente
{
    partial class MainForm
    {
        /// <summary>
        /// Variable del diseñador requerida.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén utilizando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben eliminar; false en caso contrario, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido del método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.sAmarillo = new System.Windows.Forms.PictureBox();
            this.sRojo = new System.Windows.Forms.PictureBox();
            this.sVerde = new System.Windows.Forms.PictureBox();
            this.btPing = new System.Windows.Forms.Button();
            this.lb = new System.Windows.Forms.ListBox();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.lbEmu = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.sAmarillo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.sRojo)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.sVerde)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(126, 14);
            this.label1.TabIndex = 0;
            this.label1.Text = "Estado del Servicio:";
            // 
            // sAmarillo
            // 
            this.sAmarillo.Image = global::Prominente.Properties.Resources.semaforo_amarillo;
            this.sAmarillo.Location = new System.Drawing.Point(165, 11);
            this.sAmarillo.Name = "sAmarillo";
            this.sAmarillo.Size = new System.Drawing.Size(33, 44);
            this.sAmarillo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.sAmarillo.TabIndex = 1;
            this.sAmarillo.TabStop = false;
            // 
            // sRojo
            // 
            this.sRojo.Image = global::Prominente.Properties.Resources.semaforo_rojo;
            this.sRojo.Location = new System.Drawing.Point(204, 11);
            this.sRojo.Name = "sRojo";
            this.sRojo.Size = new System.Drawing.Size(33, 44);
            this.sRojo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.sRojo.TabIndex = 2;
            this.sRojo.TabStop = false;
            // 
            // sVerde
            // 
            this.sVerde.Image = global::Prominente.Properties.Resources.semaforo_verde;
            this.sVerde.Location = new System.Drawing.Point(243, 12);
            this.sVerde.Name = "sVerde";
            this.sVerde.Size = new System.Drawing.Size(33, 44);
            this.sVerde.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.sVerde.TabIndex = 3;
            this.sVerde.TabStop = false;
            // 
            // btPing
            // 
            this.btPing.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btPing.Location = new System.Drawing.Point(15, 31);
            this.btPing.Name = "btPing";
            this.btPing.Size = new System.Drawing.Size(121, 23);
            this.btPing.TabIndex = 4;
            this.btPing.Text = "Ping";
            this.btPing.UseVisualStyleBackColor = true;
            this.btPing.Click += new System.EventHandler(this.btPing_Click);
            // 
            // lb
            // 
            this.lb.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.lb.FormattingEnabled = true;
            this.lb.Location = new System.Drawing.Point(12, 82);
            this.lb.Name = "lb";
            this.lb.Size = new System.Drawing.Size(411, 303);
            this.lb.TabIndex = 5;
            // 
            // timer
            // 
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // lbEmu
            // 
            this.lbEmu.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lbEmu.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEmu.ForeColor = System.Drawing.Color.Red;
            this.lbEmu.Location = new System.Drawing.Point(297, 11);
            this.lbEmu.Name = "lbEmu";
            this.lbEmu.Size = new System.Drawing.Size(126, 14);
            this.lbEmu.TabIndex = 6;
            this.lbEmu.Text = "¡¡¡ OJO - Emulando !!!";
            this.lbEmu.TextAlign = System.Drawing.ContentAlignment.TopRight;
            this.lbEmu.Visible = false;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(435, 390);
            this.Controls.Add(this.lbEmu);
            this.Controls.Add(this.lb);
            this.Controls.Add(this.btPing);
            this.Controls.Add(this.sVerde);
            this.Controls.Add(this.sRojo);
            this.Controls.Add(this.sAmarillo);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "MainForm";
            this.Text = " Carga Virtual - Prominente ";
            ((System.ComponentModel.ISupportInitialize)(this.sAmarillo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.sRojo)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.sVerde)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.PictureBox sAmarillo;
        private System.Windows.Forms.PictureBox sRojo;
        private System.Windows.Forms.PictureBox sVerde;
        private System.Windows.Forms.Button btPing;
        private System.Windows.Forms.ListBox lb;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.Label lbEmu;

    }
}


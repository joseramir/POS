using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi
{
    public class PagoDoc
    {
        public int MedioPago { get; set; }
        public decimal Monto { get; set; }
        public decimal Vuelto { get; set; }
        public int CodTarjeta { get; set; }
        public int Cuotas { get; set; }
        public int Lote { get; set; }
        public int Cupon { get; set; }
        public string Autorizacion { get; set; }
        public short Integracion { get; set; }
        public int Terminal { get; set; }
        public bool QR { get; set; }
        public decimal Extracash { get; set; }
        public DateTime Hora { get; set; }

        public PagoDoc()
        {
            MedioPago = 0;
            Monto = 0.0m;
            Vuelto = 0.0m;
            CodTarjeta = 0;
            Cuotas = 1; // Default to 1 installment
            Lote = 0;
            Cupon = 0;
            Autorizacion = string.Empty;
            Integracion = 0; // Default to no integration
            Terminal = 0;
            QR = false; // Default to not using QR
            Extracash = 0.0m; // Default to no extra cash
            Hora = DateTime.Now; // Default to current time
        }
    }
}

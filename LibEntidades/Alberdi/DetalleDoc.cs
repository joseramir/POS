using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi
{
    public class DetalleDoc
    {
        public int NroReg { get; set; }
        public int Codigo { get; set; }
        public int Departamento { get; set; }
        public int Subdepartamento { get; set; }
        public string Scan { get; set; }
        public decimal Importe { get; set; }
        public decimal Precio { get; set; }
        public decimal Cantidad { get; set; }
        public short IVA { get; set; }
        public bool Escaneado { get; set; }
        public int Milla { get; set; }
        public bool Anulado { get; set; }
        public DateTime Hora { get; set; }
    }
}

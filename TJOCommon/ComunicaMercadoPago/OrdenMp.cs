using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.ComunicaMercadoPago
{
    public class OrdenMp
    {
        public string Tienda { get; set; }
        public int Caja { get; set; }
        public int Cajero { get; set; }
        public decimal Monto { get; set; }
        public int Cuotas { get; set; }
        public int PtoVta { get; set; }
        public int NroTicket { get; set; }
    }
}

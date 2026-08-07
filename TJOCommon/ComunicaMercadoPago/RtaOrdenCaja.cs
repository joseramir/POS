using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.ComunicaMercadoPago
{
    public class RtaOrdenCaja : OrdenCaja
    {
        public int IdHdr { get; set; }
        public DateTime FechaHora { get; set; }
        public string ExternalReference { get; set; }
        public short Estado { get; set; }
        public string IdOperacion { get; set; }
        public bool Anulado { get; set; }
        public string Reference { get; set; }
        public string IdOrdenMercadoPago { get; set; }
    }
}

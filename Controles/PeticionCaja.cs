using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Controles
{
    public class PeticionCaja
    {
        public int seq { get; set; }
        public DateTime fhinicio { get; set; }
        public short numcaja { get; set; }
        public short ptovta { get; set; }
        public string tipocomp { get; set; }
        public int numcomp { get; set; }
        public string tienda { get; set; }
        public decimal monto { get; set; }
        public int estado { get; set; }
        public DateTime fhactua { get; set; }
        public string idmp { get; set; }
        public string idoper { get; set; }
        public long payment_id { get; set; }
    }
}

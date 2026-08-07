using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi
{
    public class ImpuestoAplicado
    {
        public int TipoImpuesto { get; set; }
        public decimal Monto { get; set; }
        public decimal Alicuota { get; set; }
        public DateTime Hora { get; set; }
    }
}

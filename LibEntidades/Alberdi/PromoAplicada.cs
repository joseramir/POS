using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi
{
    public class PromoAplicada
    {
        public long CodPromo { get; set; }
        public int Descarga { get; set; }
        public decimal Monto { get; set; }
        public decimal Cantidad { get; set; }
        public short IVA { get; set; }
        public DateTime Hora { get; set; }

        public PromoAplicada()
        {
            CodPromo = 0;
            Descarga = 0;
            Monto = 0;
            Cantidad = 0;
            IVA = 0;
            Hora = DateTime.MinValue; // Default value for DateTime
        }
    }
}

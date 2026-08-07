using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.ComunicaMercadoPago
{
    public class OrdenCashout: OrdenMp
    {
        public decimal Cashout { get; set; }

        public string ExternalReference { get; set; }
        public string IdPayment { get; set; }
        public string IdOrdenMercadoPago { get; set; }

        public string Reference { get; set; }

        public OrdenCashout():base()
        {
            this.ExternalReference = "";
            this.IdPayment = "";
            this.IdOrdenMercadoPago = "";
            this.Reference = "";
        }
    }
}

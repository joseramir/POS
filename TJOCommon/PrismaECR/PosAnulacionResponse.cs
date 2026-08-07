using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.PrismaECR
{
    public class PosAnulacionResponse
    {
        public string IdOperacionPayway { get; set; }
        public string ClaveBusq { get; set; }
        public string Estado { get; set; }
        public string Mensaje { get; set; }

        // Datos del pago original (para confirmar al operador antes de anular)
        public decimal? Monto { get; set; }
        public string CodigoAutorizacion { get; set; }
        public string NumeroLote { get; set; }
        public string NumeroCupon { get; set; }
        public string Prim6Digitos { get; set; }
        public string Ult4Digitos { get; set; }
        public string NumeroTerminalPrisma { get; set; }  // Necesario para armar el request de reversión a Payway
    }
}

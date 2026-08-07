using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.PrismaECR
{
    public class PosPaymentResponse
    {
        public string IdOperacionPayway { get; set; }  // UUID devuelto por el Gateway (clave para el polling)
        public string Estado { get; set; }             // PENDIENTE, APROBADA, RECHAZADA, ANULADA
        public string Mensaje { get; set; }

        // Datos de aprobación para imprimir el ticket
        public decimal? Monto { get; set; }            // Monto autorizado en pesos (ej. 15.50)
        public decimal? MontoExtracash { get; set; }   // Extracash/cashback en pesos; 0 si no aplica
        public string CodigoAutorizacion { get; set; }
        public string NumeroLote { get; set; }
        public string NumeroCupon { get; set; }
        public string Prim6Digitos { get; set; }       // BIN de la tarjeta usada
        public string Ult4Digitos { get; set; }        // Últimos 4 dígitos de la tarjeta
        public string MarcaTarjeta { get; set; }       // card_brand_product (relevante en pagos QR)
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.PrismaECR
{
    public class PosPaymentRequest
    {
        public int NumeroSucursal { get; set; }
        public int NumeroCaja { get; set; }
        public string NumeroCajero { get; set; }
        public string EcrTransactionId { get; set; }       // Número de ticket local de la caja
        public string NumeroComprobanteFiscal { get; set; }
        public string PuntoVentaFiscal { get; set; }
        public decimal MontoOperacion { get; set; }          // En pesos con decimales (ej. 15.50 para $15,50)
        public decimal MontoExtracash { get; set; }         // Extracash/cashback en pesos; 0 si no aplica
        public int NumeroCuotas { get; set; }
        public string MarcaTarjeta { get; set; }            // card_brand_product: "VI", "MC", etc. null = cualquier marca
        public string NumeroComercio { get; set; }          // Número de comercio registrado en
    }
}

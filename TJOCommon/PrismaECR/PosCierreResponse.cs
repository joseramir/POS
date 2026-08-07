using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.PrismaECR
{
    public class PosCierreResponse
    {
        public string SettlementId { get; set; }  // UUID devuelto por Payway (clave para polling)
        public string Estado { get; set; }  // PENDIENTE, CONFIRMADO, RECHAZADO, ERROR
        public string Mensaje { get; set; }
        public string NumeroLote { get; set; }
        public string CodigoRespuesta { get; set; }
        public string MensajeAdicional { get; set; }
        public string TicketCierre { get; set; }  // merchant_receipt_detail de Payway
        public string NumeroTerminal { get; set; }
    }
}

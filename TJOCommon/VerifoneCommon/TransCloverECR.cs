using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.VerifoneCommon
{
    // Cupon cobrado por Clover (CloverECR). Mismo uso que TransPrismaECR, pero con los ids de Clover:
    // IdOperacion (Clover) y IdOperacionPayway (Prisma) son distintos y no se mezclan.
    public class TransCloverECR: TransVerifone
    {
        public string IdOperacion { get; set; }
        // lote 3 + cupon 4 + autorizacion 6: la clave para anular o devolver la venta
        public string ClaveBusq { get; set; }
        // TARJETA o QR, y la billetera si fue QR (MODO, ...)
        public string MedioPago { get; set; }
        public string Billetera { get; set; }
        // Numero de serie del Flex que cobro la venta
        public string SerieFlex { get; set; }

        public TransCloverECR():base()
        {
            IdOperacion = "";
            ClaveBusq = "";
            MedioPago = "";
            Billetera = "";
            SerieFlex = "";
        }
    }
}

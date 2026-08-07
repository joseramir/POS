using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi.Parking
{
    public class ParkingPost
    {
        public int id_estadia { get; set; }
        public bool estado { get; set; }
        public List<PagoDoc> pagos { get; set; }
        public int caja { get; set; }

        public int PuntoVenta { get; set; }
        public short TipoComprobante { get; set; }
        public int NumeroComprobante { get; set; }

        public override string ToString()
        {
            return string.Format("id_estadia: {0}, estado: {1}, caja: {2}, PuntoVenta: {3}, TipoComprobante: {4}, NumeroComprobante: {5}", 
                id_estadia, estado, caja, PuntoVenta, TipoComprobante, NumeroComprobante);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi.Parking
{
    public class ListParking
    {
        public DatosParking[] data { get; set; }
    }

    public class DatosParking
    {
        public int Cantidad { get; set; }
        public int Codigo { get; set; }        
        public string Descripcion { get; set; }
        public decimal MontoTotal { get; set; }
        public decimal PrecioUnitario { get; set; }

        public override string ToString()
        {
            return string.Format("Codigo: {0}, Cantidad: {1}, Descripción: {2}, MontoTotal: {3}, PrecioUnitario: {4}", 
                Codigo, Cantidad, Descripcion, MontoTotal.ToString("F2"), PrecioUnitario.ToString("F2"));
        }
    }
}

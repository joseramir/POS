using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi
{
    public class Sucursal
    {
        public short IdTienda { get; set; }
        public string Tienda { get; set; }
        public string Nombre { get; set; }
        public bool Habilitado { get; set; }
    }
}

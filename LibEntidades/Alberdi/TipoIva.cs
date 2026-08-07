using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi
{
    public class TipoIva
    {
        public int IdCondIva { get; set; }
        public string Descripcion { get; set; }
        public string CondIva { get; set; }

        public TipoIva()
        {
            IdCondIva = 1;
            Descripcion = "Consumidor Final";
            CondIva = "CF";
        }
    }
}

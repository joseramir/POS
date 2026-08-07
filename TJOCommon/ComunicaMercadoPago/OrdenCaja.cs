using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.ComunicaMercadoPago
{
    public class OrdenCaja : OrdenMp
    {        
        public int AlbCodTarjeta { get; set; }
        public string TipoTarjeta { get; set; }   

        public OrdenCaja():base()
        {

        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Controles
{
    public class MotivoNotaCredito
    {
        public string CodigoMotivo { get; set; }
        public string DescripcionMotivo { get; set; }

        public MotivoNotaCredito(string pcodigo, string pdescrip)
        {
            CodigoMotivo = pcodigo;
            DescripcionMotivo = pdescrip;
        }
    }
}

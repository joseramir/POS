using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.VerifoneCommon
{
    public class TransPrismaECR: TransVerifone
    {
        public string IdOperacionPayway { get; set; }

        public TransPrismaECR():base()
        {
            IdOperacionPayway = "";
        }
    }
}

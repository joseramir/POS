using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Configuration;

namespace Alberdi.TJOCommon.Prisma
{
    public class TrxPrisma
    {
        public int Id { get; set; }
        public string Mensaje { get; set; }
        public decimal Monto { get; set; }
        public string NroFactura { get; set; }
        public int CantCuotas { get; set; }
        public string CodigoTarjeta { get; set; }
        public string CodigoPlan { get; set; }
        public decimal MontoPropina { get; set; }
        public string CodigoComercio { get; set; }
        public string NombreComercio { get; set; }
        public string CuitComercio { get; set; }
        public int LineMode { get; set; }
        public decimal MontoExtraCash { get; set; }
        public int Estado { get; set; }

        public TrxPrisma()
        {
            Id = 0;
            NroFactura = "000000000000";
            CodigoComercio = "        ";// ConfigurationManager.AppSettings["CodComercioPrisma"];
            NombreComercio = ConfigurationManager.AppSettings["NomComercioPrisma"];
            CuitComercio = ConfigurationManager.AppSettings["CuitComercioPrisma"];
            CodigoTarjeta = "";
            LineMode = 1;
            Estado = 0;
        }

        public void Reset()
        {
            Id = 0;
            NroFactura = "000000000000";
            //CodigoComercio = new string(' ', 15);
            //NombreComercio = new string(' ', 23);
            //CuitComercio = new string(' ', 23);
            CodigoComercio = "        ";// ConfigurationManager.AppSettings["CodComercioPrisma"];
            NombreComercio = ConfigurationManager.AppSettings["NomComercioPrisma"];
            CuitComercio = ConfigurationManager.AppSettings["CuitComercioPrisma"];
            CodigoTarjeta = "";
            LineMode = 1;
            Estado = 0;
        }
    }    
}

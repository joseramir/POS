using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace Controles
{
    public class ClienteSql
    {
        public long Cod { get; set; }
        public string Nombre { get; set; }
        public string Direccion { get; set; }
        public string Localidad { get; set; }
        public short Civa { get; set; }
        public string Cuit { get; set; }
        public string Telefono { get; set; }
        public decimal Saldo { get; set; }
        public decimal SalMax { get; set; }
        public short Grupo { get; set; }
        public char Estado { get; set; }
        public short Lista { get; set; }
        public decimal Consumo { get; set; }
        public decimal Porcemax { get; set; }
        public char Percepcion { get; set; }
        public DateTime Fecha_pib { get; set; }
        public decimal Alicuota { get; set; }
        public int CodPostal { get; set; }
        public char PerTissh { get; set; }
        public decimal AliTissh { get; set; }
        public char Pyme { get; set; }
        public string Email { get; set; }
        public short Repa { get; set; }
        public decimal SaldoCaja { get; set; }
        public bool Bloqueado { get; set; }
        public string CondIva { get{
            if (Civa == 2)
                return "RI";
            else if (Civa == 3)
                return "NI";
            else if (Civa == 4)
                return "MO";
            else if (Civa == 5)
                return "EX";
            else if (Civa == 6)
                return "NR";
            else
                return "CF";
        }}

        public string RepaDes { get; set; }

        public DateTime FechaNacimiento { get; set; }
        public decimal SaldoMutual { get; set; }
        public bool UsadoMutual { get; set; }
        public bool NoPercepIVA { get; set; }

    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Controles
{
    public class PromoExtra
    {    
        
        public string Codpromo { get; set; }        

        public int MarcaCli { get; set; }

        public int MedioPago { get; set; }

        public int NumTarjeta { get; set; }

        public int ExcluCli { get; set; }
        
        public List<string> Evals { get; set; }
        
        public string Filtro { get; set; }
        
        
        public List<string> Acciones { get; set; }
        
        public string Cantidad { get; set; }
        
        public List<MCanasDef> Canastas { get; set; }
        
        public PromoExtra()
        {
            MarcaCli = 0;
            MedioPago = 0;
            NumTarjeta = 0;
            ExcluCli = 0;
            Codpromo = string.Empty;
            Cantidad = string.Empty;
            Filtro = string.Empty;
            Evals = new List<string>();
            Acciones = new List<string>();
            Canastas = new List<MCanasDef>();
        }
    }

    public class LeyenPromo
    {
        private string _codPromo;
        public string CodPromo
        {
            get { return _codPromo; }
            set { _codPromo = value; }
        }
        private string _leyenda;
        public string Leyenda
        {
            get { return _leyenda; }
            set { _leyenda = value; }
        }

        public LeyenPromo()
        {
            CodPromo = "";
            Leyenda = "";
        }
        public LeyenPromo(string pcod, string pleyen)
        {
            CodPromo = pcod;
            Leyenda = pleyen;
        }
    }
}

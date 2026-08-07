using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace Controles
{
    [Serializable]
    public class ItemPos
    {
        public int Cod { get; set; }
        public decimal Cantidad { get; set; }
        public decimal Precio { get; set; }
        public int Id { get; set; }
        public bool EsPesable { get; set; }
    }

    [Serializable]
    public class PedidoPos
    {
        public int Seq { get; set; }
        public int SeqId { get; set; }
        public short IdNumTie { get; set; }
        public short Caja { get; set; }
        public DateTime FhOpera { get; set; }
        public short Estado { get; set; }
        public string Doc { get; set; }
        public string NomCli { get; set; }
        public string DirCli { get; set; }
        public decimal Total { get; set; }
        public short Sync { get; set; }
        public string Tienda { get; set; }
        public DateTime FhCreado { get; set; }
        public DateTime FhEntrega { get; set; }
        public short Cajero { get; set; }
        public List<ItemPos> items { get; set; }

        public decimal getMontoPromoDec()
        {
            decimal aux = 0;
            foreach (var item in this.items)
            {
                if (item.Precio < 0)
                    aux += (item.Precio * item.Cantidad);
            }
            return aux;
        }

        public double getMontoPromoDob()
        {            
            return Convert.ToDouble(this.getMontoPromoDec());
        }
    }
}

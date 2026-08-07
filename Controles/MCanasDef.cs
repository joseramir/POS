using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Controles
{
    public class MCanasDef
    {
        private int _id;
        public int Id
        {
            get { return _id; }
            set { _id = value; }
        }
        private int _cod;
        public int Cod
        {
            get { return _cod; }
            set { _cod = value; }
        }
        private double _cant;
        public double Cant
        {
            get { return _cant; }
            set { _cant = value; }
        }
        private double _venta;
        public double Venta
        {
            get { return _venta; }
            set { _venta = value; }
        }
        private double _ventaMonto;
        public double VentaMonto
        {
            get { return _ventaMonto; }
            set { _ventaMonto = value; }
        }
        private double _precUni;
        public double PrecUni
        {
            get { return _precUni; }
            set { _precUni = value; }
        }

        public MCanasDef()
        {
            this.Id = 0;
            this.Cod = 0;
            this.Cant = 0;
            this.Venta = 0;
            this.VentaMonto = 0;
            this.PrecUni = 0;
        }
    }

    public class MCanasta
    {
        private List<MCanasDef> unacolec;

        public MCanasDef this[int indice]
        {
            get
            {
                return unacolec[indice];
            }
            set
            {
                unacolec[indice] = value;
            }
        }

        public MCanasta()
        {
            unacolec = new List<MCanasDef>();
        }

        public MCanasta GetCanasta(int num)
        {
            MCanasta aux = new MCanasta();
            aux.unacolec = (from r in unacolec
                            where r.Id == num
                            select r).ToList<MCanasDef>();
            return aux;
        }

        public void Agregar(int pid, int pcod, double pcant)
        {
            unacolec.Add(new MCanasDef() { Id = pid, Cod = pcod, Cant = pcant, PrecUni = 0, Venta = 0, VentaMonto = 0 });
        }

        public void Limpiar()
        {
            unacolec.Clear();
        }

        public MCanasDef Encontrar(int pcod)
        {
            MCanasDef aux = null;
            for (int i = 0; i < this.unacolec.Count; i++)
            {
                if (this.unacolec[i].Cod == pcod)
                {
                    aux = this.unacolec[i];
                    break;
                }
            }
            return aux;
        }

        public int Count()
        {
            return this.unacolec.Count;
        }

        public void Reset()
        {
            for (int i = 0; i < this.unacolec.Count; i++)
            {
                this.unacolec[i].Venta = 0;
                //this.unacolec[i].Cant = 0;
                this.unacolec[i].PrecUni = 0;
                this.unacolec[i].VentaMonto = 0;
            }
        }
    }
}

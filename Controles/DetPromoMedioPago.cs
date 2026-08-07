using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Controles
{
    //Para Promocion medio de pago por numero de medio de pago
    public class PromoMedioPago
    {
        public int NumMpago { get; set; }        
        
        public decimal Monto { get; set; }

        public decimal ImporteSinDescto { get; set; }

        public int Descarga { get; set; }

        public bool Confirmado { get; set; }
        

        public int NumTarjeta { get; set; }

        //public bool AplicaInic { get; set; }

        public decimal ImporteIngresado { get; set; }

        public decimal ImporteDescuento { get; set; }

        public double PorcDescto { get; set; }
        
        public PromoMedioPago()
        {
            NumMpago = 0;
            Monto = 0.00M;
            Descarga = 0;
            ImporteSinDescto = 0.00M;
            Confirmado = false;

            NumTarjeta = 0;
            //AplicaInic = true;
            ImporteIngresado = ImporteDescuento = 0.00M;
            PorcDescto = 0;
        }
    }

    public class DetPromoMedioPago
    {
        private List<PromoMedioPago> Medios;

        public DetPromoMedioPago()
        {
            Medios = new List<PromoMedioPago>();
        }

        
        //public void Agregar(int pnum, decimal pmonto, decimal psindescto, int pdescarga)
        //{
        //    bool existe = false;
        //    foreach (PromoMedioPago vitem in this.Medios)
        //    {
        //        if (vitem.NumMpago == pnum)
        //            existe = true;
        //    }
        //    if (!existe)
        //    {
        //        PromoMedioPago aux = new PromoMedioPago() { NumMpago = pnum, Monto = pmonto, Descarga = pdescarga, Confirmado = false, ImporteSinDescto = psindescto };
        //        Medios.Add(aux);
        //    }
        //}

        //verifica de agregar un descuento por medio de pago, siempre habrá un descuento por medio de pago y tarjeta
        public void Agregar(PromoMedioPago item)
        {
            bool existe = false;
            foreach (PromoMedioPago vitem in this.Medios)
            {
                if (vitem.NumMpago == item.NumMpago && vitem.NumTarjeta == item.NumTarjeta && vitem.Descarga == item.Descarga)
                    existe = true;
            }
            if (!existe)
                Medios.Add(item);
        }

        public Decimal GetMontoDescSinConfirmar(int pmediopago)
        {
            decimal aux = 0.00M;
            foreach (PromoMedioPago item in Medios)
            {
                if (item.NumMpago == pmediopago && !item.Confirmado)// && item.NumTarjeta > 0)
                    aux += item.Monto;
            }
            return aux;
        }

        public void SetConfirmaDescto(int pmediopago, double impIngresado)
        {
            for (int i = 0; i < Medios.Count; i++)
            {
                if (Medios[i].NumMpago == pmediopago)
                {
                    Medios[i].Confirmado = true;
                    Medios[i].ImporteIngresado = Convert.ToDecimal(impIngresado);
                }
            }
        }

        public void SetConfirmaDescto(int pmediopago, int pnumtarjeta)
        {
            for (int i = 0; i < Medios.Count; i++)
            {
                if (Medios[i].NumMpago == pmediopago && Medios[i].NumTarjeta == pnumtarjeta)
                    Medios[i].Confirmado = true;
            }
        }

        //public void BorrarDescto(int pmediopago, int pnumtarjeta)
        //{            
        //    for (int i = Medios.Count - 1; i >= 0; i--)
        //    {
        //        if (Medios[i].NumMpago == pmediopago && Medios[i].NumTarjeta == pnumtarjeta)
        //            Medios.RemoveAt(i);
        //    }
        //}

        public void CalculoDescto(int pmediopago, int pnumtarjeta, double montoIng)
        {
            for (int i = 0; i < Medios.Count; i++)
            {
                if (Medios[i].NumMpago == pmediopago && Medios[i].NumTarjeta == pnumtarjeta)
                {
                    //if (Convert.ToDecimal(Medios[i].ImporteIngresado) < Medios[i].ImporteSinDescto)
                    Medios[i].ImporteIngresado = Convert.ToDecimal(montoIng);
                    if (Medios[i].ImporteIngresado < Medios[i].ImporteSinDescto)
                        //Medios[i].Monto = Convert.ToDecimal(montoIng) * Convert.ToDecimal(Medios[i].PorcDescto);
                        Medios[i].Monto = Medios[i].ImporteIngresado * Convert.ToDecimal(Medios[i].PorcDescto);
                    else
                        Medios[i].Monto = Medios[i].ImporteSinDescto * Convert.ToDecimal(Medios[i].PorcDescto);
                }
            }
        }

        public void CalculoDescto(int pmediopago, double pmontoIng)
        {
            CalculoDescto(pmediopago, 0, pmontoIng);
        }

        public void QuitaConfirmaDescto(int pmediopago, int pnumtarjeta)
        {
            for (int i = 0; i < Medios.Count; i++)
            {
                if (Medios[i].NumMpago == pmediopago && Medios[i].NumTarjeta == pnumtarjeta)                    
                    Medios[i].Confirmado = false;                
            }
        }

        public double GetMontoDescSinConfirmarDouble(int pmediopago)
        {
            return GetMontoDescSinConfirmarDouble(pmediopago, 0);
        }

        public double GetMontoDescSinConfirmarDouble(int pmediopago, int pnumtarjeta)
        {
            double aux = 0;
            foreach (PromoMedioPago item in Medios)
            {
                if (item.NumMpago == pmediopago && !item.Confirmado && item.NumTarjeta == pnumtarjeta)
                    aux = Convert.ToDouble(item.Monto);                    
            }
            return aux;
        }

        public double GetImporteSinDescto(int pmediopago)
        {
            double aux = 0;
            foreach (PromoMedioPago item in Medios)
            {
                //se agrego para que controle fijo al medio de pago efectivo
                if ((item.NumMpago == pmediopago && item.NumTarjeta > 0) || (item.NumMpago == pmediopago && pmediopago == 1))                    
                    aux = Convert.ToDouble(item.ImporteSinDescto > 0 ? (item.ImporteSinDescto - item.Monto) : item.Monto);
            }
            return aux;
        }

        public double GetMontoDescConfirmadoDoublePorDesc(int pmediopago, int descarga)
        {
            double aux = 0;
            double miImp = 0;
            double totDescto = 0;
            double miDescto = 0;
            foreach (PromoMedioPago item in Medios)
            {
                //if (item.NumMpago == pmediopago && item.Confirmado && item.ImporteIngresado >= (item.ImporteSinDescto - item.Monto))
                if (item.NumMpago == pmediopago && item.Confirmado)
                {
                    aux += Convert.ToDouble(item.Monto);
                    totDescto += Convert.ToDouble(item.ImporteSinDescto - item.Monto);
                    if (item.Descarga == descarga)
                        miDescto += Convert.ToDouble(item.Monto);
                }
                if (miImp == 0)
                    miImp = Convert.ToDouble(item.ImporteIngresado);
            }
            //if (miImp < totDescto)
            //    aux = 0;
            //else
                aux = miDescto;

            return aux;
        }

        public double GetMontoDescConfirmadoDouble(int pmediopago, int pnumtarjeta)
        {
            double aux = 0;
            foreach (PromoMedioPago item in Medios)
            {
                if (item.NumMpago == pmediopago && item.Confirmado && item.NumTarjeta == pnumtarjeta)                    
                    aux = Convert.ToDouble(item.Monto);
            }
            return aux;
        }

        public void SetMontoDescuentoCero(int pmediopago)
        {
            for (int i = 0; i < Medios.Count; i++)
                if (Medios[i].NumMpago == pmediopago)                
                    Medios[i].Monto = 0;               
                
        }

        public int GetCantidad(int pmediopago)
        {
            int aux = 0;
            for (int i = 0; i < Medios.Count; i++)
                if (Medios[i].NumMpago == pmediopago)
                    aux++;
            return aux;
        }

        public void Clear()
        {
            this.Medios.Clear();
        }

        public int Count()
        {
            return Medios.Count;
        }

        public List<PromoMedioPago> BuscarConTarjeta(int pnumMp, int pnroTarjeta)
        {
            List<PromoMedioPago> aux = null;
            for (int i = 0; i < Medios.Count; i++ )
            {
                PromoMedioPago item = Medios[i];
                if (item.NumMpago == pnumMp && item.NumTarjeta == pnroTarjeta)
                {
                    if (aux == null)
                        aux = new List<PromoMedioPago>();
                    aux.Add(item);
                    //break;
                }
            }
            return aux;
        }
    }



    //public class PromoMpTarjeta
    //{
    //    public int NumPago { get; set; }
    //    public int NumTarjeta { get; set; }
    //    public decimal MontoDescto { get; set; }
    //    public decimal ImpSinDescto { get; set; }
    //    public int Descarga { get; set; }
    //    public bool Confirmado { get; set; }
    //    public decimal ImporteIng { get; set; }

    //    public PromoMpTarjeta()
    //    {
    //        NumPago = 0;
    //        NumTarjeta = 0;
    //        MontoDescto = 0;
    //        ImpSinDescto = 0;
    //        Descarga = 0;
    //        Confirmado = false;
    //        ImporteIng = 0;
    //    }
    //}    
}

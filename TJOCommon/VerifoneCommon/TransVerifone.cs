using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.VerifoneCommon
{
    public class TransVerifone
    {
        public string OperacionTipo { get; set; }
        public string NombreEmisor { get; set; }
        public decimal Importe { get; set; }
        public decimal ImporteDescuento { get; set; }
        public decimal ImporteRecargo { get; set; }
        public decimal ImporteIngresado { get; set; }
        public decimal ImporteCashBack { get; set; }

        public string CodigoRespuesta { get; set; }
        public string MensajeRespuesta { get; set; }
        public string NroAutorizacion { get; set; }
        public int NroLote { get; set; }
        public int NroCupon { get; set; }
        public string NombreCliente { get; set; }
        public string Ultimos4Digitos { get; set; }

        public int AlbNroTarjeta { get; set; }
        public int AlbNroMpago { get; set; }
        public int AlbNumCuota { get; set; }
        public int AlbPorcDesctoCom { get; set; }
        public decimal AlbMontoDescto { get; set; }
        public int AlbEtapa { get; set; }
        public int AlbLongAuto { get; set; }
        //public bool PermiteCashBack { get; set; }

        public decimal MinCashBack { get; set; }
        public decimal MaxCashBack { get; set; }

        public bool TarjCredito { get; set; }
        public int longaut { get; set; }
        public string Vencim { get; set; }
        public string Prim6Digitos { get; set; }

        public string NroTerminal { get; set; }
        public bool QrModo { get; set; }
        //utilizado para identificar el tipo de verifone que se utiliza en la operacion. 1=Prisma,2=FISERV
        public int TipoVerifone { get; set; }


        public string CodigoTarjeta { get; set; }
        public string CodigoPlan { get; set; }
        public string NumeroComercio { get; set; }
        public string NroFactura { get; set; }
        public string FechaTransaccion { get; set; }
        public string HoraTransaccion { get; set; }
        public int Estado { get; set; } //para almacenar el estado de una operacion que se pueda anular

        public int Id { get; set; }
        public bool Cerrado { get; set; }

        public TransVerifone()
        {
            OperacionTipo = "";
            NombreEmisor = "";
            Ultimos4Digitos = "0000";
            Importe = 0;
            NroAutorizacion = "";
            NroLote = 0;
            NroCupon = 0;
            ImporteDescuento = 0;
            CodigoRespuesta = "";
            MensajeRespuesta = "";

            NombreCliente = "";
            AlbNroTarjeta = 0;
            AlbNroMpago = 0;
            AlbNumCuota = 0;
            AlbPorcDesctoCom = 0;
            AlbMontoDescto = 0;
            AlbEtapa = 0;
            AlbLongAuto = 0;
            ImporteCashBack = 0;
            MinCashBack = 0;
            MaxCashBack = 0;

            TarjCredito = false;
            longaut = 0;
            Vencim = "**/**";
            Prim6Digitos = "000000";
            NroTerminal = "0";
            QrModo = false;
            TipoVerifone = 1; //por defecto queda seteado en 1=Prisma
            NroFactura = "";
            FechaTransaccion = "";
            HoraTransaccion = "";
            Estado = -1;

            Id = 0;
            Cerrado = false;
        }

        public void Clear()
        {
            OperacionTipo = "";
            NombreEmisor = "";
            Ultimos4Digitos = "0000";
            Importe = 0;
            NroAutorizacion = "";
            NroLote = 0;
            NroCupon = 0;
            ImporteDescuento = 0;
            CodigoRespuesta = "";
            MensajeRespuesta = "";

            NombreCliente = "";
            AlbNroTarjeta = 0;
            AlbNroMpago = 0;
            AlbNumCuota = 0;
            AlbPorcDesctoCom = 0;
            AlbMontoDescto = 0;
            AlbEtapa = 0;
            AlbLongAuto = 0;
            ImporteCashBack = 0;
            MinCashBack = 0;
            MaxCashBack = 0;
            TarjCredito = false;
            longaut = 0;
            Vencim = "**/**";
            Prim6Digitos = "000000";
            NroTerminal = "0";
            QrModo = false;
            TipoVerifone = 1; //por defecto queda seteado en 1=Prisma
            FechaTransaccion = "";
            HoraTransaccion = "";
            Estado = -1;

            Id = 0;
            Cerrado = false;
        }

        public string GetFormaNroTarjeta()
        {
            return Prim6Digitos + "000000" + Ultimos4Digitos;
        }

        public string ParaTrans()
        {
            string aux = "";
            string numauto = Convert.ToString(this.NroAutorizacion);
            if (AlbLongAuto < numauto.Length)
                numauto = numauto.Substring(numauto.Length - AlbLongAuto, AlbLongAuto);
            numauto = numauto.PadLeft(AlbLongAuto, '0');
            aux = string.Format("{0},{1},{2},{3:00},0.000,0.00,{4:000},{5},{6:0000},{7:00000000},{8},{9},{10},2,{11}",//Va 2 fijo el numero de operacion que sera integrado
                "00" + this.Ultimos4Digitos, "0000", 'V', this.AlbNumCuota,
                this.NroLote, numauto, this.NroCupon, 0, 0,
                (this.ImporteCashBack > 0 ? "SI" : "NO"), this.ImporteCashBack.ToString("F2"), this.NroTerminal);
            return aux;
        }
    }
}

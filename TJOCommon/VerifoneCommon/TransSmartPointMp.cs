using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.VerifoneCommon
{
    public class TransSmartPointMp
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
        //public int NroLote { get; set; }
        //public int NroCupon { get; set; }
        //public string NombreCliente { get; set; }
        //public string Ultimos4Digitos { get; set; }

        public int AlbNroTarjeta { get; set; }
        public int AlbNroMpago { get; set; }
        public int AlbNumCuota { get; set; }
        public int AlbPorcDesctoCom { get; set; }
        public decimal AlbMontoDescto { get; set; }

        public int AlbEtapa { get; set; }
        //public int AlbLongAuto { get; set; }
        //public bool PermiteCashBack { get; set; }

        public decimal MinCashBack { get; set; }
        public decimal MaxCashBack { get; set; }

        public bool TarjCredito { get; set; }
        //public int longaut { get; set; }
        //public string Vencim { get; set; }
        //public string Prim6Digitos { get; set; }

        //public string NroTerminal { get; set; }
        public bool QrModo { get; set; }
        //utilizado para identificar el tipo de verifone que se utiliza en la operacion. 1=Prisma,2=FISERV, 3=SMART POINT MP
        public int TipoVerifone { get; set; }

        public string ExternalReference { get; set; }
        public string IdPayment { get; set; }
        public string IdOrdenMercadoPago { get; set; }
        //public string CodigoTarjeta { get; set; }
        //public string CodigoPlan { get; set; }
        //public string NumeroComercio { get; set; }
        //public string NroFactura { get; set; }
        //public string FechaTransaccion { get; set; }
        //public string HoraTransaccion { get; set; }
        //public int Estado { get; set; } //para almacenar el estado de una operacion que se pueda anular

        //public int Id { get; set; }
        //public bool Cerrado { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Alberdi.TJOCommon.Prisma
{
    public class RespuestaBase
    {
        public string FechaTransaccion { get; set; }
        public string HoraTransaccion { get; set; }
        public int TerminalId { get; set; }

        public RespuestaBase()
        {
            FechaTransaccion = "";
            HoraTransaccion = "";
            TerminalId = 0;
        }
    }

    public class RtaVenta : RespuestaBase
    {
        public int CodigoRespuesta { get; set; }
        public string MensajeRespuesta { get; set; }
        public string CodigoAutorizacion { get; set; }
        public int NumeroCupon { get; set; }
        public int NumeroLote { get; set; }
        public string NombreCliente { get; set; }
        public string Ult4Digitos { get; set; }
        public string Prim6Digitos { get; set; }
        public bool Cerrado { get; set; }

        public RtaVenta()
            : base()
        {
            CodigoRespuesta = 0;
            MensajeRespuesta = "";
            CodigoAutorizacion = "";
            NumeroCupon = 0;
            NumeroLote = 0;
            NombreCliente = "";
            Ult4Digitos = "";
            Prim6Digitos = "";
            Cerrado = false;
        }
    }

    public class RtaTrans : RtaVenta
    {
        public int TipoTransaccion { get; set; }

        public RtaTrans()
            : base()
        {
            TipoTransaccion = 0;
        }
    }

    public class RtaQrModo : RtaVenta
    {
        public string CodigoTarjeta { get; set; }
        public decimal ImporteTotal { get; set; }
        public decimal ImporteCobrado { get; set; }

        public RtaQrModo()
            : base()
        {
            CodigoTarjeta = "";
            ImporteTotal = 0;
            ImporteCobrado = 0;
        }
    }

    public class RtaTest : RespuestaBase
    {
        public string Respuesta { get; set; }
    }

    public class RtaCierre : RespuestaBase
    {
        public int CodigoRespuesta { get; set; }
    }

    public class RtaTablaTarjeta : RespuestaBase
    {
        public int IndiceRegistro { get; set; }
        public string CodigoProcesador { get; set; }
        public string CodigoTarjeta { get; set; }
        public string NombreTarjeta { get; set; }
        public int CantidadCuotas { get; set; }
    }

    public class RtaTablaPlan : RespuestaBase
    {
        public int IndiceRegistro { get; set; }
        public string CodigoTarjeta { get; set; }
        public string CodigoPlan { get; set; }
        public string NombrePlan { get; set; }
    }

    public class RtaUltCierre : RespuestaBase
    {
        public int IndiceRegistro { get; set; }
        public string CodigoProcesador { get; set; }
        public int NumeroLote { get; set; }
        public string CodigoTarjeta { get; set; }
        public int CantidadVentas { get; set; }
        public decimal MontoTotalVentas { get; set; }
        public int CantidadAnulaVenta { get; set; }
        public decimal MontoTotalAnula { get; set; }
        public int CantidadDevolVenta { get; set; }
        public decimal MontoTotalDevol { get; set; }
        public int CantidadAnulaDevol { get; set; }
        public decimal MontoTotalAnuDev { get; set; }

        public string FechaCierre { get; set; }
        public string HoraCierre { get; set; }
    }
}

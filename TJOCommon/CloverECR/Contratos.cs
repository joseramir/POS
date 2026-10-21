using System.Collections.Generic;
using System.Web.Script.Serialization;

namespace Alberdi.TJOCommon.CloverECR
{
    // Copiado de CloverIntegracion\src\CloverECR.ClientePos (Contratos.cs); solo cambia el namespace.
    // Contrato con CloverECR (api/v1/pos/clover), del lado de la caja .NET 3.5.
    // Montos en pesos (decimal). Las fechas llegan como texto ISO (JavaScriptSerializer no lee ISO 8601).

    /// <summary>
    /// Resultado mínimo de CloverECR: 202 de los POST y todos los errores.
    /// </summary>
    public class RespuestaPos
    {
        public string IdOperacion { get; set; }
        public string Estado { get; set; }
        public string Mensaje { get; set; }

        /// <summary>Status HTTP de la respuesta (202, 400, 401, 404, 409, 503...).</summary>
        [ScriptIgnore] public int HttpStatus { get; set; }
        [ScriptIgnore] public bool Aceptada { get { return HttpStatus == 202; } }
    }

    public class PagoRequest
    {
        public string NumeroCajero { get; set; }
        /// <summary>Ticket local de la caja. Además sirve para recuperar la operación si el POST se queda sin respuesta.</summary>
        public string EcrTransactionId { get; set; }
        public string PuntoVentaFiscal { get; set; }
        public string NumeroComprobanteFiscal { get; set; }
        public decimal MontoOperacion { get; set; }
        public decimal MontoExtracash { get; set; }
        public int NumeroCuotas { get; set; }
        /// <summary>VI, MC... null = cualquiera. "RQ" = QR (el cliente elige "QR PosNet" en el Flex).</summary>
        public string MarcaTarjeta { get; set; }

        public PagoRequest()
        {
            NumeroCuotas = 1;
        }
    }

    public class EstadoPago : RespuestaPos
    {
        /// <summary>TARJETA | QR</summary>
        public string MedioPago { get; set; }
        public decimal Monto { get; set; }
        public decimal MontoExtracash { get; set; }
        public int NumeroCuotas { get; set; }
        public string MarcaTarjeta { get; set; }
        public string Prim6Digitos { get; set; }
        public string Ult4Digitos { get; set; }
        public string CodigoAutorizacion { get; set; }
        public string NumeroLote { get; set; }
        public string NumeroCupon { get; set; }
        public string Billetera { get; set; }
        public string ClaveBusq { get; set; }
        public bool ReciboImpreso { get; set; }
        /// <summary>Terminal Fiserv que informa Clover; en TARJETA hoy puede venir null.</summary>
        public string NumeroTerminal { get; set; }
        /// <summary>Numero de serie del Flex que proceso la operacion.</summary>
        public string NumeroSerieFlex { get; set; }
    }

    public class VentaParaReversion : RespuestaPos
    {
        public string ClaveBusq { get; set; }
        public string MedioPago { get; set; }
        public string Billetera { get; set; }
        public decimal Monto { get; set; }
        public decimal MontoDevuelto { get; set; }
        public decimal MontoDisponible { get; set; }
        public int NumeroCuotas { get; set; }
        public string MarcaTarjeta { get; set; }
        public string Prim6Digitos { get; set; }
        public string Ult4Digitos { get; set; }
        public string CodigoAutorizacion { get; set; }
        public string NumeroLote { get; set; }
        public string NumeroCupon { get; set; }
        public int NumeroCaja { get; set; }
        public string FechaOperacion { get; set; }
        public bool LoteCerrado { get; set; }
        /// <summary>Terminal Fiserv que informa Clover; en TARJETA hoy puede venir null.</summary>
        public string NumeroTerminal { get; set; }
        /// <summary>Numero de serie del Flex que proceso la operacion.</summary>
        public string NumeroSerieFlex { get; set; }
    }

    public class EstadoOperacion : RespuestaPos
    {
        /// <summary>VENTA | ANULACION | DEVOLUCION</summary>
        public string TipoOperacion { get; set; }
        public string IdOperacionOrigen { get; set; }
        public decimal Monto { get; set; }
        public string NumeroLote { get; set; }
        public string NumeroCupon { get; set; }
        public bool ReciboImpreso { get; set; }
        public string EstadoVenta { get; set; }
        public decimal? MontoDevueltoVenta { get; set; }
        /// <summary>Terminal Fiserv que informa Clover; en TARJETA hoy puede venir null.</summary>
        public string NumeroTerminal { get; set; }
        /// <summary>Numero de serie del Flex que proceso la operacion.</summary>
        public string NumeroSerieFlex { get; set; }
    }

    public class Resumen : RespuestaPos
    {
        public string Fecha { get; set; }
        public int NumeroSucursal { get; set; }
        public int NumeroCaja { get; set; }
        /// <summary>CLOVER (leído de Clover) o CLOVERECR (Clover no respondió).</summary>
        public string Fuente { get; set; }
        public List<ResumenGrupo> Grupos { get; set; }
        public ResumenGrupo Total { get; set; }
        public int CantidadAnulados { get; set; }
        public decimal MontoAnulados { get; set; }

        public Resumen()
        {
            Grupos = new List<ResumenGrupo>();
            Total = new ResumenGrupo();
        }
    }

    public class ResumenGrupo
    {
        public string NumeroLote { get; set; }
        public string MedioPago { get; set; }
        public string Marca { get; set; }
        public int Cantidad { get; set; }
        public decimal Ventas { get; set; }
        public decimal Devoluciones { get; set; }
        public decimal Neto { get; set; }
    }

    public class Transaccion
    {
        public string IdOperacion { get; set; }
        public string TipoOperacion { get; set; }
        public string IdOperacionOrigen { get; set; }
        public string Estado { get; set; }
        public string Mensaje { get; set; }
        public decimal Monto { get; set; }
        public decimal MontoDevuelto { get; set; }
        public int NumeroCuotas { get; set; }
        public string MedioPago { get; set; }
        public string Billetera { get; set; }
        public string MarcaTarjeta { get; set; }
        public string Ult4Digitos { get; set; }
        public string CodigoAutorizacion { get; set; }
        public string NumeroLote { get; set; }
        public string NumeroCupon { get; set; }
        public string ClaveBusq { get; set; }
        public string NumeroCajero { get; set; }
        public string EcrTransactionId { get; set; }
        public bool ReciboImpreso { get; set; }
        public string Fecha { get; set; }
        /// <summary>Terminal Fiserv que informa Clover; en TARJETA hoy puede venir null.</summary>
        public string NumeroTerminal { get; set; }
        /// <summary>Numero de serie del Flex que proceso la operacion.</summary>
        public string NumeroSerieFlex { get; set; }
    }

    public static class Estados
    {
        public const string Pendiente   = "PENDIENTE";
        public const string Verificando = "VERIFICANDO";
        public const string Aprobada    = "APROBADA";
        public const string Rechazada   = "RECHAZADA";
        public const string Error       = "ERROR";
        public const string Anulada     = "ANULADA";
        public const string Devuelta    = "DEVUELTA";

        /// <summary>PENDIENTE y VERIFICANDO no son finales: hay que seguir consultando.</summary>
        public static bool EsFinal(string estado)
        {
            return estado != Pendiente && estado != Verificando;
        }
    }
}

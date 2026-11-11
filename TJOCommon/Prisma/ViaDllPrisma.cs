using System;
using System.Collections.Generic;
using System.Configuration;
using System.Globalization;

namespace Alberdi.TJOCommon.Prisma
{
    //   Opera el Newland por la DLL oficial de Prisma (VpiPc_32.dll, wrapper en Vpipc.cs) en lugar del
    //   protocolo serie propio. OperarTrxPrisma la usa cuando el app.config de la caja tiene
    //   ModoNewland = "dll"; sin esa llave (o con "serie") todo sigue por el puerto serie como siempre.
    //   Devuelve las mismas respuestas que arma ProtocoloTrxPrisma (RtaVenta, RtaQrModo, RtaCierre...),
    //   asi los formularios C++ y MsgAlertPrisma no cambian.
    //
    //   La DLL lee VpiPC.ini y escribe vpiPC.log en el directorio actual del proceso (bin de la caja).
    internal class ViaDllPrisma
    {
        //   Venta, QR, anulacion y devolucion: lo que la DLL espera a la terminal (tarjeta, PIN, host).
        private const int TIMEOUT_OPERACION = 90;
        //   Anulacion y devolucion: la terminal pide la tarjeta y despues espera al host. Con 90 s la DLL
        //   cortaba antes que la terminal (prueba del 13/10: pidio la tarjeta, se inserto y a los 90 s
        //   909). A diferencia de la venta, para estas la DLL SI respeta el timeout que se le pasa.
        private const int TIMEOUT_ANUL_DEV = 180;
        private const int TIMEOUT_CIERRE = 120;
        //   Tope de vueltas para las consultas de varios registros (tarjetas, planes, ultimo cierre).
        private const int MAX_REGISTROS = 500;

        //   La DLL tiene estado global (un solo puerto abierto por proceso): una operacion a la vez.
        private static readonly object bloqueo = new object();

        private readonly string puerto;
        private readonly Action<string> log;

        public bool ConError;
        public string MensajeError;

        public static bool Habilitada()
        {
            string modo = ConfigurationManager.AppSettings["ModoNewland"];
            return modo != null && modo.Trim().ToLower() == "dll";
        }

        public ViaDllPrisma(int portNewland, Action<string> log)
        {
            puerto = "COM" + (portNewland + 1).ToString();
            this.log = log;
            ConError = false;
            MensajeError = "";
        }

        //   Abre el puerto, ejecuta y lo cierra siempre. Ninguna excepcion sale de aca (la DLL que no
        //   esta en bin, un campo con formato inesperado): todo termina en ConError.
        private T Operar<T>(string titulo, Func<VpiPos, T> operacion) where T : class
        {
            ConError = false;
            MensajeError = "";
            log(titulo + " - POR DLL " + puerto);
            lock (bloqueo)
            {
                VpiPos pos = new VpiPos();
                try
                {
                    int rc = pos.Open(puerto);
                    if (rc != VpiCodes.OK)
                    {
                        //   Si un corte anterior dejo el puerto abierto dentro de la DLL, el open falla:
                        //   se cierra y se reintenta una vez.
                        log("vpiOpenPort = " + rc + ", se cierra y se reintenta");
                        pos.Close();
                        rc = pos.Open(puerto);
                    }
                    if (rc != VpiCodes.OK)
                    {
                        Error("Verificar la conexion al puerto " + puerto + " (" + VpiCodes.Describe(rc) + ")");
                        return null;
                    }
                    return operacion(pos);
                }
                catch (Exception ex)
                {
                    log("ERROR EX: " + ex.Message + "\n" + ex.StackTrace);
                    Error("Error al operar con la DLL del Newland: " + ex.Message);
                    return null;
                }
                finally
                {
                    try { pos.Dispose(); }
                    catch { }
                }
            }
        }

        private void Error(string mensaje)
        {
            ConError = true;
            MensajeError = mensaje;
            log(mensaje);
        }

        private void ErrorDeRetorno(string operacion, int rc)
        {
            string mensaje = "NEWLAND: " + VpiCodes.Describe(rc) + " (" + rc + ")";
            if (rc == VpiCodes.TIMEOUT_EXP || rc == VpiCodes.GENERAL_FAIL || rc == VpiCodes.FAIL)
                mensaje += "\nVERIFICAR LA PANTALLA DEL EQUIPO";
            Error(operacion + " - " + mensaje);
        }

        // ---------------- Test ----------------

        public RtaTest Test()
        {
            return Operar<RtaTest>("Test", pos =>
            {
                int rc = pos.TestConnection();
                log("vpiTestConnection = " + rc);
                //   El manual dice que devuelve 1 = POS legacy / 2 = N910, pero la DLL 3.1.0.1 devuelve
                //   el resultado del protocolo: 0 (VPI_OK) si la terminal contesto el TES. El codigo
                //   del equipo ("Code response: 2") solo queda en vpiPC.log. Probado el 10/10 con el
                //   Newland de prueba.
                if (rc != VpiCodes.OK)
                {
                    ErrorDeRetorno("Test", rc);
                    return null;
                }
                //   Los formularios esperan "002", lo mismo que contesta el TES por serie.
                RtaTest rta = new RtaTest();
                rta.Respuesta = "002";
                return rta;
            });
        }

        // ---------------- Venta con tarjeta ----------------

        public RtaVenta Venta(TrxPrisma trx)
        {
            bool extraCash = trx.MontoExtraCash > 0;
            return Operar<RtaVenta>(extraCash ? "Venta con extracash" : "Venta", pos =>
            {
                VpiNative.PurchaseIn e = new VpiNative.PurchaseIn();
                e.amount = Centavos(trx.Monto);
                e.receiptNumber = trx.NroFactura;
                e.instalmentCount = trx.CantCuotas.ToString("00");
                e.issuerCode = trx.CodigoTarjeta;
                e.planCode = trx.CodigoPlan;
                //   En las variantes ExtraCash el campo tip lleva el monto de la extraccion.
                e.tip = Centavos(trx.MontoExtraCash);
                e.merchantCode = Recorta(trx.CodigoComercio, 15);
                e.merchantName = Recorta(trx.NombreComercio, 23);
                e.cuit = Recorta(trx.CuitComercio, 23);
                e.linemode = LineModeDll(trx.LineMode);
                log(string.Format("Entrada: monto={0} factura={1} cuotas={2} tarjeta={3} plan={4} extracash={5} comercio='{6}'",
                    e.amount, e.receiptNumber, e.instalmentCount, e.issuerCode, e.planCode, e.tip, e.merchantCode));

                //   Igual que por serie: sin consulta automatica de la ultima transaccion antes ni
                //   despues (11/11, pedido para mandar logs limpios a Prisma). Un error queda como error.
                VpiTrxResult r = extraCash ? pos.PurchaseExtraCash(e, TIMEOUT_OPERACION) : pos.Purchase(e, TIMEOUT_OPERACION);
                log((extraCash ? "vpiPurchaseExtraCash: " : "vpiPurchase: ") + r);
                if (r.ReturnCode != VpiCodes.OK)
                {
                    ErrorDeRetorno("Venta", r.ReturnCode);
                    return null;
                }
                RtaVenta rta = new RtaVenta();
                CargaRtaVenta(rta, r);
                return rta;
            });
        }

        // ---------------- Venta con QR ----------------

        public RtaQrModo Qr(TrxPrisma trx)
        {
            return Operar<RtaQrModo>("Venta con QR", pos =>
            {
                int rc;
                string monto = Centavos(trx.Monto);
                log(string.Format("Entrada: monto={0} cuotas={1} plan={2}", monto, trx.CantCuotas.ToString("00"), trx.CodigoPlan));
                string[] o = pos.GetQrz(monto, trx.CantCuotas.ToString("00"), trx.CodigoPlan, TIMEOUT_OPERACION, out rc);
                log("vpiGetQRZ = " + rc + " [" + string.Join("|", o) + "]");
                if (rc != VpiCodes.OK)
                {
                    ErrorDeRetorno("Venta con QR", rc);
                    return null;
                }
                //   QRZ_OUT: respcode, respMsg, authCod, cuponNmb, loteNmb, lastFour, firstSix,
                //   trxDate, trxHr, terminalID, cardCod, impTotal, impCobrado.
                RtaQrModo rta = new RtaQrModo();
                rta.CodigoRespuesta = CodigoHost(o[0]);
                rta.MensajeRespuesta = o[1];
                if (rta.CodigoRespuesta == 0)
                {
                    rta.CodigoAutorizacion = o[2];
                    rta.NumeroCupon = Entero(o[3]);
                    rta.NumeroLote = Entero(o[4]);
                    rta.NombreCliente = "";
                    rta.Ult4Digitos = o[5];
                    rta.Prim6Digitos = o[6];
                    rta.FechaTransaccion = o[7];
                    rta.HoraTransaccion = o[8];
                    rta.TerminalId = Entero(o[9]);
                    rta.CodigoTarjeta = o[10];
                    rta.ImporteTotal = Importe(o[11]);
                    rta.ImporteCobrado = Importe(o[12]);
                }
                return rta;
            });
        }

        // ---------------- Anulacion y devolucion ----------------

        public RtaVenta Anulacion(TrxPrisma trxOrig, RtaVenta rtaOrig)
        {
            return Operar<RtaVenta>("Anulacion", pos =>
            {
                VpiNative.VoidIn e = new VpiNative.VoidIn();
                e.originalTicket = rtaOrig.NumeroCupon.ToString().PadLeft(7, '0');
                e.issuerCode = trxOrig.CodigoTarjeta;
                e.merchantName = Recorta(trxOrig.NombreComercio, 23);
                e.cuit = Recorta(trxOrig.CuitComercio, 23);
                log(string.Format("Entrada: cupon={0} tarjeta={1}", e.originalTicket, e.issuerCode));

                //   Igual que la venta: sin consulta automatica de la ultima transaccion.
                VpiTrxResult r = pos.Void(e, TIMEOUT_ANUL_DEV);
                log("vpiVoid: " + r);
                if (r.ReturnCode != VpiCodes.OK)
                {
                    ErrorDeRetorno("Anulacion", r.ReturnCode);
                    return null;
                }
                RtaVenta rta = new RtaVenta();
                CargaRtaVenta(rta, r);
                return rta;
            });
        }

        public RtaVenta Devolucion(TrxPrisma trxOrig, RtaVenta rtaOrig)
        {
            return Operar<RtaVenta>("Devolucion", pos =>
            {
                VpiNative.RefundIn e = new VpiNative.RefundIn();
                e.amount = Centavos(trxOrig.Monto);
                e.instalmentCount = trxOrig.CantCuotas.ToString("00");
                e.issuerCode = trxOrig.CodigoTarjeta;
                //   Manual: sin plan va un espacio en blanco.
                e.planCode = string.IsNullOrEmpty(trxOrig.CodigoPlan) ? " " : trxOrig.CodigoPlan;
                e.originalTicket = rtaOrig.NumeroCupon.ToString().PadLeft(7, '0');
                //   "DD/MM/AAAA", tal como la guardo la venta original.
                e.originalDate = rtaOrig.FechaTransaccion;
                e.receiptNumber = trxOrig.NroFactura;
                e.merchantCode = Recorta(trxOrig.CodigoComercio, 15);
                e.merchantName = Recorta(trxOrig.NombreComercio, 23);
                e.cuit = Recorta(trxOrig.CuitComercio, 23);
                e.linemode = LineModeDll(trxOrig.LineMode);
                log(string.Format("Entrada: monto={0} cuotas={1} tarjeta={2} plan='{3}' cupon={4} fecha={5} factura={6}",
                    e.amount, e.instalmentCount, e.issuerCode, e.planCode, e.originalTicket, e.originalDate, e.receiptNumber));

                //   Igual que la venta: sin consulta automatica de la ultima transaccion.
                VpiTrxResult r = pos.Refund(e, TIMEOUT_ANUL_DEV);
                log("vpiRefund: " + r);
                if (r.ReturnCode != VpiCodes.OK)
                {
                    ErrorDeRetorno("Devolucion", r.ReturnCode);
                    return null;
                }
                RtaVenta rta = new RtaVenta();
                CargaRtaVenta(rta, r);
                return rta;
            });
        }

        // ---------------- Cierre de lote ----------------

        public RtaCierre Cierre()
        {
            return Operar<RtaCierre>("Cierre de lote", pos =>
            {
                VpiBatchCloseResult r = pos.BatchClose(TIMEOUT_CIERRE);
                log(string.Format("vpiBatchClose = {0} host={1} fecha={2} {3} term={4}",
                    r.ReturnCode, r.HostResponseCode, r.Date, r.Time, r.TerminalId));
                if (r.ReturnCode != VpiCodes.OK)
                {
                    ErrorDeRetorno("Cierre de lote", r.ReturnCode);
                    return null;
                }
                RtaCierre rta = new RtaCierre();
                rta.CodigoRespuesta = CodigoHost(r.HostResponseCode);
                rta.FechaTransaccion = r.Date;
                rta.HoraTransaccion = r.Time;
                rta.TerminalId = Entero(r.TerminalId);
                return rta;
            });
        }

        // ---------------- Consultas ----------------

        //   Las consultas de varios registros se recorren desde el indice 0: rc 1 = dato valido y hay
        //   mas, rc 0 = dato valido y es el ultimo, cualquier otro = no hay (o error si es el primero).
        public List<RtaTablaTarjeta> Tarjetas()
        {
            return Operar<List<RtaTablaTarjeta>>("Obtiene tarjetas", pos =>
            {
                List<RtaTablaTarjeta> lista = new List<RtaTablaTarjeta>();
                for (ushort i = 0; i < MAX_REGISTROS; i++)
                {
                    VpiIssuer r = pos.GetIssuer(i);
                    log(string.Format("vpiGetIssuer({0}) = {1} {2} {3} '{4}' cuotas={5}",
                        i, r.ReturnCode, r.AcquirerCode, r.IssuerCode, r.IssuerName, r.MaxInstalments));
                    if (r.ReturnCode != VpiCodes.OK && r.ReturnCode != VpiCodes.MORE_REC)
                    {
                        if (i == 0)
                            ErrorDeRetorno("Obtiene tarjetas", r.ReturnCode);
                        break;
                    }
                    RtaTablaTarjeta t = new RtaTablaTarjeta();
                    t.IndiceRegistro = r.Index;
                    t.CodigoProcesador = r.AcquirerCode;
                    t.CodigoTarjeta = r.IssuerCode;
                    t.NombreTarjeta = r.IssuerName;
                    t.CantidadCuotas = Entero(r.MaxInstalments);
                    t.TerminalId = Entero(r.TerminalId);
                    t.FechaTransaccion = DateTime.Now.ToString("dd/MM/yyyy");
                    t.HoraTransaccion = DateTime.Now.ToString("HH:mm:ss");
                    lista.Add(t);
                    if (r.ReturnCode == VpiCodes.OK)
                        break;
                }
                return lista;
            });
        }

        public List<RtaTablaPlan> Planes()
        {
            return Operar<List<RtaTablaPlan>>("Obtiene planes", pos =>
            {
                List<RtaTablaPlan> lista = new List<RtaTablaPlan>();
                for (ushort i = 0; i < MAX_REGISTROS; i++)
                {
                    VpiPlan r = pos.GetPlan(i);
                    log(string.Format("vpiGetPlan({0}) = {1} {2} {3} '{4}'",
                        i, r.ReturnCode, r.IssuerCode, r.PlanCode, r.PlanLabel));
                    if (r.ReturnCode != VpiCodes.OK && r.ReturnCode != VpiCodes.MORE_REC)
                    {
                        if (i == 0)
                            ErrorDeRetorno("Obtiene planes", r.ReturnCode);
                        break;
                    }
                    RtaTablaPlan p = new RtaTablaPlan();
                    p.IndiceRegistro = r.Index;
                    p.CodigoTarjeta = r.IssuerCode;
                    p.CodigoPlan = r.PlanCode;
                    p.NombrePlan = r.PlanLabel;
                    p.TerminalId = Entero(r.TerminalId);
                    p.FechaTransaccion = DateTime.Now.ToString("dd/MM/yyyy");
                    p.HoraTransaccion = DateTime.Now.ToString("HH:mm:ss");
                    lista.Add(p);
                    if (r.ReturnCode == VpiCodes.OK)
                        break;
                }
                return lista;
            });
        }

        //   Por serie el ULC tampoco guarda nada: solo queda en el log.
        public List<VpiBatchCloseData> UltimoCierre()
        {
            return Operar<List<VpiBatchCloseData>>("Obtener ultimo cierre", pos =>
            {
                List<VpiBatchCloseData> lista = new List<VpiBatchCloseData>();
                for (ushort i = 0; i < MAX_REGISTROS; i++)
                {
                    VpiBatchCloseData r = pos.GetBatchCloseData(i);
                    log(string.Format("vpiGetBatchCloseData({0}) = {1} lote={2} tarjeta={3} ventas={4} ${5} anul={6} ${7} dev={8} ${9}",
                        i, r.ReturnCode, r.BatchNumber, r.IssuerCode, r.PurchaseCount, r.PurchaseAmount,
                        r.VoidCount, r.VoidAmount, r.RefundCount, r.RefundAmount));
                    if (r.ReturnCode != VpiCodes.OK && r.ReturnCode != VpiCodes.MORE_REC)
                    {
                        if (i == 0)
                            ErrorDeRetorno("Obtener ultimo cierre", r.ReturnCode);
                        break;
                    }
                    lista.Add(r);
                    if (r.ReturnCode == VpiCodes.OK)
                        break;
                }
                return lista;
            });
        }

        public RtaTrans UltimaTrans()
        {
            return Operar<RtaTrans>("Obtiene ultima operacion", pos =>
            {
                ushort tipo;
                VpiTrxResult r = pos.GetLastTrxData(out tipo);
                log("vpiGetLastTrxData: tipo=" + tipo + " " + r);
                if (r.ReturnCode != VpiCodes.OK)
                {
                    ErrorDeRetorno("Ultima operacion", r.ReturnCode);
                    return null;
                }
                RtaTrans rta = new RtaTrans();
                //   1 venta, 2 anulacion, 3 devolucion.
                rta.TipoTransaccion = tipo;
                CargaRtaVenta(rta, r);
                return rta;
            });
        }

        // ---------------- Reimpresiones ----------------

        public RespuestaBase ReimpreUltTrans()
        {
            return Operar<RespuestaBase>("Reimprime ultima transaccion", pos => Reimpresion("vpiPrintTicket", pos.PrintTicket()));
        }

        public RespuestaBase ReimpreUltCierre()
        {
            return Operar<RespuestaBase>("Reimprime ultimo cierre", pos => Reimpresion("vpiPrintBatchClose", pos.PrintBatchClose()));
        }

        private RespuestaBase Reimpresion(string funcion, int rc)
        {
            log(funcion + " = " + rc);
            if (rc != VpiCodes.OK)
            {
                ErrorDeRetorno("Reimpresion", rc);
                return null;
            }
            return new RespuestaBase();
        }

        // ---------------- Conversiones ----------------

        //   Aprobada SOLO con "00", igual que por serie. 11 y 85 (aprobadas segun el manual) y 10
        //   (aprobada parcial) quedan como rechazo: decision del 30/09. Un codigo no numerico ("BB")
        //   tambien es rechazo.
        private static int CodigoHost(string codigo)
        {
            int valor;
            if (codigo == "00")
                return 0;
            if (int.TryParse(codigo, out valor) && valor != 0)
                return valor;
            return -1;
        }

        private static void CargaRtaVenta(RtaVenta rta, VpiTrxResult r)
        {
            rta.CodigoRespuesta = CodigoHost(r.HostResponseCode);
            rta.MensajeRespuesta = r.HostMessage;
            if (rta.CodigoRespuesta == 0)
            {
                rta.CodigoAutorizacion = r.AuthCode;
                rta.NumeroCupon = Entero(r.TicketNumber);
                rta.NumeroLote = Entero(r.BatchNumber);
                rta.NombreCliente = r.CustomerName;
                rta.Ult4Digitos = r.PanLast4;
                rta.Prim6Digitos = r.PanFirst6;
                rta.FechaTransaccion = r.Date;
                rta.HoraTransaccion = r.Time;
                rta.TerminalId = Entero(r.TerminalId);
            }
        }

        //   Online = el CARACTER '1' (0x31), como la app oficial de Prisma (vpiPCAPP.log del 11/11) y el
        //   manual de la DLL ("char linemode; Online(1)"). Hasta el 11/11 se mandaba el byte 0x01 del
        //   protocolo serie: con 0x01 la venta EL $1.500,05 cerraba con LRC 0x03 y la terminal no daba ACK;
        //   con '1' el LRC se corre en 0x30 y no puede dar 0x03. Por serie sigue yendo 0x01.
        private static byte LineModeDll(int lineMode)
        {
            return lineMode == 1 ? (byte)'1' : (byte)lineMode;
        }

        private static string Centavos(decimal monto)
        {
            return Convert.ToInt64(monto * 100).ToString(CultureInfo.InvariantCulture);
        }

        private static decimal Importe(string centavos)
        {
            decimal valor;
            if (decimal.TryParse(centavos, NumberStyles.Integer, CultureInfo.InvariantCulture, out valor))
                return valor / 100;
            return 0;
        }

        private static int Entero(string texto)
        {
            int valor;
            return int.TryParse(texto, NumberStyles.Integer, CultureInfo.InvariantCulture, out valor) ? valor : 0;
        }

        private static string Recorta(string texto, int largo)
        {
            if (texto == null)
                return "";
            texto = texto.Trim();
            return texto.Length > largo ? texto.Substring(0, largo) : texto;
        }
    }
}

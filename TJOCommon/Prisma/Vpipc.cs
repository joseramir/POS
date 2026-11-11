// ============================================================================
//  VpiPc.cs  -  Wrapper C# (.NET Framework 3.5, C# 3) para VpiPc_32.dll
//  VpiPcModule - Common 32 Bits  v3.1.0.1  (Payway / LAPOS - protocolo "Common_1_0_2")
//
//  Firmas y layouts reconstruidos por análisis del binario (exports + desensamblado).
//  REQUISITOS:
//    * Proyecto con Platform target = x86  (la DLL es de 32 bits; con AnyCPU en un
//      Windows de 64 bits da BadImageFormatException).
//    * VpiPc_32.dll junto al .exe.
//    * La DLL lee ".\VpiPC.ini" y escribe ".\vpiPC.log" relativos al DIRECTORIO ACTUAL
//      del proceso (no al del exe). Ver VpiPos.UseExeDirectoryAsCurrent().
//    * Todas las funciones son bloqueantes (hasta 'timeout' segundos): no llamarlas
//      desde el hilo de UI y no hacer dos llamadas en paralelo.
// ============================================================================
using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Alberdi.TJOCommon.Prisma
{
    // ------------------------------------------------------------------------
    //  Capa 1: P/Invoke crudo. Todas las funciones son __stdcall y devuelven un
    //  código de 16 bits (WORD).
    // ------------------------------------------------------------------------
    public static class VpiNative
    {
        public const string Dll = "VpiPc_32.dll";

        /// <summary>vpiOpenPort. Layout: char* com @0, WORD baud @4, WORD byteSize @6, char parity @8, WORD stopBits @10.</summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ComParams
        {
            [MarshalAs(UnmanagedType.LPStr)]
            public string com;   // "COM1"
            public ushort baudRate;                                // 19200
            public ushort byteSize;                                // 8
            public byte parity;                                    // (byte)'N'
            public ushort stopBits;                                // 1
        }

        /// <summary>Entrada de vpiPurchase / vpiPurchaseMark / vpiPurchaseExtraCash(Mark).
        /// En las variantes ExtraCash el campo 'tip' lleva el monto de extracción.</summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct PurchaseIn
        {
            [MarshalAs(UnmanagedType.LPStr)]
            public string amount;          // @0
            [MarshalAs(UnmanagedType.LPStr)]
            public string receiptNumber;   // @4
            [MarshalAs(UnmanagedType.LPStr)]
            public string instalmentCount; // @8
            [MarshalAs(UnmanagedType.LPStr)]
            public string issuerCode;      // @12
            [MarshalAs(UnmanagedType.LPStr)]
            public string planCode;        // @16
            [MarshalAs(UnmanagedType.LPStr)]
            public string tip;             // @20 (extraCash)
            [MarshalAs(UnmanagedType.LPStr)]
            public string merchantCode;    // @24
            [MarshalAs(UnmanagedType.LPStr)]
            public string merchantName;    // @28
            [MarshalAs(UnmanagedType.LPStr)]
            public string cuit;            // @32
            public byte linemode;                                           // @36 (char)
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct RefundIn
        {
            [MarshalAs(UnmanagedType.LPStr)]
            public string amount;          // @0
            [MarshalAs(UnmanagedType.LPStr)]
            public string instalmentCount; // @4
            [MarshalAs(UnmanagedType.LPStr)]
            public string issuerCode;      // @8
            [MarshalAs(UnmanagedType.LPStr)]
            public string planCode;        // @12
            [MarshalAs(UnmanagedType.LPStr)]
            public string originalTicket;  // @16
            [MarshalAs(UnmanagedType.LPStr)]
            public string originalDate;    // @20
            [MarshalAs(UnmanagedType.LPStr)]
            public string receiptNumber;   // @24
            [MarshalAs(UnmanagedType.LPStr)]
            public string merchantCode;    // @28
            [MarshalAs(UnmanagedType.LPStr)]
            public string merchantName;    // @32
            [MarshalAs(UnmanagedType.LPStr)]
            public string cuit;            // @36
            public byte linemode;                                           // @40
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct VoidIn
        {
            [MarshalAs(UnmanagedType.LPStr)]
            public string originalTicket;  // @0
            [MarshalAs(UnmanagedType.LPStr)]
            public string issuerCode;      // @4
            [MarshalAs(UnmanagedType.LPStr)]
            public string merchantName;    // @8
            [MarshalAs(UnmanagedType.LPStr)]
            public string cuit;            // @12
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct QrzIn
        {
            [MarshalAs(UnmanagedType.LPStr)]
            public string amount;
            [MarshalAs(UnmanagedType.LPStr)]
            public string instalmentCount;
            [MarshalAs(UnmanagedType.LPStr)]
            public string planCode;
        }

        // Las estructuras de SALIDA son structs de char* cuyos buffers debe reservar
        // el llamador; por eso se pasan como IntPtr (ver NativeOutStruct).

        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiOpenPort(ref ComParams p);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiClosePort();
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiTestConnection();

        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiPurchase(ref PurchaseIn input, IntPtr trxOut, int timeoutSec);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiPurchaseExtraCash(ref PurchaseIn input, IntPtr trxOut, int timeoutSec);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiPurchaseMark(ref PurchaseIn input, IntPtr trxOutMark, int timeoutSec);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiPurchaseExtraCashMark(ref PurchaseIn input, IntPtr trxOutMark, int timeoutSec);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiRefund(ref RefundIn input, IntPtr trxOut, int timeoutSec);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiVoid(ref VoidIn input, IntPtr trxOut, int timeoutSec);

        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiBatchClose(IntPtr batchOut, int timeoutSec);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiGetBatchCloseData(ushort index, IntPtr dataOut);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiGetIssuer(ushort index, IntPtr issuerOut);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiGetPlan(ushort index, IntPtr planOut);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiGetLastTrxData(out ushort trxCode, IntPtr trxOut);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiGetPANData(IntPtr panOut);
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiPrintTicket();
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiPrintBatchClose();
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall)]
        public static extern ushort vpiGetQRZ(ref QrzIn input, IntPtr qrzOut, int timeoutSec);

        // 3er parámetro: la DLL no lo usa (se pasa el tamaño del buffer por prolijidad).
        [DllImport(Dll, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern ushort vpiSendReceiveJson([MarshalAs(UnmanagedType.LPStr)] string jsonIn, byte[] jsonOut, int outSize, int timeoutSec);

        // No declaradas a propósito:
        //  vpiJson        -> stub: loguea "TBD" y devuelve siempre 909.
        //  vpiGetVersion  -> sólo escribe un WORD (código), no la versión; semántica dudosa.
        //  vpiKeepAlive   -> estructuras in/out sin documentar (deltaT/fecha/hora + 8 campos).
    }

    // ------------------------------------------------------------------------
    //  Estructura de salida nativa: [prefijo opcional (WORD index)] + N punteros
    //  char*, cada uno apuntando a un buffer propio puesto en cero.
    // ------------------------------------------------------------------------
    internal sealed class NativeOutStruct : IDisposable
    {
        // Los campos reales miden entre 2 y 201 bytes; 512 da margen de sobra
        // (la DLL tiene inconsistencias de tamaño entre versiones de comandos).
        private const int FieldBufferSize = 512;

        private IntPtr _struct;
        private readonly IntPtr[] _fields;

        public NativeOutStruct(int fieldCount) : this(0, fieldCount) { }

        public NativeOutStruct(int prefixBytes, int fieldCount)
        {
            _fields = new IntPtr[fieldCount];
            int size = prefixBytes + fieldCount * IntPtr.Size;
            _struct = Marshal.AllocHGlobal(size);
            Marshal.Copy(new byte[size], 0, _struct, size);

            byte[] zeros = new byte[FieldBufferSize];
            for (int i = 0; i < fieldCount; i++)
            {
                _fields[i] = Marshal.AllocHGlobal(FieldBufferSize);
                Marshal.Copy(zeros, 0, _fields[i], FieldBufferSize);
                Marshal.WriteIntPtr(_struct, prefixBytes + i * IntPtr.Size, _fields[i]);
            }
        }

        public IntPtr Pointer { get { return _struct; } }

        public ushort PrefixWord { get { return (ushort)Marshal.ReadInt16(_struct, 0); } }

        public string this[int i]
        {
            get
            {
                string s = Marshal.PtrToStringAnsi(_fields[i]);
                return s == null ? string.Empty : s.Trim();
            }
        }

        public string[] ToArray()
        {
            string[] r = new string[_fields.Length];
            for (int i = 0; i < r.Length; i++) r[i] = this[i];
            return r;
        }

        public void Dispose()
        {
            for (int i = 0; i < _fields.Length; i++)
            {
                if (_fields[i] != IntPtr.Zero) { Marshal.FreeHGlobal(_fields[i]); _fields[i] = IntPtr.Zero; }
            }
            if (_struct != IntPtr.Zero) { Marshal.FreeHGlobal(_struct); _struct = IntPtr.Zero; }
        }
    }

    // ------------------------------------------------------------------------
    //  Códigos de retorno VPI.
    //  0 y 909 confirmados en el binario; el resto según el manual VPI de Payway
    //  (verificar contra la documentación de tu versión).
    // ------------------------------------------------------------------------
    public static class VpiCodes
    {
        public const int OK = 0;
        public const int MORE_REC = 1;
        public const int FAIL = 11;
        public const int TIMEOUT_EXP = 12;
        public const int INVALID_ISSUER = 101;
        public const int INVALID_TICKET = 102;
        public const int INVALID_PLAN = 103;
        public const int INVALID_INDEX = 104;
        public const int EMPTY_BATCH = 105;
        public const int TRX_CANCELED = 201;
        public const int DIF_CARD = 202;
        public const int INVALID_CARD = 203;
        public const int EXPIRED_CARD = 204;
        public const int INVALID_TRX = 205;
        public const int ERR_COM = 301;
        public const int ERR_PRINT = 302;
        public const int INVALID_IN_CMD = 901;
        public const int INVALID_IN_PARAM = 902;
        public const int INVALID_OUT_CMD = 903;
        public const int GENERAL_FAIL = 909;

        public static string Describe(int code)
        {
            switch (code)
            {
                case OK: return "OK";
                case MORE_REC: return "Hay más registros";
                case FAIL: return "Falla / comando no ejecutado";
                case TIMEOUT_EXP: return "Timeout";
                case INVALID_ISSUER: return "Tarjeta (issuer) inválida";
                case INVALID_TICKET: return "Ticket inválido";
                case INVALID_PLAN: return "Plan inválido";
                case INVALID_INDEX: return "Índice inválido";
                case EMPTY_BATCH: return "Lote vacío";
                case TRX_CANCELED: return "Transacción cancelada por el usuario";
                case DIF_CARD: return "Tarjeta distinta";
                case INVALID_CARD: return "Tarjeta inválida";
                case EXPIRED_CARD: return "Tarjeta vencida";
                case INVALID_TRX: return "Transacción original inexistente";
                case ERR_COM: return "Error de comunicación con el host";
                case ERR_PRINT: return "Error de impresión";
                case INVALID_IN_CMD: return "Comando inválido";
                case INVALID_IN_PARAM: return "Parámetros de entrada inválidos";
                case INVALID_OUT_CMD: return "Respuesta inválida";
                case GENERAL_FAIL: return "Error general";
                default: return "Código " + code;
            }
        }
    }

    // ------------------------------------------------------------------------
    //  DTOs de salida
    // ------------------------------------------------------------------------
    public class VpiTrxResult
    {
        public int ReturnCode { get; set; }
        public string HostResponseCode { get; set; }  // "00" = aprobada
        public string HostMessage { get; set; }
        public string AuthCode { get; set; }
        public string TicketNumber { get; set; }
        public string BatchNumber { get; set; }
        public string CustomerName { get; set; }
        public string PanFirst6 { get; set; }
        public string PanLast4 { get; set; }
        public string Date { get; set; }
        public string Time { get; set; }
        public string TerminalId { get; set; }
        // Sólo en vpiPurchaseMark / vpiPurchaseExtraCashMark:
        public string IssuerCode { get; set; }
        public string MerchantCode { get; set; }
        public string EmvAip { get; set; }
        public string EmvAppEmv { get; set; }
        public string PromoMessage { get; set; }

        public bool Approved { get { return ReturnCode == VpiCodes.OK && HostResponseCode == "00"; } }

        public override string ToString()
        {
            return string.Format("ret={0} ({1}) host={2} '{3}' auth={4} ticket={5} lote={6} tarjeta={7}...{8} term={9}",
                ReturnCode, VpiCodes.Describe(ReturnCode), HostResponseCode, HostMessage, AuthCode,
                TicketNumber, BatchNumber, PanFirst6, PanLast4, TerminalId);
        }

        internal const int BaseFields = 11;
        internal const int MarkFields = 16;

        internal static VpiTrxResult From(int rc, NativeOutStruct o)
        {
            VpiTrxResult r = new VpiTrxResult();
            r.ReturnCode = rc;
            r.HostResponseCode = o[0];
            r.HostMessage = o[1];
            r.AuthCode = o[2];
            r.TicketNumber = o[3];
            r.BatchNumber = o[4];
            r.CustomerName = o[5];
            // Slots 6/7: la DLL no es consistente (según el comando escribe first6/last4
            // en uno u otro). Se resuelve por longitud.
            string a = o[6], b = o[7];
            if (a.Length == 6 && b.Length == 4) { r.PanFirst6 = a; r.PanLast4 = b; }
            else { r.PanLast4 = a; r.PanFirst6 = b; }
            r.Date = o[8];
            r.Time = o[9];
            r.TerminalId = o[10];
            return r;
        }

        internal static VpiTrxResult FromMark(int rc, NativeOutStruct o)
        {
            VpiTrxResult r = From(rc, o);
            r.IssuerCode = o[11];
            r.MerchantCode = o[12];
            r.EmvAip = o[13];
            r.EmvAppEmv = o[14];
            r.PromoMessage = o[15];
            return r;
        }
    }

    public class VpiBatchCloseResult
    {
        public int ReturnCode { get; set; }
        public string HostResponseCode { get; set; }
        public string Date { get; set; }
        public string Time { get; set; }
        public string TerminalId { get; set; }
    }

    public class VpiBatchCloseData
    {
        public int ReturnCode { get; set; }
        public int Index { get; set; }
        public string AcquirerCode { get; set; }
        public string BatchNumber { get; set; }
        public string IssuerCode { get; set; }
        public string PurchaseCount { get; set; }
        public string PurchaseAmount { get; set; }
        public string VoidCount { get; set; }
        public string VoidAmount { get; set; }
        public string RefundCount { get; set; }
        public string RefundAmount { get; set; }
        public string RefVoidCount { get; set; }
        public string RefVoidAmount { get; set; }
        public string Date { get; set; }
        public string Time { get; set; }
        public string TerminalId { get; set; }
    }

    public class VpiIssuer
    {
        public int ReturnCode { get; set; }
        public int Index { get; set; }
        public string AcquirerCode { get; set; }
        public string IssuerCode { get; set; }
        public string IssuerName { get; set; }
        public string MaxInstalments { get; set; }
        public string TerminalId { get; set; }
    }

    public class VpiPlan
    {
        public int ReturnCode { get; set; }
        public int Index { get; set; }
        public string IssuerCode { get; set; }
        public string PlanCode { get; set; }
        public string PlanLabel { get; set; }
        public string TerminalId { get; set; }
    }

    public class VpiPanData
    {
        public int ReturnCode { get; set; }
        public string IssuerCode { get; set; }
        public string PanFirst6 { get; set; }
        public string PanLast4 { get; set; }
        public string Field4 { get; set; }  // 8 caracteres; la DLL lo loguea como "Ticket Number"
    }

    // ------------------------------------------------------------------------
    //  Capa 2: API de alto nivel
    // ------------------------------------------------------------------------
    public sealed class VpiPos : IDisposable
    {
        private readonly object _sync = new object();
        private bool _open;

        public int DefaultTimeoutSec { get; set; }

        public VpiPos()
        {
            DefaultTimeoutSec = 90;
        }

        /// <summary>La DLL busca VpiPC.ini y escribe vpiPC.log en el directorio actual.
        /// Llamar al inicio si el exe puede lanzarse desde otra carpeta.</summary>
        public static void UseExeDirectoryAsCurrent()
        {
            string exe = System.Reflection.Assembly.GetEntryAssembly().Location;
            Environment.CurrentDirectory = System.IO.Path.GetDirectoryName(exe);
        }

        public int Open(string comPort, ushort baudRate, ushort byteSize, char parity, ushort stopBits)
        {
            VpiNative.ComParams p = new VpiNative.ComParams();
            p.com = comPort;
            p.baudRate = baudRate;
            p.byteSize = byteSize;
            p.parity = (byte)parity;
            p.stopBits = stopBits;
            lock (_sync)
            {
                int rc = VpiNative.vpiOpenPort(ref p);
                _open = rc == VpiCodes.OK;
                return rc;
            }
        }

        /// <summary>Configuración típica del LAPOS: 19200 8N1.</summary>
        public int Open(string comPort)
        {
            return Open(comPort, 19200, 8, 'N', 1);
        }

        public int Close()
        {
            lock (_sync)
            {
                _open = false;
                return VpiNative.vpiClosePort();
            }
        }

        public int TestConnection()
        {
            lock (_sync) return VpiNative.vpiTestConnection();
        }

        // ---------------- Transacciones ----------------

        /// <param name="amount">Monto en centavos, sin separador ("12550" = $125,50).</param>
        public VpiTrxResult Purchase(VpiNative.PurchaseIn input) { return Purchase(input, DefaultTimeoutSec); }

        public VpiTrxResult Purchase(VpiNative.PurchaseIn input, int timeoutSec)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(VpiTrxResult.BaseFields))
                {
                    int rc = VpiNative.vpiPurchase(ref input, o.Pointer, timeoutSec);
                    return VpiTrxResult.From(rc, o);
                }
        }

        public VpiTrxResult PurchaseExtraCash(VpiNative.PurchaseIn input, int timeoutSec)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(VpiTrxResult.BaseFields))
                {
                    int rc = VpiNative.vpiPurchaseExtraCash(ref input, o.Pointer, timeoutSec);
                    return VpiTrxResult.From(rc, o);
                }
        }

        /// <summary>Venta con datos extendidos (issuer, merchant, EMV, mensaje promocional).</summary>
        public VpiTrxResult PurchaseMark(VpiNative.PurchaseIn input, int timeoutSec)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(VpiTrxResult.MarkFields))
                {
                    int rc = VpiNative.vpiPurchaseMark(ref input, o.Pointer, timeoutSec);
                    return VpiTrxResult.FromMark(rc, o);
                }
        }

        public VpiTrxResult PurchaseExtraCashMark(VpiNative.PurchaseIn input, int timeoutSec)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(VpiTrxResult.MarkFields))
                {
                    int rc = VpiNative.vpiPurchaseExtraCashMark(ref input, o.Pointer, timeoutSec);
                    return VpiTrxResult.FromMark(rc, o);
                }
        }

        public VpiTrxResult Refund(VpiNative.RefundIn input, int timeoutSec)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(VpiTrxResult.BaseFields))
                {
                    int rc = VpiNative.vpiRefund(ref input, o.Pointer, timeoutSec);
                    return VpiTrxResult.From(rc, o);
                }
        }

        public VpiTrxResult Void(VpiNative.VoidIn input, int timeoutSec)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(VpiTrxResult.BaseFields))
                {
                    int rc = VpiNative.vpiVoid(ref input, o.Pointer, timeoutSec);
                    return VpiTrxResult.From(rc, o);
                }
        }

        /// <summary>Datos de la última transacción (útil para recuperar tras un corte/timeout).</summary>
        public VpiTrxResult GetLastTrxData(out ushort trxCode)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(VpiTrxResult.BaseFields))
                {
                    int rc = VpiNative.vpiGetLastTrxData(out trxCode, o.Pointer);
                    return VpiTrxResult.From(rc, o);
                }
        }

        public int PrintTicket() { lock (_sync) return VpiNative.vpiPrintTicket(); }

        // ---------------- Cierre de lote ----------------

        public VpiBatchCloseResult BatchClose(int timeoutSec)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(4))
                {
                    VpiBatchCloseResult r = new VpiBatchCloseResult();
                    r.ReturnCode = VpiNative.vpiBatchClose(o.Pointer, timeoutSec);
                    r.HostResponseCode = o[0];
                    r.Date = o[1];
                    r.Time = o[2];
                    r.TerminalId = o[3];
                    return r;
                }
        }

        /// <summary>Totales del último cierre por tarjeta. Iterar index 0,1,2... mientras ReturnCode == MORE_REC.</summary>
        public VpiBatchCloseData GetBatchCloseData(ushort index)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(4, 14))
                {
                    VpiBatchCloseData r = new VpiBatchCloseData();
                    r.ReturnCode = VpiNative.vpiGetBatchCloseData(index, o.Pointer);
                    r.Index = o.PrefixWord;
                    r.AcquirerCode = o[0];
                    r.BatchNumber = o[1];
                    r.IssuerCode = o[2];
                    r.PurchaseCount = o[3];
                    r.PurchaseAmount = o[4];
                    r.VoidCount = o[5];
                    r.VoidAmount = o[6];
                    r.RefundCount = o[7];
                    r.RefundAmount = o[8];
                    r.RefVoidCount = o[9];
                    r.RefVoidAmount = o[10];
                    r.Date = o[11];
                    r.Time = o[12];
                    r.TerminalId = o[13];
                    return r;
                }
        }

        public int PrintBatchClose() { lock (_sync) return VpiNative.vpiPrintBatchClose(); }

        // ---------------- Tablas (tarjetas / planes) ----------------

        /// <summary>Tarjeta habilitada en la posición 'index'. Iterar mientras ReturnCode == MORE_REC.</summary>
        public VpiIssuer GetIssuer(ushort index)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(4, 5))
                {
                    VpiIssuer r = new VpiIssuer();
                    r.ReturnCode = VpiNative.vpiGetIssuer(index, o.Pointer);
                    r.Index = o.PrefixWord;
                    r.AcquirerCode = o[0];
                    r.IssuerCode = o[1];
                    r.IssuerName = o[2];
                    r.MaxInstalments = o[3];
                    r.TerminalId = o[4];
                    return r;
                }
        }

        public VpiPlan GetPlan(ushort index)
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(4, 4))
                {
                    VpiPlan r = new VpiPlan();
                    r.ReturnCode = VpiNative.vpiGetPlan(index, o.Pointer);
                    r.Index = o.PrefixWord;
                    r.IssuerCode = o[0];
                    r.PlanCode = o[1];
                    r.PlanLabel = o[2];
                    r.TerminalId = o[3];
                    return r;
                }
        }

        /// <summary>Lee la tarjeta (sin cobrar) y devuelve issuer + BIN + últimos 4.</summary>
        public VpiPanData GetPanData()
        {
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(4))
                {
                    VpiPanData r = new VpiPanData();
                    r.ReturnCode = VpiNative.vpiGetPANData(o.Pointer);
                    r.IssuerCode = o[0];
                    r.PanFirst6 = o[1];
                    r.PanLast4 = o[2];
                    r.Field4 = o[3];
                    return r;
                }
        }

        // ---------------- QR y JSON ----------------

        /// <summary>Pago con QR. Devuelve los 13 campos crudos de salida (nombres no documentados en la DLL).</summary>
        public string[] GetQrz(string amount, string instalmentCount, string planCode, int timeoutSec, out int returnCode)
        {
            VpiNative.QrzIn i = new VpiNative.QrzIn();
            i.amount = amount;
            i.instalmentCount = instalmentCount;
            i.planCode = planCode;
            lock (_sync)
                using (NativeOutStruct o = new NativeOutStruct(13))
                {
                    returnCode = VpiNative.vpiGetQRZ(ref i, o.Pointer, timeoutSec);
                    return o.ToArray();
                }
        }

        /// <summary>Envía un JSON al terminal y devuelve la respuesta (entrada máx. 10240 bytes).</summary>
        public string SendReceiveJson(string json, int timeoutSec, out int returnCode)
        {
            byte[] buffer = new byte[16384];
            lock (_sync) returnCode = VpiNative.vpiSendReceiveJson(json, buffer, buffer.Length, timeoutSec);
            int len = Array.IndexOf(buffer, (byte)0);
            if (len < 0) len = buffer.Length;
            return Encoding.Default.GetString(buffer, 0, len);
        }

        public void Dispose()
        {
            if (_open) Close();
        }
    }
}
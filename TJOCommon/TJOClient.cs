using System;
using System.IO;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Bitnet.TJOCommon
{
    /// <summary>
    /// Clase que implementa toda la comunicacion con el servidor JAuton de Invel
    /// </summary>
    public class TJOClient
    {
        /// <summary>
        /// La version del autorizador a usar. 1=Msdos, 2=Jauton
        /// </summary>
        int version;

        /// <summary>
        /// El logger a utilizar para seguir el progreso de las operaciones.
        /// </summary>
        ILog logger;

        /// <summary>
        /// Retorna o cambia la direccion IP donde se encuentra el servidor JAuton.
        /// </summary>
        public IPAddress JAutonAddress
        {
            get;
            set;
        }

        /// <summary>
        /// La carpeta compartida que utiliza el autorizador msdos
        /// </summary>
        public string SharedFolder { get; set; }

        /// <summary>
        /// El port IP donde el JAuton esta esperando.
        /// </summary>
        public int Port { get; set; }

        /// <summary>
        /// El ultimo mensaje de error producido.
        /// </summary>
        string rm = string.Empty;

        /// <summary>
        /// Indica si la construccion de este modulo fue satisfactoria. Si no lo fue, el modulo
        /// no esta disponible para su uso y 'ResultMessage' contiene un mensaje que indica porque.
        /// </summary>
        public bool Valido { get; private set; }

        /// <summary>
        /// Tiempo (en segundos) para esperar respuestas desde el servidor.
        /// </summary>
        public int Timeout { get; set; }

        /// <summary>
        /// Indica si la ultima operacion de autorizaion fue o no aprobada.
        /// </summary>
        public bool ResultAprobada { get; set; }

        /// <summary>
        /// Escribe un mensaje en el logger, si esta activo.
        /// </summary>
        /// <param name="msg">El mensaje a registrar</param>
        private void Write(string msg)
        {
            if (logger != null) logger.Write(msg);
        }

        /// <summary>
        /// El ultimo mensaje que produjo la autorizacion de tarjetas. No necesariamente es un
        /// mensaje de error.
        /// </summary>
        public string ResultMessage
        {
            get { return rm; }
            set { rm = value; Write(rm); }
        }

        /// <summary>
        /// Constructor del cliente.
        /// </summary>
        /// <param name="log">El logger a utilizar para la comunicacion con el servidor</param>
        public TJOClient(int version, string host, int port, int timeout, ILog log)
        {
            this.version = version;
            this.logger = log;
            this.Timeout = (timeout > 0) ? timeout : 30;
            this.Valido = true;
            switch (version)
            {
                case 1:             // Autorizador MSDOS
                    this.SharedFolder = host.Trim();
                    if (!Directory.Exists(this.SharedFolder))
                    {
                        this.ResultMessage = "No encuentro la carpeta compartida '" + this.SharedFolder + "'";
                        this.Valido = false;
                    }
                    else if (!File.Exists(this.SharedFolder + "\\autor.dat"))
                    {
                        this.ResultMessage = "No encuentro el archivo de dialogo 'autor.dat' en '" + this.SharedFolder + "'";
                        this.Valido = false;
                    }
                    break;

                case 2:             // Autorizador JAUTON (por default)
                default:
                    this.Port = (port > 0 && port < 65535) ? port : 6543;
                    IPAddress addr = null;
                    Valido = false;
                    if (IPAddress.TryParse(host, out addr))
                    {
                        this.JAutonAddress = addr;
                        Valido = true;
                    }
                    else
                    {
                        IPAddress[] addrs = Dns.GetHostAddresses(host);
                        if (addrs.Length > 0)
                        {
                            this.JAutonAddress = addrs[0];
                            Valido = true;
                        }
                    }
                    break;
            }
        }

        /// <summary>
        /// Helper method que espera que lleguen por el socket una determinada cantidad de bytes,
        /// en el periodo de tiempo indicado por 'Timeout'.
        /// </summary>
        /// <param name="s">El socket en el que se espera recibir bytes.</param>
        /// <param name="nBytes">El numero de bytes que se espera recibir</param>
        /// <returns>true si se recibieron los bytes pedidos, sino, false</returns>
        /// <remarks>
        /// Si se sale por timeout, esta funcion cierra el socket y pone en 'ResultMessage' un mensaje
        /// de error indicando que es lo que paso.
        /// </remarks>
        private bool WaitNBytes(Socket s, int nBytes)
        {
            DateTime start = DateTime.Now;
            while (s.Available < nBytes)
            {
                Thread.Sleep(100);
                TimeSpan span = DateTime.Now.Subtract(start);
                if (span.TotalSeconds > Timeout)
                {
                    ResultMessage = "Fallo en la Comunicacion (" + nBytes + ")";
                    s.Close();
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// Efectua el cierre de lote usando el autorizador MSDOS
        /// </summary>
        /// <param name="terminalID">El terminal para el que se va a hacer el cierre de lote</param>
        /// <param name="nodoID">El nodo online que se cerrara</param>
        /// <returns>true si todo salio bien, sino, false</returns>
        private bool CierraLoteUsandoMsdosAuth(int terminalID, int nodoID)
        {
            return true;
        }

        /// <summary>
        /// Efectua el cierre de lote usando el autorizador JAuton de invel
        /// </summary>
        /// <param name="terminalID">El terminal para el que se va a hacer el cierre de lote</param>
        /// <param name="nodoID">El nodo online que se cerrara</param>
        /// <returns>true si todo salio bien, sino, false</returns>
        private bool CierreLoteUsandoJAuton(int terminalID, int nodoID)
        {
            // Construye el requerimiento
            CDataBuffer buff = new CDataBuffer(8);
            buff.writeString(nodoID.ToString().PadLeft(2, '0'), 2, 2);
            buff.writeString(terminalID.ToString().PadLeft(4, '0'), 4, 4);
            buff.writeByte(2, 0);
            buff.writeByte(98, 1);

            Socket s = OpenSocket();
            if (s == null) return false;
            s.Send(buff.Buffer);

            if (!WaitNBytes(s, 2)) return false;
            byte[] ans = s.ReadFully(2);
            bool res = true;
            s.Close();
            return res;
        }

        /// <summary>
        /// Fuerza un cierre de lote.
        /// </summary>
        /// <param name="terminalID">El terminal para el que se va a hacer el cierre de lote</param>
        /// <param name="nodoID">El nodo online que se cerrara</param>
        /// <returns>true si todo salio bien, sino, false</returns>
        public bool CierreLote(int terminalID, int nodoID)
        {
            if (!Valido) return false;      // 'ResultMessage' ya tiene el error de configuracion.
            ResultMessage = "Procesando...";

            // Procesa, de acuerdo con la version en uso.
            switch (version)
            {
                case 1:         // Autorizador MSDOS
                    return CierraLoteUsandoMsdosAuth(terminalID, nodoID);

                case 2:         // Autorizador JAUTON
                default:
                    return CierreLoteUsandoJAuton(terminalID, nodoID);
            }
        }

        /// <summary>
        /// Procesa el requerimiento usando el autorizador MSDOS de invel.
        /// </summary>
        /// <param name="req">Buffer con el requerimiento online</param>
        /// <returns>Ver 'ProcessRequest' por la posible combinacion de resultados.</returns>
        private bool ProcessRequestUsingMsdosAuth(X25Request req)
        {
            return true;
        }

        /// <summary>
        /// PRocesa el requerimiento usando el JAuton de invel.
        /// </summary>
        /// <param name="req">Buffer con el requerimiento online</param>
        /// <returns>Ver 'ProcessRequest' por la posible combinacion de resultados.</returns>
        private bool ProcessRequestUsingJAuton(X25Request req)
        {
            CDataBuffer bRequest = req.WriteBytes();
            short reqSize = (short)(bRequest.Buffer.Length + 4);

            // Prepara el buffer con el header de la operacion. El body ya es 'bRequest'
            CDataBuffer bHeader = new CDataBuffer(10);
            bHeader.writeByte(1);           // Codigo de operacion
            bHeader.writeByte(0);
            bHeader.writeByte(0);
            bHeader.writeShort(reqSize);    // Pkt Size + TERMID

            bHeader.writeByte(2);           // STX
            bHeader.writeByte(1);           // TERMID
            bHeader.writeByte(2);           // TERMID
            bHeader.writeByte(3);           // TERMID
            bHeader.writeByte(4);           // TERMID

            // Calcula el CRC y construye el footer
            CRC16 crc = new CRC16();
            crc.update(bHeader.Buffer, 0, bHeader.Buffer.Length);
            crc.update(bRequest.Buffer, 0, bRequest.Buffer.Length);
            byte[] footer = new byte[3];
            footer[0] = 3;
            crc.update(footer, 0, 1);
            footer[1] = (byte)(crc.Crc & 0xff);
            footer[2] = (byte)((crc.Crc >> 8) & 0xff);

            // Abre el socket y envia la info
            Socket s = OpenSocket();
            if (s == null) return false;

            // Envia los paquetes con el requerimiento.
            s.SendWithLog(bHeader.Buffer, 1);
            s.SendWithLog(bRequest.Buffer, 2);
            s.SendWithLog(footer, 3);
            
            // Recibe el ack desde el server
            if (!WaitNBytes(s, 9)) 
                return false;
            byte[] ack = s.ReadFully(9);
            Write("Esperando Respuesta...");

            // Recibe la respuesta desde el autorizador.
            if (!WaitNBytes(s, 6)) return false;
            byte[] ansHeader = s.ReadFully(6);
            int pktLen = ansHeader[3] + (ansHeader[4] & 0xff) * 256;
            if (ansHeader[0] != 1 || ansHeader[1] != 0 || ansHeader[2] != 0 || ansHeader[5] != 2)
            {
                ResultMessage = "Error en la Comunicacion (Invalid AnsHeader)";
                s.Close();
                return false;
            }

            // Lee el cuerpo del mensaje.
            if (!WaitNBytes(s, 4)) return false;
            byte[] ansID = s.ReadFully(4);
            if (!WaitNBytes(s, pktLen - 4)) return false;
            byte[] ans = s.ReadFully(pktLen - 4);

            // Lee el pie y comprueba la integridad del mensaje recibido validando su CRC
            if (!WaitNBytes(s, 3)) return false;
            footer = s.ReadFully(3);

            int recvCRC = footer[1] + (footer[2] & 0xff) * 256;
            CRC16 rcrc = new CRC16();
            rcrc.update(ansHeader, 0, ansHeader.Length);
            rcrc.update(ansID, 0, ansID.Length);
            rcrc.update(ans, 0, ans.Length);
            rcrc.update(footer, 0, 1);
            if (rcrc.Crc != recvCRC)
            {
                ResultMessage = "Error en la Comunicacion (BADCRC)";
                s.Close();
                return false;
            }

            // Valida que la respuesta recibida sea con respecto a nuestro ultimo pedido y no a uno
            // anterior que no nos interesa.
            if (bHeader.Buffer[6] != ansID[0] ||
                bHeader.Buffer[7] != ansID[1] ||
                bHeader.Buffer[8] != ansID[2] ||
                bHeader.Buffer[9] != ansID[3])
            {
                ResultMessage = "Error en la Comunicacion (Unmatched POS-ID)";
                s.Close();
                return false;
            }

            // Confirma la recepcion y cierra el socket.
            SendAck(s);
            s.Close();

            // Desenpaqueta la informacion recibida y la analiza un poco para determinar
            // como resulto la operacion.
            X25Request ansReq = new X25Request(ans);
            ResultMessage = ansReq.mensaje;
            ResultAprobada = (ansReq.codigo_de_respuesta == 0);
            if (ResultAprobada)
            {
                req.mensaje = ansReq.mensaje;
                req.autorizacion = ansReq.autorizacion;
                req.batch = ansReq.batch;
                req.numero_de_trace = ansReq.numero_de_trace;
                req.ticket = ansReq.ticket;
            }

            return true;
        }

        /// <summary>
        /// Metodo principal que dialoga con el JAuton de invel y efectua una transaccion.
        /// </summary>
        /// <param name="req">El requerimiento a procesar.</param>
        /// <returns>true si no hubo ningun error de comunicacion. Que no se apruebe una operacion NO 
        /// es un error que se señale con false.</returns>
        /// <remarks>
        /// Se pueden dar la siguiente combinacion de resultados.
        /// 
        ///     return   ResultAprobada    Observacion
        ///     ------   --------------    ---------------------------------------------------------------
        ///     false         false        Fallo de Comunicacion
        ///     false         true         Esta combinacion NUNCA se puede producir.
        ///     true          false        La comunicacion fue OK, pero la tarjeta NO APROBO la operacion.
        ///     true          true         La operacion fue correctamente aprobada por la tarjeta.
        /// </remarks>
        public bool ProcessRequest(X25Request req)
        {
            ResultAprobada = false;
            if (!Valido) return false;      // 'ResultMessage' ya tiene el error de configuracion.
            ResultMessage = "Procesando...";

            // Procesa, segun la version en uso.
            switch (version)
            {
                case 1:         // Procesa usando el autorizador msdos
                    return ProcessRequestUsingMsdosAuth(req);

                case 2:         // Procesa usando jauton
                default:
                    return ProcessRequestUsingJAuton(req);
            }
        }

        /// <summary>
        /// Rutina utilitaria que envia un paquete de 'Ack' al JAuton remoto.
        /// </summary>
        /// <param name="s">El socket por el que estamos comunicados con el JAuton</param>
        void SendAck(Socket s)
        {
            byte[] ack = new byte[9];
            ack[0] = 6;
            ack[1] = 0;
            ack[2] = 0;
            ack[3] = 0;
            ack[4] = 0;
            ack[5] = 2;
            ack[6] = 3;

            CRC16 crc = new CRC16();
            crc.update(ack, 0, ack.Length);
            ack[7] = (byte)(crc.Crc & 0xff);
            ack[8] = (byte)((crc.Crc >> 8) & 0xff);

            s.Send(ack);
        }

        Socket OpenSocket()
        {
            Write(string.Format("Estableciendo comunicacion con el host ({0}:{1})", JAutonAddress.ToString(), Port));

            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP);
            s.DontFragment = true;
            try { s.Connect(JAutonAddress, Port); }
            catch (Exception ex)
            {
                ResultMessage = "Error de Comunicacion- Socket::Connect= " + ex.Message;
                s.Close();
                return null;
            }
            return s;
        }
    }

    /// <summary>
    /// Extension para la clase 'Socket', para simplificar la lectura de un paquete de bytes.
    /// </summary>
    public static class Extensions
    {
        /// <summary>
        /// Envia un paquete por el socket grabando previamente en un registro de log
        /// </summary>
        /// <param name="s">El socket por el que enviara el paquete</param>
        /// <param name="buff">El paquete a enviar</param>
        /// <param name="buffId">El ID para identificar el paquete</param>
        public static void SendWithLog(this Socket s, byte[] buff, int buffId)
        {
            string fn = "tjsend." + buffId.ToString();
            File.WriteAllBytes(fn, buff);
            s.Send(buff);
        }

        /// <summary>
        /// Lee completamente, desde un socket, la cantidad de bytes requeridas (inclusive usando
        /// un algoritmo de descarga partial)
        /// </summary>
        /// <param name="s">El socket al que se aplica el metodo</param>
        /// <param name="num">El numero de bytes a leer completamente</param>
        /// <returns>Los bytes leidos</returns>
        public static byte[] ReadFully(this Socket s, int num)
        {
            int offset = 0;
            byte[] res = new byte[num];
            while (offset < num)
                offset += s.Receive(res, offset, num - offset, SocketFlags.Partial);
            return res;
        }
    }
}

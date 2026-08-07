using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using CryptManaged;

namespace Bitnet.TJOCommon
{
    /// <summary>
    /// Clase que contiene un requerimiento de autorizacion, y su correspondiente respuesta.
    /// </summary>
    public class X25Request
    {
        /// <summary>
        /// El algoritmo de encriptacion de datos sensibles a usar.
        /// </summary>
        static Crypt crypt = null;

        /// <summary>
        /// Permite acceso interno al algoritmo de encriptacion a usar.
        /// </summary>
        static Crypt CryptEngine
        {
            get
            {
                if (crypt == null) crypt = new Crypt();
                return crypt;
            }
        }

        /// <summary>
        /// Constructor, inicializa a cero (o strings vacias) los distintos componentes del
        /// requerimiento.
        /// </summary>
        public X25Request()
        {
            tipo_de_mensaje = 0;
            codigo_de_procesamiento = 0;
            autorizacion = 0;
            codigo_de_respuesta = 0;
            mensaje = "";
            retrieval_reference_number = "";
            pin__working_key = "";
            nro_cta_tarjeta = "";
            fecha_expiracion = "";
            track_2 = "";
            track_1 = "";
            numero_de_comercio = "";
            bloque = new byte[512];
        }

        /// <summary>
        /// Construye un requerimiento, a partir de su imagen serializada en bytes.
        /// </summary>
        /// <param name="data"></param>
        public X25Request(byte[] data) : this()
        {
            ReadTransaction(data);
        }

        /// <summary>
        /// Los datos que contiene el requerimiento. Se hacen publicos directamente.
        /// </summary>
        public byte[] buf;
        public DateTime fecha;
        public int terminal_id;
        public int nodo_id;
        public int sucursal_id;
        public short tipo_de_mensaje;
        public int codigo_de_procesamiento;
        public byte tipo_de_tarjeta;
        public byte tipo_de_cuenta;
        public short marca_id;
        public byte modo_de_ingreso;
        public string nro_cta_tarjeta;
        public string fecha_expiracion;
        public string track_2;
        public string track_1;
        public short cvv;
        public short ticket;
        public byte moneda;
        public char plan;
        public byte cuotas;
        public float importe;
        public float entrega;
        public float importe_cuota;
        public float saldo;
        public DateTime fecha_local;
        public DateTime hora_local;
        public int autorizacion;
        public short codigo_de_respuesta;
        public int numero_de_trace;
        public int numero_de_terminal;
        public string numero_de_comercio;
        public string mensaje;
        public string retrieval_reference_number;
        public string pin__working_key;
        public short id_internacional_de_red;
        public DateTime fecha_de_diferimiento;
        public byte codigo_de_condicion;
        public short ticket_original;
        public DateTime fecha_original;
        public DateTime fecha_de_captura;
        public short tipo_de_mensaje_original;
        public int numero_de_trace_original;
        public short batch;
        public short compras;
        public short devoluciones;
        public short anulaciones;
        public double monto_compras;
        public double monto_devoluciones;
        public double monto_anulaciones;
        public byte datos_tarjetas_encriptados;
        public short bloque_id_clave;
        public short bloque_version;
        public short bloque_len;
        public byte[] bloque;
        public byte pinpad;

        /*
        /// <summary>
        /// Cambia el tipo de mensaje, tanto en la propiedad correspondiente como en el buffer de bytes
        /// </summary>
        /// <param name="cdatabuffer"></param>
        /// <param name="tipo"></param>
        public void writeTipoDeMensaje(CDataBuffer cdatabuffer, short tipo)
        {
            this.tipo_de_mensaje = tipo;
            cdatabuffer.writeShort(tipo, 9);
        }

        public void writeAutorizacion(CDataBuffer cdatabuffer, int autoCode)
        {
            this.autorizacion = autoCode;
            cdatabuffer.writeInt(autoCode, 115);
        }

        public void writeCodigoDeRespuesta(CDataBuffer cdatabuffer, short ansCode)
        {
            this.codigo_de_respuesta = ansCode;
            cdatabuffer.writeShort(ansCode, 119);
        }

        public void writeMensaje(CDataBuffer cdatabuffer, string msg)
        {
            this.mensaje = msg;
            cdatabuffer.writeStringZ(msg, 144);
        }

        public void writeBatch(CDataBuffer cdatabuffer, short batchNum)
        {
            this.batch = batchNum;
            cdatabuffer.writeShort(batchNum, 232);
        }

        public void writeImporteCuota(CDataBuffer cdatabuffer, float cuota)
        {
            this.importe_cuota = cuota;
            cdatabuffer.writeFloat(cuota, 98);
        }

        public void writeSaldo(CDataBuffer cdatabuffer, float nsal)
        {
            this.saldo = nsal;
            cdatabuffer.writeFloat(nsal, 102);
        }

        public void writeRetrievalReferenceNumber(CDataBuffer cdatabuffer, string retNum)
        {
            this.retrieval_reference_number = retNum;
            cdatabuffer.writeString(retNum, 185, 12);
        }

        public void writePinWorkingKey(CDataBuffer cdatabuffer, String wk)
        {
            this.pin__working_key = wk;
            cdatabuffer.writeString(wk, 197, 8);
        }


        public void writeTransaction(CDataBuffer cdatabuffer, X25Request transaction)
        {
            cdatabuffer.writeInt(transaction.numero_de_trace, 121);
            cdatabuffer.writeInt(transaction.numero_de_terminal, 125);
            cdatabuffer.writeShort(transaction.id_internacional_de_red, 205);
            cdatabuffer.writeShort(transaction.batch, 232);
            cdatabuffer.writeByte(transaction.pinpad, 212);
            switch (transaction.tipo_de_mensaje)
            {
                default:
                    break;

                case 200:
                case 220:
                    switch (transaction.codigo_de_procesamiento)
                    {
                        case 20000:
                        case 220000:
                            writeRetrievalReferenceNumber(cdatabuffer, transaction.retrieval_reference_number);
                            break;
                    }
                    break;

                case 400:
                    cdatabuffer.writeShort(transaction.tipo_de_mensaje_original, 226);
                    break;

                case 500:
                    cdatabuffer.writeString(transaction.numero_de_comercio, 129, 15);
                    break;
            }

            //Node node = Auxiliaries.getNodeInfo(transaction.nodo_id, vnodes);
            //cdatabuffer.writeByte(node.tipo, 211);
        }

         * */

        /// <summary>
        /// Lee la transaccion des el buffer de bytes que se indica.
        /// </summary>
        /// <param name="buffer">El byte de buffers a llenar con los datos de la transaccion</param>
        public void ReadTransaction(byte[] buffer)
        {
            CDataBuffer cdb = new CDataBuffer(buffer);
            ReadTransaction(cdb);
        }

        /// <summary>
        /// Lee una transaccion desde el CDataBuffer que se le indica.
        /// </summary>
        /// <param name="cdatabuffer">El buffer desde el que se leera la transaccion</param>
        public void ReadTransaction(CDataBuffer cdatabuffer)
        {
            this.fecha = DateTime.Now;
            this.tipo_de_mensaje = cdatabuffer.readShort(9);
            this.terminal_id = cdatabuffer.readInt(4);
            this.nodo_id = cdatabuffer.readByte(8);
            this.codigo_de_procesamiento = cdatabuffer.readInt(11);
            this.tipo_de_tarjeta = cdatabuffer.readByte(15);
            this.tipo_de_cuenta = cdatabuffer.readByte(16);
            this.marca_id = cdatabuffer.readShort(17);
            this.modo_de_ingreso = cdatabuffer.readByte(19);
            this.nro_cta_tarjeta = cdatabuffer.readStringZ(20, 20);
            this.fecha_expiracion = cdatabuffer.readString(41, 4);
            this.track_2 = cdatabuffer.readStringZ(45, 37);
            this.cvv = cdatabuffer.readShort(83);
            this.ticket = cdatabuffer.readShort(85);
            this.moneda = cdatabuffer.readByte(87);
            this.plan = (char)cdatabuffer.readByte(88);
            this.cuotas = cdatabuffer.readByte(89);
            this.importe = cdatabuffer.readFloat(90);
            this.entrega = cdatabuffer.readFloat(94);
            this.importe_cuota = cdatabuffer.readFloat(98);
            this.saldo = cdatabuffer.readFloat(102);
            this.hora_local = cdatabuffer.readDateTime(111, 107);
            this.fecha_local = cdatabuffer.readDate(111);
            this.autorizacion = cdatabuffer.readInt(115);
            this.codigo_de_respuesta = cdatabuffer.readShort(119);
            this.numero_de_trace = cdatabuffer.readInt(121);
            this.numero_de_terminal = cdatabuffer.readInt(125);
            this.numero_de_comercio = cdatabuffer.readString(129, 15);
            this.mensaje = cdatabuffer.readStringZ(144, 41);
            this.retrieval_reference_number = cdatabuffer.readString(185, 12);
            this.pin__working_key = cdatabuffer.readString(197, 8);
            this.id_internacional_de_red = cdatabuffer.readShort(205);
            this.codigo_de_condicion = cdatabuffer.readByte(213);
            this.ticket_original = cdatabuffer.readShort(216);
            this.fecha_original = cdatabuffer.readDate(218);
            this.tipo_de_mensaje_original = cdatabuffer.readShort(226);
            this.numero_de_trace_original = cdatabuffer.readInt(228);
            this.batch = cdatabuffer.readShort(232);
            this.compras = cdatabuffer.readShort(234);
            this.devoluciones = cdatabuffer.readShort(236);
            this.anulaciones = cdatabuffer.readShort(238);
            this.monto_compras = cdatabuffer.readDouble(240);
            this.monto_devoluciones = cdatabuffer.readDouble(248);
            this.monto_anulaciones = cdatabuffer.readDouble(256);
            this.track_1 = cdatabuffer.readStringZ(264, 80);
            this.datos_tarjetas_encriptados = cdatabuffer.readByte(458);
            this.bloque_id_clave = cdatabuffer.readShort(459);
            this.bloque_version = cdatabuffer.readShort(461);
            this.bloque_len = cdatabuffer.readShort(463);
            this.bloque = cdatabuffer.readBytes(465, 512);
        }

        /// <summary>
        /// Desencripta los datos sensibles, si estan encriptados.
        /// </summary>
        public void Decrypt()
        {
            if (this.datos_tarjetas_encriptados != 0)
            {
                byte[] resd = CryptEngine.Decrypt(bloque, (uint)bloque_len, 1);
                CDataBuffer d = new CDataBuffer(resd);
                this.track_1 = d.readStringZ(0, 0x50);
                this.track_2 = d.readStringZ(0x50, 0x26);
                this.fecha_expiracion = d.readString(0x76, 4);
                this.cvv = d.readShort(0x7a);
                this.datos_tarjetas_encriptados = 0;
                Array.Clear(this.bloque, 0, this.bloque.Length);
            }
        }

        /// <summary>
        /// Encripta los datos sensibles, si no estan encriptados ya.
        /// </summary>
        public void Encrypt()
        {
            if (this.datos_tarjetas_encriptados == 0)
            {
                CDataBuffer d = new CDataBuffer(124);
                d.writeStringZ(this.track_1, 0);
                d.writeStringZ(this.track_2, 0x50);
                d.writeString(this.fecha_expiracion, 0x76, 4);
                d.writeShort(this.cvv, 0x7a);

                byte[] resd = CryptEngine.Encrypt(d.Buffer, (uint)d.Buffer.Length, 1);

                if (resd != null)
                {
                    resd.CopyTo(this.bloque, 0);
                    this.bloque_len = (short)resd.Length;
                    this.bloque_id_clave = 0;
                    this.bloque_version = 1;
                    this.track_1 = "";
                    this.track_2 = "";
                    this.fecha_expiracion = "";
                    this.cvv = 0;
                    this.datos_tarjetas_encriptados = 1;
                }
            }
        }

        /// <summary>
        /// Escribe este X25Request sobre un buffer de bytes.
        /// </summary>
        /// <returns>El buffer de bytes con los datos de este X25Request</returns>
        public CDataBuffer WriteBytes()
        {
            CDataBuffer res = new CDataBuffer(977);
            res.writeShort(this.tipo_de_mensaje, 9);
            res.writeInt(this.terminal_id, 4);
            res.writeByte(this.nodo_id, 8);
            res.writeInt(this.codigo_de_procesamiento, 11);
            res.writeByte(this.tipo_de_tarjeta, 15);
            res.writeByte(this.tipo_de_cuenta, 16);
            res.writeShort(this.marca_id, 17);
            res.writeByte(this.modo_de_ingreso, 19);
            res.writeStringZ(this.nro_cta_tarjeta, 20);
            res.writeString(this.fecha_expiracion, 41, 4);
            res.writeStringZ(this.track_2, 45);
            res.writeShort(this.cvv, 83);
            res.writeShort(this.ticket, 85);
            res.writeByte(this.moneda, 87);
            res.writeByte(this.plan, 88);
            res.writeByte(this.cuotas, 89);
            res.writeFloat(this.importe, 90);
            res.writeFloat(this.entrega, 94);
            res.writeFloat(this.importe_cuota, 98);
            res.writeFloat(this.saldo, 102);
            res.writeTime(this.hora_local, 107);
            res.writeDate(this.fecha_local, 111);
            res.writeInt(this.autorizacion, 115);
            res.writeShort(this.codigo_de_respuesta, 119);
            res.writeInt(this.numero_de_trace, 121);
            res.writeInt(this.numero_de_terminal, 125);
            res.writeString(this.numero_de_comercio, 129, 15);
            res.writeStringZ(this.mensaje, 144);
            res.writeString(this.retrieval_reference_number, 185, 12);
            res.writeString(this.pin__working_key, 197, 8);
            res.writeShort(this.id_internacional_de_red, 205);
            res.writeByte(this.codigo_de_condicion, 213);
            res.writeShort(this.ticket_original, 216);
            res.writeDate(this.fecha_original, 218);
            res.writeShort(this.tipo_de_mensaje_original, 226);
            res.writeInt(this.numero_de_trace_original, 228);
            res.writeShort(this.batch, 232);
            res.writeShort(this.compras, 234);
            res.writeShort(this.devoluciones, 236);
            res.writeShort(this.anulaciones, 238);
            res.writeDouble(this.monto_compras, 240);
            res.writeDouble(this.monto_devoluciones, 248);
            res.writeDouble(this.monto_anulaciones, 256);
            res.writeStringZ(this.track_1, 264);
            res.writeByte(this.datos_tarjetas_encriptados, 458);
            res.writeShort(this.bloque_id_clave, 459);
            res.writeShort(this.bloque_version, 461);
            res.writeShort(this.bloque_len, 463);
            res.writeBytes(this.bloque, 465);
           
            return res;
        }
    }
}

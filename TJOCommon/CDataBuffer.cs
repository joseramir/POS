using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Bitnet.TJOCommon
{
    /// <summary>
    /// Permite manejar un buffer de bytes, agregando o seteando distintas partes del mismo
    /// con tipos manejados.
    /// </summary>
    public class CDataBuffer
    {
        /// <summary>
        /// Buffer que es manejado por esta clase. Solo se modifica en el constructor.
        /// </summary>
        byte[] buf;

        /// <summary>
        /// El largo actual del buffer.
        /// </summary>
        int len;

        /// <summary>
        /// La 'posicion actual' en el buffer, para los metodos de lectura/escritura secuencial
        /// </summary>
        int pos;
        /// <summary>
        /// Inicializa el buffer a partir de un buffer existente.
        /// </summary>
        /// <param name="abyte0">El buffer que manejara este CDataBuffer.</param>
        public CDataBuffer(byte[] abyte0)
        {
            buf = abyte0;
            pos = 0;
            len = abyte0.Length;
        }

        /// <summary>
        /// Inicializa un nuevo buffer con hasta 'len' bytes de capacidad.
        /// </summary>
        /// <param name="len"></param>
        public CDataBuffer(int len)
        {
            this.len = len;
            buf = new byte[len];
            pos = 0;
        }

        /// <summary>
        /// Limpia el buffer, poniendo todos sus bytes a cero.
        /// </summary>
        public void clear()
        {
            for (int i = 0; i < len; i++)
                buf[i] = 0;
        }

        /// <summary>
        /// Limpia 'j' bytes del buffer, a partir de la posicion 'i'
        /// </summary>
        /// <param name="i"></param>
        /// <param name="j"></param>
        public void clear(int i, int j)
        {
            while (j-- > 0 && i < len)
                buf[i++] = 0;
        }

        /// <summary>
        /// Lee un byte desde el buffer.
        /// </summary>
        /// <param name="i">La posicion del buffer desde donde se leera el byte</param>
        /// <returns>El byte pedido.</returns>
        public byte readByte(int i)
        {
            if (i < len)
            {
                byte b = buf[i];
                return b;
            }
            return 0;
        }

        /// <summary>
        /// Lee un byte del buffer, desde la posicion 'actual'
        /// </summary>
        /// <returns>El byte pedido</returns>
        public byte readByte()
        {
            if (pos < len)
            {
                byte b = readByte(pos++);
                return b;
            }
            return 0;
        }

        /// <summary>
        /// Lee una secuencia de bytes, desde la posicion actual.
        /// </summary>
        /// <param name="i">La cantidad de bytes a leer, desde la posicion actual.</param>
        /// <returns>Los bytes requeridos.</returns>
        public byte[] readBytes(int i)
        {
            byte[] res = new byte[i];
            for (int j = 0; j < i; j++)
                res[j] = readByte();
            return res;
        }

        /// <summary>
        /// Lee una secuencia de 'j' bytes, desde la posicion 'i'.
        /// </summary>
        /// <param name="i">La posicion desde la que leeran los bytes</param>
        /// <param name="j">La cantidad de bytes a retornar.</param>
        /// <returns></returns>
        public byte[] readBytes(int i, int j)
        {
            byte[] res = new byte[j];
            for (int k = 0; k < j; k++)
                res[k] = readByte(i + k);
            return res;
        }

        /// <summary>
        /// Lee un short (entero de 16 bits)
        /// </summary>
        /// <param name="i">La posicion desde la que se leera el short</param>
        /// <returns>El short pedido</returns>
        public short readShort(int i)
        {
            if (i + 1 < len)
            {
                short word = (short)(buf[i + 1] << 8 | buf[i] & 0xff);
                return word;
            }
            return 0;
        }

        /// <summary>
        /// Lee un short (entero de 16 bits) desde la posicion actual.
        /// </summary>
        /// <returns>El short pedido.</returns>
        public short readShort()
        {
            if (pos + 1 < len)
            {
                short word = readShort(pos);
                pos += 2;
                return word;
            }
            return 0;
        }

        /// <summary>
        /// Lee un int (entero de 32 bits) desde la posicion que se indica.
        /// </summary>
        /// <param name="i">La posicion desde la cual leer el int</param>
        /// <returns>El int pedido</returns>
        public int readInt(int i)
        {
            if (i + 3 < len)
            {
                int j = (buf[i + 3] & 0xff) << 24 |
                        (buf[i + 2] & 0xff) << 16 |
                        (buf[i + 1] & 0xff) << 8 |
                         buf[i] & 0xff;
                return j;
            }
            return 0;
        }

        /// <summary>
        /// Lee un int (entero de 32 bits) desde la posicion actual.
        /// </summary>
        /// <returns>El int pedido</returns>
        public int readInt()
        {
            if (pos + 3 < len)
            {
                int i = readInt(pos);
                pos += 4;
                return i;
            }
            return 0;
        }

        /// <summary>
        /// Lee un long (entero de 64 bits) desde la posicion que se indica
        /// </summary>
        /// <param name="i">La posicion desde la que se leera el long</param>
        /// <returns>El long pedido</returns>
        public long readLong(int i)
        {
            if (i + 7 < len)
            {
                long l = (long)(
                          (ulong)(((ulong)(buf[i + 7] & 0xff)) << 56) |
                          (ulong)(((ulong)(buf[i + 6] & 0xff)) << 48) |
                          (ulong)(((ulong)(buf[i + 5] & 0xff)) << 40) |
                          (ulong)(((ulong)(buf[i + 4] & 0xff)) << 32) |
                          (ulong)(((ulong)(buf[i + 3] & 0xff)) << 24) |
                          (ulong)(((ulong)(buf[i + 2] & 0xff)) << 16) |
                          (ulong)(((ulong)(buf[i + 1] & 0xff)) << 8)  |
                          (ulong)(((ulong)(buf[i] & 0xff)))
                          );
                return l;
            }
            return 0;
        }

        /// <summary>
        /// Lee un long (entero de 64 bits) desde la posicion actual
        /// </summary>
        /// <returns></returns>
        public long readLong()
        {
            if (pos + 7 < len)
            {
                long l = readLong(pos);
                pos += 8;
                return l;
            }
            return 0;
        }

        /// <summary>
        /// Lee un float (numero decimal de precision simple - 4 bytes) desde la posicion que se indica.
        /// </summary>
        /// <param name="i">La posicion desde la que se debe leer el float</param>
        /// <returns>El float pedido</returns>
        public float readFloat(int i)
        {
            if (i + 3 < len)
                return BitConverter.ToSingle(buf, i);
            return 0;
        }

        /// <summary>
        /// Lee un float (numero decimal de precision simple - 4 bytes) desde la posicion actual.
        /// </summary>
        /// <returns>El float pedido</returns>
        public float readFloat()
        {
            if (pos + 3 < len)
            {
                float f = readFloat(pos);
                pos += 4;
                return f;
            }
            return 0;
        }

        /// <summary>
        /// Lee un double (numero decimal de precision doble - 8 bytes) desde la posicion que se indica.
        /// </summary>
        /// <param name="i">La posicion desde la cual se debe leer el double</param>
        /// <returns>El double pedido</returns>
        public double readDouble(int i)
        {
            if (i + 7 < len)
                return BitConverter.ToDouble(buf, i);
            return 0;
        }

        /// <summary>
        /// Lee un double (numero decimal de precision doble - 8 bytes) desde la posicion actual.
        /// </summary>
        /// <returns>El double pedido</returns>
        public double readDouble()
        {
            if (pos + 7 < len)
            {
                double d = readDouble(pos);
                pos += 8;
                return d;
            }
            return 0;
        }

        /// <summary>
        /// Lee una string desde la posicion que se indica.
        /// </summary>
        /// <param name="i">La posicion desde la que se debe leer la string.</param>
        /// <param name="j">La cantidad de bytes que la string ocupa en el buffer.</param>
        /// <returns>La string pedida</returns>
        public string readString(int i, int j)
        {
            StringBuilder sb = new StringBuilder();
            for (int k = i; k < i + j; k++)
            {
                if (k < len)
                    sb.Append((char)buf[k]);
                else
                    sb.Append(' ');
            }
            return sb.ToString();
        }

        /// <summary>
        /// Lee una string, desde la posicion actual.
        /// </summary>
        /// <param name="i">La cantidad de bytes que la string ocupa en el buffer</param>
        /// <returns>La string pedida.</returns>
        public string readString(int i)
        {
            string s = readString(pos, i);
            pos += i;
            return s;
        }

        /// <summary>
        /// Lee una string cuyo fin esta marcado por el byte 0, desde la posicion que se indica.
        /// </summary>
        /// <param name="i">La posicion desde la que se leera la string</param>
        /// <param name="j">El maximo largo de la string en el buffer</param>
        /// <returns>La string pedida</returns>
        public String readStringZ(int i, int j)
        {
            int k;
            for (k = i; k < i + j; k++)
                if (buf[k] == 0)
                    break;

            j = k - i;
            return readString(i, j);
        }

        /// <summary>
        /// Lee una fecha desde el buffer. La fecha se almacena en el buffer como el siguiente entero:
        /// fecha = año * 0x10000 + mes * 0x100 + dia
        /// </summary>
        /// <param name="i">La posicion desde la que leera la fecha</param>
        /// <returns>LA fecha pedida. Hora/Minutos/Segundos se devuelven en cero.</returns>
        public DateTime readDate(int i)
        {
            long i1 = readInt(i);
            int y = (int)((i1 & 0xffff0000) >> 16);
            int m = (int)((i1 & 0xff00) >> 8);
            int d = (int)(i1 & 0xff);

            if (y == 0) y++;
            if (m == 0) m++;
            if (d == 0) d++;
            return new DateTime(y, m, d);
        }

        /// <summary>
        /// Lee una hora desde el buffer. La hora se almacena como:
        /// xx = hora * 0x1000000 + minutos * 0x10000 + segundos * 0x100 (Queda espacio para
        /// 'milisegundos' que, actualmente, no se usa).
        /// </summary>
        /// <param name="i">La posicion desde la cual se leera la hora.</param>
        /// <returns>La hora pedida. Dia/Mes/Año se retornan en cero.</returns>
        public DateTime readTime(int i)
        {
            long i1 = readInt(i);
            int h = (int)((i1 & 0xff000000) >> 24);
            int m = (int)((i1 & 0xff0000) >> 16);
            int s = (int)((i1 & 0xff00) >> 8);
            return new DateTime(0, 0, 0, h, m, s);
        }

        /// <summary>
        /// Lee una fecha/hora desde el buffer. Los datos estan almacenados en dos enteros 32
        /// consecutivos, con el formato indicado en 'readDate' y 'readTime0
        /// </summary>
        /// <param name="i">La posicion en el buffer donde se encuentra la fecha</param>
        /// <param name="j">La posicion en el buffer donde se encuentra la hora</param>
        /// <returns>La fecha/hora pedida</returns>
        public DateTime readDateTime(int i, int j)
        {
            long tm = readInt(j);
            long dt = readInt(i);

            int h = (int)((tm & 0xff000000) >> 24);
            int m = (int)((tm & 0xff0000) >> 16);
            int s = (int)((tm & 0xff00) >> 8);

            int y = (int)((dt & 0xffff0000) >> 16);
            int mo = (int)((dt & 0xff00) >> 8);
            int d = (int)(dt & 0xff);

            if (y == 0) y = 1980;
            if (mo == 0) mo++;
            if (d == 0) d++;
            return new DateTime(y, mo, d, h, m, s);
        }

        /// <summary>
        /// Escribe un byte en el buffer
        /// </summary>
        /// <param name="i">El byte a escribir</param>
        /// <param name="j">La posicion donde se escribira el byte</param>
        public void writeByte(int i, int j)
        {
            if (j < len)
                buf[j] = (byte)(0xff & i);
        }

        /// <summary>
        /// Escribe un byte en el buffer, en la posicion actual.
        /// </summary>
        /// <param name="i">El byte a escribir en el buffer</param>
        public void writeByte(int i)
        {
            if (pos < len)
            {
                writeByte(i, pos);
                pos++;
            }
        }

        /// <summary>
        /// Escribe una secuencia de bytes en el buffer
        /// </summary>
        /// <param name="abyte">La secuencia de bytes a escribir en el buffer</param>
        /// <param name="i">La posicion del buffer donde se escribiran los bytes</param>
        public void writeBytes(byte[] abyte, int i)
        {
            for (int j = 0; j < abyte.Length; j++)
                if (i + j < len)
                    buf[i + j] = abyte[j];
        }

        /// <summary>
        /// Escribe una secuencia de bytes en el buffer, a partir de la posicion actual.
        /// </summary>
        /// <param name="abyte">La secuencia de bytes a escribir en el buffer</param>
        public void writeBytes(byte[] abyte)
        {
            writeBytes(abyte, pos);
            pos += abyte.Length;
            if (pos >= len) pos = len - 1;

        }

        /// <summary>
        /// Escribe un short (entero de 16 bits) en el buffer.
        /// </summary>
        /// <param name="i">El short que se escribira</param>
        /// <param name="j">La posicion del buffer donde se escribira el short</param>
        public void writeShort(int i, int j)
        {
            if (j + 1 < len)
            {
                buf[j + 1] = (byte)(0xff & (i >> 8));
                buf[j] = (byte)(0xff & i);
            }
        }

        /// <summary>
        /// Escribe un short (entero de 16 bits) en el buffer, a partir de la posicion actual.
        /// </summary>
        /// <param name="i">El short a escribir</param>
        public void writeShort(int i)
        {
            writeShort(i, pos);
            pos += 2;
            if (pos >= len) pos = len - 1;
        }

        /// <summary>
        /// Escribe un int (entero de 32 bits) en el buffer
        /// </summary>
        /// <param name="i">El entero a escribir</param>
        /// <param name="j">La posicion en el buffer donde se escribira el int</param>
        public void writeInt(int i, int j)
        {
            if (j + 3 < len)
            {
                buf[j + 3] = (byte)(0xff & (i >> 24));
                buf[j + 2] = (byte)(0xff & (i >> 16));
                buf[j + 1] = (byte)(0xff & (i >> 8));
                buf[j] = (byte)(0xff & i);
            }
        }

        /// <summary>
        /// Escribe un int (entero de 32 bits) en el buffer, a partir de la posicion actual.
        /// </summary>
        /// <param name="i">El entero a escribir</param>
        public void writeInt(int i)
        {
            writeInt(i, pos);
            pos += 4;
            if (pos >= len) pos = len - 1;
        }

        /// <summary>
        /// Escribe un long (entero de 64 bits) en el buffer.
        /// </summary>
        /// <param name="l">El long a escribir</param>
        /// <param name="i">La posicion del buffer desde la que se escribira el long</param>
        public void writeLong(long l, int i)
        {
            if (i + 7 < len)
            {
                buf[i + 7] = (byte)(255L & (l >> 56));
                buf[i + 6] = (byte)(255L & (l >> 48));
                buf[i + 5] = (byte)(255L & (l >> 40));
                buf[i + 4] = (byte)(255L & (l >> 32));
                buf[i + 3] = (byte)(255L & (l >> 24));
                buf[i + 2] = (byte)(255L & (l >> 16));
                buf[i + 1] = (byte)(255L & (l >> 8));
                buf[i] = (byte)(255L & l);
            }
        }

        /// <summary>
        /// Escribe un long (entero de 64 bits) en el buffer, a partir de la posicion actual
        /// </summary>
        /// <param name="l">El long a escribir</param>
        public void writeLong(long l)
        {
            writeLong(l, pos);
            pos += 8;
            if (pos >= len) pos = len - 1;
        }

        /// <summary>
        /// Escribe un float (decimal de precision simple - 4 bytes) en el buffer.
        /// </summary>
        /// <param name="f">El float a escribir</param>
        /// <param name="i">La posicion del buffer en la que se escribira el float</param>
        public void writeFloat(float f, int i)
        {
            byte[] b = BitConverter.GetBytes(f);
            writeBytes(b, i);
        }

        /// <summary>
        /// Escribe un float (decimal de precision simple - 4 bytes) en el buffer.
        /// </summary>
        /// <param name="f">El float a escribir</param>
        public void writeFloat(float f)
        {
            writeFloat(f, pos);
            pos += 4;
            if (pos >= len) pos = len - 1;
        }

        /// <summary>
        /// Escribe un double (decimal de precision doble - 8 bytes) en el buffer
        /// </summary>
        /// <param name="d">El double a escribir</param>
        /// <param name="i">La posicion en el buffer en la cual se escribira el double</param>
        public void writeDouble(double d, int i)
        {
            byte[] b = BitConverter.GetBytes(d);
            writeBytes(b, i);
        }

        /// <summary>
        /// Escribe un double (decimal de precision doble - 8 bytes) en la posicion actual del buffer
        /// </summary>
        /// <param name="d">El double a escribir</param>
        public void writeDouble(double d)
        {
            writeDouble(d, pos);
            pos += 8;
            if (pos >= len) pos = len - 1;
        }

        /// <summary>
        /// Escribe una string en el buffer
        /// </summary>
        /// <param name="s">La string a escribir.</param>
        /// <param name="i">La posicion del buffer donde se escribira la string</param>
        /// <param name="maxlen">El largo reservado en el buffer para la string. Si el largo de la string
        /// fuera inferior a 'maxlen', despues de la string se escribiran espacios en blanco.</param>
        public void writeString(string s, int i, int maxlen)
        {
            if (s.Length > maxlen) s = s.Substring(0, maxlen);
            int j;
            for (j = 0; j < s.Length; j++)
                if (i + j < len)
                    buf[i + j] = (byte)(s[j]);

            while (j < maxlen && (i + j) < len)
                buf[i + j++] = 32;
        }

        /// <summary>
        /// Escribe una string en el buffer. En el buffer, el final de la string es marcado por un byte 0.
        /// </summary>
        /// <param name="s">La string a escribir</param>
        /// <param name="i">La posicion del buffer donde escribir la string.</param>
        /// <remarks>Se presupone que el buffer tiene suficientes bytes como para contener cualquier
        /// posible string que se le pase a este metodo.</remarks>
        public void writeStringZ(string s, int i)
        {
            int j;
            for (j = 0; j < s.Length; j++)
                if (i + j < len)
                    buf[i + j] = (byte)(s[j]);
            buf[i + j] = 0;
        }

        /// <summary>
        /// Escribe una fecha en el buffer. Ver 'readDate' por el formato con el que se almacena la
        /// fecha en el buffer.
        /// </summary>
        /// <param name="date">La fecha a escribir en el buffer.</param>
        /// <param name="i">LA posicion del buffer a donde escribir la fecha.</param>
        public void writeDate(DateTime date, int i)
        {
            int j = date.Year << 16 | date.Month << 8 | date.Day;
            writeInt(j, i);
        }

        /// <summary>
        /// Escribe una hora en el buffer. Ver 'readTime' por el formato con que se almacena la hora
        /// en el buffer.
        /// </summary>
        /// <param name="time">La hora a almacenar en el buffer.</param>
        /// <param name="i">La posicion del buffer en la que escribir la hora</param>
        public void writeTime(DateTime time, int i)
        {
            int j = time.Hour << 24 | time.Minute << 16 | time.Second << 8 | 0;
            writeInt(j, i);
        }

        /// <summary>
        /// Propiedad que da acceso al buffer manejado por esta clase.
        /// </summary>
        public byte[] Buffer { get { return buf; } }

        public int Posicion
        {
            get { return pos; }
            set
            {
                if (pos >= len) pos = len - 1;
                if (pos < 0) pos = 0;
                pos = value;
            }
        }
    }
}

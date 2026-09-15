using System;
using System.Globalization;
using System.Security.Cryptography;
using System.Text;

namespace LibEntidades.Alberdi
{
    /// <summary>
    /// Calcula el Seq de un comprobante a partir de sus datos (UUID version 5, RFC 4122).
    ///
    /// Antes el Seq era un Guid aleatorio creado junto con el HeaderDoc. Si la caja se cae
    /// despues de grabar el comprobante en el buffer y antes de dar el ticket por cerrado
    /// (act_trans en WriteEOPFiscal), al reiniciar se reprocesa el trans, el ticket se vuelve a
    /// cerrar con un HeaderDoc nuevo y viajaba con OTRO Seq: ni el INSERT OR IGNORE del buffer ni
    /// el indice unico del webapi lo frenaban. Derivado de los datos, ese segundo cierre tiene el
    /// mismo Seq: el buffer lo ignora y el webapi responde 409.
    ///
    /// Sigue siendo un GUID de 36 caracteres, asi que el contrato con el webapi no cambia.
    /// </summary>
    public static class SeqComprobante
    {
        /// <summary>
        /// Espacio de nombres de los Seq del POS. NO CAMBIARLO: cambiaria el Seq de todos los
        /// comprobantes y un reintento dejaria de reconocerse como el mismo.
        /// </summary>
        private static readonly Guid Espacio = new Guid("3f6c2b1e-8d4a-4c7e-9b52-1a7d0e6f4c93");

        /// <summary>
        /// Devuelve el Seq del comprobante, o null si no hay datos suficientes para identificarlo
        /// (sin numero o sin punto de venta, por ejemplo con el impresor fiscal caido): en ese caso
        /// el llamador conserva el Guid aleatorio, porque dos comprobantes distintos sin numero
        /// terminarian con el mismo Seq y el segundo se perderia como "ya sincronizado".
        ///
        /// La clave lleva, ademas del numero fiscal, la fecha, si esta anulado, el total y la
        /// cantidad de lineas: asi dos comprobantes distintos solo coinciden si repiten TODO eso
        /// (numeracion reiniciada por cambio de impresor el mismo dia con el mismo importe y lineas).
        /// </summary>
        public static string Calcular(HeaderDoc doc)
        {
            if (doc == null || doc.NumeroComprobante <= 0 || doc.PuntoVenta <= 0)
                return null;

            DateTime fecha = (doc.FechaHora == DateTime.MinValue) ? DateTime.Now : doc.FechaHora;

            string clave = string.Format(CultureInfo.InvariantCulture,
                "acumventas|{0}|{1}|{2}|{3}|{4}|{5:yyyyMMdd}|{6}|{7:0.00}|{8}",
                doc.Tienda != null ? doc.Tienda.IdTienda : 0,
                doc.Caja,
                doc.PuntoVenta,
                doc.TipoComprobante,
                doc.NumeroComprobante,
                fecha,
                doc.Anulado ? 1 : 0,
                doc.Total,
                doc.Detalle != null ? doc.Detalle.Count : 0);

            return UuidV5(Espacio, clave).ToString();
        }

        /// <summary>UUID version 5 (SHA-1) segun RFC 4122, seccion 4.3.</summary>
        public static Guid UuidV5(Guid espacio, string nombre)
        {
            // Guid.ToByteArray deja los tres primeros campos en little-endian; el RFC los quiere
            // en orden de red.
            byte[] ns = espacio.ToByteArray();
            OrdenDeRed(ns);
            byte[] nom = Encoding.UTF8.GetBytes(nombre);

            byte[] datos = new byte[ns.Length + nom.Length];
            Buffer.BlockCopy(ns, 0, datos, 0, ns.Length);
            Buffer.BlockCopy(nom, 0, datos, ns.Length, nom.Length);

            byte[] hash;
            using (SHA1 sha1 = SHA1.Create())
                hash = sha1.ComputeHash(datos);

            byte[] g = new byte[16];
            Array.Copy(hash, 0, g, 0, 16);
            g[6] = (byte)((g[6] & 0x0F) | 0x50);   // version 5
            g[8] = (byte)((g[8] & 0x3F) | 0x80);   // variante RFC 4122

            OrdenDeRed(g);
            return new Guid(g);
        }

        private static void OrdenDeRed(byte[] g)
        {
            Intercambiar(g, 0, 3);
            Intercambiar(g, 1, 2);
            Intercambiar(g, 4, 5);
            Intercambiar(g, 6, 7);
        }

        private static void Intercambiar(byte[] b, int i, int j)
        {
            byte t = b[i];
            b[i] = b[j];
            b[j] = t;
        }
    }
}

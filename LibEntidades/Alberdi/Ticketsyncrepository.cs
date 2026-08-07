using System;
using System.Collections.Generic;
using System.Data.SQLite;
using System.IO;
using Newtonsoft.Json;

namespace LibEntidades.Alberdi
{
    /// <summary>
    /// Repositorio local SQLite que actúa como buffer confiable de tickets
    /// antes de sincronizarlos al endpoint central.
    /// 
    /// Requiere: System.Data.SQLite (compatible .NET Framework 3.5)
    ///           Newtonsoft.Json (ya referenciado en el proyecto)
    /// 
    /// La base de datos y la tabla se crean automáticamente si no existen.
    /// Si ya existen, se reutilizan sin modificar datos previos.
    /// </summary>
    public class TicketSyncRepository
    {
        private string _connStr;

        // Estados posibles de un ticket en el buffer
        public const string ESTADO_PENDIENTE         = "PENDIENTE";
        public const string ESTADO_SINCRONIZADO      = "SINCRONIZADO";
        public const string ESTADO_ERROR_PERMANENTE  = "ERROR_PERMANENTE";

        // A partir de este número de intentos fallidos se marca ERROR_PERMANENTE
        private const int MAX_INTENTOS = 10;

        /// <summary>
        /// Inicializa el repositorio.
        /// </summary>
        /// <param name="dbPath">
        ///   Ruta completa al archivo SQLite, ej:
        ///   C:\POS\sync\ticketsync.db
        ///   Si se pasa solo un nombre de archivo se crea en el directorio
        ///   del ejecutable.
        /// </param>
        public TicketSyncRepository(string dbPath)
        {
            // Si solo se pasa nombre de archivo, ubicarlo junto al ejecutable
            if (!Path.IsPathRooted(dbPath))
                dbPath = Path.Combine(
                    AppDomain.CurrentDomain.BaseDirectory, dbPath);

            // Crear el directorio si no existe
            string dir = Path.GetDirectoryName(dbPath);
            if (!string.IsNullOrEmpty(dir) && !Directory.Exists(dir))
                Directory.CreateDirectory(dir);

            _connStr = string.Format(
                "Data Source={0};Version=3;Journal Mode=WAL;Pooling=True;",
                dbPath);

            EnsureSchema();
        }

        // ----------------------------------------------------------------
        //  SCHEMA
        // ----------------------------------------------------------------

        /// <summary>
        /// Crea la tabla ticket_sync si no existe.
        /// Idempotente: se puede llamar siempre al iniciar sin riesgo.
        /// </summary>
        private void EnsureSchema()
        {
            const string ddl = @"
                CREATE TABLE IF NOT EXISTS ticket_sync (
                    id              INTEGER PRIMARY KEY AUTOINCREMENT,
                    seq             TEXT    NOT NULL,
                    idtienda        INTEGER NOT NULL DEFAULT 0,
                    caja            INTEGER NOT NULL DEFAULT 0,
                    payload         TEXT    NOT NULL,
                    estado          TEXT    NOT NULL DEFAULT 'PENDIENTE',
                    intentos        INTEGER NOT NULL DEFAULT 0,
                    ultimo_intento  TEXT,
                    fecha_ticket    TEXT    NOT NULL,
                    error_detalle   TEXT
                );

                -- Índice para acelerar la consulta de pendientes
                CREATE UNIQUE INDEX IF NOT EXISTS uix_seq
                    ON ticket_sync (seq);

                CREATE INDEX IF NOT EXISTS ix_estado
                    ON ticket_sync (estado);";

            using (SQLiteConnection conn = AbrirConexion())
            {
                // CREATE TABLE IF NOT EXISTS permite múltiples llamadas seguras
                using (SQLiteCommand cmd = new SQLiteCommand(ddl, conn))
                    cmd.ExecuteNonQuery();
            }
        }

        // ----------------------------------------------------------------
        //  ESCRITURA
        // ----------------------------------------------------------------

        /// <summary>
        /// Inserta un HeaderDoc como ticket pendiente de sincronización.
        /// Si el mismo Seq ya existe (reinicio, doble llamada) se ignora
        /// silenciosamente: INSERT OR IGNORE garantiza idempotencia.
        /// </summary>
        public void InsertarPendiente(HeaderDoc doc)
        {
            string payload = JsonConvert.SerializeObject(
                doc,
                Formatting.None,
                new JsonSerializerSettings { NullValueHandling = NullValueHandling.Ignore });

            const string sql = @"
                INSERT OR IGNORE INTO ticket_sync
                    (seq, idtienda, caja, payload, estado, fecha_ticket)
                VALUES
                    (@seq, @idtienda, @caja, @payload, 'PENDIENTE', @fecha);";

            using (SQLiteConnection conn = AbrirConexion())
            using (SQLiteCommand cmd = new SQLiteCommand(sql, conn))
            {
                cmd.Parameters.AddWithValue("@seq",      doc.Seq);
                cmd.Parameters.AddWithValue("@idtienda", doc.Tienda != null ? doc.Tienda.IdTienda : 0);
                cmd.Parameters.AddWithValue("@caja",     doc.Caja);
                cmd.Parameters.AddWithValue("@payload",  payload);
                cmd.Parameters.AddWithValue("@fecha",
                    doc.FechaHora == DateTime.MinValue
                        ? DateTime.Now.ToString("o")
                        : doc.FechaHora.ToString("o"));
                cmd.ExecuteNonQuery();
            }
        }

        // ----------------------------------------------------------------
        //  LECTURA
        // ----------------------------------------------------------------

        /// <summary>
        /// Devuelve los tickets pendientes de sincronización,
        /// ordenados cronológicamente (más antiguos primero).
        /// </summary>
        /// <param name="limite">Cantidad máxima a devolver por lote.</param>
        public List<TicketPendiente> ObtenerPendientes(int limite)
        {
            const string sql = @"
                SELECT id, seq, payload, intentos
                FROM   ticket_sync
                WHERE  estado = 'PENDIENTE'
                ORDER  BY fecha_ticket ASC
                LIMIT  @limite;";

            List<TicketPendiente> lista = new List<TicketPendiente>();

            using (SQLiteConnection conn = AbrirConexion())
            using (SQLiteCommand cmd = new SQLiteCommand(sql, conn))
            {
                cmd.Parameters.AddWithValue("@limite", limite);
                using (SQLiteDataReader rdr = cmd.ExecuteReader())
                {
                    while (rdr.Read())
                    {
                        lista.Add(new TicketPendiente
                        {
                            Id       = rdr.GetInt64(0),
                            Seq      = rdr.GetString(1),
                            Payload  = rdr.GetString(2),
                            Intentos = rdr.GetInt32(3)
                        });
                    }
                }
            }
            return lista;
        }

        /// <summary>
        /// Cantidad de tickets pendientes (útil para mostrar en UI o log).
        /// </summary>
        public long ContarPendientes()
        {
            const string sql = "SELECT COUNT(*) FROM ticket_sync WHERE estado = 'PENDIENTE';";
            using (SQLiteConnection conn = AbrirConexion())
            using (SQLiteCommand cmd = new SQLiteCommand(sql, conn))
                return (long)cmd.ExecuteScalar();
        }

        // ----------------------------------------------------------------
        //  ACTUALIZACIÓN DE ESTADO
        // ----------------------------------------------------------------

        /// <summary>
        /// Marca un ticket como sincronizado exitosamente.
        /// </summary>
        public void MarcarSincronizado(long id)
        {
            const string sql = @"
                UPDATE ticket_sync SET
                    estado        = 'SINCRONIZADO',
                    ultimo_intento = @ahora,
                    error_detalle  = NULL
                WHERE id = @id;";

            using (SQLiteConnection conn = AbrirConexion())
            using (SQLiteCommand cmd = new SQLiteCommand(sql, conn))
            {
                cmd.Parameters.AddWithValue("@ahora", DateTime.Now.ToString("o"));
                cmd.Parameters.AddWithValue("@id",    id);
                cmd.ExecuteNonQuery();
            }
        }

        /// <summary>
        /// Registra un intento fallido.
        /// Si se supera MAX_INTENTOS pasa a ERROR_PERMANENTE
        /// (requiere revisión manual / alerta).
        /// </summary>
        public void MarcarError(long id, int intentos, string detalle)
        {
            string nuevoEstado = intentos >= MAX_INTENTOS
                ? ESTADO_ERROR_PERMANENTE
                : ESTADO_PENDIENTE;

            const string sql = @"
                UPDATE ticket_sync SET
                    estado         = @estado,
                    intentos       = @intentos,
                    ultimo_intento = @ahora,
                    error_detalle  = @error
                WHERE id = @id;";

            using (SQLiteConnection conn = AbrirConexion())
            using (SQLiteCommand cmd = new SQLiteCommand(sql, conn))
            {
                cmd.Parameters.AddWithValue("@estado",   nuevoEstado);
                cmd.Parameters.AddWithValue("@intentos", intentos);
                cmd.Parameters.AddWithValue("@ahora",    DateTime.Now.ToString("o"));
                cmd.Parameters.AddWithValue("@error",    detalle ?? "");
                cmd.Parameters.AddWithValue("@id",       id);
                cmd.ExecuteNonQuery();
            }
        }

        // ----------------------------------------------------------------
        //  MANTENIMIENTO
        // ----------------------------------------------------------------

        /// <summary>
        /// Elimina tickets ya sincronizados más antiguos que los días indicados.
        /// Llamar periódicamente (ej: al iniciar la caja cada día) para no
        /// acumular registros innecesarios.
        /// </summary>
        public int LimpiarSincronizados(int diasAntiguedad)
        {
            string fechaCorte = DateTime.Now
                .AddDays(-diasAntiguedad)
                .ToString("o");

            const string sql = @"
                DELETE FROM ticket_sync
                WHERE  estado = 'SINCRONIZADO'
                AND    fecha_ticket < @corte;";

            using (SQLiteConnection conn = AbrirConexion())
            using (SQLiteCommand cmd = new SQLiteCommand(sql, conn))
            {
                cmd.Parameters.AddWithValue("@corte", fechaCorte);
                return cmd.ExecuteNonQuery();
            }
        }

        // ----------------------------------------------------------------
        //  HELPERS
        // ----------------------------------------------------------------

        private SQLiteConnection AbrirConexion()
        {
            SQLiteConnection conn = new SQLiteConnection(_connStr);
            conn.Open();
            return conn;
        }
    }

    // ----------------------------------------------------------------
    //  DTO interno: resultado de ObtenerPendientes
    // ----------------------------------------------------------------

    /// <summary>
    /// Representa una fila pendiente del buffer local.
    /// </summary>
    public class TicketPendiente
    {
        public long   Id       { get; set; }
        public string Seq      { get; set; }
        public string Payload  { get; set; }   // JSON del HeaderDoc completo
        public int    Intentos { get; set; }
    }
}
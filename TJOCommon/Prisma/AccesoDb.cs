using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Data;
using System.Data.SQLite;
using System.Configuration;


namespace Alberdi.TJOCommon.Prisma
{
    public class AccesoDb
    {
        /// <summary>
        /// Metodo utilizado para realizar el mantenimiento de la estructura de tablas
        /// En este caso solo modifica las tablas ActualCierre y Transacciones, agrega la columna TipoVerifone
        /// 
        /// </summary>
        /// <param name="trx"></param>
        /// <param name="rta"></param>
        /// <returns></returns>
        public static bool ModificacionEstructura()
        {
            bool aux = false;
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                //para actualizar el squema
                int actualcierre = 0;
                try
                {
                    cmd.CommandText = "select numact from actualcierre where tipoverifone = 1";
                    actualcierre = Convert.ToInt32(cmd.ExecuteScalar());
                }
                catch (System.Exception ex)
                {
                    try
                    {
                        cmd.CommandText = "ALTER TABLE ActualCierre ADD COLUMN TipoVerifone int not null DEFAULT 1;";
                        cmd.ExecuteNonQuery();
                    }
                    catch (Exception)
                    {

                    }
                    try
                    {
                        cmd.CommandText = "ALTER TABLE transacciones ADD COLUMN TipoVerifone int not null DEFAULT 1;";
                        cmd.ExecuteNonQuery();
                    }
                    catch (Exception)
                    {

                    }
                }                
                conn.Close();
            }
            return aux;
        }
        /// <summary>
        /// ////////////////////////////////////
        /// </summary>
        public static void LimpiarTarjetas()
        {
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "delete from tarjetas";
                cmd.ExecuteNonQuery();
                conn.Close();
            }
        }



        public static void AgregarTarjeta(RtaTablaTarjeta unatar)
        {
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "insert into Tarjetas (Indice, CodigoProcesador, Codigo, Nombre, MaximaCuota, Habilitado) values " +
                    "(@Indice, @CodigoProcesador, @Codigo, @Nombre, @MaximaCuota, @Habilitado);";
                cmd.Parameters.AddWithValue("@Indice", unatar.IndiceRegistro);
                cmd.Parameters.AddWithValue("@CodigoProcesador", unatar.CodigoProcesador);
                cmd.Parameters.AddWithValue("@Codigo", unatar.CodigoTarjeta);
                cmd.Parameters.AddWithValue("@Nombre", unatar.NombreTarjeta);
                cmd.Parameters.AddWithValue("@MaximaCuota", unatar.CantidadCuotas);
                cmd.Parameters.AddWithValue("@Habilitado", true);
                cmd.ExecuteNonQuery();
                conn.Close();
            }
        }

        public static List<RtaTablaTarjeta> GetTarjetas()
        {
            List<RtaTablaTarjeta> lista = null;
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "select * from tarjetas where habilitado = 1;";
                SQLiteDataReader rdr = cmd.ExecuteReader();
                lista = new List<RtaTablaTarjeta>();
                while (rdr.Read())
                {
                    RtaTablaTarjeta trade = new RtaTablaTarjeta();
                    trade.IndiceRegistro = Convert.ToInt32(rdr["Indice"]);
                    trade.CodigoProcesador = rdr["CodigoProcesador"].ToString();
                    trade.CodigoTarjeta = rdr["Codigo"].ToString();
                    trade.NombreTarjeta = rdr["Nombre"].ToString();
                    trade.CantidadCuotas = Convert.ToInt32(rdr["MaximaCuota"]);
                    lista.Add(trade);
                }
                conn.Close();
            }
            return lista;
        }



        public static void LimpiarPlanes()
        {
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "delete from planes";
                cmd.ExecuteNonQuery();
                conn.Close();
            }
        }

        public static void AgregarPlan(RtaTablaPlan unplan)
        {
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "insert into planes (Indice, CodigoTarjeta, CodigoPlan, Nombre, Habilitado) values " +
                    "(@Indice, @CodigoTarjeta, @CodigoPlan, @Nombre, @Habilitado);";
                cmd.Parameters.AddWithValue("@Indice", unplan.IndiceRegistro);
                cmd.Parameters.AddWithValue("@CodigoTarjeta", unplan.CodigoTarjeta);
                cmd.Parameters.AddWithValue("@CodigoPlan", unplan.CodigoPlan);
                cmd.Parameters.AddWithValue("@Nombre", unplan.NombrePlan);
                cmd.Parameters.AddWithValue("@Habilitado", true);
                cmd.ExecuteNonQuery();
                conn.Close();
            }
        }

        public static List<RtaTablaPlan> GetPlanes(string codTarjeta)
        {
            List<RtaTablaPlan> lista = null;
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "select * from planes where CodigoTarjeta = '" + codTarjeta + "' and habilitado = 1;";
                SQLiteDataReader rdr = cmd.ExecuteReader();
                lista = new List<RtaTablaPlan>();
                while (rdr.Read())
                {
                    RtaTablaPlan trade = new RtaTablaPlan();
                    trade.IndiceRegistro = Convert.ToInt32(rdr["Indice"]);
                    trade.CodigoTarjeta = rdr["CodigoTarjeta"].ToString();
                    trade.CodigoPlan = rdr["CodigoPlan"].ToString();
                    trade.NombrePlan = rdr["Nombre"].ToString();
                    lista.Add(trade);
                }
                conn.Close();
            }
            return lista;
        }

        public static bool GuardarTransaccion(TrxPrisma trx, RtaVenta rta)
        {
            bool aux = false;
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                //para actualizar el squema
                int actualcierre = 0;
                //try
                //{
                //    cmd.CommandText = "select numact from actualcierre where tipoverifone = 1";
                //    actualcierre = Convert.ToInt32(cmd.ExecuteScalar());
                //}
                //catch (System.Exception ex)
                //{
                //    cmd.CommandText = "ALTER TABLE ActualCierre ADD COLUMN TipoVerifone int not null DEFAULT 1;";
                //    cmd.ExecuteNonQuery();
                //    cmd.CommandText = "ALTER TABLE transacciones ADD COLUMN TipoVerifone int not null DEFAULT 1;";
                //    cmd.ExecuteNonQuery();
                //}
                cmd.CommandText = "select numact from actualcierre where tipoverifone = 1";
                actualcierre = Convert.ToInt32(cmd.ExecuteScalar());
                cmd.CommandText = "insert into transacciones (monto, nrofactura, cuotas, codigotarjeta, codigoplan, montoextracash, codigocomercio, " +
                    "estado, codigorespuesta, mensajerespuesta, codigoautorizacion, numerocupon, numerolote, nombrecliente, ult4digitos, prim6digitos, " +
                    "fechatransaccion, horatransaccion, terminalid, fhcaja, clavebusq, nrocierre, cerrado, cobrado, tipoverifone) values (@monto, @nrofactura, @cuotas, @codigotarjeta, @codigoplan, " +
                    "@montoextracash, @codigocomercio, @estado, @codigorespuesta, @mensajerespuesta, @codigoautorizacion, @numerocupon, @numerolote, " +
                    "@nombrecliente, @ult4digitos, @prim6digitos, @fechatransaccion, @horatransaccion, @terminalid, @fhcaja, @clavebusq, @nrocierre, @cerrado, 0, 1);";
                cmd.Parameters.AddWithValue("@monto", trx.Monto);
                cmd.Parameters.AddWithValue("@nrofactura", trx.NroFactura);
                cmd.Parameters.AddWithValue("@cuotas", trx.CantCuotas);
                cmd.Parameters.AddWithValue("@codigotarjeta", trx.CodigoTarjeta);
                cmd.Parameters.AddWithValue("@codigoplan", trx.CodigoPlan);
                cmd.Parameters.AddWithValue("@montoextracash", trx.MontoExtraCash);
                cmd.Parameters.AddWithValue("@codigocomercio", trx.CodigoComercio);
                cmd.Parameters.AddWithValue("@estado", trx.Estado);
                cmd.Parameters.AddWithValue("@codigorespuesta", rta.CodigoRespuesta);
                cmd.Parameters.AddWithValue("@mensajerespuesta", rta.MensajeRespuesta);
                cmd.Parameters.AddWithValue("@codigoautorizacion", rta.CodigoAutorizacion);
                cmd.Parameters.AddWithValue("@numerocupon", rta.NumeroCupon);
                cmd.Parameters.AddWithValue("@numerolote", rta.NumeroLote);
                cmd.Parameters.AddWithValue("@nombrecliente", rta.NombreCliente);
                cmd.Parameters.AddWithValue("@ult4digitos", rta.Ult4Digitos);
                cmd.Parameters.AddWithValue("@prim6digitos", rta.Prim6Digitos);
                cmd.Parameters.AddWithValue("@fechatransaccion", rta.FechaTransaccion);
                cmd.Parameters.AddWithValue("@horatransaccion", rta.HoraTransaccion);
                cmd.Parameters.AddWithValue("@terminalid", rta.TerminalId);
                cmd.Parameters.AddWithValue("@fhcaja", DateTime.Now);
                cmd.Parameters.AddWithValue("@clavebusq", string.Format("{0}{1}{2}", rta.NumeroLote.ToString().PadLeft(3, '0'),
                    rta.NumeroCupon.ToString().PadLeft(4, '0'), rta.CodigoAutorizacion.PadLeft(6, '0')));
                cmd.Parameters.AddWithValue("@nrocierre", actualcierre);
                cmd.Parameters.AddWithValue("@cerrado", false);
                aux = cmd.ExecuteNonQuery() == 1;
                conn.Close();
            }
            return aux;
        }

        public static bool GuardarTransaccionQr(TrxPrisma trx, RtaQrModo rta)
        {
            bool aux = false;
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "select numact from actualcierre where tipoverifone = 1";
                int actualcierre = Convert.ToInt32(cmd.ExecuteScalar());
                cmd.CommandText = "insert into transacciones (monto, nrofactura, cuotas, codigotarjeta, codigoplan, montoextracash, codigocomercio, " +
                    "estado, codigorespuesta, mensajerespuesta, codigoautorizacion, numerocupon, numerolote, nombrecliente, ult4digitos, prim6digitos, " +
                    "fechatransaccion, horatransaccion, terminalid, fhcaja, clavebusq, nrocierre, cerrado, cobrado, tipoverifone) values (@monto, @nrofactura, @cuotas, @codigotarjeta, @codigoplan, " +
                    "@montoextracash, @codigocomercio, @estado, @codigorespuesta, @mensajerespuesta, @codigoautorizacion, @numerocupon, @numerolote, " +
                    "@nombrecliente, @ult4digitos, @prim6digitos, @fechatransaccion, @horatransaccion, @terminalid, @fhcaja, @clavebusq, @nrocierre, @cerrado, @cobrado, 1);";
                cmd.Parameters.AddWithValue("@monto", trx.Monto);
                cmd.Parameters.AddWithValue("@nrofactura", trx.NroFactura);
                cmd.Parameters.AddWithValue("@cuotas", trx.CantCuotas);
                cmd.Parameters.AddWithValue("@codigotarjeta", trx.CodigoTarjeta);
                cmd.Parameters.AddWithValue("@codigoplan", trx.CodigoPlan);
                cmd.Parameters.AddWithValue("@montoextracash", trx.MontoExtraCash);
                cmd.Parameters.AddWithValue("@codigocomercio", trx.CodigoComercio);
                cmd.Parameters.AddWithValue("@estado", trx.Estado);
                cmd.Parameters.AddWithValue("@codigorespuesta", rta.CodigoRespuesta);
                cmd.Parameters.AddWithValue("@mensajerespuesta", rta.MensajeRespuesta);
                cmd.Parameters.AddWithValue("@codigoautorizacion", rta.CodigoAutorizacion);
                cmd.Parameters.AddWithValue("@numerocupon", rta.NumeroCupon);
                cmd.Parameters.AddWithValue("@numerolote", rta.NumeroLote);
                cmd.Parameters.AddWithValue("@nombrecliente", rta.NombreCliente);
                cmd.Parameters.AddWithValue("@ult4digitos", rta.Ult4Digitos);
                cmd.Parameters.AddWithValue("@prim6digitos", rta.Prim6Digitos);
                cmd.Parameters.AddWithValue("@fechatransaccion", rta.FechaTransaccion);
                cmd.Parameters.AddWithValue("@horatransaccion", rta.HoraTransaccion);
                cmd.Parameters.AddWithValue("@terminalid", rta.TerminalId);
                cmd.Parameters.AddWithValue("@fhcaja", DateTime.Now);
                cmd.Parameters.AddWithValue("@clavebusq", string.Format("{0}{1}{2}", rta.NumeroLote.ToString().PadLeft(3, '0'),
                    rta.NumeroCupon.ToString().PadLeft(4, '0'), rta.CodigoAutorizacion.PadLeft(6, '0')));
                cmd.Parameters.AddWithValue("@nrocierre", actualcierre);
                cmd.Parameters.AddWithValue("@cerrado", false);
                cmd.Parameters.AddWithValue("@cobrado", rta.ImporteCobrado);
                aux = cmd.ExecuteNonQuery() == 1;
                conn.Close();
            }
            return aux;
        }

        //rta tienen que venir con los datos de lote, cupon, autorizacion al combinar esos datos se obtiene la clave de busqueda
        public static bool GetTransaccion(ref TrxPrisma trx, ref RtaVenta rta)
        {
            bool aux = false;
            //rta = null;
            string clave = string.Format("{0}{1}{2}", rta.NumeroLote.ToString().PadLeft(3, '0'),
                rta.NumeroCupon.ToString().PadLeft(4, '0'), rta.CodigoAutorizacion.PadLeft(6, '0'));
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "select id, monto, nrofactura, cuotas, codigotarjeta, codigoplan, montoextracash, codigocomercio, " +
                    "estado, codigorespuesta, mensajerespuesta, codigoautorizacion, numerocupon, numerolote, nombrecliente, ult4digitos, prim6digitos, " +
                    "fechatransaccion, horatransaccion, terminalid, fhcaja, nrocierre, cerrado, cobrado from transacciones where clavebusq = '" +
                    clave + "' and tipoverifone = 1";
                SQLiteDataReader rdr = cmd.ExecuteReader();
                if (rdr.HasRows)
                {
                    rdr.Read();
                    trx.Id = Convert.ToInt32(rdr["Id"]);
                    trx.Monto = Convert.ToDecimal(rdr["monto"]);
                    trx.NroFactura = rdr["nrofactura"].ToString();
                    trx.CantCuotas = Convert.ToInt32(rdr["cuotas"]);
                    trx.CodigoTarjeta = rdr["codigotarjeta"].ToString();
                    trx.CodigoPlan = rdr["codigoplan"].ToString();
                    trx.MontoExtraCash = Convert.ToDecimal(rdr["montoextracash"]);
                    trx.CodigoComercio = rdr["codigocomercio"].ToString();
                    trx.Estado = Convert.ToInt32(rdr["estado"]);

                    rta.CodigoRespuesta = Convert.ToInt32(rdr["codigorespuesta"]);
                    rta.MensajeRespuesta = rdr["mensajerespuesta"].ToString();
                    rta.CodigoAutorizacion = rdr["codigoautorizacion"].ToString();
                    rta.NumeroCupon = Convert.ToInt32(rdr["numerocupon"]);
                    rta.NumeroLote = Convert.ToInt32(rdr["numerolote"]);
                    rta.NombreCliente = rdr["nombrecliente"].ToString();
                    rta.Ult4Digitos = rdr["ult4digitos"].ToString();
                    rta.Prim6Digitos = rdr["prim6digitos"].ToString();
                    rta.FechaTransaccion = rdr["fechatransaccion"].ToString();
                    rta.HoraTransaccion = rdr["horatransaccion"].ToString();
                    rta.Cerrado = Convert.ToBoolean(rdr["cerrado"]);
                    aux = true;
                }
                conn.Close();
            }
            return aux;
        }

        //Actualiza el estado de la transaccion
        //usado en caso de Anulacion y devolucion de una transaccion de venta
        public static bool WriteEstadoTransaccion(TrxPrisma trx)
        {
            bool aux = false;
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "update transacciones set estado =@estado where id = @id";
                cmd.Parameters.AddWithValue("@estado", trx.Estado);
                cmd.Parameters.AddWithValue("@id", trx.Id);
                aux = cmd.ExecuteNonQuery() == 1;
                conn.Close();
            }
            return aux;
        }

        public static bool WriteCierre()
        {
            bool aux = false;
            using (SQLiteConnection conn = new SQLiteConnection(ConfigurationManager.ConnectionStrings["dbPrismatrans"].ConnectionString))
            {
                conn.Open();
                SQLiteCommand cmd = conn.CreateCommand();
                cmd.CommandText = "select numact from actualcierre where tipoverifone = 1";
                int actualcierre = Convert.ToInt32(cmd.ExecuteScalar());

                cmd.CommandText = "update transacciones set cerrado = 1 where nrocierre = " + actualcierre + " and tipoverifone = 1";
                cmd.ExecuteNonQuery();

                if (actualcierre == 100000)
                    actualcierre = 1;
                else
                    actualcierre++;
                cmd.CommandText = "update actualcierre set numact = " + actualcierre + " and tipoverifone = 1";
                cmd.ExecuteNonQuery();
                cmd.CommandText = "delete from transacciones where fhcaja < datetime('now', '-30 days') and tipoverifone = 1";
                cmd.ExecuteNonQuery();
                aux = true;
                conn.Close();
            }
            return aux;
        }
    }

}

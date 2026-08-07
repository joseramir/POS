using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Data;
using System.Data.SqlClient;

namespace TransSql
{
    public class DbTrans
    {
        //public static List<DRegTrans> GetTicket(DataTable dt)
        //{
        //    List<DRegTrans> lista = new List<DRegTrans>();                
        //    foreach (DataRow dr in dt.Rows)
        //    {
        //        switch (Convert.ToInt16(dr["func"]))
        //        {
        //            case 2:
        //                lista.Add(GetPago(dr));
        //                break;
        //            case 3:
        //                lista.Add(GetEOP(dr));
        //                break;
        //            case 4:
        //                lista.Add(GetItem(dr));
        //                break;
        //            case 12:
        //                lista.Add(GetEnvase(dr));
        //                break;
        //            case 19:
        //                lista.Add(GetTickFact(dr));
        //                break;
        //            case 20:
        //                lista.Add(GetCcOnln(dr));
        //                break;
        //            case 26:
        //                break;
        //            case 28:
        //                break;

        //        }
        //    }
        //    return lista;
        //}

        public static DRegTrans GetDItem(DataRow dr)
        {
            DRegTrans regtrans = new DRegTrans();
            switch (Convert.ToInt16(dr["func"]))
            {
                case 2:
                    regtrans = GetPago(dr);
                    break;
                case 3:
                    regtrans = GetEOP(dr);
                    break;
                case 4:
                     regtrans = GetItem(dr);
                    break;
                case 12:
                    regtrans = GetEnvase(dr);
                    break;
                case 19:
                    regtrans = GetTickFact(dr);
                    break;
                case 20:
                    regtrans = GetCcOnln(dr);
                    break;
                case 26:
                    break;
                case 28:
                    break;
                case 31:
                    break;
                case 38:
                    break;
                case 40:
                    regtrans = GetVendMay(dr);
                    break;
            }            
            return regtrans;
        }

        private static MDPlu GetItem(DataRow dr)
        {
            MDPlu item = new MDPlu();
            item.Func = 4;
            item.Cod = Convert.ToUInt32(dr["cod"]);
            item.S_c_flag = Convert.ToBoolean(dr["uso_scan"]) ? 'T' : 'F';
            item.UCant = Convert.ToBoolean(dr["uso_cant"]) ? 'T' : 'F';
            item.Iva = Convert.ToUInt16(dr["iva"]);
            item.Pesado = Convert.ToBoolean(dr["pesado"]) ? 'T' : 'F';
            item.Dnro = Convert.ToUInt16(dr["nro"]);
            item.Envas = Convert.ToUInt32(dr["envase"]);
            item.YReg = dr["cant"].ToString();
            item.XReg = dr["imp"].ToString();
            item.Miles = dr["millas"].ToString();
            item.OrigCode = dr["teorico"].ToString();
            item.OtroP = Convert.ToUInt16(dr["otrop"]);
            item.Anulado = Convert.ToBoolean(dr["anulado"]) ? 'T' : 'F';
            item.Precuni = Convert.ToUInt32(dr["vuelto"]);
            item.Uni = Convert.ToInt16(dr["numero"]);
            item.EsBulto = Convert.ToBoolean(dr["u_vuelto"]) ? 'T' : 'F';
            item.Dun14 = Convert.ToBoolean(dr["flag"]) ? 'T' : 'F';
            item.Sdnro = Convert.ToInt16(dr["posi"]);
            item.Impiva = dr["impiva"].ToString();
            item.Precmay = dr["fecha_c"].ToString();
            item.Ppuntual = Convert.ToUInt32(dr["fecha_z"]);
            item.Umillas = Convert.ToInt16(dr["modo"]);
            item.Caja = Convert.ToInt16(dr["caja"]);
            item.Tienda = dr["tienda"].ToString();
            item.CnlVta = dr["cnlvta"].ToString();
            item.Fecha = Convert.ToUInt32(dr["fecha"]);
            item.Hora = Convert.ToUInt32(dr["hora"]);
            item.Cajero = Convert.ToInt16(dr["cajero"]);            
            return item;
        }

        public static MDMpag GetPago(DataRow dr)
        {
            MDMpag medio = new MDMpag();
            medio.Func = 4;
            medio.Nro = Convert.ToInt16(dr["nro"]);
            medio.UVuelto = Convert.ToBoolean(dr["u_vuelto"]) ? 'T' : 'F';
            medio.UPend = Convert.ToBoolean(dr["u_pend"]) ? 'T' : 'F';
            medio.Importe = dr["imp"].ToString();
            medio.Vuelto = dr["vuelto"].ToString();
            medio.Cc = dr["codcli"].ToString();
            medio.Numero = dr["numero"].ToString();
            medio.Fecha_c = dr["fecha_c"].ToString();
            medio.Nombre = dr["nombre"].ToString();
            medio.Dni = dr["dni"].ToString();
            medio.Telef = dr["telefono"].ToString();
            medio.Direc = dr["direcc"].ToString();
            medio.Local = dr["local"].ToString();
            medio.Civa = dr["condiva"].ToString();
            medio.Barrio = dr["barrio"].ToString();
            medio.Mtodesc = dr["cant"].ToString();
            medio.UOnline = Convert.ToBoolean(dr["flag"]) ? 'T' : 'F';
            medio.Dtarjeta = dr["teorico"].ToString();
            medio.Nticket = Convert.ToUInt32(dr["fecha_z"]);
            medio.Alipercep = Convert.ToInt32(dr["modo"]);
            medio.Escredito = Convert.ToBoolean(dr["uso_cant"]) ? 'T' : 'F';
            medio.Esposnet = Convert.ToBoolean(dr["uso_scan"]) ? 'T' : 'F';
            medio.Caja = Convert.ToInt16(dr["caja"]);
            medio.Tienda = dr["tienda"].ToString();
            medio.CnlVta = dr["cnlvta"].ToString();
            medio.Fecha = Convert.ToUInt32(dr["fecha"]);
            medio.Hora = Convert.ToUInt32(dr["hora"]);
            medio.Cajero = Convert.ToInt16(dr["cajero"]);            
            return medio;
        }

        private static MDEop GetEOP(DataRow dr)
        {
            MDEop fin = new MDEop();
            fin.Func = 3;
            fin.Tipo = Convert.ToUInt16(dr["tipo"]);
            fin.Ntick = Convert.ToUInt32(dr["ticket"]);
            fin.Imp = dr["imp"].ToString();
            fin.AnulFlag = Convert.ToBoolean(dr["flag"]) ? 'T' : 'F';            
            fin.MontoAnul = dr["cant"].ToString();
            fin.Ptovta = Convert.ToInt16(dr["cod"]);
            fin.Caja = Convert.ToInt16(dr["caja"]);
            fin.Tienda = dr["tienda"].ToString();
            fin.CnlVta = dr["cnlvta"].ToString();
            fin.Fecha = Convert.ToUInt32(dr["fecha"]);
            fin.Hora = Convert.ToUInt32(dr["hora"]);
            fin.Cajero = Convert.ToInt16(dr["cajero"]);            
            return fin;
        }

        private static MDTickFac GetTickFact(DataRow dr)
        {
            MDTickFac tick = new MDTickFac();
            tick.Func = 19;
            tick.Nro = Convert.ToInt16(dr["nro"]);
            tick.Flag = Convert.ToBoolean(dr["flag"]) ? 'T' : 'F';
            tick.Caja = Convert.ToInt16(dr["caja"]);
            tick.Tienda = dr["tienda"].ToString();
            tick.CnlVta = dr["cnlvta"].ToString();
            tick.Fecha = Convert.ToUInt32(dr["fecha"]);
            tick.Hora = Convert.ToUInt32(dr["hora"]);
            tick.Cajero = Convert.ToInt16(dr["cajero"]);            
            return tick;
        }

        private static MDCcOnln GetCcOnln(DataRow dr)
        {
            MDCcOnln clien = new MDCcOnln();
            clien.Func = 20;
            clien.Ctacte = Convert.ToInt64(dr["nro"]);
            clien.Flagprec = Convert.ToInt16(dr["ticket"]);
            clien.Caja = Convert.ToInt16(dr["caja"]);
            clien.Tienda = dr["tienda"].ToString();
            clien.CnlVta = dr["cnlvta"].ToString();
            clien.Fecha = Convert.ToUInt32(dr["fecha"]);
            clien.Hora = Convert.ToUInt32(dr["hora"]);
            clien.Cajero = Convert.ToInt16(dr["cajero"]);
            return clien;
        }

        private static MDPercep GetPercep(DataRow dr)
        {
            MDPercep percep = new MDPercep();
            percep.Func = 28;
            percep.Percep = dr["imp"].ToString();
            percep.Montoneto = dr["teorico"].ToString();
            percep.Alicuota = dr["cant"].ToString();
            percep.Caja = Convert.ToInt16(dr["caja"]);
            percep.Tienda = dr["tienda"].ToString();
            percep.CnlVta = dr["cnlvta"].ToString();
            percep.Fecha = Convert.ToUInt32(dr["fecha"]);
            percep.Hora = Convert.ToUInt32(dr["hora"]);
            percep.Cajero = Convert.ToInt16(dr["cajero"]);
            return percep;
        }

        /// ///////////////////////////////////////////////////////////////////
        private static MDPerTissh GetPerTissh(DataRow dr)
        {
            MDPerTissh percep = new MDPerTissh();
            percep.Func = 31;
            percep.Percep = dr["imp"].ToString();
            percep.Montoneto = dr["teorico"].ToString();
            percep.Alicuota = dr["cant"].ToString();
            percep.Caja = Convert.ToInt16(dr["caja"]);
            percep.Tienda = dr["tienda"].ToString();
            percep.CnlVta = dr["cnlvta"].ToString();
            percep.Fecha = Convert.ToUInt32(dr["fecha"]);
            percep.Hora = Convert.ToUInt32(dr["hora"]);
            percep.Cajero = Convert.ToInt16(dr["cajero"]);
            return percep;
        }

        private static MDPerIva GetPerIva(DataRow dr)
        {
            MDPerIva percep = new MDPerIva();
            percep.Func = 38;
            percep.Percep = dr["imp"].ToString();
            percep.Montoneto = dr["teorico"].ToString();
            percep.Alicuota = dr["cant"].ToString();
            percep.Caja = Convert.ToInt16(dr["caja"]);
            percep.Tienda = dr["tienda"].ToString();
            percep.CnlVta = dr["cnlvta"].ToString();
            percep.Fecha = Convert.ToUInt32(dr["fecha"]);
            percep.Hora = Convert.ToUInt32(dr["hora"]);
            percep.Cajero = Convert.ToInt16(dr["cajero"]);
            return percep;
        }

        private static MDVendMay GetVendMay(DataRow dr)
        {
            MDVendMay percep = new MDVendMay();
            percep.Func = 40;
            percep.NroVendedor = Convert.ToInt16(dr["nro"]);
            percep.Caja = Convert.ToInt16(dr["caja"]);
            percep.Tienda = dr["tienda"].ToString();
            percep.CnlVta = dr["cnlvta"].ToString();
            percep.Fecha = Convert.ToUInt32(dr["fecha"]);
            percep.Hora = Convert.ToUInt32(dr["hora"]);
            percep.Cajero = Convert.ToInt16(dr["cajero"]);
            return percep;
        }

        private static MDEnvas GetEnvase(DataRow dr)
        {
            MDEnvas env = new MDEnvas();
            env.Func = 12;
            env.Cod = Convert.ToUInt32(dr["cod"]);
            env.Cant = Convert.ToInt16(dr["cant"]);
            env.Flag = Convert.ToBoolean(dr["flag"]) ? 'T' : 'F';
            env.Caja = Convert.ToInt16(dr["caja"]);
            env.Tienda = dr["tienda"].ToString();
            env.CnlVta = dr["cnlvta"].ToString();
            env.Fecha = Convert.ToUInt32(dr["fecha"]);
            env.Hora = Convert.ToUInt32(dr["hora"]);
            env.Cajero = Convert.ToInt16(dr["cajero"]);
            return env;
        }

        private static MDPromo GetPromo(DataRow dr)
        {
            MDPromo promo = new MDPromo();
            promo.Func = 26;
            promo.Nro = Convert.ToInt16(dr["nro"]);
            promo.Cod = Convert.ToInt32(dr["cod"]);
            promo.Codpromo = Convert.ToInt64(dr["envase"]);
            promo.Tpremio = Convert.ToInt16(dr["mpag"]);
            promo.Imprime = Convert.ToBoolean(dr["flag"]) ? 'T' : 'F';
            promo.Linea = Convert.ToInt16(dr["posi"]);
            promo.P1 = dr["direcc"].ToString();
            promo.P2 = dr["barrio"].ToString();
            promo.P3 = dr["local"].ToString();
            promo.Cantpromo = dr["cant"].ToString();
            promo.Preciouni = dr["vuelto"].ToString();
            promo.Caja = Convert.ToInt16(dr["caja"]);
            promo.Tienda = dr["tienda"].ToString();
            promo.CnlVta = dr["cnlvta"].ToString();
            promo.Fecha = Convert.ToUInt32(dr["fecha"]);
            promo.Hora = Convert.ToUInt32(dr["hora"]);
            promo.Cajero = Convert.ToInt16(dr["cajero"]);
            return promo;
        }
    }

    public class WriteSqlTrans
    {
        public static void InsertPlu(long numseq, MDPlu item)
        {
            using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
            {
                SqlCommand cmd = con.CreateCommand();
                cmd.CommandType = CommandType.Text;
                cmd.CommandText = "INSERT INTO Trans VALUES (@seq,@tienda,@caja,@func,@nprec,@mpag,@cajero,@imp," +
                    "@teorico,@nro,@flag,@uso_cant,@iva,@cant,@que_anula,@posi,@cod,@uso_scan,@pesado,@envase," +
                    "@millas,@otrop,@u_vuelto,@u_pend,@vuelto,@codcli,@numero,@fecha_c,@nombre,@dni,@telefono,@direcc," +
                    "@barrio,@condiva,@local,@tipo,@ticket,@zeta,@fecha_z,@modo,@anulado,@fecha,@hora,@cnlvta," +
                    "@impiva,@estado)";
                cmd.Parameters.AddWithValue("@seq", numseq);
                cmd.Parameters.AddWithValue("@tienda", item.Tienda);
                cmd.Parameters.AddWithValue("@caja", (decimal) item.Caja);
                cmd.Parameters.AddWithValue("@func", (decimal) item.Func);
                cmd.Parameters.AddWithValue("@nprec", 0);
                cmd.Parameters.AddWithValue("@mpag", 0);
                cmd.Parameters.AddWithValue("@cajero", (decimal) item.Cajero);
                cmd.Parameters.AddWithValue("@imp", item.XReg);
                cmd.Parameters.AddWithValue("@teorico", item.OrigCode);
                cmd.Parameters.AddWithValue("@nro", (decimal) item.Dnro);
                cmd.Parameters.AddWithValue("@flag", item.Dun14 != '\0');
                cmd.Parameters.AddWithValue("@uso_cant", item.UCant != '\0');
                cmd.Parameters.AddWithValue("@iva", (decimal) item.Iva);
                cmd.Parameters.AddWithValue("@cant", item.YReg);
                cmd.Parameters.AddWithValue("@que_anula", 0);
                cmd.Parameters.AddWithValue("@posi", (decimal) item.Sdnro);
                cmd.Parameters.AddWithValue("@cod", (decimal) item.Cod);
                cmd.Parameters.AddWithValue("@uso_scan", item.S_c_flag != '\0');
                cmd.Parameters.AddWithValue("@pesado", item.Pesado != '\0');
                cmd.Parameters.AddWithValue("@envase", (decimal) item.Envas);
                cmd.Parameters.AddWithValue("@millas", item.Miles);
                cmd.Parameters.AddWithValue("@otrop", (decimal) item.OtroP);
                cmd.Parameters.AddWithValue("@u_vuelto", item.EsBulto != '\0');
                cmd.Parameters.AddWithValue("@u_pend", false);
                cmd.Parameters.AddWithValue("@vuelto", (decimal) item.Precuni);
                cmd.Parameters.AddWithValue("@codcli", string.Empty);
                cmd.Parameters.AddWithValue("@numero", (decimal) item.Uni);
                cmd.Parameters.AddWithValue("@fecha_c", item.Precmay);
                cmd.Parameters.AddWithValue("@nombre", string.Empty);
                cmd.Parameters.AddWithValue("@dni", string.Empty);
                cmd.Parameters.AddWithValue("@telefono", string.Empty);
                cmd.Parameters.AddWithValue("@direcc", string.Empty);                
                cmd.Parameters.AddWithValue("@barrio", string.Empty);
                cmd.Parameters.AddWithValue("@condiva", string.Empty);
                cmd.Parameters.AddWithValue("@local", string.Empty);
                cmd.Parameters.AddWithValue("@tipo", 0);
                cmd.Parameters.AddWithValue("@ticket", 0);
                cmd.Parameters.AddWithValue("@zeta", 0);
                cmd.Parameters.AddWithValue("@fecha_z", (decimal) item.Ppuntual);
                cmd.Parameters.AddWithValue("@modo", (decimal) item.Umillas);
                cmd.Parameters.AddWithValue("@anulado", item.Anulado != '\0');
                cmd.Parameters.AddWithValue("@fecha", (decimal) item.Fecha);
                cmd.Parameters.AddWithValue("@hora", (decimal) item.Hora);
                cmd.Parameters.AddWithValue("@cnlvta", item.CnlVta);
                cmd.Parameters.AddWithValue("@impiva", item.Impiva);
                cmd.Parameters.AddWithValue("@estado", 3);
                con.Open();
                cmd.ExecuteNonQuery();
                con.Close();
            }
        }


        public static void InsertMPago(long numseq, MDMpag medio)
        {
            try
            {
                using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
                {
                    SqlCommand cmd = con.CreateCommand();
                    cmd.CommandType = CommandType.Text;
                    cmd.CommandText = "INSERT INTO Trans VALUES (@seq,@tienda,@caja,@func,@nprec,@mpag,@cajero,@imp," +
                        "@teorico,@nro,@flag,@uso_cant,@iva,@cant,@que_anula,@posi,@cod,@uso_scan,@pesado,@envase," +
                        "@millas,@otrop,@u_vuelto,@u_pend,@vuelto,@codcli,@numero,@fecha_c,@nombre,@dni,@telefono,@direcc," +
                        "@barrio,@condiva,@local,@tipo,@ticket,@zeta,@fecha_z,@modo,@anulado,@fecha,@hora,@cnlvta," +
                        "@impiva,@estado)";
                    cmd.Parameters.AddWithValue("@seq", numseq);
                    cmd.Parameters.AddWithValue("@tienda", medio.Tienda);
                    cmd.Parameters.AddWithValue("@caja", (decimal) medio.Caja);
                    cmd.Parameters.AddWithValue("@func", (decimal) medio.Func);
                    cmd.Parameters.AddWithValue("@nprec", 0);
                    cmd.Parameters.AddWithValue("@mpag", 0);
                    cmd.Parameters.AddWithValue("@cajero", (decimal) medio.Cajero);
                    cmd.Parameters.AddWithValue("@imp", medio.Importe);
                    cmd.Parameters.AddWithValue("@teorico", medio.Dtarjeta);
                    cmd.Parameters.AddWithValue("@nro", (decimal) medio.Nro);
                    cmd.Parameters.AddWithValue("@flag", medio.UOnline != '\0');
                    cmd.Parameters.AddWithValue("@uso_cant", medio.Escredito != '\0');
                    cmd.Parameters.AddWithValue("@iva", 0);
                    cmd.Parameters.AddWithValue("@cant", medio.Mtodesc);
                    cmd.Parameters.AddWithValue("@que_anula", 0);
                    cmd.Parameters.AddWithValue("@posi", 0);
                    cmd.Parameters.AddWithValue("@cod", 0);
                    cmd.Parameters.AddWithValue("@uso_scan", medio.Esposnet != '\0');
                    cmd.Parameters.AddWithValue("@pesado", false);
                    cmd.Parameters.AddWithValue("@envase", 0);
                    cmd.Parameters.AddWithValue("@millas", string.Empty);
                    cmd.Parameters.AddWithValue("@otrop", 0);
                    cmd.Parameters.AddWithValue("@u_vuelto", medio.UVuelto != '\0');
                    cmd.Parameters.AddWithValue("@u_pend", medio.UPend != '\0');
                    cmd.Parameters.AddWithValue("@vuelto", medio.Vuelto);
                    cmd.Parameters.AddWithValue("@codcli", medio.Cc);
                    cmd.Parameters.AddWithValue("@numero", medio.Numero);
                    cmd.Parameters.AddWithValue("@fecha_c", medio.Fecha_c);
                    cmd.Parameters.AddWithValue("@nombre", medio.Nombre);
                    cmd.Parameters.AddWithValue("@dni", medio.Dni);
                    cmd.Parameters.AddWithValue("@telefono", medio.Telef);
                    cmd.Parameters.AddWithValue("@direcc", medio.Direc);
                    cmd.Parameters.AddWithValue("@barrio", medio.Barrio);
                    cmd.Parameters.AddWithValue("@condiva", medio.Civa);
                    cmd.Parameters.AddWithValue("@local", medio.Local);
                    cmd.Parameters.AddWithValue("@tipo", 0);
                    cmd.Parameters.AddWithValue("@ticket", 0);
                    cmd.Parameters.AddWithValue("@zeta", 0);
                    cmd.Parameters.AddWithValue("@fecha_z", (decimal) medio.Nticket);
                    cmd.Parameters.AddWithValue("@modo", (decimal) medio.Alipercep);
                    cmd.Parameters.AddWithValue("@anulado", false);
                    cmd.Parameters.AddWithValue("@fecha", (decimal) medio.Fecha);
                    cmd.Parameters.AddWithValue("@hora", (decimal) medio.Hora);
                    cmd.Parameters.AddWithValue("@cnlvta", medio.CnlVta);
                    cmd.Parameters.AddWithValue("@impiva", string.Empty);
                    cmd.Parameters.AddWithValue("@estado", 3);
                    con.Open();
                    cmd.ExecuteNonQuery();
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                DatosSql.WriteLog(ex, "Exporta");
                throw ex;	
            }
        }

        public static void InsertDEop(long numseq, MDEop fin)
        {
            using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
            {
                SqlCommand cmd = con.CreateCommand();
                cmd.CommandType = CommandType.Text;
                cmd.CommandText = "INSERT INTO Trans VALUES (@seq,@tienda,@caja,@func,@nprec,@mpag,@cajero,@imp," +
                    "@teorico,@nro,@flag,@uso_cant,@iva,@cant,@que_anula,@posi,@cod,@uso_scan,@pesado,@envase," +
                    "@millas,@otrop,@u_vuelto,@u_pend,@vuelto,@codcli,@numero,@fecha_c,@nombre,@dni,@telefono,@direcc," +
                    "@barrio,@condiva,@local,@tipo,@ticket,@zeta,@fecha_z,@modo,@anulado,@fecha,@hora,@cnlvta," +
                    "@impiva,@estado)";
                cmd.Parameters.AddWithValue("@seq", numseq);
                cmd.Parameters.AddWithValue("@tienda", fin.Tienda);
                cmd.Parameters.AddWithValue("@caja", (decimal) fin.Caja);
                cmd.Parameters.AddWithValue("@func", (decimal) fin.Func);
                cmd.Parameters.AddWithValue("@nprec", 0);
                cmd.Parameters.AddWithValue("@mpag", 0);
                cmd.Parameters.AddWithValue("@cajero", (decimal) fin.Cajero);
                cmd.Parameters.AddWithValue("@imp", fin.Imp);
                cmd.Parameters.AddWithValue("@teorico", string.Empty);
                cmd.Parameters.AddWithValue("@nro", 0);
                cmd.Parameters.AddWithValue("@flag", fin.AnulFlag != '\0');
                cmd.Parameters.AddWithValue("@uso_cant", false);
                cmd.Parameters.AddWithValue("@iva", 0);
                cmd.Parameters.AddWithValue("@cant", fin.MontoAnul);
                cmd.Parameters.AddWithValue("@que_anula", 0);
                cmd.Parameters.AddWithValue("@posi", 0);
                cmd.Parameters.AddWithValue("@cod", (decimal) fin.Ptovta);
                cmd.Parameters.AddWithValue("@uso_scan", false);
                cmd.Parameters.AddWithValue("@pesado", false);
                cmd.Parameters.AddWithValue("@envase", 0);
                cmd.Parameters.AddWithValue("@millas", string.Empty);
                cmd.Parameters.AddWithValue("@otrop", 0);
                cmd.Parameters.AddWithValue("@u_vuelto", false);
                cmd.Parameters.AddWithValue("@u_pend", false);
                cmd.Parameters.AddWithValue("@vuelto", string.Empty);
                cmd.Parameters.AddWithValue("@codcli", string.Empty);
                cmd.Parameters.AddWithValue("@numero", string.Empty);
                cmd.Parameters.AddWithValue("@fecha_c", string.Empty);
                cmd.Parameters.AddWithValue("@nombre", string.Empty);
                cmd.Parameters.AddWithValue("@dni", string.Empty);
                cmd.Parameters.AddWithValue("@telefono", string.Empty);
                cmd.Parameters.AddWithValue("@direcc", string.Empty);
                cmd.Parameters.AddWithValue("@barrio", string.Empty);
                cmd.Parameters.AddWithValue("@condiva", string.Empty);
                cmd.Parameters.AddWithValue("@local", string.Empty);
                cmd.Parameters.AddWithValue("@tipo", (decimal) fin.Tipo);
                cmd.Parameters.AddWithValue("@ticket", (decimal) fin.Ntick);
                cmd.Parameters.AddWithValue("@zeta", 0);
                cmd.Parameters.AddWithValue("@fecha_z", 0);
                cmd.Parameters.AddWithValue("@modo", 0);
                cmd.Parameters.AddWithValue("@anulado", false);
                cmd.Parameters.AddWithValue("@fecha", (decimal) fin.Fecha);
                cmd.Parameters.AddWithValue("@hora", (decimal) fin.Hora);
                cmd.Parameters.AddWithValue("@cnlvta", fin.CnlVta);
                cmd.Parameters.AddWithValue("@impiva", string.Empty);
                cmd.Parameters.AddWithValue("@estado", 3);
                con.Open();
                cmd.ExecuteNonQuery();
                con.Close();                
            }
            Exporta.FinTicket(numseq, fin.Caja, fin.Cajero);
        }

        public static void InsertDPromo(long numseq, MDPromo promo)
        {
            using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
            {
                SqlCommand cmd = con.CreateCommand();
                cmd.CommandType = CommandType.Text;
                cmd.CommandText = "INSERT INTO Trans VALUES (@seq,@tienda,@caja,@func,@nprec,@mpag,@cajero,@imp," +
                    "@teorico,@nro,@flag,@uso_cant,@iva,@cant,@que_anula,@posi,@cod,@uso_scan,@pesado,@envase," +
                    "@millas,@otrop,@u_vuelto,@u_pend,@vuelto,@codcli,@numero,@fecha_c,@nombre,@dni,@telefono,@direcc," +
                    "@barrio,@condiva,@local,@tipo,@ticket,@zeta,@fecha_z,@modo,@anulado,@fecha,@hora,@cnlvta," +
                    "@impiva,@estado)";
                cmd.Parameters.AddWithValue("@seq", numseq);
                cmd.Parameters.AddWithValue("@tienda", promo.Tienda);
                cmd.Parameters.AddWithValue("@caja", (decimal) promo.Caja);
                cmd.Parameters.AddWithValue("@func", (decimal) promo.Func);
                cmd.Parameters.AddWithValue("@nprec", 0);
                cmd.Parameters.AddWithValue("@mpag", (decimal) promo.Tpremio);
                cmd.Parameters.AddWithValue("@cajero", (decimal) promo.Cajero);
                cmd.Parameters.AddWithValue("@imp", string.Empty);
                cmd.Parameters.AddWithValue("@teorico", string.Empty);
                cmd.Parameters.AddWithValue("@nro", (decimal) promo.Nro);
                cmd.Parameters.AddWithValue("@flag", promo.Imprime != '\0');
                cmd.Parameters.AddWithValue("@uso_cant", false);
                cmd.Parameters.AddWithValue("@iva", 0);
                cmd.Parameters.AddWithValue("@cant", promo.Cantpromo);
                cmd.Parameters.AddWithValue("@que_anula", 0);
                cmd.Parameters.AddWithValue("@posi", promo.Linea);
                cmd.Parameters.AddWithValue("@cod", promo.Cod);
                cmd.Parameters.AddWithValue("@uso_scan", false);
                cmd.Parameters.AddWithValue("@pesado", false);
                cmd.Parameters.AddWithValue("@envase", (decimal) promo.Codpromo);
                cmd.Parameters.AddWithValue("@millas", string.Empty);
                cmd.Parameters.AddWithValue("@otrop", 0);
                cmd.Parameters.AddWithValue("@u_vuelto", false);
                cmd.Parameters.AddWithValue("@u_pend", false);
                cmd.Parameters.AddWithValue("@vuelto", promo.Preciouni);
                cmd.Parameters.AddWithValue("@codcli", string.Empty);
                cmd.Parameters.AddWithValue("@numero", string.Empty);
                cmd.Parameters.AddWithValue("@fecha_c", string.Empty);
                cmd.Parameters.AddWithValue("@nombre", string.Empty);
                cmd.Parameters.AddWithValue("@dni", string.Empty);
                cmd.Parameters.AddWithValue("@telefono", string.Empty);
                cmd.Parameters.AddWithValue("@direcc", promo.P1);
                cmd.Parameters.AddWithValue("@barrio", promo.P2);
                cmd.Parameters.AddWithValue("@condiva", string.Empty);
                cmd.Parameters.AddWithValue("@local", promo.P3);
                cmd.Parameters.AddWithValue("@tipo", 0);
                cmd.Parameters.AddWithValue("@ticket", 0);
                cmd.Parameters.AddWithValue("@zeta", 0);
                cmd.Parameters.AddWithValue("@fecha_z", 0);
                cmd.Parameters.AddWithValue("@modo", 0);
                cmd.Parameters.AddWithValue("@anulado", false);
                cmd.Parameters.AddWithValue("@fecha", (decimal) promo.Fecha);
                cmd.Parameters.AddWithValue("@hora", (decimal) promo.Hora);
                cmd.Parameters.AddWithValue("@cnlvta", promo.CnlVta);
                cmd.Parameters.AddWithValue("@impiva", string.Empty);
                cmd.Parameters.AddWithValue("@estado", 3);
                con.Open();
                cmd.ExecuteNonQuery();
                con.Close();
            }
        }

        public static void InsertPercep(long numseq, MDPercep percep)
        {
            using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
            {
                SqlCommand cmd = con.CreateCommand();
                cmd.CommandType = CommandType.Text;
                cmd.CommandText = "INSERT INTO Trans VALUES (@seq,@tienda,@caja,@func,@nprec,@mpag,@cajero,@imp," +
                    "@teorico,@nro,@flag,@uso_cant,@iva,@cant,@que_anula,@posi,@cod,@uso_scan,@pesado,@envase," +
                    "@millas,@otrop,@u_vuelto,@u_pend,@vuelto,@codcli,@numero,@fecha_c,@nombre,@dni,@telefono,@direcc," +
                    "@barrio,@condiva,@local,@tipo,@ticket,@zeta,@fecha_z,@modo,@anulado,@fecha,@hora,@cnlvta," +
                    "@impiva,@estado)";
                cmd.Parameters.AddWithValue("@seq", numseq);
                cmd.Parameters.AddWithValue("@tienda", percep.Tienda);
                cmd.Parameters.AddWithValue("@caja", (decimal) percep.Caja);
                cmd.Parameters.AddWithValue("@func", (decimal) percep.Func);
                cmd.Parameters.AddWithValue("@nprec", 0);
                cmd.Parameters.AddWithValue("@mpag", 0);
                cmd.Parameters.AddWithValue("@cajero", (decimal) percep.Cajero);
                cmd.Parameters.AddWithValue("@imp", percep.Percep);
                cmd.Parameters.AddWithValue("@teorico", percep.Montoneto);
                cmd.Parameters.AddWithValue("@nro", 0);
                cmd.Parameters.AddWithValue("@flag", false);
                cmd.Parameters.AddWithValue("@uso_cant", false);
                cmd.Parameters.AddWithValue("@iva", 0);
                cmd.Parameters.AddWithValue("@cant", percep.Alicuota);
                cmd.Parameters.AddWithValue("@que_anula", 0);
                cmd.Parameters.AddWithValue("@posi", 0);
                cmd.Parameters.AddWithValue("@cod", 0);
                cmd.Parameters.AddWithValue("@uso_scan", false);
                cmd.Parameters.AddWithValue("@pesado", false);
                cmd.Parameters.AddWithValue("@envase", 0);
                cmd.Parameters.AddWithValue("@millas", string.Empty);
                cmd.Parameters.AddWithValue("@otrop", 0);
                cmd.Parameters.AddWithValue("@u_vuelto", false);
                cmd.Parameters.AddWithValue("@u_pend", false);
                cmd.Parameters.AddWithValue("@vuelto", string.Empty);
                cmd.Parameters.AddWithValue("@codcli", string.Empty);
                cmd.Parameters.AddWithValue("@numero", string.Empty);
                cmd.Parameters.AddWithValue("@fecha_c", string.Empty);
                cmd.Parameters.AddWithValue("@nombre", string.Empty);
                cmd.Parameters.AddWithValue("@dni", string.Empty);
                cmd.Parameters.AddWithValue("@telefono", string.Empty);
                cmd.Parameters.AddWithValue("@direcc", string.Empty);
                cmd.Parameters.AddWithValue("@barrio", string.Empty);
                cmd.Parameters.AddWithValue("@condiva", string.Empty);
                cmd.Parameters.AddWithValue("@local", string.Empty);
                cmd.Parameters.AddWithValue("@tipo", 0);
                cmd.Parameters.AddWithValue("@ticket", 0);
                cmd.Parameters.AddWithValue("@zeta", 0);
                cmd.Parameters.AddWithValue("@fecha_z", 0);
                cmd.Parameters.AddWithValue("@modo", 0);
                cmd.Parameters.AddWithValue("@anulado", false);
                cmd.Parameters.AddWithValue("@fecha", (decimal) percep.Fecha);
                cmd.Parameters.AddWithValue("@hora", (decimal) percep.Hora);
                cmd.Parameters.AddWithValue("@cnlvta", percep.CnlVta);
                cmd.Parameters.AddWithValue("@impiva", string.Empty);
                cmd.Parameters.AddWithValue("@estado", 3);
                con.Open();
                cmd.ExecuteNonQuery();
                con.Close();
            }
        }

        public static void InsertPerTissh(long numseq, MDPerTissh percep)
        {
            using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
            {
                SqlCommand cmd = con.CreateCommand();
                cmd.CommandType = CommandType.Text;
                cmd.CommandText = "INSERT INTO Trans VALUES (@seq,@tienda,@caja,@func,@nprec,@mpag,@cajero,@imp," +
                    "@teorico,@nro,@flag,@uso_cant,@iva,@cant,@que_anula,@posi,@cod,@uso_scan,@pesado,@envase," +
                    "@millas,@otrop,@u_vuelto,@u_pend,@vuelto,@codcli,@numero,@fecha_c,@nombre,@dni,@telefono,@direcc," +
                    "@barrio,@condiva,@local,@tipo,@ticket,@zeta,@fecha_z,@modo,@anulado,@fecha,@hora,@cnlvta," +
                    "@impiva,@estado)";
                cmd.Parameters.AddWithValue("@seq", numseq);
                cmd.Parameters.AddWithValue("@tienda", percep.Tienda);
                cmd.Parameters.AddWithValue("@caja", (decimal)percep.Caja);
                cmd.Parameters.AddWithValue("@func", (decimal)percep.Func);
                cmd.Parameters.AddWithValue("@nprec", 0);
                cmd.Parameters.AddWithValue("@mpag", 0);
                cmd.Parameters.AddWithValue("@cajero", (decimal)percep.Cajero);
                cmd.Parameters.AddWithValue("@imp", percep.Percep);
                cmd.Parameters.AddWithValue("@teorico", percep.Montoneto);
                cmd.Parameters.AddWithValue("@nro", 0);
                cmd.Parameters.AddWithValue("@flag", false);
                cmd.Parameters.AddWithValue("@uso_cant", false);
                cmd.Parameters.AddWithValue("@iva", 0);
                cmd.Parameters.AddWithValue("@cant", percep.Alicuota);
                cmd.Parameters.AddWithValue("@que_anula", 0);
                cmd.Parameters.AddWithValue("@posi", 0);
                cmd.Parameters.AddWithValue("@cod", 0);
                cmd.Parameters.AddWithValue("@uso_scan", false);
                cmd.Parameters.AddWithValue("@pesado", false);
                cmd.Parameters.AddWithValue("@envase", 0);
                cmd.Parameters.AddWithValue("@millas", string.Empty);
                cmd.Parameters.AddWithValue("@otrop", 0);
                cmd.Parameters.AddWithValue("@u_vuelto", false);
                cmd.Parameters.AddWithValue("@u_pend", false);
                cmd.Parameters.AddWithValue("@vuelto", string.Empty);
                cmd.Parameters.AddWithValue("@codcli", string.Empty);
                cmd.Parameters.AddWithValue("@numero", string.Empty);
                cmd.Parameters.AddWithValue("@fecha_c", string.Empty);
                cmd.Parameters.AddWithValue("@nombre", string.Empty);
                cmd.Parameters.AddWithValue("@dni", string.Empty);
                cmd.Parameters.AddWithValue("@telefono", string.Empty);
                cmd.Parameters.AddWithValue("@direcc", string.Empty);
                cmd.Parameters.AddWithValue("@barrio", string.Empty);
                cmd.Parameters.AddWithValue("@condiva", string.Empty);
                cmd.Parameters.AddWithValue("@local", string.Empty);
                cmd.Parameters.AddWithValue("@tipo", 0);
                cmd.Parameters.AddWithValue("@ticket", 0);
                cmd.Parameters.AddWithValue("@zeta", 0);
                cmd.Parameters.AddWithValue("@fecha_z", 0);
                cmd.Parameters.AddWithValue("@modo", 0);
                cmd.Parameters.AddWithValue("@anulado", false);
                cmd.Parameters.AddWithValue("@fecha", (decimal)percep.Fecha);
                cmd.Parameters.AddWithValue("@hora", (decimal)percep.Hora);
                cmd.Parameters.AddWithValue("@cnlvta", percep.CnlVta);
                cmd.Parameters.AddWithValue("@impiva", string.Empty);
                cmd.Parameters.AddWithValue("@estado", 3);
                con.Open();
                cmd.ExecuteNonQuery();
                con.Close();
            }
        }

        public static void InsertPerIva(long numseq, MDPerIva percep)
        {
            using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
            {
                SqlCommand cmd = con.CreateCommand();
                cmd.CommandType = CommandType.Text;
                cmd.CommandText = "INSERT INTO Trans VALUES (@seq,@tienda,@caja,@func,@nprec,@mpag,@cajero,@imp," +
                    "@teorico,@nro,@flag,@uso_cant,@iva,@cant,@que_anula,@posi,@cod,@uso_scan,@pesado,@envase," +
                    "@millas,@otrop,@u_vuelto,@u_pend,@vuelto,@codcli,@numero,@fecha_c,@nombre,@dni,@telefono,@direcc," +
                    "@barrio,@condiva,@local,@tipo,@ticket,@zeta,@fecha_z,@modo,@anulado,@fecha,@hora,@cnlvta," +
                    "@impiva,@estado)";
                cmd.Parameters.AddWithValue("@seq", numseq);
                cmd.Parameters.AddWithValue("@tienda", percep.Tienda);
                cmd.Parameters.AddWithValue("@caja", (decimal)percep.Caja);
                cmd.Parameters.AddWithValue("@func", (decimal)percep.Func);
                cmd.Parameters.AddWithValue("@nprec", 0);
                cmd.Parameters.AddWithValue("@mpag", 0);
                cmd.Parameters.AddWithValue("@cajero", (decimal)percep.Cajero);
                cmd.Parameters.AddWithValue("@imp", percep.Percep);
                cmd.Parameters.AddWithValue("@teorico", percep.Montoneto);
                cmd.Parameters.AddWithValue("@nro", 0);
                cmd.Parameters.AddWithValue("@flag", false);
                cmd.Parameters.AddWithValue("@uso_cant", false);
                cmd.Parameters.AddWithValue("@iva", 0);
                cmd.Parameters.AddWithValue("@cant", percep.Alicuota);
                cmd.Parameters.AddWithValue("@que_anula", 0);
                cmd.Parameters.AddWithValue("@posi", 0);
                cmd.Parameters.AddWithValue("@cod", 0);
                cmd.Parameters.AddWithValue("@uso_scan", false);
                cmd.Parameters.AddWithValue("@pesado", false);
                cmd.Parameters.AddWithValue("@envase", 0);
                cmd.Parameters.AddWithValue("@millas", string.Empty);
                cmd.Parameters.AddWithValue("@otrop", 0);
                cmd.Parameters.AddWithValue("@u_vuelto", false);
                cmd.Parameters.AddWithValue("@u_pend", false);
                cmd.Parameters.AddWithValue("@vuelto", string.Empty);
                cmd.Parameters.AddWithValue("@codcli", string.Empty);
                cmd.Parameters.AddWithValue("@numero", string.Empty);
                cmd.Parameters.AddWithValue("@fecha_c", string.Empty);
                cmd.Parameters.AddWithValue("@nombre", string.Empty);
                cmd.Parameters.AddWithValue("@dni", string.Empty);
                cmd.Parameters.AddWithValue("@telefono", string.Empty);
                cmd.Parameters.AddWithValue("@direcc", string.Empty);
                cmd.Parameters.AddWithValue("@barrio", string.Empty);
                cmd.Parameters.AddWithValue("@condiva", string.Empty);
                cmd.Parameters.AddWithValue("@local", string.Empty);
                cmd.Parameters.AddWithValue("@tipo", 0);
                cmd.Parameters.AddWithValue("@ticket", 0);
                cmd.Parameters.AddWithValue("@zeta", 0);
                cmd.Parameters.AddWithValue("@fecha_z", 0);
                cmd.Parameters.AddWithValue("@modo", 0);
                cmd.Parameters.AddWithValue("@anulado", false);
                cmd.Parameters.AddWithValue("@fecha", (decimal)percep.Fecha);
                cmd.Parameters.AddWithValue("@hora", (decimal)percep.Hora);
                cmd.Parameters.AddWithValue("@cnlvta", percep.CnlVta);
                cmd.Parameters.AddWithValue("@impiva", string.Empty);
                cmd.Parameters.AddWithValue("@estado", 3);
                con.Open();
                cmd.ExecuteNonQuery();
                con.Close();
            }
        }

        public static void WriteTrans(long numseq, List<DRegTrans> lista)
        {
            using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
            {
                con.Open();
                con.Close();
            }
            foreach (DRegTrans reg in lista)
            {
                if (reg.Towrite)
                {
                    switch (reg.Func)
                    {
                        case 2:
                            InsertMPago(numseq, (MDMpag)reg);
                            break;
                        case 3:
                            InsertDEop(numseq, (MDEop)reg);
                            break;
                        case 4:
                            InsertPlu(numseq, (MDPlu)reg);
                            break;
                        case 26:
                            InsertDPromo(numseq, (MDPromo)reg);
                            break;
                        case 28:
                            InsertPercep(numseq, (MDPercep)reg);
                            break;

                        case 31:
                            InsertPerTissh(numseq, (MDPerTissh)reg);
                            break;
                        case 38:
                            InsertPerIva(numseq, (MDPerIva)reg);
                            break;
                    }
                }
            }
        }
    }
}

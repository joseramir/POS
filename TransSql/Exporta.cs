using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.OleDb;
using System.Data.SqlClient;
using System.IO;

namespace TransSql
{    
    public class Exporta
    {
        //public static string CadenaConexion = null;
        /// <summary>
        /// Exporta el TRANS A LA BASE DE DATOS 
        /// </summary>
        /// <param name="fname">Nombre del archivo DBF, con estructura del TRANS</param>
        public static void ToBdSql(string fname, string tienda, int caja, int cajero, int fecha, int hora, string canalvta)
        {
            string fullname = Directory.GetCurrentDirectory() + "\\" + fname;
            decimal vtotal = 0;
            decimal perimp = 0;
            //bool confunc8 = false;
            if (!File.Exists(fullname))
                return;
            if (DatosSql.CadenaConexion == null)
                return;
            try
            {
                OleDbConnection dbfcon = new OleDbConnection(@"Provider=VFPOLEDB.1;Data Source=" + Directory.GetCurrentDirectory() + ";Exclusive=Yes;Collate=Windows;");
                string dbfquery = "select * from " + fname + " where func = 8;";
                DataTable dt = new DataTable();
                OleDbDataAdapter dbfdapt = new OleDbDataAdapter(dbfquery, dbfcon);
                dbfdapt.Fill(dt);
                using (SqlConnection mycon = new SqlConnection(DatosSql.CadenaConexion))
                {
                    mycon.Open();
                    SqlCommand mycmd = new SqlCommand();
                    mycmd.CommandText = "GetSeq";
                    mycmd.Connection = mycon;
                    mycmd.CommandType = CommandType.StoredProcedure;
                    mycmd.Parameters.Add("@secuencia", SqlDbType.BigInt);
                    mycmd.Parameters["@secuencia"].Direction = ParameterDirection.Output;
                    mycmd.ExecuteScalar();
                    long numseq = Convert.ToInt64(mycmd.Parameters["@secuencia"].Value.ToString());
                    if (numseq > 0)
                    {
                        //si el cliente es consumidor final setea el encabezado de la factura en la BD      
                        if (dt.Rows.Count > 0)
                        {
                            DataRow unafila = dt.Rows[0];
                            SqlCommand comando = new SqlCommand();
                            comando.CommandText = "SetConsFinal";
                            comando.Connection = mycon;
                            comando.CommandType = CommandType.StoredProcedure;
                            comando.Parameters.AddWithValue("@seq", numseq);
                            comando.Parameters.AddWithValue("@tienda", tienda);
                            comando.Parameters.AddWithValue("@caja", caja);
                            comando.Parameters.AddWithValue("@cajero", cajero);
                            comando.Parameters.AddWithValue("@fecha", fecha);
                            comando.Parameters.AddWithValue("@hora", hora);
                            comando.Parameters.AddWithValue("@canal", canalvta);
                            comando.Parameters.AddWithValue("@nombre", unafila["nombre"].ToString());
                            comando.ExecuteNonQuery();
                            comando = null;
                        }

                        dbfquery = "select * from " + fname;
                        dt = new DataTable();
                        dbfdapt = new OleDbDataAdapter(dbfquery, dbfcon);
                        dbfdapt.Fill(dt);
                        foreach (DataRow dr in dt.Rows)
                        {
                            int nfunc = Convert.ToInt32(dr["func"].ToString());
                            //solo se envia a la BD los FUNCS = 4 de la factura, se suprimen los anulados suspendidos y logueos de supervisor.
                            if ((nfunc == 4 && !Convert.ToBoolean(dr["anulado"])) || 
                                nfunc == 2 || nfunc == 20 || nfunc == 19 || nfunc == 12 || nfunc == 8
                                || nfunc == 40)
                            {
                                string qinsert = "INSERT INTO trans VALUES (@SEQ,@TIENDA,@CAJA,@FUNC,@NPREC,@MPAG,@CAJERO,@IMP," +
                                "@TEORICO, @NRO, @FLAG, @USO_CANT, @IVA, @CANT,@QUE_ANULA,@POSI,@COD,@USO_SCAN,@PESADO,@ENVASE,@MILLAS," +
                                "@OTROP, @U_VUELTO, @U_PEND, @VUELTO, @CODCLI, @NUMERO, @FECHA_C, @NOMBRE, @DNI, @TELEFONO," +
                                "@DIRECC, @BARRIO, @CONDIVA, @LOCAL, @TIPO, @TICKET, @ZETA, @FECHA_Z, @MODO, @ANULADO, @FECHA, @HORA," +
                                "@CNLVTA,@IMPIVA, 1);";
                                SqlCommand newcmd = new SqlCommand(qinsert, mycon);
                                newcmd.Parameters.AddWithValue("@SEQ", numseq); newcmd.Parameters.AddWithValue("@TIENDA", dr["TIENDA"]);
                                newcmd.Parameters.AddWithValue("@CAJA", dr["CAJA"]); newcmd.Parameters.AddWithValue("@FUNC", dr["FUNC"]);
                                newcmd.Parameters.AddWithValue("@NPREC", dr["NPREC"]); newcmd.Parameters.AddWithValue("@MPAG", dr["MPAG"]);
                                newcmd.Parameters.AddWithValue("@CAJERO", dr["CAJERO"]); newcmd.Parameters.AddWithValue("@IMP", dr["IMP"]);
                                newcmd.Parameters.AddWithValue("@TEORICO", dr["TEORICO"]); newcmd.Parameters.AddWithValue("@NRO", dr["NRO"]);
                                newcmd.Parameters.AddWithValue("@FLAG", dr["FLAG"]); newcmd.Parameters.AddWithValue("@USO_CANT", dr["USO_CANT"]);
                                newcmd.Parameters.AddWithValue("@IVA", dr["IVA"]); newcmd.Parameters.AddWithValue("@CANT", dr["CANT"]);
                                newcmd.Parameters.AddWithValue("@QUE_ANULA", dr["QUE_ANULA"]); newcmd.Parameters.AddWithValue("@POSI", dr["POSI"]);
                                newcmd.Parameters.AddWithValue("@COD", dr["COD"]); newcmd.Parameters.AddWithValue("@USO_SCAN", dr["USO_SCAN"]);
                                newcmd.Parameters.AddWithValue("@PESADO", dr["PESADO"]); newcmd.Parameters.AddWithValue("@ENVASE", dr["ENVASE"]);
                                newcmd.Parameters.AddWithValue("@MILLAS", dr["MILLAS"]); newcmd.Parameters.AddWithValue("@OTROP", dr["OTROP"]);
                                newcmd.Parameters.AddWithValue("@U_VUELTO", dr["U_VUELTO"]); newcmd.Parameters.AddWithValue("@U_PEND", dr["U_PEND"]);
                                newcmd.Parameters.AddWithValue("@VUELTO", dr["VUELTO"]); newcmd.Parameters.AddWithValue("@CODCLI", dr["CODCLI"]);
                                newcmd.Parameters.AddWithValue("@NUMERO", dr["NUMERO"]); newcmd.Parameters.AddWithValue("@FECHA_C", dr["FECHA_C"]);
                                newcmd.Parameters.AddWithValue("@NOMBRE", dr["NOMBRE"]); newcmd.Parameters.AddWithValue("@DNI", dr["DNI"]);
                                newcmd.Parameters.AddWithValue("@TELEFONO", dr["TELEFONO"]); newcmd.Parameters.AddWithValue("@DIRECC", dr["DIRECC"]);
                                newcmd.Parameters.AddWithValue("@BARRIO", dr["BARRIO"]); newcmd.Parameters.AddWithValue("@CONDIVA", dr["CONDIVA"]);
                                newcmd.Parameters.AddWithValue("@LOCAL", dr["LOCAL"]); newcmd.Parameters.AddWithValue("@TIPO", dr["TIPO"]);
                                newcmd.Parameters.AddWithValue("@TICKET", dr["TICKET"]); newcmd.Parameters.AddWithValue("@ZETA", dr["ZETA"]);
                                newcmd.Parameters.AddWithValue("@FECHA_Z", dr["FECHA_Z"]); newcmd.Parameters.AddWithValue("@MODO", dr["MODO"]);
                                newcmd.Parameters.AddWithValue("@ANULADO", dr["ANULADO"]); newcmd.Parameters.AddWithValue("@FECHA", dr["FECHA"]);
                                newcmd.Parameters.AddWithValue("@HORA", dr["HORA"]); newcmd.Parameters.AddWithValue("@CNLVTA", dr["CNLVTA"]);
                                newcmd.Parameters.AddWithValue("@IMPIVA", dr["IMPIVA"]);
                                if (Convert.ToInt16(dr["func"]) == 4 && !Convert.ToBoolean(dr["anulado"]))
                                    vtotal += decimal.Parse(dr["imp"].ToString());
                                //Supone que es un descuento de una promocion, lo ignora
                                if (nfunc == 4 && decimal.Parse(dr["imp"].ToString()) < 0)
                                    continue;
                                newcmd.ExecuteNonQuery();
                            }
                            //toma en cuenta para el total las percepciones IIBB, TISSH, Percepcion IVA RG 5329
                            if (nfunc == 28 || nfunc == 31 || nfunc == 38)
                            {
                                vtotal += decimal.Parse(dr["imp"].ToString());
                                decimal tpaux = decimal.Parse(dr["imp"].ToString());
                                perimp += Convert.ToDecimal(dr["imp"].ToString().Trim());
                            }
                        }
                        SqlCommand upcmd = new SqlCommand();
                        upcmd.Connection = mycon;                        
                        upcmd.CommandText = "update trans set imp = '" + vtotal.ToString("F2") + "', impiva = '"+ perimp.ToString("F2") + "' where seq = " + numseq + " and func = 2 and nro = 9999;";

                        upcmd.ExecuteNonQuery();
                    }
                    mycon.Close();
                }
            }
            catch (System.Exception ex)
            {
                DatosSql.WriteLog(ex, "ToBdSql");
                throw ex;
            }
        }

        public static void FinTicket(long numseq, int caja, int cajero)
        {
            try
            {
                using (SqlConnection mycon = new SqlConnection(DatosSql.CadenaConexion))
                using (SqlCommand cmd = new SqlCommand())
                {
                    mycon.Open();
                    cmd.Connection = mycon;
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "SetFinTicket";
                    cmd.Parameters.AddWithValue("@numseq", numseq);                    
                    cmd.Parameters.AddWithValue("@caja", caja);
                    cmd.Parameters.AddWithValue("@cajero", cajero);
                    cmd.ExecuteNonQuery();
                    mycon.Close();
                }
            }
            catch (System.Exception ex)
            {
                DatosSql.WriteLog(ex, "FinTicket");
            }
        }

        public static void CancelTicket(long numseq)
        {
            try
            {
                using (SqlConnection mycon = new SqlConnection(DatosSql.CadenaConexion))
                using (SqlCommand cmd = new SqlCommand())
                {
                    mycon.Open();
                    cmd.Connection = mycon;
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "SetCancelTicket";
                    cmd.Parameters.AddWithValue("@numseq", numseq);                    
                    cmd.ExecuteNonQuery();
                    mycon.Close();
                }
            }
            catch (System.Exception ex)
            {
                DatosSql.WriteLog(ex, "CancelTicket");
            }
        }

        public static void DevolverTicket(long numseq)
        {
            try
            {
                using (SqlConnection mycon = new SqlConnection(DatosSql.CadenaConexion))
                using (SqlCommand cmd = new SqlCommand())
                {
                    mycon.Open();
                    cmd.Connection = mycon;
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "SetDevuelveTicket";
                    cmd.Parameters.AddWithValue("@numseq", numseq);
                    cmd.ExecuteNonQuery();
                    mycon.Close();
                }
            }
            catch (System.Exception ex)
            {
                DatosSql.WriteLog(ex, "DevolverTicket");
            }
        }
    }
}

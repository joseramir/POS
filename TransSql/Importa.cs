using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.IO;
using System.Data.SqlClient;
using System.Data.OleDb;

namespace TransSql
{
    public class Importa
    {        

        /// <summary>
        /// Extrae los datos de la BD SQl a un archivo DBF
        /// </summary>
        /// <param name="fname">Archivo DBF para salvar la info</param>
        /// <param name="numseq">Secuencia (factura) a recuperar</param>
        /// <param name="tienda">Tienda configurada en la caja</param>
        /// <param name="pcajero">Cajero que esta utilizando el POS</param>
        /// <param name="pcaja">Caja que esta realizando el proceso</param>
        /// <param name="solofact">Discrimina si el controlador soporta Solo facturas</param>
        public static List<DRegTrans> ToDbf(string fname, long numseq, string tienda, int pcajero, int pcaja, bool solofact)
        {
            string dirname = Directory.GetCurrentDirectory();
            List<DRegTrans> lista = new List<DRegTrans>();
            if (DatosSql.CadenaConexion == null)
                return lista;            
            DateTime now = DateTime.Now;
            int lfecha = now.Year * 10000 + now.Month * 100 + now.Day;
            int lhora = now.Hour * 10000 + now.Minute * 100 + now.Second;
            try
            {
                using (OleDbConnection dbfcon = new OleDbConnection(@"Provider=VFPOLEDB.1;Data Source=" + dirname + ";Exclusive=No;Collate=Windows;"))
                {
                    dbfcon.Open();
                    DataTable dt = new DataTable();
                    using (SqlConnection mycon = new SqlConnection(DatosSql.CadenaConexion))
                    {
                        SqlCommand mycmd = new SqlCommand();
                        mycmd.CommandText = "GetTrans";
                        mycmd.Connection = mycon;
                        mycmd.CommandType = CommandType.StoredProcedure;
                        mycmd.Parameters.AddWithValue("@pnumseq", numseq);
                        SqlParameter param = new SqlParameter("@pesticket", SqlDbType.Int);
                        param.Direction = ParameterDirection.Output;
                        mycmd.Parameters.Add(param);
                        SqlDataAdapter mydapt = new SqlDataAdapter();
                        mydapt.SelectCommand = mycmd;
                        mydapt.Fill(dt);
                        bool esticket = Convert.ToInt16(param.Value) == 1;
                        foreach (DataRow dr in dt.Rows)
                        {
                            if (Convert.ToInt16(dr["func"]) == 8 || Convert.ToInt16(dr["func"]) == 28 || Convert.ToInt16(dr["func"]) == 31 || Convert.ToInt16(dr["func"]) == 38)
                                continue;
                            if (esticket && !solofact && (Convert.ToInt16(dr["func"]) == 19 ||
                                (Convert.ToInt16(dr["func"]) == 2 && Convert.ToInt16(dr["nro"]) == 9999) || Convert.ToInt16(dr["func"]) == 20))
                                continue;
                            if (Convert.ToInt16(dr["func"]) == 2 && Convert.ToInt16(dr["nro"]) == 9999)
                            {
                                dr["imp"] = string.Empty;
                                dr["impiva"] = string.Empty;
                            }

                            OleDbCommand newcmd = new OleDbCommand();
                            newcmd.Connection = dbfcon;
                            newcmd.CommandText = "INSERT INTO " + fname + " VALUES ('" + tienda + "'," + pcaja +
                                "," + dr["FUNC"].ToString() + "," + dr["NPREC"].ToString() + "," + dr["MPAG"].ToString() +
                                "," + pcajero + ",'" + dr["IMP"].ToString() + "','" + dr["TEORICO"].ToString() + "'," +
                                dr["NRO"].ToString() + "," + (dr.IsNull("FLAG") ? 0 : Convert.ToInt16(dr["FLAG"])) + "," +
                                (dr.IsNull("USO_CANT") ? 0 : Convert.ToInt16(dr["USO_CANT"])) + "," + dr["IVA"].ToString() +
                                ",'" + dr["CANT"].ToString() + "'," + dr["QUE_ANULA"].ToString() + "," + dr["POSI"].ToString() +
                                "," + dr["COD"].ToString() + "," + (dr.IsNull("USO_SCAN") ? 0 : Convert.ToInt16(dr["USO_SCAN"])) +
                                "," + (dr.IsNull("PESADO") ? 0 : Convert.ToInt16(dr["PESADO"])) + "," + dr["ENVASE"].ToString() +
                                ",'" + dr["MILLAS"].ToString() + "'," + dr["OTROP"].ToString() + "," +
                                (dr.IsNull("U_VUELTO") ? 0 : Convert.ToInt16(dr["U_VUELTO"])) + "," +
                                (dr.IsNull("U_PEND") ? 0 : Convert.ToInt16(dr["U_PEND"])) + ",'" + dr["VUELTO"].ToString() +
                                "','" + dr["CODCLI"].ToString() + "','" + dr["NUMERO"].ToString() + "','" + dr["FECHA_C"].ToString() +
                                "','" + dr["NOMBRE"].ToString() + "','" + dr["DNI"].ToString() + "','" + dr["TELEFONO"].ToString() +
                                "','" + dr["DIRECC"].ToString() + "','" + dr["BARRIO"].ToString() + "','" + dr["CONDIVA"].ToString() +
                                "','" + dr["LOCAL"].ToString() + "'," + dr["TIPO"].ToString() + "," + dr["TICKET"].ToString() +
                                "," + dr["ZETA"].ToString() + "," + dr["FECHA_Z"].ToString() + "," + dr["MODO"].ToString() +
                                "," + (dr.IsNull("ANULADO") ? 0 : Convert.ToInt16(dr["ANULADO"])) + "," +
                                lfecha + "," + lhora + ",'" + dr["CNLVTA"].ToString() + "','" + 
                                dr["IMPIVA"].ToString().Trim() + "')";
                            newcmd.ExecuteNonQuery();
                            lista.Add(DbTrans.GetDItem(dr));
                        }                        
                    }
                    dbfcon.Close();                    
                    dbfcon.Dispose();                    
                }
                return lista;
            }
            catch (System.Exception ex)
            {
                DatosSql.WriteLog(ex, "Importa");
                throw ex;
            }
        }

        /// <summary>
        /// Verifica si puede abrir un archivo
        /// </summary>
        /// <param name="filePath">Ruta del archivo</param>
        /// <returns>Devuelve True si se puede abrir el archivo en modo exclusivo</returns>
        public static bool TryOpen(string filePath)
        {
            bool aux = false;
            FileStream fs = null;
            
            try
            {
                fs = File.Open(filePath, FileMode.Create, FileAccess.Write, FileShare.Write);                    
                fs.Close();
                aux = true;                    
            }
            catch (IOException ioEx)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - El archivo \"{1}\" esta ocupado", DateTime.Now, filePath);
                writer.Close();
            }

            return aux;
        }
    }
}

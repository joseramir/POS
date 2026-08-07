using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.Data.SqlClient;
using System.Net.NetworkInformation;

using System.IO;

namespace TransSql
{
    /// <summary>
    /// Clase que retorna la informacion del servidor 
    /// </summary>
    public class DatosSql
    {
        public static string CadenaConexion = null;
        public static string ipsql = null;
        //static private SqlConnection singleton = null;

        //public static SqlConnection GetConexion()
        //{
        //    if (singleton == null)
        //    {
        //        singleton = new SqlConnection();
        //        singleton.ConnectionString = CadenaConexion;
        //    }
        //    return singleton;
        //}

        public static DataTable GetTransPendientes(int fecha)
        {
            if (CadenaConexion == null)
                return null;
            DataTable dt = new DataTable();
            try
            {	            
                using (SqlConnection mycon = new SqlConnection(DatosSql.CadenaConexion))
                using (SqlCommand mycmd = new SqlCommand())
                {                
                    mycmd.CommandText = "GetTransPendientes";
                    mycmd.CommandType = CommandType.StoredProcedure;
                    mycmd.Parameters.AddWithValue("@fecha", fecha);
                    mycmd.Connection = mycon;
                    SqlDataAdapter mydapt = new SqlDataAdapter(mycmd);
                    mydapt.Fill(dt);
                }
            }
            catch (System.Exception ex)
            {
                WriteLog(ex, "GetTransPendientes");
            }
            return dt;
        }

        public static void SetCadenaConexion(String cadena)
        {
            CadenaConexion = cadena;
            if (cadena.Contains("\\"))
            {
                string[] aux = cadena.Split('\\');
                StringBuilder sb = new StringBuilder();
                bool enip = false;
                foreach (char c in aux[0])
                {
                    if (enip && (char.IsDigit(c) || c == '.'))
                        sb.Append(c);
                    else if (enip)
                        break;
                    else if (char.IsDigit(c))
                    {
                        sb.Append(c);
                        enip = true;
                    }    
                }

                if (sb.Length > 0)
                    ipsql = sb.ToString();
            }
        }

        public static bool SendPing()
        {
            bool aux = false;
            Ping ping = new Ping();
	        PingOptions pingOptions = new PingOptions(128, true);
	        byte[] buffer = System.Text.Encoding.ASCII.GetBytes("bitnet ping");	        
	        try
	        {
		        PingReply ans = ping.Send(ipsql, 120, buffer, pingOptions);
		        aux = (ans.Status == IPStatus.Success);
	        }
	        catch 
            { aux = false; }
            return aux;
        }

        public static void Cierre()
        {
            try
            {
                using (SqlConnection con = new SqlConnection(DatosSql.CadenaConexion))
                using (SqlCommand cmd = new SqlCommand())
                {
                    cmd.Connection = con;
                    cmd.CommandText = "ProcesarCierre";
                    cmd.CommandType = CommandType.StoredProcedure;
                    con.Open();
                    cmd.ExecuteNonQuery();
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                WriteLog(ex, "Cierre");
            }
        }

        public static void WriteLog(Exception excep, string nomfunc)
        {
            StreamWriter writer = File.AppendText("ErrorSql.txt");		 
		    String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, nomfunc);
		    writer.WriteLine(fhdatos);
		    writer.WriteLine("Mensaje: " + excep.Message);
		    writer.WriteLine("StackTrace: " + excep.StackTrace);		 
		    writer.WriteLine("----------------------------------");
            writer.Close();
        }
    }
}

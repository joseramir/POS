using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Configuration;
using System.Data.SqlClient;
using System.IO;

namespace Controles
{
    public class FuncVendedorMay
    {
        public static List<VendedorMayor> BuscaVendedores()
        {
            List<VendedorMayor> listado = new List<VendedorMayor>();            
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    cmd.CommandText = "Select v.IdVendedor, v.Nombre from VendedoresMay v where habilitado = 1 order by v.IdVendedor";                    
                    SqlDataReader rdr = cmd.ExecuteReader();
                    if (rdr.HasRows)
                    {
                        listado.Add(new VendedorMayor() { IdVendedor = 0, Nombre = "Seleccionar Vendedor..." });
                        while (rdr.Read())
                        {
                            listado.Add(
                                new VendedorMayor() { IdVendedor = Convert.ToInt32(rdr["IdVendedor"]), Nombre = rdr["Nombre"].ToString() }
                                );
                        }
                    }
                    rdr.Close();
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "BuscaVendedores");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
                listado = new List<VendedorMayor>();
            }
            return listado;
        }
    }
}

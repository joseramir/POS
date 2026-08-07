using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Data;
using System.Data.SqlClient;
using System.Configuration;

using System.IO;

namespace Controles
{
    public class FuncClientes
    {
        public static bool AbrirClientesSql()
        {
            bool aux = false;
            try
            {
                using (SqlConnection connection = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))
                {
                    string query = "select 1";
                    
                    SqlCommand command = new SqlCommand(query, connection);

                    connection.Open();
                    
                    command.ExecuteScalar();
                    connection.Close();
                    aux = true;
                }
            }
            catch (Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "AbrirClientesSql");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }

        public static ClienteSql BuscaClientePorCod(string pcod)
        {
            ClienteSql aux = null;
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    //cmd.CommandText = "Select c.*, r.descrip from clientes c inner join reparticiones r on c.repa = r.codrepa where cod = @cod;";
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "spGetCliente";
                    cmd.Parameters.AddWithValue("@pcod", pcod);
                    SqlDataReader rdr = cmd.ExecuteReader();
                    if (rdr.HasRows)
                    {
                        rdr.Read();
                        aux = new ClienteSql();
                        aux.Cod = Convert.ToInt64(rdr["cod"]);
                        aux.Nombre = rdr["rs"].ToString();
                        aux.Direccion = rdr["direcc"].ToString();
                        aux.Localidad = rdr["localidad"].ToString();
                        aux.Civa = Convert.ToInt16(rdr["civa"]);
                        aux.Cuit = rdr["cuit"].ToString();
                        aux.Telefono = rdr["telefono"].ToString();
                        aux.Saldo = Convert.ToDecimal(rdr["saldo"]);
                        aux.SalMax = Convert.ToDecimal(rdr["salmax"]);
                        aux.Grupo = Convert.ToInt16(rdr["grupo"]);
                        aux.Estado = Convert.ToChar(rdr["estado"]);
                        aux.Lista = Convert.ToInt16(rdr["lista"]);
                        aux.Consumo = Convert.ToDecimal(rdr["consumo"]);
                        aux.Porcemax = Convert.ToDecimal(rdr["porcemax"]);
                        aux.Percepcion = Convert.ToChar(rdr["percepcion"]);
                        aux.Alicuota = Convert.ToDecimal(rdr["alicuota"]);
                        aux.Fecha_pib = Convert.ToDateTime(rdr["fecha_pib"]);
                        aux.CodPostal = Convert.ToInt32(rdr["codpostal"]);
                        aux.PerTissh = Convert.ToChar(rdr["pertissh"]);
                        aux.AliTissh = Convert.ToDecimal(rdr["alitissh"]);
                        aux.Pyme = Convert.ToChar(rdr["pyme"]);
                        aux.Email = rdr["email"].ToString();
                        aux.Repa = Convert.ToInt16(rdr["repa"]);
                        aux.SaldoCaja = Convert.ToDecimal(rdr["saldocaja"]);
                        aux.RepaDes = rdr["descrip"].ToString();
                        aux.Bloqueado = Convert.ToBoolean(rdr["bloqueado"]);
                        try
                        {
                            int colIndex = rdr.GetOrdinal("FecNac");
                            if (rdr.IsDBNull(colIndex))
                                aux.FechaNacimiento = new DateTime(1810, 5, 25);
                            else
                                aux.FechaNacimiento = Convert.ToDateTime(rdr["FecNac"]);
                        }
                        catch (Exception)
                        {
                            aux.FechaNacimiento = new DateTime(1810, 5, 25);
                        }
                        aux.SaldoMutual = Convert.ToDecimal(rdr["saldoMut"]);
                        aux.UsadoMutual = Convert.ToBoolean(rdr["usadoMut"]);
                        aux.NoPercepIVA = Convert.ToBoolean(rdr["NoPercepIVA"]);
                    }
                    rdr.Close();
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "BuscaClientePorCod");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }

        public static List<ClienteSql> BuscaClientePorNom(string pnom)
        {
            List<ClienteSql> lista = null;
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))                
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    //cmd.CommandText = "Select top 1000 c.*, r.descrip from clientes c inner join reparticiones r on c.repa = r.codrepa where upper(c.rs) like '" + pnom + "%';";
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "spGetClientexNom";
                    cmd.Parameters.AddWithValue("@pnom", pnom.ToUpper());
                    SqlDataReader rdr = cmd.ExecuteReader();
                    lista = new List<ClienteSql>();
                    while (rdr.Read())
                    {
                        ClienteSql aux = new ClienteSql();
                        aux.Cod = Convert.ToInt64(rdr["cod"]);
                        aux.Nombre = rdr["rs"].ToString();
                        aux.Direccion = rdr["direcc"].ToString();
                        aux.Localidad = rdr["localidad"].ToString();
                        aux.Civa = Convert.ToInt16(rdr["civa"]);
                        aux.Cuit = rdr["cuit"].ToString();
                        aux.Telefono = rdr["telefono"].ToString();
                        aux.Saldo = Convert.ToDecimal(rdr["saldo"]);
                        aux.SalMax = Convert.ToDecimal(rdr["salmax"]);
                        aux.Grupo = Convert.ToInt16(rdr["grupo"]);
                        aux.Estado = Convert.ToChar(rdr["estado"]);
                        aux.Lista = Convert.ToInt16(rdr["lista"]);
                        aux.Consumo = Convert.ToDecimal(rdr["consumo"]);
                        aux.Porcemax = Convert.ToDecimal(rdr["porcemax"]);
                        aux.Percepcion = Convert.ToChar(rdr["percepcion"]);
                        aux.Alicuota = Convert.ToDecimal(rdr["alicuota"]);
                        aux.Fecha_pib = Convert.ToDateTime(rdr["fecha_pib"]);
                        aux.CodPostal = Convert.ToInt32(rdr["codpostal"]);
                        aux.PerTissh = Convert.ToChar(rdr["pertissh"]);
                        aux.AliTissh = Convert.ToDecimal(rdr["alitissh"]);
                        aux.Pyme = Convert.ToChar(rdr["pyme"]);
                        aux.Email = rdr["email"].ToString();
                        aux.Repa = Convert.ToInt16(rdr["repa"]);
                        aux.SaldoCaja = Convert.ToDecimal(rdr["saldocaja"]);
                        aux.RepaDes = rdr["descrip"].ToString();

                        try
                        {
                            int colIndex = rdr.GetOrdinal("FecNac");
                            if (rdr.IsDBNull(colIndex))
                                aux.FechaNacimiento = new DateTime(1810, 5, 25);
                            else
                                aux.FechaNacimiento = Convert.ToDateTime(rdr["FecNac"]);
                        }
                        catch (Exception)
                        {
                            aux.FechaNacimiento = new DateTime(1810, 5, 25);
                        }
                        aux.SaldoMutual = Convert.ToDecimal(rdr["saldoMut"]);
                        aux.UsadoMutual = Convert.ToBoolean(rdr["usadoMut"]);
                        aux.NoPercepIVA = Convert.ToBoolean(rdr["NoPercepIVA"]);

                        lista.Add(aux);
                    }
                    rdr.Close();
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                lista = null;
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "BuscaClientePorNom");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return lista;
        }


        public static bool ActualizaSaldoCli(ClienteSql cli, string saldo)
        {
            bool aux = false;
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))                
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    cmd.CommandText = "update clientes set saldo = @saldo where cod = @cod;";
                    cmd.Parameters.AddWithValue("@saldo", Convert.ToDecimal(saldo));                    
                    cmd.Parameters.AddWithValue("@cod", cli.Cod);                    
                    aux = cmd.ExecuteNonQuery() == 1;
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "ActualizaSaldoCli");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }

        public static string LeeSaldoCli(long pcod)
        {
            string aux = "0.00";
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    cmd.CommandText = "select saldo from clientes where cod = @cod;";
                    cmd.Parameters.AddWithValue("@cod", pcod);

                    SqlDataReader rdr = cmd.ExecuteReader();
                    if (rdr.HasRows)
                    {
                        rdr.Read();
                        aux = Convert.ToDecimal(rdr["saldo"]).ToString("F2");
                    }
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "LeeSaldoCli");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }

        public static bool ActualizaConsumo(long pcod, double consumo)
        {
            bool aux = false;
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    cmd.CommandText = "update clientes set consumo = consumo + @consumo where cod = @cod;";
                    cmd.Parameters.AddWithValue("@consumo", Convert.ToDecimal(consumo));
                    cmd.Parameters.AddWithValue("@cod", pcod);
                    aux = cmd.ExecuteNonQuery() == 1;
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "ActualizaConsumoCli");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }

        //public static bool ActualizaConsumoCli(long pcod, double consumo, double saldo)
        public static bool ActualizaConsumoCli(ClienteSql cli, double consumo, double saldo)
        {
            bool aux = false;
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))                
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    //cmd.CommandText = "update clientes set consumo = consumo + @consumo, saldocaja = saldocaja - @saldo, estado = 'M', saldoMut = @saldoMut, usadoMut = @usadoMut where cod = @cod;";                    
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "spUpdConsumoCli";
                    cmd.Parameters.AddWithValue("@pconsumo", Convert.ToDecimal(consumo));
                    cmd.Parameters.AddWithValue("@psaldo", Convert.ToDecimal(saldo));
                    cmd.Parameters.AddWithValue("@psaldoMut", cli.SaldoMutual);
                    cmd.Parameters.AddWithValue("@pusadoMut", cli.UsadoMutual);
                    cmd.Parameters.AddWithValue("@pcod", cli.Cod);
                    aux = cmd.ExecuteNonQuery() == 1;
                    con.Close();
                }
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "ActualizaConsumoCli");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }

        public static string LeeSaldoPromoCli(long pcod)
        {
            string aux = "0.00";
            try
            {
                using (SqlConnection con = new SqlConnection(ConfigurationManager.ConnectionStrings["dbsuper"].ConnectionString))                
                {
                    con.Open();
                    SqlCommand cmd = con.CreateCommand();
                    //cmd.CommandText = "select saldocaja from clientes where cod = @cod;";
                    cmd.CommandType = CommandType.StoredProcedure;
                    cmd.CommandText = "spLeeSaldoPromoCli";
                    cmd.Parameters.AddWithValue("@pcod", pcod);

                    SqlDataReader rdr = cmd.ExecuteReader();
                    if (rdr.HasRows)
                    {
                        rdr.Read();
                        aux = Convert.ToDecimal(rdr["saldocaja"]).ToString("F2");
                    }
                    con.Close();

                }
            }
            catch (System.Exception ex)
            {
                StreamWriter writer = File.AppendText("ErrorSql.txt");
                String fhdatos = String.Format("{0} - Error capturado en funcion {1}", DateTime.Now, "LeeSaldoPromoCli");
                writer.WriteLine(fhdatos);
                writer.WriteLine("Mensaje: " + ex.Message);
                writer.WriteLine("StackTrace: " + ex.StackTrace);
                writer.WriteLine("----------------------------------");
                writer.Close();
            }
            return aux;
        }
    }
}

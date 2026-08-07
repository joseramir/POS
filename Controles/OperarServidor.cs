using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using System.IO;
using System.Text.RegularExpressions;

using System.IO.Ports;
using System.Threading;

using System.Configuration;
using System.Web.Script.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;
using System.Xml.Serialization;
using System.Xml;

using Newtonsoft.Json;
using RestSharp;

namespace Controles
{
    public class OperarServidor
    {
        private static bool ComFUEnUso = false;

        public static string VerificarOrden(string codbarra, string ip, int puerto)
        {
            string rta = "NOK, No puede verificar servidor";
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);                    
                    //writer.Write(string.Format("1,{0},V", codbarra));
                    writer.Write(string.Format("10,{0},V", codbarra));
                    rta = reader.ReadString();

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                rta = "NOK,Error en conexion con servidor";
            }

            return rta;
        }

        
        public static string GetValorBillete(string codbarra, string ip, int puerto)
        {
            string rta = "NOK, No puede verificar servidor";
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("2,{0},1", codbarra));
                    rta = reader.ReadString();

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
                else
                    rta = "NOK,El Servicio no se ejecuta. Verificar Servidor";
            }
            catch (System.Exception ex)
            {
                rta = "NOK,Error en conexion con servidor";
            }

            return rta;
        }

        public static string VerificarBillete(string codbarra, string numserie, string ip, int puerto)
        {
            string rta = "NOK, No puede verificar servidor";
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("2,{0},2,{1}", codbarra, numserie));
                    rta = reader.ReadString();

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                rta = "NOK,Error en conexion con servidor";
            }

            return rta;
        }

        public static string UtilizarBillete(string codbarra, string numserie, string ip, int puerto, int caja, int cajero, int ticket)
        {
            string rta = "NOK, No puede verificar servidor";
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("1,{0},3,{1},{2},{3},{4}", codbarra, numserie, caja, cajero, ticket));
                    rta = reader.ReadString();

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                rta = "NOK,Error en conexion con servidor";
            }

            return rta;
        }

        //Obtiene el numero de cupon de servidor
        public static string GetNumeroCupon(string ip, int puerto, int caja, int cajero, int ticket)
        {
            string rta = "NOK, No puede verificar servidor";
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("3,{0},{1},{2}", caja, cajero, ticket));
                    rta = reader.ReadString();

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                rta = "NOK,Error en conexion con servidor";
            }

            return rta;
        }

        //Obtiene el numero de cupon de servidor
        public static int GetPremioStock(string ip, int puerto, int codigo, int cantped, int caja, int cajero, int ticket, bool simula)
        {
            int cantrta = 0;
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("4,{0},{1},{2},{3},{4},{5}", codigo, cantped, caja, cajero, ticket, (simula ? "1" : "0")));
                    string rta = reader.ReadString();
                    string[] vrta = rta.Split(',');
                    if (vrta[0] == "OK")
                        cantrta = Convert.ToInt32(vrta[1]);
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                cantrta = 0;
            }

            return cantrta;
        }

        //Obtiene el numero de cupon de servidor
        public static void GetGeneraPremioStock(string ip, int puerto, int milla, int caja, int cajero, int ticket,
            out string codpromo, out string leyenda1, out string leyenda2, out string leyenda3, out string vdesde, out string vhasta)
        {
            codpromo = "";
            leyenda1 = "";
            leyenda2 = "";
            leyenda3 = "";
            vdesde = "";
            vhasta = "";            
            try
            {
                
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("5,{0},{1},{2},{3}", milla, caja, cajero, ticket));
                    string rta = reader.ReadString();
                    int posic = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posic);
                    string rtat = rta.Substring(posic + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        codpromo = cols[0];
                        leyenda1 = cols[1];
                        leyenda2 = cols[2];
                        leyenda3 = cols[3];
                        vdesde = cols[4];
                        vhasta = cols[5];
                    }
                        
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }

        //usado para promocion de aniversario 06/2022
        public static void ObtenerPromoGen(string ip, int puerto, string codpromo, int caja, int cajero, int ticket,out PromoExtra newpromo, out string leyenda)
        {
            newpromo = null;
            leyenda = "";
            try
            {

                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("6,{0},{1},{2},{3}", codpromo, caja, cajero, ticket));
                    string rta = reader.ReadString();
                    int posicion = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posicion);
                    string rtat = rta.Substring(posicion + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        //codpromo = 0, sinpromo = 1, leyenda = 2
                        newpromo = new PromoExtra();
                        string[] linespromo = cols[1].Split(new string[] { Environment.NewLine }, StringSplitOptions.None);
                        leyenda = cols[2];
                        foreach (string item in linespromo)
                        {
                            int posic = item.IndexOf('=');
                            string part1 = item.Substring(posic + 1);
                            if (item.Trim().StartsWith("codpromo"))
                                newpromo.Codpromo = part1.Trim();
                            if (item.Trim().StartsWith("eval"))
                            {
                                //if (regcan.IsMatch(part1))
                                //{
                                //    Match mcan = regcan.Match(part1);
                                //    lnumcan.Add(mcan.Groups["numcan"].Value, setnumcan);
                                //    newpromo.Evals.Add(part1.Replace(mcan.Groups["numcan"].Value, setnumcan.ToString()).Trim());
                                //    setnumcan++;
                                //}
                                //else
                                    newpromo.Evals.Add(part1.Trim());
                            }
                            if (item.Trim().StartsWith("filtro"))
                                newpromo.Filtro = part1.Trim();
                            if (item.Trim().StartsWith("accion"))
                                newpromo.Acciones.Add(part1.Trim());
                            if (item.Trim().StartsWith("cantidad"))
                                newpromo.Cantidad = part1.Trim();

                            if (item.Trim().StartsWith("marcacli"))
                                newpromo.MarcaCli = Convert.ToInt32(part1.Trim());
                            if (item.Trim().StartsWith("mediopago"))
                                newpromo.MedioPago = Convert.ToInt32(part1.Trim());
                            if (item.Trim().StartsWith("numtarjeta"))
                                newpromo.NumTarjeta = Convert.ToInt32(part1.Trim());
                            if (item.Trim().StartsWith("exclucli"))
                                newpromo.ExcluCli = Convert.ToInt32(part1.Trim());
                        }
                        codpromo = cols[0];                        
                    }
                    

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }

        //usado para promocion Fan Club Comodin
        public static void ObtenerPromoGen33(string ip, int puerto, string codpromo, int caja, int cajero, int ticket, long dni, out PromoExtra newpromo, out string leyenda)
        {
            newpromo = null;
            leyenda = "";            
            try
            {
                
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("6,{0},{1},{2},{3},{4}", codpromo, caja, cajero, ticket, dni));
                    string rta = reader.ReadString();
                    int posicion = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posicion);
                    string rtat = rta.Substring(posicion + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        //codpromo = 0, sinpromo = 1, leyenda = 2
                        newpromo = new PromoExtra();
                        string[] linespromo = cols[1].Split(new string[] { Environment.NewLine }, StringSplitOptions.None);
                        leyenda = cols[2];
                        foreach (string item in linespromo)
                        {
                            int posic = item.IndexOf('=');
                            string part1 = item.Substring(posic + 1);
                            if (item.Trim().StartsWith("codpromo"))
                                newpromo.Codpromo = part1.Trim();
                            if (item.Trim().StartsWith("eval"))
                            {
                                //if (regcan.IsMatch(part1))
                                //{
                                //    Match mcan = regcan.Match(part1);
                                //    lnumcan.Add(mcan.Groups["numcan"].Value, setnumcan);
                                //    newpromo.Evals.Add(part1.Replace(mcan.Groups["numcan"].Value, setnumcan.ToString()).Trim());
                                //    setnumcan++;
                                //}
                                //else
                                newpromo.Evals.Add(part1.Trim());
                            }
                            if (item.Trim().StartsWith("filtro"))
                                newpromo.Filtro = part1.Trim();
                            if (item.Trim().StartsWith("accion"))
                                newpromo.Acciones.Add(part1.Trim());
                            if (item.Trim().StartsWith("cantidad"))
                                newpromo.Cantidad = part1.Trim();

                            if (item.Trim().StartsWith("marcacli"))
                                newpromo.MarcaCli = Convert.ToInt32(part1.Trim());
                            if (item.Trim().StartsWith("mediopago"))
                                newpromo.MedioPago = Convert.ToInt32(part1.Trim());
                            if (item.Trim().StartsWith("numtarjeta"))
                                newpromo.NumTarjeta = Convert.ToInt32(part1.Trim());
                            if (item.Trim().StartsWith("exclucli"))
                                newpromo.ExcluCli = Convert.ToInt32(part1.Trim());
                        }
                        codpromo = cols[0];
                    }
                    else
                        leyenda = rtat;
                    

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }

        public static string FinPromoGen(bool anulado, string codpromo, string ip, int puerto, int caja, int cajero, int ticket)
        {
            string rta = "NOK, No puede verificar servidor";
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    
                    writer.Write(string.Format("7,{0},{1},{2},{3},{4}", codpromo, (anulado ? "1" : "0"), caja, cajero, ticket));
                    rta = reader.ReadString();

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                rta = "NOK,Error en conexion con servidor";
            }

            return rta;
        }


        //Obtiene el numero de cupon de servidor
        public static void GetGeneraPremioStock(string ip, int puerto, int milla, double mtodescto, int caja, int cajero, int ticket,
            out string codpromo, out string leyenda1, out string leyenda2, out string leyenda3, out string vdesde, out string vhasta)
        {
            codpromo = "";
            leyenda1 = "";
            leyenda2 = "";
            leyenda3 = "";
            vdesde = "";
            vhasta = "";
            try
            {

                if (ConexionRed.comprobarIP(ip, 30))
                {
                    decimal dmtodescto = Decimal.Round(Convert.ToDecimal(mtodescto), 2);
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("8,{0},{1},{2},{3},{4}", milla, dmtodescto.ToString("F2"), caja, cajero, ticket));
                    string rta = reader.ReadString();
                    int posic = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posic);
                    string rtat = rta.Substring(posic + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        codpromo = cols[0];
                        leyenda1 = cols[1];
                        leyenda2 = cols[2];
                        leyenda3 = cols[3];
                        vdesde = cols[4];
                        vhasta = cols[5];
                    }

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }


        public static void GetGeneraPremioStock(string ip, int puerto, int milla, int caja, int cajero, int ticket,
            out string codpromo, out string leyenda1, out string leyenda2, out string leyenda3)
        {
            codpromo = "";
            leyenda1 = "";
            leyenda2 = "";
            leyenda3 = "";
            
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    //decimal dmtodescto = Decimal.Round(Convert.ToDecimal(mtodescto), 2);
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("9,{0},{1},{2},{3}", milla, caja, cajero, ticket));
                    string rta = reader.ReadString();
                    int posic = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posic);
                    string rtat = rta.Substring(posic + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        codpromo = cols[0];
                        leyenda1 = cols[1];
                        leyenda2 = cols[2];
                        leyenda3 = cols[3];                        
                    }

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }

        public static void GetGeneraVoucherStock(string ip, int puerto, int milla, int caja, int cajero, int ticket, double vta,
            out string codpromo, out string leyenda1, out string vdesde, out string vhasta)
        {
            codpromo = "";
            leyenda1 = "";
            vdesde = "";
            vhasta = "";

            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    //decimal dmtodescto = Decimal.Round(Convert.ToDecimal(mtodescto), 2);
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("11,{0},{1},{2},{3},{4}", milla, caja, cajero, ticket, vta));
                    string rta = reader.ReadString();
                    int posic = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posic);
                    string rtat = rta.Substring(posic + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        codpromo = cols[0];
                        leyenda1 = cols[1];
                        vdesde = cols[2];
                        vhasta = cols[3];
                    }

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }

        //Mama feliz de 06/2022
        public static void GetGeneraVoucherStock22(string ip, int puerto, int milla, int caja, int cajero, int ticket, double vta,
            out string codpromo, out string leyenda1, out string leyenda2, out string leyenda3, out string vdesde, out string vhasta)
        {
            codpromo = "";
            leyenda1 = "";
            leyenda2 = "";
            leyenda3 = "";
            vdesde = "";
            vhasta = "";

            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    //decimal dmtodescto = Decimal.Round(Convert.ToDecimal(mtodescto), 2);
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("8,{0},{1},{2},{3},{4}", milla, vta, caja, cajero, ticket));
                    string rta = reader.ReadString();
                    int posic = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posic);
                    string rtat = rta.Substring(posic + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        codpromo = cols[0];
                        leyenda1 = cols[1];
                        leyenda2 = cols[2];
                        leyenda3 = cols[3];
                        vdesde = cols[4];
                        vhasta = cols[5];
                    }

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }

        public static void GetGeneraVoucherStock33(string ip, int puerto, int milla, int caja, int cajero, int ticket, double vta, int dni, 
            out string codpromo, out string leyenda1, out string leyenda2, out string leyenda3, out string vdesde, out string vhasta)
        {
            codpromo = "";
            leyenda1 = "";
            leyenda2 = "";
            leyenda3 = "";
            vdesde = "";
            vhasta = "";

            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    //decimal dmtodescto = Decimal.Round(Convert.ToDecimal(mtodescto), 2);
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("8,{0},{1},{2},{3},{4},{5}", milla, vta, caja, cajero, ticket, dni));
                    string rta = reader.ReadString();
                    int posic = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posic);
                    string rtat = rta.Substring(posic + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        codpromo = cols[0];
                        leyenda1 = cols[1];
                        leyenda2 = cols[2];
                        leyenda3 = cols[3];
                        vdesde = cols[4];
                        vhasta = cols[5];
                    }

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                codpromo = "";
            }
        }

        public static void GetGeneraValeOtro(string ip, int puerto, int milla, int caja, int cajero, int ticket, double CodigoParaRegalar,
            out string RtaCodigo)
        {
            RtaCodigo = "NOK";
            
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    //decimal dmtodescto = Decimal.Round(Convert.ToDecimal(mtodescto), 2);
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(string.Format("12,{0},{1},{2},{3},{4}", milla, Convert.ToInt32(CodigoParaRegalar), caja, cajero, ticket));
                    string rta = reader.ReadString();
                    int posic = rta.IndexOf(',');
                    string vrta = rta.Substring(0, posic);
                    string rtat = rta.Substring(posic + 1);
                    if (vrta == "OK")
                    {
                        string[] cols = rtat.Split('|');
                        RtaCodigo = cols[0];
                    }

                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (System.Exception ex)
            {
                RtaCodigo = "ERR";
            }
        }

        public static string InformarCajaLibre(int caja, string ip, int puerto, int ComPort, int segs)
        {
            string rta = "NOK, No responde servidor";
            try
            {
                if (ConexionRed.comprobarIP(ip, 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ip), puerto);
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(caja);
                    //rta = reader.ReadString();                    
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();

                    if (!ComFUEnUso)
                    {
                        Thread myNewThread = new Thread(() => EncenderFilaUnica(ComPort, segs));
                        myNewThread.Start();
                    }
                }
                else
                    rta = "NOK,El Servicio no se ejecuta. Verificar Servidor";
            }
            catch (System.Exception ex)
            {
                rta = "NOK,Error en conexion con servidor";
            }

            return rta;
        }

        public static void EncenderFilaUnica(int ComPort, int segs)
        {
            ComFUEnUso = true;
            SerialPort sp = new SerialPort();
            sp.PortName = string.Format("COM{0}", ComPort);
            try
            {
	            sp.Open();
                sp.DiscardInBuffer();
                sp.DiscardInBuffer();
                for (int i = 0; i < segs; i++)
                {
                    sp.RtsEnable = !sp.RtsEnable;
                    Thread.Sleep(1000);
                }
                if (sp.RtsEnable)
                    sp.RtsEnable = false;
                sp.Close();
            }
            catch (System.Exception ex)
            {
                if (sp.IsOpen)
                    sp.Close();
            }
            finally
            {
                if (sp.IsOpen)
                    sp.Close();
                sp.Dispose();
            }
            ComFUEnUso = false;
        }       


        //public static void MpCrearOrden(PeticionCaja unCaja)
        //{
        //    //PeticionCaja unCaja = null;            
        //    try
        //    {
        //        if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 30))
        //        {
        //            //unCaja = new PeticionCaja();
        //            //unCaja.numcaja = pcaja;
        //            //unCaja.tienda = tienda;
        //            //unCaja.ptovta = ptovta;
        //            //unCaja.tipocomp = tipocomp;
        //            //unCaja.numcomp = numticket;
        //            //unCaja.monto = Convert.ToDecimal(pmonto);
        //            //unCaja.fhinicio = unCaja.fhactua = DateTime.Now;
        //            //unCaja.idmp = "";
        //            //unCaja.idoper = "";
        //            //unCaja.estado = 0;
        //            //unCaja.payment_id = 0;

        //            JavaScriptSerializer ser = new JavaScriptSerializer();
        //            string salida = ser.Serialize(unCaja);

        //            TcpClient cliente = new TcpClient();
        //            cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
        //            NetworkStream output = cliente.GetStream();
        //            BinaryWriter writer = new BinaryWriter(output);
        //            BinaryReader reader = new BinaryReader(output);
        //            writer.Write(salida);
        //            string rta = reader.ReadString();
        //            if (rta == "OK")
        //            {
        //                unCaja.seq = reader.ReadInt32();
        //                unCaja.estado = reader.ReadInt32();
        //                unCaja.idmp = reader.ReadString();
        //                unCaja.idoper = reader.ReadString();                        
        //            }
        //            reader.Close();
        //            writer.Close();
        //            output.Close();
        //            cliente.Close();
        //        }

        //    }
        //    catch (System.Exception ex)
        //    {
        //        bool cual = false;
        //        //rta = "NOK,Error en conexion con servidor";
        //    }

        //    //return unCaja;
        //}

        //public static void MpConsultaOrden(ref PeticionCaja unCaja)
        //{
        //    System.Threading.Thread.Sleep(5000);
        //    try
        //    {
        //        if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 30))
        //        {
        //            unCaja.estado = 3;

        //            JavaScriptSerializer ser = new JavaScriptSerializer();
        //            string salida = ser.Serialize(unCaja);                    

        //            TcpClient cliente = new TcpClient();
        //            cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
        //            NetworkStream output = cliente.GetStream();
        //            BinaryWriter writer = new BinaryWriter(output);
        //            BinaryReader reader = new BinaryReader(output);
        //            writer.Write(salida);
        //            string rta = reader.ReadString();
        //            if (rta == "OK")
        //            {
        //                unCaja.estado = reader.ReadInt32();
        //                unCaja.payment_id = reader.ReadInt64();
        //            }
        //            reader.Close();
        //            writer.Close();
        //            output.Close();
        //            cliente.Close();
        //        }

        //    }
        //    catch (System.Exception ex)
        //    {
        //        //rta = "NOK,Error en conexion con servidor";
        //    }
        //    //return otro;
        //}

        //public static void MpConsultaPagoAMp(ref PeticionCaja unCaja)
        //{
        //    System.Threading.Thread.Sleep(5000);
        //    try
        //    {
        //        if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 30))
        //        {
        //            unCaja.estado = 9;

        //            JavaScriptSerializer ser = new JavaScriptSerializer();
        //            string salida = ser.Serialize(unCaja);

        //            TcpClient cliente = new TcpClient();
        //            cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
        //            NetworkStream output = cliente.GetStream();
        //            BinaryWriter writer = new BinaryWriter(output);
        //            BinaryReader reader = new BinaryReader(output);
        //            writer.Write(salida);
        //            string rta = reader.ReadString();
        //            if (rta == "OK")
        //            {
        //                unCaja.estado = reader.ReadInt32();
        //                unCaja.payment_id = reader.ReadInt64();
        //            }
        //            reader.Close();
        //            writer.Close();
        //            output.Close();
        //            cliente.Close();
        //        }

        //    }
        //    catch (System.Exception ex)
        //    {
        //        //rta = "NOK,Error en conexion con servidor";
        //    }
        //    //return otro;
        //}

        public static PeticionCaja MpDevolucion(string tienda, short ptovta, short pcaja, int pidoper)
        {
            PeticionCaja unCaja = null;
            try
            {
                if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 30))
                {
                    unCaja = new PeticionCaja();
                    unCaja.seq = pidoper;
                    unCaja.numcaja = pcaja;
                    unCaja.tienda = tienda;
                    unCaja.ptovta = ptovta;
                    unCaja.tipocomp = "";
                    unCaja.numcomp = 0;
                    unCaja.monto = 0;
                    unCaja.fhinicio = unCaja.fhactua = DateTime.Now;
                    unCaja.idmp = "";
                    unCaja.idoper = "";
                    unCaja.estado = 10;

                    JavaScriptSerializer ser = new JavaScriptSerializer();
                    string salida = ser.Serialize(unCaja);

                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    writer.Write(salida);
                    string rta = reader.ReadString();
                    if (rta == "OK")
                    {
                        //unCaja.seq = reader.ReadInt32();
                        unCaja.estado = reader.ReadInt32();
                        unCaja.idoper = reader.ReadString();
                        unCaja.payment_id = reader.ReadInt64();
                        unCaja.monto = reader.ReadDecimal();
                        
                    }
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }

            }
            catch (System.Exception ex)
            {
                bool cual = false;
                //rta = "NOK,Error en conexion con servidor";
            }

            return unCaja;
        }

        //public static void MpCancelarOrden(ref PeticionCaja unCaja)
        //{
        //    System.Threading.Thread.Sleep(10000);
        //    try
        //    {
        //        if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwaymp"], 30))
        //        {
        //            unCaja.estado = 13;

        //            JavaScriptSerializer ser = new JavaScriptSerializer();
        //            string salida = ser.Serialize(unCaja);

        //            TcpClient cliente = new TcpClient();
        //            cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwaymp"]), int.Parse(ConfigurationManager.AppSettings["portgatwaymp"]));
        //            NetworkStream output = cliente.GetStream();
        //            BinaryWriter writer = new BinaryWriter(output);
        //            BinaryReader reader = new BinaryReader(output);
        //            writer.Write(salida);
        //            string rta = reader.ReadString();
        //            if (rta == "OK")
        //            {
        //                unCaja.estado = reader.ReadInt32();
        //            }
        //            reader.Close();
        //            writer.Close();
        //            output.Close();
        //            cliente.Close();
        //        }

        //    }
        //    catch (System.Exception ex)
        //    {
        //        //rta = "NOK,Error en conexion con servidor";
        //    }
        //    //return otro;
        //}

        public static PedidoPos JanisGetOrden(string numpedido, string tienda, int caja, int cajero)
        {            
            PedidoPos pp = null;
            if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwayjanis"], 30))
            {
                TcpClient cliente = new TcpClient();
                cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwayjanis"]), int.Parse(ConfigurationManager.AppSettings["portgatwayjanis"]));
                NetworkStream output = cliente.GetStream();
                BinaryWriter writer = new BinaryWriter(output);
                BinaryReader reader = new BinaryReader(output);
                string request = string.Format("1,{0},{1},{2},{3}", numpedido, tienda, caja, cajero);
                writer.Write(request);
                string rta = reader.ReadString();
                if (rta == "OK")
                {
                    rta = reader.ReadString();
                    try
                    {
                        var xmlserializer = new XmlSerializer(typeof(PedidoPos));
                        var stringReader = new StringReader(rta);
                        XmlDataDocument xmldoc = new XmlDataDocument();
                        pp = new PedidoPos();
                        xmldoc.Load(stringReader);
                        foreach (XmlNode item in xmldoc.ChildNodes)
                        {
                            foreach (XmlNode elem in item.ChildNodes)
                            {
                                switch (elem.Name)
                                {
                                    case "Seq":
                                        pp.Seq = Convert.ToInt32(elem.InnerText);
                                        break;
                                    case "SeqId":
                                        pp.SeqId = Convert.ToInt32(elem.InnerText);
                                        break;
                                    case "IdNumTie":
                                        pp.IdNumTie = Convert.ToInt16(elem.InnerText);
                                        break;
                                    case "Caja":
                                        pp.Caja = Convert.ToInt16(elem.InnerText);
                                        break;
                                    case "FhOpera":
                                        pp.FhOpera = Convert.ToDateTime(elem.InnerText);
                                        break;
                                    case "Estado":
                                        pp.Estado = Convert.ToInt16(elem.InnerText);
                                        break;
                                    case "Doc":
                                        pp.Doc = elem.InnerText;
                                        break;
                                    case "NomCli":
                                        pp.NomCli = elem.InnerText;
                                        break;
                                    case "DirCli":
                                        pp.DirCli = elem.InnerText;
                                        break;
                                    case "Total":
                                        pp.Total = Convert.ToDecimal(elem.InnerText);
                                        break;
                                    case "Sync":
                                        pp.Sync = Convert.ToInt16(elem.InnerText);
                                        break;
                                    case "Tienda":
                                        pp.Tienda = elem.InnerText;
                                        break;
                                    case "FhCreado":
                                        pp.FhCreado = Convert.ToDateTime(elem.InnerText);
                                        break;
                                    case "FhEntrega":
                                        pp.FhEntrega = Convert.ToDateTime(elem.InnerText);
                                        break;
                                    case "Cajero":
                                        pp.Cajero = Convert.ToInt16(elem.InnerText);
                                        break;
                                    case "items":
                                        ItemPos ipelem = null;
                                        pp.items = new List<ItemPos>();
                                        foreach (XmlNode xxitem in elem.ChildNodes)
                                        {
                                            foreach (XmlNode xitem in xxitem.ChildNodes)
                                            {
                                                if (xitem.Name == "Cod")
                                                {
                                                    ipelem = new ItemPos();
                                                    ipelem.Cod = Convert.ToInt32(xitem.InnerText);
                                                }
                                                else if (xitem.Name == "Cantidad")
                                                    ipelem.Cantidad = Convert.ToDecimal(xitem.InnerText);
                                                else if (xitem.Name == "Precio")
                                                    ipelem.Precio = Convert.ToDecimal(xitem.InnerText);
                                                else if (xitem.Name == "Id")
                                                    ipelem.Id = Convert.ToInt32(xitem.InnerText);
                                                else if (xitem.Name == "EsPesable")
                                                {
                                                    ipelem.EsPesable = Convert.ToBoolean(xitem.InnerText);
                                                    pp.items.Add(ipelem);
                                                }
                                            }
                                        }
                                        break;
                                }
                            }

                        }
                    }
                    catch (Exception ex)
                    {
                        pp = null;
                    }

                    rta = "";
                }
                reader.Close();
                writer.Close();
                output.Close();
                cliente.Close();

            }
            return pp;
        }

        public static string  JanisDatosTicket(int numpedido, int ptovta, int ticket, double importe)
        {
            string msgrta = "";
            string nrofactura = string.Format("{0:0000}-{1:0000000}", ptovta, ticket);
            try
            {
                if (ConexionRed.comprobarIP(ConfigurationManager.AppSettings["ipgatwayjanis"], 30))
                {
                    TcpClient cliente = new TcpClient();
                    cliente.Connect(IPAddress.Parse(ConfigurationManager.AppSettings["ipgatwayjanis"]), int.Parse(ConfigurationManager.AppSettings["portgatwayjanis"]));
                    NetworkStream output = cliente.GetStream();
                    BinaryWriter writer = new BinaryWriter(output);
                    BinaryReader reader = new BinaryReader(output);
                    string request = string.Format("2,{0},{1},{2},{3}", numpedido, nrofactura, DateTime.Now.ToString("yyyy-MM-dd"), importe);
                    writer.Write(request);
                    string rta = reader.ReadString();
                    if (rta == "OK")
                    {
                        msgrta = reader.ReadString();
                    }
                    reader.Close();
                    writer.Close();
                    output.Close();
                    cliente.Close();
                }
            }
            catch (Exception ex)
            {
                msgrta = "NOK - No se pudo cerrar operacion en Janis";
            }
            return msgrta;
        }

                
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;
using System.Text.RegularExpressions;

namespace Controles
{
    public class PromoOnLine
    {
        public static int GetPremios(string ipserv, int codigo, int caja, int pedido, bool simula)
        {
            int aux = 0;

            string pattern = @"\\\\(?<IP>\d+.\d+.\d+.\d+)";
            Match mc = Regex.Match(ipserv, pattern);
            if (mc != null)
            {
                string ipobt = mc.Groups["IP"].Value; 
                try
                {
                    if (comprobarIP(ipobt, 5))
                    {
                        TcpClient cliente = new TcpClient();
                        cliente.Connect(IPAddress.Parse(ipobt), 6100);
                        NetworkStream output = cliente.GetStream();
                        BinaryWriter writer = new BinaryWriter(output);
                        BinaryReader reader = new BinaryReader(output);
                        string mensaje = string.Format("{0},{1},{2},{3}", codigo, caja, pedido, (simula ? "SIM" : "NOSIM"));
                        writer.Write(mensaje);
                        string rta = reader.ReadString();

                        aux = int.Parse(rta);
                        reader.Close();
                        writer.Close();
                        output.Close();
                        cliente.Close();
                    }
                }
                catch (Exception ex)
                {
                    //Loging.EscribeExcepcion("Main", ex);
                }
            } 
            return aux;
        }

        /**
         * Comprueba una IP lanzando un ping
         */
        private static bool comprobarIP(string ip, int timeoutSegs)
        {
            // Creamos el objeto base y lo configuramos
            Ping ping = new Ping();
            PingOptions opciones = new PingOptions();
            opciones.DontFragment = true;
            // Añadimos 32 bytes de datos
            string datos = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
            byte[] buffer = Encoding.ASCII.GetBytes(datos);
            try
            {
                // Lanzamos el ping y esperamos la respuesta
                PingReply respuesta = ping.Send(ip, timeoutSegs * 1000, buffer, opciones);
                if (respuesta.Status == IPStatus.Success) return true;
                else return false;
            }
            catch (Exception ex)
            {
                // Algo ha pasado...
                return false;
            }
        }

    }
}

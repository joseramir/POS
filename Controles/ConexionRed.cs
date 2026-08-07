using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Net;
using System.Net.Sockets;
using System.Net.NetworkInformation;


namespace Controles
{
    public class ConexionRed
    {
        /**
         * Comprueba una IP lanzando un ping
         */
        public static bool comprobarIP(string ip, int timeoutSegs)
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

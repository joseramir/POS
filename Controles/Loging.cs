using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;

namespace Controles
{
    public class Loging
    {
        public static void EscribeMensaje(string line)
        {
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogContro.txt");
            writer.WriteLine("-------------------------------------------------------------------");
            writer.WriteLine(String.Format("{0} - {1}", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), line));
            writer.Close();
        }

        public static void EscribeExcepcion(string metodo, Exception ex)
        {
            Console.WriteLine("Ocurrio un error al procesar la solicitud, controlar Log.");
            StreamWriter writer = File.AppendText(Directory.GetCurrentDirectory() + "\\LogContro.txt");
            writer.WriteLine("-------------------------------------------------------------------");
            writer.WriteLine(string.Format("{0} - Ocurrio un error en metodo: {1}", DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), metodo));
            writer.WriteLine(ex.Message);
            writer.WriteLine(ex.StackTrace);
            writer.Close();
        }
    }
}

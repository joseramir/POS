using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;

namespace LibEntidades
{
    public class Loging
    {
        private static readonly object _lockLog = new object();
        private static readonly string _logPath =
            Path.Combine(Directory.GetCurrentDirectory(), "LogAPIVentas.txt");

        public static void EscribeMensaje(string line)
        {
            lock (_lockLog)
            {
                try
                {
                    using (FileStream fs = new FileStream(
                        _logPath,
                        FileMode.Append,
                        FileAccess.Write,
                        FileShare.ReadWrite))
                    using (StreamWriter writer = new StreamWriter(fs))
                    {
                        writer.WriteLine("-------------------------------------------------------------------");
                        writer.WriteLine(String.Format("{0} - {1}",
                            DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), line));
                    }
                }
                catch { }
            }
        }

        public static void EscribeExcepcion(string metodo, Exception ex)
        {
            Console.WriteLine("Ocurrio un error al procesar la solicitud, controlar Log.");
            lock (_lockLog)
            {
                try
                {
                    using (FileStream fs = new FileStream(
                        _logPath,
                        FileMode.Append,
                        FileAccess.Write,
                        FileShare.ReadWrite))
                    using (StreamWriter writer = new StreamWriter(fs))
                    {
                        writer.WriteLine("-------------------------------------------------------------------");
                        writer.WriteLine(string.Format("{0} - Ocurrio un error en metodo: {1}",
                            DateTime.Now.ToString("dd/MM/yyyy - HH:mm:ss"), metodo));
                        writer.WriteLine(ex.Message);
                        writer.WriteLine(ex.StackTrace);
                    }
                }
                catch { }
            }
        }
    }
}
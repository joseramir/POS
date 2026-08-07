using System;
using System.IO;
using System.Collections.Generic;
using System.Security.Cryptography;
using System.Linq;
using System.Text;

using Bitnet.TJOCommon;

namespace ConsoleApplication1
{
    class Program : ILog
    {
        static X25Request Sample(string fn)
        {
            byte[] bdata = File.ReadAllBytes(fn);
            X25Request req = new X25Request(bdata);
            return req;
        }

        static void Main(string[] args)
        {
            //X25Request sample = Sample("E:\\ja\\d2\\rqdata.bin");
            X25Request req = new X25Request();
            //sample.Decrypt();

            //%B4357609421893172^VILLAFANE/RICARDO FEDERICO^11101018514100323000000?;4357609421893172=111010185141323?
            //%B6280480140177496^VILLAFA&ES RICARDO FEDERICO^091010301111111?;6280480140177496=09101017300000000008?

            int probar = 1;
            switch(probar)
            {
                case 1:
                req.nro_cta_tarjeta = "4357609421893172";
                req.track_1 = "4357609421893172^VILLAFANE/RICARDO FEDERICO^11101018514100323000000";
                req.track_2 = "4357609421893172=111010185141323";
                req.fecha_expiracion = "1110";  // AAMM
                req.marca_id = 6;
                req.numero_de_comercio = "01465061";
                req.nodo_id = 2;
                req.numero_de_terminal = 75201001;
                break;

                case 2: 
                req.nro_cta_tarjeta = "6280480140177496";
                req.track_1 = ""; // "6280480140177496^VILLAFA&ES RICARDO FEDERICO^091010301111111";
                req.track_2 = "6280480140177496=09101017300000000008";
                req.fecha_expiracion = "0910";  // AAMM
                req.marca_id = 10;
                req.numero_de_comercio = "1";
                req.nodo_id = 1;
                req.numero_de_terminal = 72001001;
                break;

                case 3:
                req.nro_cta_tarjeta = "4517650551447683";
                req.track_1 = "4517650551447683^VILLAFA&ES/RICARDO FEDER ^12091211000000118000000";
                req.track_2 = "4517650551447683=12091211180000000000";
                req.fecha_expiracion = "1209";  // AAMM
                req.marca_id = 6;
                req.numero_de_comercio = "01465061";
                req.nodo_id = 2;
                req.numero_de_terminal = 75201001;
                req.cvv = 828;
                break;

                default:
                req.nodo_id = 2;
                break;
            }
            
            req.modo_de_ingreso = 1;

            req.cuotas = 1;
            req.importe = (probar == 9999) ? 0f : 1.00f;
            req.terminal_id = 1;
            req.plan = '0';
            req.ticket = 1234;
            req.tipo_de_mensaje = 200;
            req.bloque_version = 1;
            req.bloque_id_clave = 1;
            req.fecha = req.fecha_local = req.hora_local = DateTime.Now;

            //req.Encrypt();

            Program p = new Program();
            TJOClient clt = new TJOClient("192.168.0.79", 6543, 30, p);
            if (probar == 9999)
            {
                if(!clt.CierreLote(req.terminal_id, 1))
                    p.Write(string.Format("Fallo el Cierre de Lote. Terminal: {0}, Nodo: 1", req.terminal_id));
                else
                    p.Write(string.Format("Cierre de Lote OK. Terminal: {0}, Nodo 1", req.terminal_id));

                if (!clt.CierreLote(req.terminal_id, 2))
                    p.Write(string.Format("Fallo el Cierre de Lote. Terminal: {0}, Nodo: 2", req.terminal_id));
                else
                    p.Write(string.Format("Cierre de Lote OK. Terminal: {0}, Nodo 2", req.terminal_id));
            }
            else
            {
                if (!clt.ProcessRequest(req))
                    p.Write(string.Format("Fallo al enviar la transaccion: '{0}'", clt.ResultMessage));
                else
                {
                    p.Write(string.Format("Resultado= '{0}'", clt.ResultMessage));
                    if (clt.ResultAprobada)
                    {
                        p.Write(string.Format("\n\tAutorizacion: {0}", req.autorizacion));
                        p.Write(string.Format("\tBatch: {0}", req.batch));
                        p.Write(string.Format("\tTrace: {0}\n", req.numero_de_trace));
                    }

                }
            }

            Console.WriteLine("Listo. Pulsar ENTER para salir...");
            Console.ReadLine();
        }

        public void Write(string msg)
        {
            Console.WriteLine(msg);
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;

namespace Alberdi.TJOCommon.Prisma
{
    public class ProtocoloTrxPrisma
    {
        private const char STX = '\x02';
        private const char ETX = '\x03';

        private const char ENQ = '\x05';
        private const char ACK = '\x06';

        private StringBuilder sb = new StringBuilder();
        private int chkXor = 0;
        private string comando = "";

        enum CodigoComando
        {
            TEST,
            VENTA,
            VENTAEXTRACASH,
            CIERRELOTE,
            TABLATARJETAS,
            TABLAPLANES,
            ANULACION,
            DEVOLUCION,

            ULTIMATRANS,
            ULTIMOCIERRE,
            REIMPRETRANS,
            REIMPRECIERRE,
            QRMODO
        }

        private CodigoComando UltimoComando;

        public string GetTest()
        {
            comando = "";
            sb.Length = 0;
            sb.Append("TES");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(0), Convert.ToChar(0)));//Largo del paquete
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.TEST;
            return comando;
        }

        public string GetMensajeVta(TrxPrisma trx)
        {
            sb.Length = 0;
            trx.Mensaje = "VEN";
            sb.Append(trx.Mensaje);
            sb.Append(String.Format("{0}{1}", Convert.ToChar(104), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", Convert.ToInt64(trx.Monto * 100)).ToString().PadLeft(12, '0'));
            sb.Append(string.Format("{0}", trx.NroFactura.PadLeft(12, '0')));
            sb.Append(string.Format("{0:00}", trx.CantCuotas));
            sb.Append(string.Format("{0:000}", trx.CodigoTarjeta));
            sb.Append(string.Format("{0}", trx.CodigoPlan));
            sb.Append(string.Format("{0}", "000000000000"));
            sb.Append(string.Format("{0}", trx.CodigoComercio.PadRight(15, ' ')));
            sb.Append(string.Format("{0}", trx.NombreComercio.PadRight(23, ' ')));
            sb.Append(string.Format("{0}", trx.CuitComercio.PadRight(23, ' ')));
            sb.Append(string.Format("{0}", Convert.ToChar(trx.LineMode)));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.VENTA;
            return comando;
        }

        public string GetMensajeVtaExtraCash(TrxPrisma trx)
        {
            sb.Length = 0;
            trx.Mensaje = "VEC";
            sb.Append(trx.Mensaje);
            sb.Append(String.Format("{0}{1}", Convert.ToChar(104), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", (Convert.ToInt64(trx.Monto * 100)).ToString().PadLeft(12, '0')));
            sb.Append(string.Format("{0}", trx.NroFactura.PadLeft(12, '0')));
            sb.Append(string.Format("{0:00}", trx.CantCuotas));
            sb.Append(string.Format("{0:000}", trx.CodigoTarjeta));
            sb.Append(string.Format("{0}", trx.CodigoPlan));
            sb.Append(string.Format("{0}", (Convert.ToInt64(trx.MontoExtraCash * 100)).ToString().PadLeft(12, '0')));
            sb.Append(string.Format("{0}", trx.CodigoComercio.PadRight(15, ' ')));
            sb.Append(string.Format("{0}", trx.NombreComercio.PadRight(23, ' ')));
            sb.Append(string.Format("{0}", trx.CuitComercio.PadRight(23, ' ')));
            sb.Append(string.Format("{0}", Convert.ToChar(trx.LineMode)));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.VENTAEXTRACASH;
            return comando;
        }

        public string GetMensajeAnulacion(int nrocupon, string CodigoTarjeta)
        {
            sb.Length = 0;
            sb.Append("ANV");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(10), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", nrocupon.ToString().PadLeft(7, '0')));
            sb.Append(string.Format("{0}", CodigoTarjeta));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.ANULACION;
            return comando;
        }

        public string GetMensajeDevolucion(TrxPrisma trx, RtaVenta taux)
        {
            sb.Length = 0;
            trx.Mensaje = "DEV";
            sb.Append(trx.Mensaje);
            sb.Append(String.Format("{0}{1}", Convert.ToChar(109), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", Convert.ToInt64(trx.Monto * 100)).ToString().PadLeft(12, '0'));
            sb.Append(string.Format("{0:000}", trx.CodigoTarjeta));
            sb.Append(string.Format("{0}", trx.CodigoPlan));
            sb.Append(string.Format("{0:00}", trx.CantCuotas));
            sb.Append(string.Format("{0}", taux.NumeroCupon.ToString().PadLeft(7, '0')));
            sb.Append(string.Format("{0}", taux.FechaTransaccion));
            sb.Append(string.Format("{0}", trx.NroFactura.PadLeft(12, '0')));
            sb.Append(string.Format("{0}", trx.CodigoComercio.PadRight(15, ' ')));
            sb.Append(string.Format("{0}", trx.NombreComercio.PadRight(23, ' ')));
            sb.Append(string.Format("{0}", trx.CuitComercio.PadRight(23, ' ')));
            sb.Append(string.Format("{0}", Convert.ToChar(trx.LineMode)));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.DEVOLUCION;
            return comando;
        }

        public string GetUltimaOperacion()
        {
            sb.Length = 0;
            sb.Append("ULT");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(0), Convert.ToChar(0)));//Largo del paquete
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.ULTIMATRANS;
            return comando;
        }


        //pARA obtener tabla de tarjetas
        public string GetTablaTarjetas(int indice)
        {
            sb.Length = 0;
            sb.Append("TAR");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(4), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", indice.ToString().PadLeft(4, '0')));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.TABLATARJETAS;
            return comando;
        }

        public string GetTablaPlanes(int indice)
        {
            sb.Length = 0;
            sb.Append("PLA");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(4), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", indice.ToString().PadLeft(4, '0')));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.TABLAPLANES;
            return comando;
        }

        public string GetCierre()
        {
            comando = "";
            sb.Length = 0;
            sb.Append("CIE");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(0), Convert.ToChar(0)));//Largo del paquete
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.CIERRELOTE;
            return comando;
        }

        public string GetMensajeQrModo(TrxPrisma trx)
        {
            sb.Length = 0;
            sb.Append("QRZ");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(15), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", Convert.ToInt64(trx.Monto * 100)).ToString().PadLeft(12, '0'));
            sb.Append(string.Format("{0:00}", trx.CantCuotas));
            sb.Append(string.Format("{0}", trx.CodigoPlan));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.QRMODO;
            return comando;
        }

        public RespuestaBase ResolverRespuesta(string cadena)
        {
            RespuestaBase rta = null;
            if (UltimoComando == CodigoComando.VENTA || UltimoComando == CodigoComando.VENTAEXTRACASH ||
                UltimoComando == CodigoComando.ANULACION || UltimoComando == CodigoComando.DEVOLUCION)
                rta = GetRtaVenta(cadena);
            else if (UltimoComando == CodigoComando.QRMODO)
                rta = GetRtaQrModo(cadena);
            else if (UltimoComando == CodigoComando.CIERRELOTE)
                rta = GetRtaCierre(cadena);
            else if (UltimoComando == CodigoComando.TABLATARJETAS)
                rta = GetRtaTablaTarjeta(cadena);
            else if (UltimoComando == CodigoComando.TABLAPLANES)
                rta = GetRtaTablaPlan(cadena);
            else if (UltimoComando == CodigoComando.ULTIMATRANS)
                rta = GetRtaUltTrans(cadena);
            else if (UltimoComando == CodigoComando.ULTIMOCIERRE)
                rta = GetRtaUltCierre(cadena);
            else if (UltimoComando == CodigoComando.TEST)
                rta = GetRtaTest(cadena);
            return rta;
        }

        public RtaTest GetRtaTest(string cadena)
        {
            RtaTest aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 8)
            {
                string ssub = cadena.Substring(3);
                aux = new RtaTest();
                //la segunda fuente de error es en la autorizacion con el emisor (HOST)
                //Autorizado cuando CodigoRespuesta = 0, falta ver como viene en otros casos el resto de campos
                aux.Respuesta = ssub.Substring(0, 3);
            }
            return aux;
        }



        public RtaVenta GetRtaVenta(string cadena)
        {
            RtaVenta aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 106)
            {
                string ssub = cadena.Substring(8);
                aux = new RtaVenta();
                //la segunda fuente de error es en la autorizacion con el emisor (HOST)
                //Autorizado cuando CodigoRespuesta = 0, falta ver como viene en otros casos el resto de campos
                aux.CodigoRespuesta = Convert.ToInt32(ssub.Substring(0, 2));
                aux.MensajeRespuesta = ssub.Substring(2, 32);
                if (aux.CodigoRespuesta == 0)
                {
                    aux.CodigoAutorizacion = ssub.Substring(34, 6);
                    aux.NumeroCupon = Convert.ToInt32(ssub.Substring(40, 7));
                    aux.NumeroLote = Convert.ToInt32(ssub.Substring(47, 3));
                    aux.NombreCliente = ssub.Substring(50, 26);
                    aux.Ult4Digitos = ssub.Substring(76, 4);
                    aux.Prim6Digitos = ssub.Substring(80, 6);
                    aux.FechaTransaccion = ssub.Substring(86, 10);
                    aux.HoraTransaccion = ssub.Substring(96, 8);
                    try
                    {
                        aux.TerminalId = Convert.ToInt32(ssub.Substring(104, 8));
                    }
                    catch (System.Exception ex)
                    {

                    }

                }
            }
            return aux;
        }


        public RtaQrModo GetRtaQrModo(string cadena)
        {
            RtaQrModo aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 106)
            {
                string ssub = cadena.Substring(8);
                aux = new RtaQrModo();
                //la segunda fuente de error es en la autorizacion con el emisor (HOST)
                //Autorizado cuando CodigoRespuesta = 0, falta ver como viene en otros casos el resto de campos
                aux.CodigoRespuesta = Convert.ToInt32(ssub.Substring(0, 2));
                aux.MensajeRespuesta = ssub.Substring(2, 32);
                if (aux.CodigoRespuesta == 0)
                {
                    aux.CodigoAutorizacion = ssub.Substring(34, 6);
                    aux.NumeroCupon = Convert.ToInt32(ssub.Substring(40, 7));
                    aux.NumeroLote = Convert.ToInt32(ssub.Substring(47, 3));
                    aux.NombreCliente = "";
                    aux.Ult4Digitos = ssub.Substring(50, 4);
                    aux.Prim6Digitos = ssub.Substring(54, 6);
                    aux.FechaTransaccion = ssub.Substring(60, 10);
                    aux.HoraTransaccion = ssub.Substring(70, 8);
                    aux.TerminalId = Convert.ToInt32(ssub.Substring(78, 8));
                    aux.CodigoTarjeta = ssub.Substring(86, 3);
                    aux.ImporteTotal = Convert.ToDecimal(ssub.Substring(89, 12)) / 100;
                    aux.ImporteCobrado = Convert.ToDecimal(ssub.Substring(101, 12)) / 100;
                }
            }
            return aux;
        }

        public RtaCierre GetRtaCierre(string cadena)
        {
            RtaCierre aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 28)
            {
                string ssub = cadena.Substring(8);
                aux = new RtaCierre();
                aux.CodigoRespuesta = Convert.ToInt32(ssub.Substring(0, 2));
                aux.FechaTransaccion = ssub.Substring(2, 10);
                aux.HoraTransaccion = ssub.Substring(12, 8);
                aux.TerminalId = Convert.ToInt32(ssub.Substring(20, 8));
            }
            return aux;
        }

        public RtaTablaTarjeta GetRtaTablaTarjeta(string cadena)
        {
            RtaTablaTarjeta aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 36)
            {
                string ssub = cadena.Substring(8);
                aux = new RtaTablaTarjeta();
                aux.IndiceRegistro = Convert.ToInt32(ssub.Substring(0, 4));
                aux.CodigoProcesador = ssub.Substring(4, 3);
                aux.CodigoTarjeta = ssub.Substring(7, 3);
                aux.NombreTarjeta = ssub.Substring(10, 16);
                aux.CantidadCuotas = Convert.ToInt32(ssub.Substring(26, 2));
                aux.TerminalId = Convert.ToInt32(ssub.Substring(28, 8));

                aux.FechaTransaccion = DateTime.Now.ToString("dd/MM/yyyy");
                aux.HoraTransaccion = DateTime.Now.ToString("HH:mm:ss");
            }
            return aux;
        }

        public RtaTablaPlan GetRtaTablaPlan(string cadena)
        {
            RtaTablaPlan aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 36)
            {
                string ssub = cadena.Substring(8);
                aux = new RtaTablaPlan();
                aux.IndiceRegistro = Convert.ToInt32(ssub.Substring(0, 4));
                aux.CodigoTarjeta = ssub.Substring(4, 3);
                aux.CodigoPlan = ssub.Substring(7, 2);
                aux.NombrePlan = ssub.Substring(9, 14);
                aux.TerminalId = Convert.ToInt32(ssub.Substring(23, 8));

                aux.FechaTransaccion = DateTime.Now.ToString("dd/MM/yyyy");
                aux.HoraTransaccion = DateTime.Now.ToString("HH:mm:ss");
            }
            return aux;
        }

        public RtaTrans GetRtaUltTrans(string cadena)
        {
            RtaTrans aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 106)
            {
                string ssub = cadena.Substring(8);
                aux = new RtaTrans();
                //la segunda fuente de error es en la autorizacion con el emisor (HOST)
                //Autorizado cuando CodigoRespuesta = 0, falta ver como viene en otros casos el resto de campos
                aux.TipoTransaccion = Convert.ToInt32(ssub.Substring(0, 1));
                aux.CodigoRespuesta = Convert.ToInt32(ssub.Substring(1, 2));
                aux.MensajeRespuesta = ssub.Substring(3, 32);
                if (aux.CodigoRespuesta == 0)
                {
                    aux.CodigoAutorizacion = ssub.Substring(35, 6);
                    aux.NumeroCupon = Convert.ToInt32(ssub.Substring(41, 7));
                    aux.NumeroLote = Convert.ToInt32(ssub.Substring(48, 3));
                    aux.NombreCliente = ssub.Substring(51, 26);

                    aux.Ult4Digitos = ssub.Substring(77, 4);
                    aux.Prim6Digitos = ssub.Substring(81, 6);

                    aux.FechaTransaccion = ssub.Substring(87, 10);
                    aux.HoraTransaccion = ssub.Substring(97, 8);
                    aux.TerminalId = Convert.ToInt32(ssub.Substring(105, 8));
                }
            }
            return aux;
        }

        public RtaUltCierre GetRtaUltCierre(string cadena)
        {
            RtaUltCierre aux = null;
            if (!string.IsNullOrEmpty(cadena) && cadena.Length >= 95)
            {
                string ssub = cadena.Substring(8);
                aux = new RtaUltCierre();
                aux.IndiceRegistro = Convert.ToInt32(ssub.Substring(0, 4));
                aux.CodigoProcesador = ssub.Substring(4, 3);
                aux.NumeroLote = Convert.ToInt32(ssub.Substring(7, 3));
                aux.CodigoTarjeta = ssub.Substring(10, 3);
                aux.CantidadVentas = Convert.ToInt32(ssub.Substring(13, 4));
                aux.MontoTotalVentas = Convert.ToDecimal(ssub.Substring(17, 12)) / 100;
                aux.CantidadAnulaVenta = Convert.ToInt32(ssub.Substring(29, 4));
                aux.MontoTotalAnula = Convert.ToDecimal(ssub.Substring(33, 12)) / 100;
                aux.CantidadDevolVenta = Convert.ToInt32(ssub.Substring(45, 4));
                aux.MontoTotalDevol = Convert.ToDecimal(ssub.Substring(49, 12)) / 100;
                aux.CantidadAnulaDevol = Convert.ToInt32(ssub.Substring(61, 4));
                aux.MontoTotalAnuDev = Convert.ToDecimal(ssub.Substring(65, 12)) / 100;

                aux.FechaCierre = ssub.Substring(77, 10);
                aux.HoraCierre = ssub.Substring(87, 8);
                aux.TerminalId = Convert.ToInt32(ssub.Substring(95, 8));

                aux.FechaTransaccion = DateTime.Now.ToString("dd/MM/yyyy");
                aux.HoraTransaccion = DateTime.Now.ToString("HH:mm:ss");
            }
            return aux;
        }

        public string GetUltCierre(int indice)
        {
            sb.Length = 0;
            sb.Append("ULC");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(4), Convert.ToChar(0)));//Largo del paquete
            sb.Append(string.Format("{0}", indice.ToString().PadLeft(4, '0')));
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.ULTIMOCIERRE;
            return comando;
        }

        public string ReimpreUltTrans()
        {
            sb.Length = 0;
            sb.Append("IMT");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(0), Convert.ToChar(0)));//Largo del paquete
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.REIMPRETRANS;
            return comando;
        }

        public string ReimpreUltCierre()
        {
            sb.Length = 0;
            sb.Append("IMC");
            sb.Append(String.Format("{0}{1}", Convert.ToChar(0), Convert.ToChar(0)));//Largo del paquete
            sb.Append(ETX);
            chkXor = CalcularXor(sb.ToString());
            comando = string.Format("{0}{1}{2}", STX, sb.ToString(), Convert.ToChar(chkXor));
            UltimoComando = CodigoComando.REIMPRECIERRE;
            return comando;
        }

        public string ResolverError(string cadena)
        {
            string aux = cadena;
            switch (cadena.Substring(3, 3))
            {
                case "001":
                    aux = "Quedan registros por enviar";
                    break;
                case "102":
                    aux = "El numero de ticket no existe";
                    break;
                case "103":
                    aux = "El codigo de plan no existe";
                    break;
                case "104":
                    aux = "El indice de registro no existe";
                    break;

                case "201":
                    aux = "Transaccion cancelada por el usuario";
                    break;
                case "202":
                    aux = "La tarjeta deslizada por el usuario no coincide con la pedida";
                    break;
                case "203":
                    aux = "La tarjeta deslizada no es valida";
                    break;
                case "204":
                    aux = "La tarjeta deslizada esta vencida";
                    break;
                case "205":
                    aux = "Transaccion original inexistente";
                    break;
                case "206":
                    aux = "No hay transacciones en el lote";
                    break;

                case "301":
                    aux = "El POS no pudo comunicarse con el host";
                    break;
                case "302":
                    aux = "El POS no pudo imprimir el ticket";
                    break;

                case "901":
                    aux = "Nombre del comando inexistente";
                    break;
                case "902":
                    aux = "Largo de los parametros invalido para este comando";
                    break;
                case "903":
                    aux = "Formato de algun parametro no es correcto";
                    break;
                case "909":
                    aux = "Error general en la operacion";
                    break;

                default:
                    aux = "Otro error desconocido, Sin Definir Respuesta";
                    break;
            }

            return aux;
        }


        private int CalcularXor(string sparam)
        {
            int aux = 0;
            foreach (char item in sparam)
            {
                aux = aux ^ Convert.ToInt32(item);
            }
            return aux;
        }
    }
}

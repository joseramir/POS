using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Threading;

namespace Alberdi.TJOCommon.CloverECR
{
    public delegate void ActualizaMsgCloverDelegate(string tit1, string tit2);

    // Ventana de espera de las operaciones con Clover, como MsgAlertPrismaECR: la operacion corre en un hilo,
    // la ventana muestra lo que informa CloverECR y se cierra sola al terminar. ESC pide cancelar la venta
    // (solo sirve antes de que el cliente presente la tarjeta; el resultado igual llega por el polling).
    // Cada Show* se usa una vez por instancia.
    public partial class MsgAlertCloverECR : Form
    {
        public EstadoPago RespuestaVenta = null;
        public VentaParaReversion RespuestaBusqueda = null;
        public EstadoOperacion RespuestaOperacion = null;
        public Resumen RespuestaResumen = null;
        public RespuestaPos RespuestaReimpresion = null;
        // IdOperacion de la venta apenas CloverECR la acepta: queda aunque la espera se corte
        public string IdOperacion = "";

        // ConError = la operacion no se pudo completar del lado de la caja (configuracion, sin conexion, excepcion).
        // Los rechazos y errores de CloverECR vienen en la respuesta (Estado / Mensaje / HttpStatus).
        public bool ConError;
        public string MensajeError;

        CloverClient clover = null;

        volatile bool terminado;
        volatile bool cancelarPedido;
        volatile bool SePuedeCancelar;

        public MsgAlertCloverECR()
        {
            InitializeComponent();
            this.KeyPreview = true; // necesario para que PreviewKeyDown reciba Escape
            ConError = false;
            MensajeError = "";
            SePuedeCancelar = false;
            lblCancelar.Visible = false;
            terminado = true;
        }

        private void ActualizaTitulos(string tit1, string tit2)
        {
            if (InvokeRequired)
            {
                Invoke(new ActualizaMsgCloverDelegate(ActualizaTitulos), new object[] { tit1, tit2 });
            }
            else
            {
                label1.Text = tit1;
                label2.Text = tit2;
            }
        }

        private void MostrarEstadoVenta(EstadoPago e)
        {
            if (e.Estado == Estados.Pendiente)
                ActualizaTitulos("Esperando al cliente en el Flex", e.Mensaje);
            else if (e.Estado == Estados.Verificando)
                ActualizaTitulos("Verificando con Clover: NO cobrar de nuevo", e.Mensaje);
            else if (e.Estado == Estados.Aprobada)
                ActualizaTitulos("Operación APROBADA", "");
            else
                ActualizaTitulos("FALLÓ LA APROBACIÓN", e.Mensaje);
        }

        private void MostrarEstadoOperacion(EstadoOperacion e)
        {
            if (e.Estado == Estados.Pendiente)
                ActualizaTitulos("Esperando la confirmación del Flex", e.Mensaje);
            else if (e.Estado == Estados.Verificando)
                ActualizaTitulos("Verificando con Clover", e.Mensaje);
            else if (e.Estado == Estados.Aprobada)
                ActualizaTitulos("Operación APROBADA", "");
            else
                ActualizaTitulos("NO SE PUDO COMPLETAR", e.Mensaje);
        }

        // Venta: POST /pagos y polling hasta el resultado final (o hasta la espera maxima de CloverClient).
        public void ShowMsgProcesaVenta(string tit1, string tit2, PagoRequest pedido)
        {
            RespuestaVenta = null;
            Ejecutar(tit1, tit2, true, delegate
            {
                CloverClient.EscribeLog("VENTA " + pedido.EcrTransactionId);
                RespuestaVenta = clover.Pagar(pedido,
                    delegate { return cancelarPedido; },
                    MostrarEstadoVenta,
                    delegate(string id) { IdOperacion = id; CloverClient.EscribeLog("Venta aceptada, IdOperacion=" + id, "CAJA"); });
            });
        }

        // Vuelve a consultar una venta que quedo sin resultado final (PENDIENTE/VERIFICANDO)
        public void ShowMsgEsperaVenta(string tit1, string tit2, string idOperacion)
        {
            RespuestaVenta = null;
            IdOperacion = idOperacion;
            Ejecutar(tit1, tit2, false, delegate
            {
                CloverClient.EscribeLog("VENTA re-consulta, IdOperacion=" + idOperacion);
                RespuestaVenta = clover.EsperarPago(idOperacion, MostrarEstadoVenta);
            });
        }

        public void ShowMsgBuscaVenta(string tit1, string tit2, string claveBusq)
        {
            RespuestaBusqueda = null;
            Ejecutar(tit1, tit2, false, delegate
            {
                CloverClient.EscribeLog("BUSCAR VENTA " + claveBusq);
                RespuestaBusqueda = clover.BuscarVenta(claveBusq);
            });
        }

        public void ShowMsgProcesaAnulacion(string tit1, string tit2, string claveBusq, string numeroCajero)
        {
            RespuestaOperacion = null;
            Ejecutar(tit1, tit2, false, delegate
            {
                CloverClient.EscribeLog("ANULACION " + claveBusq);
                RespuestaOperacion = clover.Anular(claveBusq, numeroCajero, MostrarEstadoOperacion);
            });
        }

        // monto null = devuelve todo lo que queda
        public void ShowMsgProcesaDevolucion(string tit1, string tit2, string claveBusq, decimal? monto, string numeroCajero)
        {
            RespuestaOperacion = null;
            Ejecutar(tit1, tit2, false, delegate
            {
                CloverClient.EscribeLog("DEVOLUCION " + claveBusq + " monto=" + (monto.HasValue ? monto.Value.ToString("F2") : "TOTAL"));
                RespuestaOperacion = clover.Devolver(claveBusq, monto, numeroCajero, MostrarEstadoOperacion);
            });
        }

        public void ShowMsgResumen(string tit1, string tit2, DateTime fecha)
        {
            RespuestaResumen = null;
            Ejecutar(tit1, tit2, false, delegate
            {
                CloverClient.EscribeLog("RESUMEN " + fecha.ToString("yyyy-MM-dd"));
                RespuestaResumen = clover.ObtenerResumen(fecha);
            });
        }

        public void ShowMsgReimprimir(string tit1, string tit2, string idOperacion)
        {
            RespuestaReimpresion = null;
            Ejecutar(tit1, tit2, false, delegate
            {
                CloverClient.EscribeLog("REIMPRIMIR " + idOperacion);
                RespuestaReimpresion = clover.Reimprimir(idOperacion);
            });
        }

        // Corre "trabajo" en un hilo con la ventana abierta y la cierra cuando termina (bien o con error).
        private void Ejecutar(string tit1, string tit2, bool permiteCancelar, ThreadStart trabajo)
        {
            label1.Text = tit1;
            label2.Text = tit2;
            lblCancelar.Visible = false;
            ConError = false;
            MensajeError = "";
            cancelarPedido = false;
            SePuedeCancelar = false;
            terminado = false;

            // El handle tiene que existir antes de arrancar el hilo, para que ActualizaTitulos use Invoke
            IntPtr handle = this.Handle;

            tmrSePuedeCancelar.Enabled = permiteCancelar;
            tmrControlCierre.Enabled = true;

            Thread hilo = new Thread(delegate()
            {
                try
                {
                    if (clover == null)
                        clover = CloverClient.DesdeConfiguracion();
                    trabajo();
                }
                catch (CloverEcrSinConexionException ex)
                {
                    this.ConError = true;
                    this.MensajeError = "Sin conexión con CloverECR. " + ex.Message;
                    CloverClient.EscribeLog("Error: " + ex.Message, "CAJA");
                }
                catch (Exception ex)
                {
                    this.ConError = true;
                    this.MensajeError = ex.Message;
                    CloverClient.EscribeLog("Error: " + ex.Message, "CAJA");
                    CloverClient.EscribeLog("Error: " + ex.StackTrace, "CAJA");
                }
                finally
                {
                    terminado = true;
                }
            });
            hilo.IsBackground = true;
            hilo.Start();
            this.ShowDialog();
            hilo.Join();
        }

        private void MsgAlertCloverECR_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyData == Keys.Escape && SePuedeCancelar && !terminado)
            {
                // El hilo de la venta ve el pedido en la proxima consulta y manda la cancelacion al Flex una sola vez
                SePuedeCancelar = false;
                cancelarPedido = true;
                lblCancelar.Visible = false;
                label1.Text = "CANCELANDO la operación en el Flex";
                label2.Text = "Aguarde el resultado...";
            }
        }

        private void tmrSePuedeCancelar_Tick(object sender, EventArgs e)
        {
            tmrSePuedeCancelar.Enabled = false;
            if (!terminado)
            {
                SePuedeCancelar = true;
                lblCancelar.Visible = true;
            }
        }

        private void tmrControlCierre_Tick(object sender, EventArgs e)
        {
            if (terminado)
            {
                tmrControlCierre.Enabled = false;
                tmrSePuedeCancelar.Enabled = false;
                this.Close();
            }
        }
    }
}

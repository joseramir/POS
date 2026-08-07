using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Alberdi
{
    public class HeaderDoc
    {
        public string Seq { get; set; }
        //public short IdTienda { get; set; }
        public Sucursal Tienda { get; set; }
        public int Caja { get; set; }
        public int Cajero { get; set; }
        public string CanalVenta { get; set; }
        public int PuntoVenta { get; set; }
        public short TipoComprobante { get; set; }
        public int NumeroComprobante { get; set; }
        public string CodCli { get; set; }
        public string Nombre { get; set; }
        public string Direccion { get; set; }
        public TipoIva mTipoIva { get; set; }
        public Reparticion mReparticion { get; set; }

        public bool Anulado { get; set; }
        public decimal Total { get; set; }
        public DateTime FechaHora { get; set; }

        public string ReferenciaComprobante { get; set; }
        public short Sync { get; set; }

        public int NroVendedor { get; set; }

        public string MotivoNC { get; set; }

        public List<DetalleDoc> Detalle { get; set; }
        public List<PromoAplicada> Promociones { get; set; }
        public List<ImpuestoAplicado> Impuestos { get; set; }
        public List<PagoDoc> Pagos { get; set; }

        public HeaderDoc()
        {
            Detalle = new List<DetalleDoc>();
            Promociones = new List<PromoAplicada>();
            Impuestos = new List<ImpuestoAplicado>();
            Pagos = new List<PagoDoc>();
            mTipoIva = new TipoIva() { CondIva = "CF", Descripcion = "Consumidor Final", IdCondIva = 1 };
            Caja = 0;
            Cajero = 0;
            CanalVenta = ""; // Default to 1 (Retail)
            TipoComprobante = 83; // Default to 83 (Ticket Comun)
            NumeroComprobante = 0;
            CodCli = "";
            Nombre = "";
            Direccion = "";
            mReparticion = new Reparticion() { ReparticionID = 1, Descripcion = "Sin Reparticion", Habilitado = true };
            Total = 0.0m;
            Tienda = new Sucursal() { IdTienda = 0, Tienda = "No definido", Habilitado = false };
            Seq = Guid.NewGuid().ToString();
            Anulado = false;
            ReferenciaComprobante = "";
            Sync = 0;
            MotivoNC = "";
        }

        public void Clear()
        {
            Seq = "";
            Tienda = new Sucursal() { IdTienda = 0, Tienda = "No definido", Habilitado = false };
            Caja = 0;
            Cajero = 0;
            CanalVenta = ""; // Default to 1 (Retail)
            PuntoVenta = 0;
            TipoComprobante = 83; // Default to 83 (Ticket Comun)
            NumeroComprobante = 0;
            CodCli = string.Empty;
            Nombre = string.Empty;
            Direccion = string.Empty;
            mTipoIva = new TipoIva();
            mReparticion = new Reparticion() { ReparticionID = 1, Descripcion = "Sin Reparticion", Habilitado = true };
            Total = 0.0m;
            FechaHora = DateTime.MinValue;
            Detalle.Clear();
            Promociones.Clear();
            Impuestos.Clear();
            Pagos.Clear();
            Anulado = false;
            ReferenciaComprobante = "";
            Sync = 0;
            MotivoNC = "";
        }

        //Marca un item del Ticket como anulado
        public void AnularItem(int NroReg)
        {
            for (int i = 0; i < this.Detalle.Count; i++)
            {
                if (this.Detalle[i].NroReg == NroReg)
                {
                    this.Detalle[i].Anulado = true;
                    break;
                }
            }
        }
    }
}

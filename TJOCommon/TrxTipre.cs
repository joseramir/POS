using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Bitnet.TJOCommon
{
    public class CampoTipre
    {
        private string _valor;
        public string Valor
        {
            get { return _valor; }
            set { _valor = value; }
        }        
        private bool _requerido;
        public bool Requerido
        {
            get { return _requerido; }
            set { _requerido = value; }
        }
        
        public CampoTipre()
        {
            Valor = "";
            Requerido = false;
        }
    }

    

    public class TrxTipre
    {
        private Dictionary<string, CampoTipre> _elem;
        public Dictionary<string, CampoTipre> Elem
        {
            get { return _elem; }
            set { _elem = value; }
        }
        private Dictionary<string, CampoTipre> _planes;
        public Dictionary<string, CampoTipre> Planes
        {
            get { return _planes; }
            set { _planes = value; }
        }

        public TrxTipre()
        {
            Elem = new Dictionary<string, CampoTipre>();
            Elem.Add("OPERACION", new CampoTipre());
            Elem.Add("SUCURSAL", new CampoTipre());
            Elem.Add("NROPOS", new CampoTipre());
            Elem.Add("NROTICKET", new CampoTipre());
            Elem.Add("ID", new CampoTipre());
            Elem.Add("IDTIPO", new CampoTipre());
            Elem.Add("NOMBREEMISOR", new CampoTipre());
            Elem.Add("CODIGORESPUESTA", new CampoTipre());
            Elem.Add("MENSAJERESPUESTA", new CampoTipre());
            Elem.Add("CODIGOSEGURIDAD", new CampoTipre());
            Elem.Add("CODIGOSEGURIDADLEN", new CampoTipre());
            Elem.Add("DOCUMENTO", new CampoTipre());
            Elem.Add("FECHAVTO", new CampoTipre());
            Elem.Add("IMPORTE", new CampoTipre());
            Elem.Add("NROAUTORIZACION", new CampoTipre());
            Elem.Add("NROCUPON", new CampoTipre());
            Elem.Add("NROLOTE", new CampoTipre());
            Elem.Add("IDTRXTIPRE", new CampoTipre());
            Elem.Add("PLANDEPAGO", new CampoTipre()); 
            Elem.Add("IMPORTE_DESCUENTO", new CampoTipre());
            Elem.Add("IMPORTE_RECARGO", new CampoTipre());
            Elem.Add("VOUCHERCOMERCIO", new CampoTipre());
            Elem.Add("VOUCHERCLIENTE", new CampoTipre());
            Elem.Add("FECHAHORATRX", new CampoTipre());
            Elem.Add("WORKINGKEYNUEVA", new CampoTipre());
            Elem.Add("PIN", new CampoTipre());
            Elem.Add("TIPODECUENTA", new CampoTipre());
            Elem.Add("REQUIEREPINPAD", new CampoTipre());
            Elem.Add("NRO_TARJETA", new CampoTipre());
            Elem.Add("CODIGOBANCO", new CampoTipre());
            Elem.Add("SERVICECODE", new CampoTipre());
            Elem.Add("REQUIERECUENTA", new CampoTipre());
            Elem.Add("ENCRIPTAR", new CampoTipre());
            Elem.Add("MDI_MODOINGRESOTARJETA", new CampoTipre());
            Elem.Add("NSL_NROSERIELOGPP", new CampoTipre());
            Elem.Add("TDE_TIPODEENCRIPCION", new CampoTipre());
            Elem.Add("EPT_DATOSENCRIPTADOS", new CampoTipre());
            Elem.Add("NSF_NROSERIEFISPP", new CampoTipre());
            Elem.Add("TC1_TRACK1", new CampoTipre());
            Elem.Add("TC2_TRACK2", new CampoTipre());
            Elem.Add("CPG_CRIPTOGRAMAEMV", new CampoTipre());
            Elem.Add("RCP_CRIPTOGRAMAEMV", new CampoTipre());

            Planes = new Dictionary<string, CampoTipre>();
        }

        
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TransSql
{
    public class DRegTrans
    {
        
        public string Tienda {get; set;}                
        
        public short Caja  {get; set;}       
        
        public short Func  {get; set;}
        
        public short Cajero  {get; set;}
        
        public uint Fecha {get; set;}        
        
        public uint Hora {get; set;} 
        
        public string CnlVta {get; set;}
        
        public bool Towrite {get; set;}
        
        public DRegTrans()
        {
            this.Tienda = this.CnlVta = string.Empty;
            this.Caja = this.Func = this.Cajero = 0;
            this.Fecha = this.Hora = 0;
            this.Towrite = false;
        }
    }
    public class MDMpag: DRegTrans
    {        
        public int Nro {get; set;}        
        
        public char UVuelto {get; set;}                
        
        public char UPend {get; set;}        
        
        public string Importe {get; set;}
        
        public string Vuelto {get; set;}
        
        public string Cc {get; set;}    
        
        public string Numero {get; set;}        
        
        public string Fecha_c {get; set;}  
        
        public string Nombre {get; set;}
        
        public string Dni {get; set;}
        
        public string Telef {get; set;}       
        
        public string Direc {get; set;}        
        
        public string Local {get; set;}        
        
        public string Civa {get; set;}        
        
        public string Barrio {get; set;}
        
        public string Mtodesc {get; set;}       
        
        public char UOnline {get; set;}
        
        public string Dtarjeta {get; set;}       
        
        public uint Nticket {get; set;}        
        
        public int Alipercep {get; set;}
        
        public char Escredito {get; set;}        
        
        public char Esposnet {get; set;}
        

        public MDMpag():base()
        {
            this.Func = 0;
            this.Nro = 0;
            this.UVuelto = this.UPend = 'F';
            this.Importe = this.Vuelto = "0.00";
            this.Cc = this.Numero = this.Fecha_c = this.Nombre = this.Dni = this.Telef = this.Direc = this.Local = this.Civa = this.Barrio = this.Mtodesc = string.Empty;
            this.UOnline = 'F';            
            this.Cajero = 0;
            this.Dtarjeta = string.Empty;
            this.Alipercep = 0;
            this.Escredito = this.Esposnet = 'F';
        }
    }

    public class MDEop : DRegTrans
    {
        private ushort _tipo;
        public ushort Tipo
        {
            get { return _tipo; }
            set { _tipo = value; }
        }
        private uint _ntick;
        public uint Ntick
        {
            get { return _ntick; }
            set { _ntick = value; }
        }
        private string _imp;
        public string Imp
        {
            get { return _imp; }
            set { _imp = value; }
        }
        private char _anulFlag;
        public char AnulFlag
        {
            get { return _anulFlag; }
            set { _anulFlag = value; }
        }
        private string _montoAnul;
        public string MontoAnul
        {
            get { return _montoAnul; }
            set { _montoAnul = value; }
        }
        private short _ptovta;
        public short Ptovta
        {
            get { return _ptovta; }
            set { _ptovta = value; }
        }

        public MDEop():base()
        {
            this.Tipo = 0;
            this.Ntick = 0;
            this.Imp = this.MontoAnul = "0.00";
            this.AnulFlag = 'F';
        }
    }

    public class MDPlu : DRegTrans
    {
        
        public uint Cod { get; set; }        
        
        public char S_c_flag { get; set; }
        
        public char UCant { get; set; }        
        
        public ushort Iva { get; set; }
        
        public char Pesado { get; set; }        
        
        public ushort Dnro { get; set; }
        
        public uint Envas { get; set; }        
        
        public string YReg { get; set; }        
        
        public string XReg { get; set; }

        public string Miles { get; set; }        
        
        public string OrigCode { get; set; }        
        
        public ushort OtroP { get; set; }
        
        public char Anulado { get; set; }        
        
        public uint Precuni { get; set; }        
        
        public short Uni { get; set; }        
        
        public char EsBulto { get; set; }
        
        public char Dun14 { get; set; }        
        
        public short Sdnro { get; set; }        
        
        public string Impiva { get; set; }
        
        public string Precmay { get; set; }        
        
        public uint Ppuntual { get; set; }        
        
        public short Umillas { get; set; }

        public string PesoEnBalanza { get; set; }

        public char ApliPerIva { get; set; }
        public uint PrecPuntual { get; set; }

        public MDPlu(): base()
        {
            this.Cod = 0;
            this.S_c_flag = this.UCant = this.Pesado = 'F';
            this.Iva = 0;
            this.Dnro = 0;
            this.Envas = 0;
            this.YReg = this.XReg = "0.00";
            this.Miles = this.OrigCode = string.Empty;
            this.OtroP = 0;
            this.Anulado = 'F';
            this.Precuni = 0;
            this.Uni = 0;
            this.EsBulto = this.Dun14 = 'F';
            this.Sdnro = 0;
            this.Impiva = "0.00";
            this.Precmay = string.Empty;
            this.Ppuntual = 0;
            this.Umillas = 0;

            this.PesoEnBalanza = "0.000";
            this.ApliPerIva = 'F';
            PrecPuntual = 0;
        }
    }

    public class MDTickFac : DRegTrans
    {
        private short _nro;
        public short Nro
        {
            get { return _nro; }
            set { _nro = value; }
        }
        private char _flag;
        public char Flag
        {
            get { return _flag; }
            set { _flag = value; }
        }

        public MDTickFac():base()
        {
            this.Func = 19;                        
            this.Flag = 'F';
            this.Nro = 0;            
        }
    }

    public class MDCcOnln : DRegTrans
    {
        private long _ctacte;
        public long Ctacte
        {
            get { return _ctacte; }
            set { _ctacte = value; }
        }
        private short _flagprec;
        public short Flagprec
        {
            get { return _flagprec; }
            set { _flagprec = value; }
        }
        public MDCcOnln()
            : base()
        {
            this.Ctacte = 0;
            this.Flagprec = 0;
        }
    }

    public class MDPromo : DRegTrans
    {
        private short _nro;
        public short Nro
        {
            get { return _nro; }
            set { _nro = value; }
        }
        private int _cod;
        public int Cod
        {
            get { return _cod; }
            set { _cod = value; }
        }
        private long _codpromo;
        public long Codpromo
        {
            get { return _codpromo; }
            set { _codpromo = value; }
        }
        private short _tpremio;
        public short Tpremio
        {
            get { return _tpremio; }
            set { _tpremio = value; }
        }
        private char _imprime;
        public char Imprime
        {
            get { return _imprime; }
            set { _imprime = value; }
        }
        private short _linea;
        public short Linea
        {
            get { return _linea; }
            set { _linea = value; }
        }
        private string _p1;
        public string P1
        {
            get { return _p1; }
            set { _p1 = value; }
        }
        private string _p2;
        public string P2
        {
            get { return _p2; }
            set { _p2 = value; }
        }
        private string _p3;
        public string P3
        {
            get { return _p3; }
            set { _p3 = value; }
        }
        private string _cantpromo;
        public string Cantpromo
        {
            get { return _cantpromo; }
            set { _cantpromo = value; }
        }
        private string _preciouni;
        public string Preciouni
        {
            get { return _preciouni; }
            set { _preciouni = value; }
        }

        public MDPromo()
            : base()
        {
            this.Nro = 0;
            this.Cod = 0;
            this.Codpromo = 0;
            this.Tpremio = 0;
            this.Imprime = 'F';
            this.Linea = 0;
            this.P1 = this.P2 = this.P3 = string.Empty;
            this.Cantpromo = this.Preciouni = "0.00";
        }
    }

    public class MDPercep : DRegTrans
    {        
        public string Percep { get; set; }        
        
        public string Montoneto { get; set; }        
        
        public string Alicuota { get; set; }        

        public MDPercep()
            : base()
        {
            this.Montoneto = this.Percep = this.Alicuota = "0.00";            
        }
    }

    public class MDPerTissh : DRegTrans
    {
        public string Percep { get; set; }

        public string Montoneto { get; set; }

        public string Alicuota { get; set; }

        public MDPerTissh()
            : base()
        {
            this.Montoneto = this.Percep = this.Alicuota = "0.00";
        }
    }

    public class MDPerIva : DRegTrans
    {
        public string Percep { get; set; }

        public string Montoneto { get; set; }

        public string Alicuota { get; set; }

        public MDPerIva()
            : base()
        {
            this.Montoneto = this.Percep = this.Alicuota = "0.00";
        }
    }

    public class MDVendMay : DRegTrans
    {
        public int NroVendedor { get; set; }

        public MDVendMay()
            : base()
        {
            this.NroVendedor = 0;
        }
    }

    /// ////////////////////////////////////////////////////////////////////////////////////////////
    

    public class MDEnvas : DRegTrans
    {
        private uint _cod;
        public uint Cod
        {
            get { return _cod; }
            set { _cod = value; }
        }
        private short _cant;
        public short Cant
        {
            get { return _cant; }
            set { _cant = value; }
        }
        private char _flag;
        public char Flag
        {
            get { return _flag; }
            set { _flag = value; }
        }
        public MDEnvas()
            : base()
        {
            this.Cod = 0;
            this.Cant = 0;
            this.Flag = 'F';
        }
    }

}

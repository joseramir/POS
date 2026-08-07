using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Tax
    {
        public string id { get; set; }
        public string name { get; set; }
        public double value { get; set; }
    }

    public class ItemJanis
    {
        public int id { get; set; }
        public double quantity { get; set; }
        public double price { get; set; }
        public List<Tax> taxes { get; set; }
    }

    public class TotalTax
    {
        public string id { get; set; }
        public string name { get; set; }
        public double value { get; set; }
    }

    public class Invoice
    {
        public string invoiceNumber { get; set; }
        public double invoiceValue { get; set; }
        //public string invoiceUrl { get; set; }
        public string invoiceDate { get; set; }
        //public long trackingNumber { get; set; }
        //public string trackingUrl { get; set; }
        public List<ItemJanis> items { get; set; }
        public List<TotalTax> totalTaxes { get; set; }

        public Invoice()
        {
            invoiceNumber = "";
            invoiceValue = 0;
            //invoiceUrl = "";
            invoiceDate = "";
            //trackingNumber = 0;
            //trackingUrl = "";
            items = new List<ItemJanis>();
            totalTaxes = new List<TotalTax>();
        }
    }
}

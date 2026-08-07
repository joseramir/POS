using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Pedido
    {
        public int id { get; set; }
        public EcommerceAccount ecommerceAccount { get; set; }
        public string seqId { get; set; }
        public string ecommerceId { get; set; }
        public string cartId { get; set; }
        public string salesChannel { get; set; }
        public string store { get; set; }
        public Status status { get; set; }
        public double value { get; set; }
        public string creationDate { get; set; }
        public Totals totals { get; set; }
        public StoreData storeData { get; set; }
        public List<Item> items { get; set; }
        public Client client { get; set; }
        public Marketing marketing { get; set; }
        public List<Shipping> shipping { get; set; }
        public object route { get; set; }
        public List<Payment> payments { get; set; }
        public InvoiceData invoiceData { get; set; }
        public object clientObservation { get; set; }
        public string pickingToken { get; set; }
        public bool inAudit { get; set; }
    }
}

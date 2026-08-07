using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Payment
    {
        public string id { get; set; }
        public string transactionId { get; set; }
        public string paymentSystem { get; set; }
        public object paymentSystemRefId { get; set; }
        public string paymentSystemName { get; set; }
        public string paymentGroup { get; set; }
        public double value { get; set; }
        public int installments { get; set; }
        public double referenceValue { get; set; }
        public PaymentMethodExtraInfo paymentMethodExtraInfo { get; set; }
        public List<object> paymentMethodCustomFields { get; set; }
        public bool isCash { get; set; }
    }
}

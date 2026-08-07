using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Logistic
    {
        public Carrier carrier { get; set; }
        public int shippingEstimate { get; set; }
        public string shippingDate { get; set; }
        public DateTime? shippingWindowStart { get; set; }
        public DateTime? shippingWindowEnd { get; set; }
    }
}

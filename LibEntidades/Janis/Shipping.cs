using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Shipping
    {
        public string id { get; set; }
        public bool main { get; set; }
        public Address address { get; set; }
        public Logistic logistic { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Carrier
    {
        public string id { get; set; }
        public string name { get; set; }
        public string shippingType { get; set; }
        public object group { get; set; }
        public object company { get; set; }
    }
}

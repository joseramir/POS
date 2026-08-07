using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Address
    {
        public ReceiverData receiverData { get; set; }
        public object postalCode { get; set; }
        public string city { get; set; }
        public string state { get; set; }
        public string country { get; set; }
        public string street { get; set; }
        public string number { get; set; }
        public string neighborhood { get; set; }
        public string complement { get; set; }
        public object reference { get; set; }
        public double? lat { get; set; }
        public double? lng { get; set; }
    }
}

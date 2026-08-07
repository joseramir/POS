using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Promotion
    {
        public int id { get; set; }
        public string refId { get; set; }
        public string name { get; set; }
        public string description { get; set; }
        public double value { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Warehouse
    {
        public List<Bought> bought { get; set; }
        public List<Picked> picked { get; set; }
    }
}

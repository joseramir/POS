using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class RtaReqPedido
    {
        public int code { get; set; }
        public object message { get; set; }
        public Pedido data { get; set; }
    }
}

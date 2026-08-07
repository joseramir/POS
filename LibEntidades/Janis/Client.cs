using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Client
    {
        public int id { get; set; }
        public object ecommerceId { get; set; }
        public string email { get; set; }
        public string firstName { get; set; }
        public string lastName { get; set; }
        public string documentType { get; set; }
        public string document { get; set; }
        public string birthdate { get; set; }
        public object gender { get; set; }
        public string phone { get; set; }
        public object corporate { get; set; }
        public object groups { get; set; }
        public bool isEmployee { get; set; }
    }
}

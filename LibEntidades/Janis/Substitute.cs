using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class Subsitute
    {
        public int id { get; set; }
        public string refId { get; set; }
        public string name { get; set; }
        public string image { get; set; }
        public Sku sku { get; set; }
        public Product product { get; set; }
        public Brand brand { get; set; }
        public Category category { get; set; }
        public string ean { get; set; }
        public Quantity quantity { get; set; }
        public Prices prices { get; set; }
        public object warehouse { get; set; }
        public List<Weighable> weighables { get; set; }
        public object components { get; set; }
        public int tax { get; set; }
        public object seal { get; set; }
        public object basket { get; set; }
        public object picker { get; set; }
        public string measurementUnit { get; set; }
        public double unitMultiplier { get; set; }
        public List<Promotion> promotions { get; set; }
        public string commercialCondition { get; set; }
        public object shippingId { get; set; }
        public bool isFractionable { get; set; }
        public bool isFresh { get; set; }
        public bool isFrozen { get; set; }
        public bool isWeighable { get; set; }
        public object note { get; set; }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LibEntidades.Janis
{
    public class PaymentMethodExtraInfo
    {
        public object cardHolder { get; set; }
        public object cardNumber { get; set; }
        public object firstDigits { get; set; }
        public object lastDigits { get; set; }
        public object cvv2 { get; set; }
        public object expirationMonth { get; set; }
        public object expirationYear { get; set; }
        public object ccBank { get; set; }
        public object ccBrand { get; set; }
        public object ccType { get; set; }
        public object ccLevel { get; set; }
        public object authId { get; set; }
        public DateTime? authDate { get; set; }
        public object tid { get; set; }
        public object terminal { get; set; }
        public object authorizationCode { get; set; }
        public object coupon { get; set; }
        public object batchNumber { get; set; }
        public object merchantId { get; set; }
        public object traceNumber { get; set; }
        public object ipgTransactionId { get; set; }
        public object connectorResponses { get; set; }
    }
}

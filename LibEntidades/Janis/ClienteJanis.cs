using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using RestSharp;

namespace LibEntidades.Janis
{
    public class ClienteJanis
    {
        private string janis_client = "alberdisa";
        private string janis_api_key = "e8ceccf594d1659ba256bd0b7dd7f9feaa1fae3f";
        private string janis_api_secret = "c23be2e3e0ccf30a142b81dddba84a5516cf342a3764336552fd88ac2a3cd830";

        public ClienteJanis() { }

        public PedidoPos JanisGetOrden(string numpedido, string tienda)
        {
            PedidoPos pp = null;
            try
            {
                System.Net.ServicePointManager.SecurityProtocol = (System.Net.SecurityProtocolType)3072;// System.Net.SecurityProtocolType.Tls12;
                //RestClient cliente = new RestClient("https://janis.in/api/order/get?seqId=" + numpedido + "&status=picked");
                RestClient cliente = new RestClient("https://janis.in/api/order/get?seqId=" + numpedido);
                RestRequest request = new RestRequest(Method.GET);

                request.AddHeader("Content-Type", "application/json");
                request.AddHeader("Janis-Client", janis_client);
                request.AddHeader("Janis-API-Key", janis_api_key);
                request.AddHeader("Janis-API-Secret", janis_api_secret);

                IRestResponse response = cliente.Execute(request);
                if (response.StatusCode == System.Net.HttpStatusCode.OK)
                {
                    RtaReqPedido rep = Newtonsoft.Json.JsonConvert.DeserializeObject<RtaReqPedido>(response.Content.ToString());
                    //jsonResult = response.Content.ToString();
                    pp = new PedidoPos();

                    if (rep != null)
                    {
                        PedidoPos posped = new PedidoPos();
                        posped.SeqId = Convert.ToInt32(rep.data.seqId);
                        //posped.IdNumTie = Convert.ToInt16(ConfigurationManager.AppSettings["idsucursal"]);
                        //posped.Tienda = request[2];
                        //posped.Caja = Convert.ToInt16(request[3]);
                        //posped.Cajero = Convert.ToInt16(request[4]);
                        posped.FhOpera = rep.data.payments[0].paymentMethodExtraInfo.authDate == null ? new DateTime(2001, 1, 1) : (DateTime) rep.data.payments[0].paymentMethodExtraInfo.authDate;
                        posped.Estado = 1;
                        posped.Doc = rep.data.client.document;
                        posped.NomCli = rep.data.client.lastName + " " + rep.data.client.firstName;
                        posped.DirCli = rep.data.shipping[0].address.street + " " + rep.data.shipping[0].address.state;
                        posped.Total = Convert.ToDecimal(rep.data.totals.total);
                        posped.FhCreado = Convert.ToDateTime(rep.data.creationDate);
                        posped.FhEntrega = DateTime.Now;
                        posped.Sync = 0;
                        posped.items = new List<ItemPos>();
                        
                        //recorre el JSON para obtener los articulos que se ficharon en el pedido
                        foreach (Item item in rep.data.items)
                        {
                            procesarItem(ref posped, item);
                            //para los substitutos
                            if (item.subsitutes != null)
                                procesarSustituto(ref posped, item);
                        }
                        //Agrega el material para facturar el envio a Domicilio
                        if (rep.data.totals.shipping > 0)
                            posped.items.Add(new ItemPos()
                            {
                                Cod = 31199, //Convert.ToInt32(ConfigurationManager.AppSettings["codenviodom"]),
                                Cantidad = 1,
                                Precio = Convert.ToDecimal(rep.data.totals.shipping),
                                Id = 0
                            });
                        //Agrega los intereses con el Codigo del Recargo Financiero
                        foreach (Payment item in rep.data.payments)
                        {
                            double interes = item.value - item.referenceValue;
                            if (interes > 0.01)
                                posped.items.Add(new ItemPos()
                                {
                                    Cod = 99997,
                                    Cantidad = 1,
                                    EsPesable = false,
                                    Id = 0,
                                    Precio = Convert.ToDecimal(interes)
                                });
                        }
                        //Agrega las promociones o sea los descuentos
                        foreach (Item item in rep.data.items)
                        {
                            if (item.promotions != null)
                                procesaPromocion(ref posped, item);
                            if (item.subsitutes != null)
                            {
                                foreach (Subsitute sitem in item.subsitutes)
                                {
                                    if (sitem.promotions != null)
                                        procesaPromocionSustituto(ref posped, sitem);
                                }
                            }

                        }
                        pp = posped;
                    }
                }
            }
            catch (System.Exception ex)
            {
                pp = null;
            }
            return pp;
        }

        private void procesarItem(ref PedidoPos posped, Item item)
        {
            ItemPos nitem = buscaCodEnTicket(posped, Convert.ToInt32(item.refId));
            if (nitem == null)
            {
                nitem = new ItemPos();
                nitem.Cod = Convert.ToInt32(item.refId);
                if (item.isWeighable)
                {
                    nitem.EsPesable = true;
                    if (item.weighables != null)
                    {
                        foreach (var pesable in item.weighables)
                        {
                            nitem.Cantidad += Convert.ToDecimal(pesable.weight) / 1000;
                            nitem.Precio += Convert.ToDecimal(item.prices.price);
                        }
                    }
                }
                else
                {
                    nitem.Cantidad = Convert.ToDecimal(item.quantity.picked);
                    nitem.Precio = Convert.ToDecimal(item.prices.price);
                }
                if (nitem.Cantidad > 0)
                {
                    nitem.Id = item.id;
                    posped.items.Add(nitem);
                }
            }
            else
            {
                if (item.isWeighable)
                {
                    if (item.weighables != null)
                    {
                        foreach (var pesable in item.weighables)
                        {
                            nitem.Cantidad += Convert.ToDecimal(pesable.weight) / 1000;
                            //nitem.Precio += Convert.ToDecimal(item.prices.price);
                        }
                    }
                }
                else
                    if (item.quantity.picked > 0)
                        nitem.Cantidad += Convert.ToDecimal(item.quantity.picked);
            }

            //if (item.promotions == null && !item.isWeighable)
            //    nitem.Precio = Convert.ToDecimal(item.prices.sellingPrice);
        }

        private void procesarSustituto(ref PedidoPos posped, Item item)
        {
            foreach (var sitem in item.subsitutes)
            {
                ItemPos snitem = buscaCodEnTicket(posped, Convert.ToInt32(sitem.refId));
                //no encuentra el articulo, lo agrega al final
                if (snitem == null)
                {
                    snitem = new ItemPos();
                    snitem.Cod = Convert.ToInt32(sitem.refId);
                    if (sitem.isWeighable)
                    {
                        snitem.EsPesable = true;
                        if (sitem.weighables != null)
                        {
                            foreach (var spesable in sitem.weighables)
                            {
                                snitem.Cantidad += Convert.ToDecimal(spesable.weight) / 1000;
                                snitem.Precio += Convert.ToDecimal(sitem.prices.price);
                            }
                        }
                    }
                    else
                    {
                        snitem.Cantidad = Convert.ToDecimal(sitem.quantity.picked);
                        snitem.Precio = Convert.ToDecimal(sitem.prices.price);
                    }
                    if (snitem.Cantidad > 0)
                    {
                        snitem.Id = sitem.id;
                        posped.items.Add(snitem);
                    }
                }
                else//lo encuentra, actualiza la cantidad
                {
                    if (sitem.isWeighable)
                    {
                        if (sitem.weighables != null)
                        {
                            foreach (var spesable in sitem.weighables)
                            {
                                snitem.Cantidad += Convert.ToDecimal(spesable.weight) / 1000;
                                //snitem.Precio += Convert.ToDecimal(sitem.prices.price);
                            }
                        }
                    }
                    else
                        if (sitem.quantity.picked > 0)
                            snitem.Cantidad += Convert.ToDecimal(sitem.quantity.picked);
                }
                //if (sitem.promotions == null && !sitem.isWeighable)
                //    snitem.Precio = Convert.ToDecimal(sitem.prices.sellingPrice);
            }
        }

        public void procesaPromocion(ref PedidoPos posped, Item item)
        {
            foreach (var pitem in item.promotions)
            {
                int auxdescarga = 0;
                int.TryParse(pitem.description, out auxdescarga);
                //Si no encuentra la descarga definida en la promocion de VTEX pone una generica
                if (auxdescarga == 0)
                    auxdescarga = 30064;

                ItemPos nitem = buscaCodEnTicket(posped, auxdescarga);
                //No encuentra la descarga, la agrega
                if (nitem == null)
                {
                    nitem = new ItemPos();
                    nitem.Cod = auxdescarga;
                    nitem.Cantidad = 1;
                    if (item.isWeighable)
                        //nitem.Precio = calcularDesctoPesable(item);
                        nitem.Precio = Convert.ToDecimal(pitem.value);
                    else
                    {
                        if (item.quantity.picked > 0)
                        {
                            nitem.Precio = decimal.Round(Convert.ToDecimal((pitem.value / item.quantity.bought) * item.quantity.picked), 2);
                        }
                    }
                    if (nitem.Precio < 0)
                    {
                        nitem.Id = pitem.id;
                        posped.items.Add(nitem);
                    }
                }
                else
                {
                    if (item.isWeighable)
                        //nitem.Precio += calcularDesctoPesable(item);
                        nitem.Precio += Convert.ToDecimal(pitem.value);
                    else
                    {
                        if (item.quantity.picked > 0)
                            //nitem.Precio += Convert.ToDecimal(pitem.value);
                            nitem.Precio += decimal.Round(Convert.ToDecimal((pitem.value / item.quantity.bought) * item.quantity.picked), 2);
                    }
                }
            }
        }

        public void procesaPromocionSustituto(ref PedidoPos posped, Subsitute item)
        {
            foreach (var pitem in item.promotions)
            {
                int auxdescarga = 0;
                int.TryParse(pitem.description, out auxdescarga);
                //Si no encuentra la descarga definida en la promocion de VTEX pone una generica
                if (auxdescarga == 0)
                    auxdescarga = 30064;

                ItemPos nitem = buscaCodEnTicket(posped, auxdescarga);
                //No encuentra la descarga, la agrega
                if (nitem == null)
                {
                    nitem = new ItemPos();
                    nitem.Cod = auxdescarga;
                    nitem.Cantidad = 1;
                    if (item.isWeighable)
                        //nitem.Precio = calcularDesctoPesableSus(item);
                        nitem.Precio = Convert.ToDecimal(pitem.value);
                    else
                    {
                        if (item.quantity.picked > 0)
                            nitem.Precio = decimal.Round(Convert.ToDecimal((pitem.value / item.quantity.bought) * item.quantity.picked), 2);
                    }
                    if (nitem.Precio < 0)
                    {
                        nitem.Id = pitem.id;
                        posped.items.Add(nitem);
                    }
                }
                else
                {
                    if (item.isWeighable)
                        //nitem.Precio += calcularDesctoPesableSus(item);
                        nitem.Precio += Convert.ToDecimal(pitem.value);
                    else
                    {
                        if (item.quantity.picked > 0)
                            nitem.Precio += Convert.ToDecimal(pitem.value);
                    }
                }
            }
        }

        private decimal calcularDesctoPesable(Item item)
        {
            decimal wmonto = 0;
            decimal wcantidad = 0;
            decimal Descto = 0;
            if (item.weighables != null)
                foreach (var witem in item.weighables)
                {
                    wmonto += Convert.ToDecimal(witem.price);
                    wcantidad += Convert.ToDecimal(witem.weight) / 1000;
                }
            if (wcantidad > 0)
            {
                Descto = (Convert.ToDecimal(item.prices.price) * wcantidad) - wmonto;
                if (Descto > 0)
                    Descto = decimal.Round(Descto, 2) * (-1);
            }
            return Descto;
        }

        private decimal calcularDesctoPesableSus(Subsitute item)
        {
            decimal wmonto = 0;
            decimal wcantidad = 0;
            decimal Descto = 0;
            if (item.weighables != null)
                foreach (var witem in item.weighables)
                {
                    wmonto += Convert.ToDecimal(witem.price);
                    wcantidad += Convert.ToDecimal(witem.weight) / 1000;
                }
            if (wcantidad > 0)
            {
                Descto = (Convert.ToDecimal(item.prices.price) * wcantidad) - wmonto;
                if (Descto > 0)
                    Descto = decimal.Round(Descto, 2) * (-1);
            }
            return Descto;
        }

        private ItemPos buscaCodEnTicket(PedidoPos posped, int codigo)
        {
            ItemPos nitem = null;
            for (int i = 0; i < posped.items.Count; i++)
            {
                if (posped.items[i].Cod == codigo)
                    nitem = posped.items[i];
            }
            return nitem;
        }
    }
}

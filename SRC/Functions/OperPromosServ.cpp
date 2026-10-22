#include "STDAFX.H"
#include "FUNCS.H"
#include "GetPromoLine.h"
#include "frmIngNumPedido.h"
#include "frmIngNumParking.h"

//#include "promo.h"

//extern int CargaPromosFiel();

void ing_codpromo(int cual)
{
	if (consmode || rMode)
		return;
	if (ClienteBenef < 1 && ClienteBenef > 12)
	{
		Alert("No se puede usar Voucher de Descuento", "Solo habilitado para Fan Club");
		return;
	}
	GetPromoLine ^fpromoline = gcnew GetPromoLine();
	if (fpromoline->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		if (Dump::promosfiel != nullptr && Dump::promosfiel->Count > 0)
		{			
			CargaPromosExtra();
			Dump::promosfiel->Clear();
		}
	}
}


void infCajaLibre(int cual)
{
	if (ckaNro != 0 )
	{
		if (inOper == 0)
			Controles::OperarServidor::InformarCajaLibre(caja, gcnew String(ipServCL), portCL, ComPortCL + 1, SegsLuzCL);
		else
			Alert("No se puede llamar en medio de un ticket", "");
	}
	else
		Alert("No hay cajero habilitado", "");
}

void ObtenerPedJanis(int cual)
{
	if (ckaNro != 0 )
	{
		if (inOper == 0)
		{		
			if (janisEcom)
			{		
				frmIngNumPedido ^fnumpedido = gcnew frmIngNumPedido(false);
				fnumpedido->ShowDialog();
				if (fnumpedido->DialogResult == DialogResult::OK) 
				{				
					NumPedEcommerce = fnumpedido->NumPedido == "" ? 0 : System::Convert::ToInt64(fnumpedido->NumPedido);
					delete fnumpedido;
					
					Dump::PedidoJanis = Controles::OperarServidor::JanisGetOrden(System::Convert::ToString(NumPedEcommerce), gcnew String(saptienda),caja, ckaNro);
					//LibEntidades::Janis::ClienteJanis^ cliente = gcnew LibEntidades::Janis::ClienteJanis();
					//Dump::PedidoJanis = cliente->JanisGetOrden(System::Convert::ToString(NumPedEcommerce), gcnew String(saptienda));
					if (Dump::PedidoJanis != nullptr)
					{		
						for (int i = 0; i < Dump::PedidoJanis->items->Count; i++)
						{
							Controles::ItemPos^ item = Dump::PedidoJanis->items[i];
							//LibEntidades::Janis::ItemPos^ item = Dump::PedidoJanis->items[i];
							if (item->Precio > 0 && item->Cantidad > 0)
							{						
								xReg = System::Convert::ToDouble(item->Cod);
								if (item->EsPesable)
									yReg = System::Convert::ToDouble(item->Cantidad * item->Precio);
								else
									yReg = System::Convert::ToDouble(item->Cantidad);
								xPrecEcom = System::Convert::ToDouble(item->Precio) + 2e-10;
								xsJanisEsPesable = item->EsPesable;
								Strings::StringToChar(item->Precio.ToString("F2"), xsPrecEcom, 30);
								plu(0);
							}
						}
					}
					else
						Alert("No encuentro el pedido indicado", "Verificar numero y que estado sea pickeado");
				}
			}
			else
				Alert("No se puede llamar en medio de un ticket", "");
		}
		else
			Alert("Atencion!! no esta habilitada la opcion", "Para Ecommerce con Janis");
	}
	else
		Alert("No hay cajero habilitado", "");
}

void IngIdParking(int cual)
{
	//   idParking NO se limpia aca: si el ticket ya esta abierto con una estadia, volver a tocar la
	//   tecla borraba el id y al cerrar no se mandaba el POST a Parking. Se limpia recien cuando se
	//   va a pedir un id nuevo.
	if (ckaNro != 0)
	{
		if (inOper == 0)
		{
			char msgConsu[200];
			if (procesaParking)
			{
				//   Antes de operar con el estacionamiento se verifica que la aplicacion de Parking
				//   tenga hecho su cierre ('autorizado' en true). Si todavia no lo hizo, no se opera.
				//   Si no se puede consultar el endpoint se deja operar igual (para no dejar la caja
				//   parada por una falla del servicio), pero queda registrado en el log.
				validadoCierreEstacionamiento = false;
				LibEntidades::Alberdi::Parking::ClienteParking^ cliParking = gcnew LibEntidades::Alberdi::Parking::ClienteParking();
				LibEntidades::Alberdi::Parking::AutorizacionCierre^ consuCierre = cliParking->ConsultaCierreRealizado();
				bool cierreRealizado = true;

				msgConsu[0] = '\0';
				if (consuCierre == nullptr)
					WLog("PARKING - No se pudo consultar el cierre de Estacionamiento; se permite operar");
				else if (!consuCierre->autorizado)
				{
					// Falta el cierre en la aplicacion de Parking: no se puede operar.
					cierreRealizado = false;
					if (consuCierre->mensaje != nullptr)
						Strings::StringToChar(consuCierre->mensaje, msgConsu, 200);
					else
						strcpy(msgConsu, "Falta realizar el cierre en Estacionamiento");
					WLog("PARKING - Cierre de Estacionamiento sin realizar, no se opera. Msg='%s'", msgConsu);
				}
				else
				{
					validadoCierreEstacionamiento = true;		// Ya verificado, no reconsultar en este ticket.
					if (consuCierre->mensaje != nullptr)
						Strings::StringToChar(consuCierre->mensaje, msgConsu, 200);
				}

				delete consuCierre;

				if (!cierreRealizado)
				{
					AlertDosLineas(msgConsu);
					return;
				}
				if (msgConsu[0])
					AlertDosLineas(msgConsu);

				idParking = 0;
				frmIngNumParking^ fnumpedido = gcnew frmIngNumParking();
				fnumpedido->ShowDialog();
				if (fnumpedido->DialogResult == DialogResult::OK)
				{
					idParking = fnumpedido->NumIdParking == "" ? 0 : System::Convert::ToInt32(fnumpedido->NumIdParking);
					delete fnumpedido;

					LibEntidades::Alberdi::Parking::DatosParking ^datosP = cliParking->GetDatosParking(idParking);
					//LibEntidades::Alberdi::Parking::ListParking^ listaP = cliParking->GetDatosParking(NumIdParking);
					//   Si no se pudo traer la estadia, el id vuelve a 0: si no, quedaba cargado sin item y un
					//   ticket posterior cargado a mano hacia el POST a Parking con esa estadia.
					if (cliParking->ConError)
					{
						idParking = 0;
						Alert("Error al obtener datos de Estacionamiento", "");
						return;
					}

					if (datosP != nullptr)
					{
						if (datosP->PrecioUnitario > 0 && datosP->Cantidad > 0)
						{
							xReg = System::Convert::ToDouble(datosP->Codigo);
							yReg = System::Convert::ToDouble(datosP->Cantidad);
							xPrecEcom = System::Convert::ToDouble(datosP->PrecioUnitario) + 2e-10;
							//xsJanisEsPesable = item->EsPesable;
							Strings::StringToChar(datosP->PrecioUnitario.ToString("F2"), xsPrecEcom, 30);
							plu(0);
							//   El precio de Parking es solo para este item: si quedaba cargado, los articulos
							//   que se fichaban despues en el mismo ticket salian al precio de la estadia.
							xPrecEcom = 0;
							xsPrecEcom[0] = '\0';
							if (inOper == 0)		// plu no abrio el ticket: la estadia no quedo cargada.
								idParking = 0;
						}
						else
							idParking = 0;
					}
					else
					{
						idParking = 0;
						Alert("No encuentro el ID Estacionamiento", "Verificar numero y que este listo para cobrar");
					}
				}
			}
			else
				Alert("Opcion de Estacionamiento", "No habilitado");
		}
		else
			Alert("Atencion!! no esta habilitada la opcion", "Para Estacionamiento");
	}
	else
		Alert("No hay cajero habilitado", "");
}
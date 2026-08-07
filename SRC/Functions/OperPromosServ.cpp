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
	idParking = 0;
	if (ckaNro != 0)
	{
		if (inOper == 0)
		{
			if (procesaParking)
			{
				frmIngNumParking^ fnumpedido = gcnew frmIngNumParking();
				fnumpedido->ShowDialog();
				if (fnumpedido->DialogResult == DialogResult::OK)
				{
					idParking = fnumpedido->NumIdParking == "" ? 0 : System::Convert::ToInt32(fnumpedido->NumIdParking);
					delete fnumpedido;
					LibEntidades::Alberdi::Parking::ClienteParking^ cliParking = gcnew LibEntidades::Alberdi::Parking::ClienteParking();
					LibEntidades::Alberdi::Parking::DatosParking ^datosP = cliParking->GetDatosParking(idParking);
					//LibEntidades::Alberdi::Parking::ListParking^ listaP = cliParking->GetDatosParking(NumIdParking);
					if (cliParking->ConError)
					{
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
						}						
					}
					else
						Alert("No encuentro el ID Estacionamiento", "Verificar numero y que este listo para cobrar");
				}
			}
			else
				Alert("No se puede llamar en medio de un ticket", "");
		}
		else
			Alert("Atencion!! no esta habilitada la opcion", "Para Estacionamiento");
	}
	else
		Alert("No hay cajero habilitado", "");
}
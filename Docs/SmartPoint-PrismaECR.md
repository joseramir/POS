# Integraciones de pago: SmartPoint (Mercado Pago) y PrismaECR

Documentación técnica de las dos integraciones de cobro con tarjeta vía terminal
electrónica que usa el POS. Generada a partir de una lectura completa del código fuente
(2026-07-29). Los números de línea citados corresponden al estado del repo en esa fecha;
pueden desactualizarse con el tiempo.

## Patrón común a ambas integraciones

El POS (C++/CLI) **no habla directo con el gateway de pago** (Mercado Pago / Payway). En los
dos casos habla vía REST/JSON contra una **Web API intermedia propia**, corriendo en la red
local, que a su vez integra con el gateway real. Esto explica varias cosas compartidas:

- Transporte: **RestSharp** (`RestClient` / `RestRequest` / `IRestResponse`).
- Serialización: **Newtonsoft.Json** (`JsonConvert.SerializeObject` / `DeserializeObject`),
  con `NullValueHandling.Ignore` en los POST con body. Los DTOs son POCOs planos, sin
  atributos `[JsonProperty]`/`[DataMember]` — el nombre del campo JSON es el nombre de la
  propiedad C# tal cual (con mayúscula inicial).
- Headers: únicamente `Content-Type: application/json`. **Ninguna de las dos integraciones
  usa autenticación HTTP** (sin API key, sin token, sin Basic Auth) contra su Web API local.
- Host/puerto: config en `app.config` (y `bin\posXP.exe.config`), leído vía
  `ConfigurationManager.AppSettings`. La URL completa se arma por concatenación manual de
  strings en cada método (no hay un builder de URI centralizado).
- Patrón crear + poolear: la creación devuelve un ID de operación; un formulario modal
  (`MsgAlertSmartPointMp` / `MsgAlertPrismaECR`) lanza un hilo de fondo que consulta el estado
  cada **2000 ms**, con un límite de reintentos (~2 a 4 minutos según el caso). El operador
  puede cancelar apretando **Escape** mientras el polling está activo.
- Logging: cada integración escribe su propio log de texto plano (`LogSmartPoint.txt` /
  `LogPrismaECR.txt`) con el JSON enviado y recibido en crudo — útil para ver payloads
  reales de producción.

---

## 1. SmartPoint (Mercado Pago)

### 1.1 Configuración y transporte

`app.config` (líneas 61-62):

```
<add key="ipsmartpointmp" value="192.168.0.13"/>
<add key="portsmartpointmp" value="5024"/>
```

URL base: `http://{ipsmartpointmp}:{portsmartpointmp}/api/alberdimp/...` (siempre HTTP, sin
HTTPS). Clase cliente: `OperaWebSmart`
(`TJOCommon/ComunicaMercadoPago/OperaWebSmart.cs`).

Archivos C++/CLI orquestadores:
- `SRC/Forms/frmTarjSmartPointMp.h` / `.cpp` — cobro con tarjeta.
- `SRC/Forms/frmAnulaSmartPoint.h` / `.cpp` — pantalla de anulación (pide el
  External Reference).
- `SRC/Functions/TjOnline.cpp` (función `AnularTarjetaSmartPoint`, líneas 1083-1139).
- `SRC/Functions/MPAGO.CPP` (líneas 1805-1836) — integra el pago ya aprobado al ticket
  fiscal (medio de pago fijo = **49**), sin llamadas HTTP adicionales en ese paso.

### 1.2 Estructuras (DTOs)

**`OrdenMp`** (base, `TJOCommon/ComunicaMercadoPago/OrdenMp.cs:8-17`):

| Campo | Tipo |
|---|---|
| Tienda | string |
| Caja | int |
| Cajero | int |
| Monto | decimal |
| Cuotas | int |
| PtoVta | int |
| NroTicket | int |

**`OrdenCaja : OrdenMp`** (request de creación,
`TJOCommon/ComunicaMercadoPago/OrdenCaja.cs:8-17`) agrega:

| Campo | Tipo | Notas |
|---|---|---|
| AlbCodTarjeta | int | |
| TipoTarjeta | string | valores usados: `"credit_card"`, `"debit_card"`, `"qr"` |

**`RtaOrdenCaja : OrdenCaja`** (response de crear/consultar/cancelar/devolver,
`TJOCommon/ComunicaMercadoPago/RtaOrdenCaja.cs:8-18`) agrega:

| Campo | Tipo | Notas |
|---|---|---|
| IdHdr | int | |
| FechaHora | DateTime | |
| ExternalReference | string | clave usada para poolear/anular |
| Estado | short | `2`=creada, `3`=aprobada, `5`=cancelada, `6`=falta aprobar, otro=fallo |
| IdOperacion | string | |
| Anulado | bool | |
| Reference | string | |
| IdOrdenMercadoPago | string | |

### 1.3 Crear orden (cobro)

**Método:** `OperaWebSmart.CrearOrdenSmartPoint(OrdenCaja peticion)` (`OperaWebSmart.cs:22-70`)

`POST http://{ipsmartpointmp}:{portsmartpointmp}/api/alberdimp/OrdenCaja`

Request (`OrdenCaja`):
```json
{
  "Tienda": "CO05",
  "Caja": 3,
  "Cajero": 12,
  "Monto": 1578.90,
  "Cuotas": 1,
  "PtoVta": 1,
  "NroTicket": 165865,
  "AlbCodTarjeta": 4,
  "TipoTarjeta": "credit_card"
}
```

Response (`RtaOrdenCaja`):
```json
{
  "Tienda": "CO05",
  "Caja": 3,
  "Cajero": 12,
  "Monto": 1578.90,
  "Cuotas": 1,
  "PtoVta": 1,
  "NroTicket": 165865,
  "AlbCodTarjeta": 4,
  "TipoTarjeta": "credit_card",
  "IdHdr": 1024,
  "FechaHora": "2026-07-29T14:32:10",
  "ExternalReference": "SP-20260729-000123",
  "Estado": 2,
  "IdOperacion": "op-abc123",
  "Anulado": false,
  "Reference": "ref-xyz",
  "IdOrdenMercadoPago": null
}
```

Códigos HTTP: `200`=OK; `404`="No se encontro terminal para la caja y tienda indicada";
`500`="No se pudo crear la Orden en BD WebService"; otro=error genérico de conexión.

### 1.4 Poolear / consultar estado

Dos métodos usados en cascada por `MsgAlertSmartPointMp.ShowMsgProcesaVenta`
(`MsgAlertSmartPointMp.cs:99-201`):

**a) Contra el WebService local:**
`GET http://{ipsmartpointmp}:{portsmartpointmp}/api/alberdimp/ConsuCaja?external_reference={ExternalReference}`
(`OperaWebSmart.cs:72-127`) — sin body. Response: `RtaOrdenCaja`.
Se llama cada 2000 ms, hasta 60 intentos (~2 minutos).

**b) Directo contra Mercado Pago (a través del mismo WebService):**
`GET http://{ipsmartpointmp}:{portsmartpointmp}/api/alberdimp/ConsuMP?external_reference={ExternalReference}`
(`OperaWebSmart.cs:130-181`) — mismo formato. Si (a) agota los 60 intentos sin resolución,
se pasa a este, cada 2000 ms, hasta 120 intentos (~4 minutos).

Response de ambos: mismo JSON que la creación (`RtaOrdenCaja`), evaluando `Estado`
(`3`=aprobada, `6`=falta aprobar en la pantalla del SmartPoint, otro=fallo).

Código `404` con contenido `"PROCESANDO..."` = sigue en curso, no es error (continúa el
polling sin marcar `ConError`).

### 1.5 Cancelar orden (durante el polling, tecla Escape)

**Método:** `OperaWebSmart.CancelarOrden(RtaOrdenCaja peticion)` (`OperaWebSmart.cs:186-219`)

`POST http://{ipsmartpointmp}:{portsmartpointmp}/api/alberdimp/CancelarOrden`

Request: se reenvía el objeto **`RtaOrdenCaja` completo** que devolvió la creación (no
un DTO nuevo). Solo disponible mientras `Estado==2` (orden aún no cerrada) y el timer
`SePuedeCancelar` lo habilita. Response: `RtaOrdenCaja`; error HTTP => "No se pudo Cancelar.
Anular en la Terminal".

### 1.6 Anular (devolución de una operación ya aprobada)

**Método:** `OperaWebSmart.DevolverOrden(string external_reference)`
(`OperaWebSmart.cs:222-248`)

`GET http://{ipsmartpointmp}:{portsmartpointmp}/api/alberdimp/DevolverOrden?external_reference={ExternalReference}`

Sin body. Response: `RtaOrdenCaja` si `200`; si no, devuelve `null` (no informa
`MensajeError` en ese caso — inconsistencia respecto a los demás métodos).

Flujo: `frmAnulaSmartPoint` pide el **External Reference** por teclado ->
`TjOnline.cpp:AnularTarjetaSmartPoint` (líneas 1083-1139) crea un
`MsgAlertSmartPointMp` vacío y llama `ShowMsgDevuelveVenta(...)`, que internamente invoca
`DevolverOrden`. No graba un registro de anulación en la base local (a diferencia de
PrismaECR); hay código comentado en `TjOnline.cpp:1105-1133` que sugiere que se planeó
pero quedó deshabilitado.

### 1.7 Cierre de lote

**No implementado.** No existe ningún método de cierre/lote en `OperaWebSmart.cs` ni en
ningún `.cpp`/`.h` de `SRC` para SmartPoint (se buscó explícitamente `lote|batch|cierre`
en toda la carpeta). Es consistente con el modelo de Mercado Pago Point/QR, donde la
liquidación se gestiona del lado de Mercado Pago y no requiere cierre manual de lote como
las terminales tradicionales.

### 1.8 Resumen SmartPoint

| Operación | Método (archivo:línea) | HTTP | Endpoint | Request | Response |
|---|---|---|---|---|---|
| Crear orden | `CrearOrdenSmartPoint` (OperaWebSmart.cs:22) | POST | `/api/alberdimp/OrdenCaja` | `OrdenCaja` | `RtaOrdenCaja` |
| Consultar (local) | `ConsultarStatusOrden` (OperaWebSmart.cs:72) | GET | `/api/alberdimp/ConsuCaja?external_reference=` | - | `RtaOrdenCaja` |
| Consultar (Mercado Pago) | `ConsultarStatusOrdenMercadoPago` (OperaWebSmart.cs:130) | GET | `/api/alberdimp/ConsuMP?external_reference=` | - | `RtaOrdenCaja` |
| Cancelar (en creación) | `CancelarOrden` (OperaWebSmart.cs:186) | POST | `/api/alberdimp/CancelarOrden` | `RtaOrdenCaja` | `RtaOrdenCaja` |
| Anular (post-aprobación) | `DevolverOrden` (OperaWebSmart.cs:222) | GET | `/api/alberdimp/DevolverOrden?external_reference=` | - | `RtaOrdenCaja` o null |
| Cierre de lote | No implementado | - | - | - | - |

---

## 2. PrismaECR

### 2.1 Configuración y transporte

`app.config` (líneas 70-71):

```
<add key="ipapiprismaecr" value="192.168.0.13"/>
<add key="portapiprismaecr" value="5029"/>
```

URL base: `http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/...`. Clase cliente:
`OperaPrismaECR` (`TJOCommon/PrismaECR/OperaPrismaECR.cs`).

Archivos C++/CLI orquestadores:
- `SRC/Forms/frmPrismaECR.h` / `.cpp` — cobro con tarjeta.
- `SRC/Forms/frmAnulaPrismaECR.h` / `.cpp` — pide Lote/Cupón/Autorización.
- `SRC/Functions/TjOnline.cpp` — `AnularTarjetaPrismaEcr()` (líneas 1034-1080) y
  `CierreLotePrismaEcr()` (líneas 1141-1149).

### 2.2 Crear pago

**Método:** `OperaPrismaECR.CreaIntencionPago(PosPaymentRequest peticion)`
(`OperaPrismaECR.cs:22-77`)

`POST http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/pagos`

Request (`PosPaymentRequest`):

| Campo | Tipo | Notas |
|---|---|---|
| NumeroSucursal | int | |
| NumeroCaja | int | |
| NumeroCajero | string | |
| EcrTransactionId | string | número de ticket local; en la práctica se manda `""` (frmPrismaECR.h:547) |
| NumeroComprobanteFiscal | string | formateado `"{0:000000000000}"` desde `pf->GetTickNro(GlobalTF)` |
| PuntoVentaFiscal | string | `pf->ptovta` |
| MontoOperacion | decimal | en pesos con decimales |
| MontoExtracash | decimal | 0 si no aplica |
| NumeroCuotas | int | |
| MarcaTarjeta | string | `card_brand_product`: "VI", "MC", etc.; null = cualquier marca |
| NumeroComercio | string | **hardcodeado** a `"03659307"` en frmPrismaECR.h:555 (antes venía del catálogo de tarjetas, coment nó `mdatos->NroComercio`) |

```json
{
  "NumeroSucursal": 1,
  "NumeroCaja": 3,
  "NumeroCajero": "12",
  "EcrTransactionId": "",
  "NumeroComprobanteFiscal": "000000165865",
  "PuntoVentaFiscal": "1",
  "MontoOperacion": 1578.90,
  "MontoExtracash": 0,
  "NumeroCuotas": 1,
  "MarcaTarjeta": "VI",
  "NumeroComercio": "03659307"
}
```

Response (`PosPaymentResponse`):

| Campo | Tipo |
|---|---|
| IdOperacionPayway | string |
| Estado | string (`PENDIENTE`, `APROBADA`, `RECHAZADA`, `ANULADA`) |
| Mensaje | string |
| Monto | decimal? |
| MontoExtracash | decimal? |
| CodigoAutorizacion | string |
| NumeroLote | string |
| NumeroCupon | string |
| Prim6Digitos | string |
| Ult4Digitos | string |
| MarcaTarjeta | string |

```json
{
  "IdOperacionPayway": "a1b2c3d4-...",
  "Estado": "PENDIENTE",
  "Mensaje": null,
  "Monto": null,
  "MontoExtracash": null,
  "CodigoAutorizacion": null,
  "NumeroLote": null,
  "NumeroCupon": null,
  "Prim6Digitos": null,
  "Ult4Digitos": null,
  "MarcaTarjeta": null
}
```

### 2.3 Poolear / consultar pago

**Método:** `OperaPrismaECR.ConsultarStatusPago(string IdOperacionPayway)`
(`OperaPrismaECR.cs:80-131`)

`GET http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/pagos/{IdOperacionPayway}`

Sin body. Response: mismo `PosPaymentResponse`, ahora con `Estado` actualizado
(`APROBADA`/`RECHAZADA`/`ANULADA`/`PENDIENTE`) y los campos de cupón completos si fue
aprobado.

Orquestación (`MsgAlertPrismaECR.ShowMsgProcesaVenta`, líneas 89-188): usa el
`IdOperacionPayway` devuelto por la creación, consulta cada 2000 ms, `MAX_INTENTOS = 60`
(~2 minutos). Al agotar los intentos, o si el operador presiona Escape, llama
automáticamente a la cancelación (2.4).

### 2.4 Cancelar intención de pago (timeout / Escape)

**Método:** `OperaPrismaECR.CancelarIntencionPago(string IdOperacionPayway)`
(`OperaPrismaECR.cs:134-187`)

`PUT http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/pagos/{IdOperacionPayway}/cancelaciones`

Sin body. Response: `PosPaymentResponse`.

### 2.5 Anular (reversión de un pago ya aprobado)

Flujo de 3 llamadas: crear intención de anulación -> poolear -> (opcional) cancelar.

**a) Crear:** `OperaPrismaECR.CreaIntencionAnula(string claveBusq)`
(`OperaPrismaECR.cs:247-301`)

`POST http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/anulaciones/{claveBusq}`

No hay body JSON; el único dato es `claveBusq` en el path, armado en `TjOnline.cpp:1047`:

```
claveBusq = Lote (3 digitos, con ceros) + Cupon (4 digitos, con ceros) + Autorizacion (6 caracteres, mayusculas, con ceros a la izquierda)
```

Esos tres datos (Lote/Cupón/Autorización) se piden al operador en `frmAnulaPrismaECR`
(campos `txtLote` máx 3, `txtCupon` máx 4, `txtAutorizacion` máx 6).

Response (`PosAnulacionResponse`):

| Campo | Tipo | Notas |
|---|---|---|
| IdOperacionPayway | string | clave para poolear la anulación |
| ClaveBusq | string | |
| Estado | string (`ANULACION_PENDIENTE`, `PENDIENTE`, `ANULADA`, `RECHAZADA`) | |
| Mensaje | string | |
| Monto | decimal? | datos del pago original, para confirmar al operador antes de anular |
| CodigoAutorizacion | string | |
| NumeroLote | string | |
| NumeroCupon | string | |
| Prim6Digitos | string | |
| Ult4Digitos | string | |
| NumeroTerminalPrisma | string | |

```json
{
  "IdOperacionPayway": "e5f6g7h8-...",
  "ClaveBusq": "003016586ABC123",
  "Estado": "ANULACION_PENDIENTE",
  "Mensaje": null,
  "Monto": 1578.90,
  "CodigoAutorizacion": "ABC123",
  "NumeroLote": "003",
  "NumeroCupon": "0165",
  "Prim6Digitos": "450799",
  "Ult4Digitos": "1234",
  "NumeroTerminalPrisma": "12345678"
}
```

**b) Poolear:** `OperaPrismaECR.ConsultarStatusAnulacion(string idOperacionPayway)`
(`OperaPrismaECR.cs:304-355`)

`GET http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/anulaciones/{idOperacionPayway}/estado`

Usa el `IdOperacionPayway` **de la anulación** (no el del pago original). Cada 2000 ms,
`MAX_INTENTOS = 60` (~2 minutos, `MsgAlertPrismaECR.cs:221-264`). Response:
`PosAnulacionResponse`.

**c) Cancelar (timeout/Escape):** `OperaPrismaECR.CancelarIntencionAnulacion(string idOperacionPayway)`
(`OperaPrismaECR.cs:358-411`)

`PUT http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/anulaciones/{idOperacionPayway}/cancelaciones`

Sin body. Response: `PosAnulacionResponse`.

Flujo completo: `TjOnline.cpp:AnularTarjetaPrismaEcr()` (líneas 1034-1080). Si
`Estado=="ANULADA"`, además de avisar al operador graba localmente con
`GrabarAnulacionPrismaEcr(0)` (función local, sin HTTP).

**Nota:** existe `OperaPrismaECR.buscaTransaccionAnula(string claveBusq)`
(`OperaPrismaECR.cs:190-244`, `GET api/v1/pos/anulaciones/{claveBusq}` — sin barra inicial,
inconsistente) que devuelve `PosPaymentResponse` en vez de `PosAnulacionResponse`. Se
confirmó por búsqueda en todo el repo que **no se invoca desde ningún lado** — es
código muerto.

### 2.6 Cierre de lote

**a) Crear:** `OperaPrismaECR.CreaIntencionCierre(int numSucursal, int numCaja)`
(`OperaPrismaECR.cs:414-470`)

`POST http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/cierres`

Request (`PosCierreRequest`):
```json
{
  "NumeroSucursal": 1,
  "NumeroCaja": 3
}
```

Response (`PosCierreResponse`):

| Campo | Tipo | Notas |
|---|---|---|
| SettlementId | string | clave para poolear el cierre |
| Estado | string (`PENDIENTE` implícito, `CONFIRMADO`, `RECHAZADO`, `ERROR`) | |
| Mensaje | string | |
| NumeroLote | string | |
| CodigoRespuesta | string | |
| MensajeAdicional | string | |
| TicketCierre | string | `merchant_receipt_detail` de Payway |
| NumeroTerminal | string | |

```json
{
  "SettlementId": "s1t2t3l4-...",
  "Estado": "PENDIENTE",
  "Mensaje": null,
  "NumeroLote": null,
  "CodigoRespuesta": null,
  "MensajeAdicional": null,
  "TicketCierre": null,
  "NumeroTerminal": null
}
```

**b) Poolear:** `OperaPrismaECR.ConsultarStatusCierre(string idOperacionPayway)`
(`OperaPrismaECR.cs:473-524`)

`GET http://{ipapiprismaecr}:{portapiprismaecr}/api/v1/pos/cierres/{SettlementId}`

(El nombre del parámetro en el código dice `idOperacionPayway` pero en realidad se pasa el
`SettlementId` devuelto por la creación). Cada 2000 ms, `MAX_INTENTOS = 60` (~2 minutos,
`MsgAlertPrismaECR.cs:320-362`). **A diferencia de pago y anulación, si se agota el
timeout NO se llama a ningún endpoint de cancelación** (no existe
`CancelarIntencionCierre`). Response: `PosCierreResponse`.

Disparador: `TjOnline.cpp:CierreLotePrismaEcr()` (líneas 1141-1149). **Nota:** el bloque
que debería manejar el éxito (`if (!ConError)`) está vacío en el código actual — no
imprime `TicketCierre` ni graba nada localmente; parece incompleto.

### 2.7 Resumen PrismaECR

| Operación | Método (archivo:línea) | HTTP | Endpoint | Request | Response |
|---|---|---|---|---|---|
| Crear pago | `CreaIntencionPago` (OperaPrismaECR.cs:22) | POST | `/api/v1/pos/pagos` | `PosPaymentRequest` | `PosPaymentResponse` |
| Consultar pago | `ConsultarStatusPago` (OperaPrismaECR.cs:80) | GET | `/api/v1/pos/pagos/{IdOperacionPayway}` | - | `PosPaymentResponse` |
| Cancelar pago | `CancelarIntencionPago` (OperaPrismaECR.cs:134) | PUT | `/api/v1/pos/pagos/{IdOperacionPayway}/cancelaciones` | - | `PosPaymentResponse` |
| Crear anulación | `CreaIntencionAnula` (OperaPrismaECR.cs:247) | POST | `/api/v1/pos/anulaciones/{claveBusq}` | - (path) | `PosAnulacionResponse` |
| Consultar anulación | `ConsultarStatusAnulacion` (OperaPrismaECR.cs:304) | GET | `/api/v1/pos/anulaciones/{idOperacionPayway}/estado` | - | `PosAnulacionResponse` |
| Cancelar anulación | `CancelarIntencionAnulacion` (OperaPrismaECR.cs:358) | PUT | `/api/v1/pos/anulaciones/{idOperacionPayway}/cancelaciones` | - | `PosAnulacionResponse` |
| Crear cierre | `CreaIntencionCierre` (OperaPrismaECR.cs:414) | POST | `/api/v1/pos/cierres` | `PosCierreRequest` | `PosCierreResponse` |
| Consultar cierre | `ConsultarStatusCierre` (OperaPrismaECR.cs:473) | GET | `/api/v1/pos/cierres/{SettlementId}` | - | `PosCierreResponse` |
| (código muerto) | `buscaTransaccionAnula` (OperaPrismaECR.cs:190) | GET | `api/v1/pos/anulaciones/{claveBusq}` | - | `PosPaymentResponse` |

---

## 3. Comparación rápida

| | SmartPoint | PrismaECR |
|---|---|---|
| Config host/puerto | `ipsmartpointmp` / `portsmartpointmp` (5024) | `ipapiprismaecr` / `portapiprismaecr` (5029) |
| Prefijo API | `/api/alberdimp/` | `/api/v1/pos/` |
| Clave de polling | `ExternalReference` | `IdOperacionPayway` (pago) / `IdOperacionPayway` propio (anulación) / `SettlementId` (cierre) |
| Intervalo de polling | 2000 ms | 2000 ms |
| Límite de intentos | 60 (creación) + 120 (fallback MP) | 60 en todos los casos |
| Cancelación automática por timeout | Sí (pago, vía `CancelarOrden`) | Sí (pago y anulación); **no** en cierre |
| Anulación post-aprobación | 1 llamada (`DevolverOrden`, GET) | 3 llamadas (crear + poolear + cancelar opcional) |
| Cierre de lote | **No implementado** | Sí (crear + poolear; manejo de éxito incompleto en el C++) |
| Autenticación HTTP | Ninguna | Ninguna |
| Registro local de anulación en BD | No | Sí (`GrabarAnulacionPrismaEcr`) |

---

## 4. Riesgos / deuda técnica detectada (no corregidos, solo documentados)

- **SmartPoint `DevolverOrden`** no informa `MensajeError` cuando el HTTP falla (devuelve
  `null` directo) — dificulta diagnosticar fallas de anulación.
- **PrismaECR `buscaTransaccionAnula`** es código muerto con un endpoint mal formado (falta
  `/` inicial) y devuelve el DTO equivocado (`PosPaymentResponse` en vez de
  `PosAnulacionResponse`). Candidato a eliminar si se confirma que no se va a usar.
- **PrismaECR, cierre de lote:** no hay cancelación automática por timeout, y el bloque
  `if (!ConError)` en `CierreLotePrismaEcr()` (`TjOnline.cpp`) está vacío — el éxito del
  cierre no imprime `TicketCierre` ni graba nada localmente.
- **PrismaECR `NumeroComercio`** viene hardcodeado (`"03659307"`) en vez de leerse del
  catálogo de tarjetas (hay una línea comentada que sugiere que antes sí se leía
  dinámicamente).
- Ninguna de las dos integraciones autentica las llamadas a su Web API local — dependen
  enteramente de que esa Web API solo sea alcanzable desde la red local del comercio.

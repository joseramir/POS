# Changelog - POS (posXP)

Registro de cambios relevantes del proyecto principal `pos`.
Formato de fecha: AAAA-MM-DD.

---

## 2026-08-18 - PrismaECR: lista propia de cupones, recargo financiero propio, y trazas para el recargo que queda en $0.01

### Contexto

Los cupones de PrismaECR se guardaban en `Dump::TarjPrisma`, la lista de la integracion
"Verifone Prisma" (la vieja), aprovechando que `TransPrismaECR` hereda de `TransVerifone`.
`Dump::TarjPrismaECR` existia pero se usaba solo para anulaciones. Como consecuencia, el
recargo financiero de las tarjetas ECR se facturaba dentro del loop de Prisma, mezclado
con el de la otra integracion.

Ademas sigue abierto un bug reportado 4 veces (dos de ellas la semana del 2026-08-11): en
las cajas con PrismaECR habilitado, el renglon de recargo financiero (PLU 99997) queda
facturado en **$0.01** en vez del monto que corresponde. La pantalla muestra el interes
correcto y la tarjeta se autoriza por el importe correcto con recargo incluido, asi que
el valor se pierde en algun punto entre el calculo y la facturacion. Tres hipotesis
previas fueron descartadas por el usuario, y una cuarta (que `TransPrismaECR` sombreara la
propiedad `ImporteRecargo`) se verifico y tambien se descarto: hereda limpio.

### Parche

**`SRC/Forms/frmPrismaECR.h`:** los cupones ECR pasan a guardarse en `Dump::TarjPrismaECR`
(su lista propia) y `LlenarLvw()` la recorre con el tipo correcto (`TransPrismaECR` en vez
de `TransVerifone`).

**`SRC/Functions/MPAGO.CPP`:** loop propio de recargo financiero para
`Dump::TarjPrismaECR`, con su `FacturarRecargo()` y su confirmacion de descuentos por
medio de pago, en linea con los que ya existian para Posnet, Prisma y SmartPoint.

### Trazas de diagnostico del recargo $0.01

Se agrego una cadena de `WLog` con el prefijo comun `RECAFIN.` que permite seguir el
recargo desde su origen hasta el renglon facturado, para resolver el bug en la proxima
ocurrencia en vez de seguir deduciendo:

- `frmPrismaECR.h` - `RECAFIN.ECR - Calculado`: tasa, monto, descuento por medio de pago y
  el `ImporteRecargo` resultante, en el momento del calculo.
- `frmPrismaECR.h` - `RECAFIN.ECR - AlGuardar`: el valor con el que el cupon entra a la
  lista, DESPUES de la espera de la autorizacion (ventana de hasta 2 minutos).
- `MPAGO.CPP` - `RECAFIN.POSNET|PRISMA|ECR|SMARTPOINT - Total a facturar`: el total de cada
  lista antes de facturarlo, mas un renglon por cupon en los loops de Prisma y ECR. El
  total se imprime con `%.17lg` para distinguir un 0.01 exacto de un residuo de redondeo.
- `PLU.CPP` - `RECAFIN.FACTURA` (dentro de `FacturarRecargo()`): el valor recibido, el
  string devuelto por `BDecimal::ToString()`, el decimal parseado, `precuni` y el importe
  final. Es el que separa "llego mal desde el medio de pago" de "se corrompio en la
  conversion".

### Nota sobre la conversion de `FacturarRecargo()`

La auditoria de esa cadena dejo dos defectos anotados, no corregidos aca: `DecToString()`
escribe en un buffer estatico global (`rgch`), por lo que no es reentrante ni thread-safe,
y `BDecimal::ToString()` lo copia recien en la linea siguiente; y el separador decimal se
emite hardcodeado como `.` mientras que `Convert::ToDecimal(String^)` parsea con la
cultura del sistema, lo que rompe si alguna caja se configura con cultura es-AR.

---

## 2026-08-08 - QuickVentas: archivos de venta rapida agarrados en el server, arqueo de cajero flotante sin impresion

### Contexto / sintoma

A veces, al hacer un arqueo, no salia la impresion porque el archivo de ventas generado
por otra caja quedaba "agarrado" en la carpeta compartida del server y no se podia leer.
La solucion manual era reiniciar la caja para liberar el archivo.

Al cerrar sesion un cajero, `QuickVentas()` graba en la carpeta compartida un binario con
el resumen de venta de ese cajero en esa caja. Un cajero "flotante" (que vende en varias
cajas), al hacer arqueo, dispara `IncorporaVentaRemota()`, que busca TODOS los archivos
de ese cajero en las demas cajas y los suma con `IncorporeOneFile()`.

### Causa

1. `QuickVentas()` e `IncorporeOneFile()` eran las unicas funciones del proyecto que
   abrian archivos de la carpeta compartida con `open()`/`creat()` planos, sin modo de
   comparticion explicito. El resto del codigo usa siempre `sopen()`/`_sopen()` con
   `SH_DENYNO`/`SH_DENYRW`, necesario porque sin flag explicito el runtime intenta modo
   "compatibilidad" (estilo DOS), no soportado en unidades de red (SMB): puede degradar a
   un lock mas restrictivo de lo esperado o directamente a sharing violation cuando
   escritor y lector se cruzan.
2. Carrera real entre escritor y lector: si un cajero cierra sesion en una caja mientras
   se dispara un arqueo de ese mismo cajero en otra, pueden pisarse.
3. `IncorporeOneFile()` no logueaba nada si `open()` fallaba: el arqueo seguia adelante
   como si esa caja no tuviera ventas, sin dejar rastro. Por eso el sintoma era "no sale
   la impresion" sin ningun aviso.

### Parche

**Archivo:** `SRC/Kernel/DUMP.CPP`

1. `sopen()` con modo de comparticion explicito: `SH_DENYRW` para el temporal de escritura
   en `QuickVentas()`, `SH_DENYNO` para la lectura en `IncorporeOneFile()`, siguiendo el
   mismo patron que el resto del proyecto.
2. Escritura atomica: `QuickVentas()` ya no escribe directo sobre el nombre definitivo.
   Escribe a `<archivo>.tmp` y al cerrar publica con
   `MoveFileExA(..., MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)`. Un lector nunca
   puede toparse con el archivo a medio escribir ni competir por el handle con el escritor.
3. Logging con `WLog()` en los tres puntos de fallo (crear el temporal, publicar via
   MoveFileEx, abrir el remoto para el arqueo), que antes fallaban en silencio.

Compilado el 2026-08-08. En produccion desde el 2026-08-18. Pendiente: validar el
escenario real (cierre de sesion de un cajero flotante en una caja mientras se dispara el
arqueo de ese mismo cajero en otra).

---

## 2026-08-18 - Parking: no se factura en el POS mientras la aplicacion de Estacionamiento no tenga hecho su cierre

### Contexto / objetivo

En las cajas con `procesaParking` habilitado, el POS consulta a la aplicacion que factura el
estacionamiento si ya se realizo su cierre de caja. Si el cierre NO esta hecho, no se permite
facturar: el objetivo es forzar que esos cierres se hagan en la aplicacion de Parking.

Endpoint nuevo `GET /autorizacion-cierre-caja` (header `X-API-Key`), consumido desde
`ClienteParking::ConsultaCierreRealizado()`, que devuelve la entidad `AutorizacionCierre`
(`autorizado` + `mensaje`). `autorizado == true` significa "el cierre ya esta realizado, se
puede facturar".

El control se aplica en dos puntos, gobernados por la global nueva
`validadoCierreEstacionamiento` (que `ResetPOSAcumInternal()` resetea en cada ticket, asi que
se consulta una vez por ticket y no una vez por articulo):

- `IngIdParking()` (`OperPromosServ.cpp`): al entrar a la opcion de estacionamiento.
- `plu_()` (`PLU.CPP`): antes de vender, para cubrir el caso en que no se paso por la opcion.

**Ante un fallo de comunicacion con el endpoint se permite facturar igual (fail-open)**, para no
dejar la caja parada por una caida del servicio de parking, dejando registro con `WLog`. Es una
decision explicita: se asume que el control se puede evadir dejando el endpoint inaccesible, y a
cambio queda el rastro en el log.

### Correcciones sobre la primera version del parche

La logica original tenia las condiciones invertidas en los dos puntos de control:

1. **`PLU.CPP` bloqueaba la venta justo cuando el cierre SI estaba hecho.** La segunda rama
   (`else if (consuCierre != nullptr)`) solo se alcanzaba con `autorizado == true`, y mostraba
   "No se pudo Verficar el estado de cierre" antes de un `return`. En cambio, ante un fallo real
   de comunicacion (`consuCierre == nullptr`) no entraba a ninguna rama y seguia de largo sin
   avisar. Con `procesaParking` activo, eso bloqueaba toda venta normal.

2. **`OperPromosServ.cpp` tenia una tercera rama muerta.** `else if (consuCierre != nullptr)`
   nunca se alcanzaba, porque las dos anteriores ya cubrian ese caso; el fallo de comunicacion
   no mostraba nada ni abria el dialogo. Ademas marcaba `validadoCierreEstacionamiento = true`
   **tambien cuando el cierre no estaba hecho**, lo que salteaba el control de `plu_()` durante
   el resto del ticket.

Ambos se reescribieron con las mismas tres ramas explicitas (`nullptr` / `!autorizado` /
autorizado), y solo se marca `validadoCierreEstacionamiento` en la rama autorizada, para que al
hacer el cierre en la aplicacion de Parking se pueda reintentar sin cerrar el ticket. De paso:
se agrego el chequeo de `mensaje != nullptr` con un texto por defecto, el buffer del mensaje paso
de `char[50]` a `char[100]`, y `delete consuCierre` quedo en un punto alcanzable (en `PLU.CPP`
era inalcanzable por los `return` previos).

### Parche

**Archivo nuevo:**
- `LibEntidades/Alberdi/Parking/AutorizacionCierre.cs`: entidad de la respuesta del endpoint.

**Archivos modificados:**
- `LibEntidades/Alberdi/Parking/ClienteParking.cs`: `ConsultaCierreRealizado()`.
- `LibEntidades/LibEntidades.csproj`: entrada de compilacion del archivo nuevo.
- `SRC/Include/OPC.H`, `SRC/Functions/VARIAB.CPP`: global `validadoCierreEstacionamiento`.
- `SRC/Kernel/DUMP.CPP`: reset de la global en `ResetPOSAcumInternal()`.
- `SRC/Functions/OperPromosServ.cpp`, `SRC/Functions/PLU.CPP`: los dos puntos de control.

**Configuracion (NO incluida en este commit):** el parche necesita en `app.config` las claves
`ipapiparking` / `portapiparking` apuntando al servidor de parking y la clave nueva
`apikeyparking`. Se dejo `app.config` deliberadamente fuera del commit para no publicar la API
key en el repositorio, asi que hay que configurarlo en cada caja.

---

## 2026-08-18 - Carga manual de cupon Prisma: se quita la consulta de ultima operacion y se abre la autorizacion a cualquier cajero

### Contexto / sintoma

Dos cambios pedidos sobre la funcionalidad agregada el 2026-08-13 (carga manual de cupon
en las cajas de autocobro, `SRC/Forms/TarjOnlineTouch.h`):

1. **La consulta de ultima operacion es riesgosa.** El boton "Consultar Ultima Operacion"
   traia del Prisma Integrado los datos de la ultima transaccion aprobada (comando `ULT`) y
   precargaba el dialogo del cupon con ellos. Como el operador solo tiene que apretar
   Aceptar, resulta tentador confirmarla sin controlar que el cupon fisico corresponda
   realmente a esa operacion. Al no devolver el protocolo ni monto ni cuotas, no hay forma
   de validar automaticamente que sea la operacion correcta: el control es puramente visual
   y depende de que el operador lo haga.

2. **La autorizacion exigia un supervisor.** Se pedia codigo de un usuario con `nivel >= 3`
   en `CAJEROS.DBF`. Se pide que pueda autorizarla cualquier usuario/cajero dado de alta,
   ingresando su numero de cajero y su clave.

### Parche

**Modificacion 1 - se elimina la consulta de ultima operacion:**
- `SRC/Forms/TarjOnlineTouch.h`: se elimino el boton `btnConsultarUltima` (declaracion,
  `InitializeComponent` y handler `btnConsultarUltima_Click`). Queda unicamente
  `btnCargaManual`, que sigue oculto por defecto y solo se muestra cuando
  `otroMsg->ConError` en `btOk_Click`.
- `SRC/Forms/frmCuponManualPrisma.h`: se elimino el constructor precargado de 5 parametros
  (lote/cupon/autorizacion + digitos parciales de tarjeta) y el label `lblVerificar` que
  mostraba los digitos a confirmar; ambos quedaban sin uso. El dialogo arranca siempre con
  los 3 campos vacios, para que se tipeen leyendo el cupon fisico.
- No se toco `OperarTrxPrisma.ObtenerUltimaTrans()` en `TJOCommon/Prisma/`: sigue en uso
  desde `frmAdicPrisma.h`.

**Modificacion 2 - autoriza cualquier cajero, no solo un supervisor:**
- `SRC/Functions/CAJERO.CPP`, `SRC/Include/POS.H`: `AutorizaSupervisorPuntual()` pasa a
  llamarse `AutorizaUsuarioPuntual()` y se le quito la exigencia de `nivel >= 3`. Ahora
  valida solamente que el codigo exista en `CAJEROS.DBF` y que la clave coincida via
  `CheckPassword`. Se mantiene sin cambios el resto del comportamiento: no loguinea al
  usuario ni cambia el modo de la caja, limite de 3 intentos, y log de auditoria de cada
  intento (el prefijo paso de `AUTORIZ.SUPERV` a `AUTORIZ.USUARIO`).
- Igual que antes, un usuario con el campo `clave` vacio en `CAJEROS.DBF` autoriza sin que
  se le pida clave -- mismo criterio que usa el login normal de cajero (`superv()`).
- `SRC/Forms/frmIngCodSuperv.h`: textos en pantalla ("Autorizacion de Usuario", "Codigo de
  Cajero:"). El nombre del form y su archivo quedaron sin cambio para no tener que tocar
  `POS.VCPROJ` ni el `.resx`.
- La marca de trazabilidad del cupon pasa de `CARGA MANUAL - Superv: <nombre>` a
  `CARGA MANUAL - Usuario: <nombre>`.

Pendiente: compilar y validar en caja.

---

## 2026-08-13 - Promociones: control por accion ante reinicio de caja (saldocaja y servicios externos)

### Contexto / sintoma

El resguardo que evita reaplicar promociones tras un reinicio de caja (`PromoYaAplicada()`,
parche del 2026-08-06) trabajaba **por promocion**: si la promo ya tenia grabado su renglon
de descuento real (`tpremio == 3`) se salteaba entera, y si no, se re-ejecutaba entera.
Ese corte grueso dejaba dos agujeros:

1. **`saldocaja` sin descontar.** Al saltear la promo entera, `F_DescTotalIva2`
   (`DESC_TOTIVACC`) no volvia a correr y la global `acumSaldoPromo` -que es de proceso y se
   pierde en el reinicio- quedaba en 0. Al cerrar el ticket, `AlmacConsumo()` tomaba la rama
   `ActualizaConsumo()` en vez de `ActualizaConsumoCli()`, asi que el `saldocaja` del cliente
   NUNCA se descontaba en la BD aunque el ticket ya le hubiera dado el descuento.

2. **Doble llamada a servicios externos.** Solo `tpremio == 3` marcaba la promo como aplicada,
   pero TODAS las acciones con efecto externo (`VOUCHER_STOCK`, `VOUCHER_CUPON`, `VOUCHER_PROMO`,
   `VOUCHER_DESCTO`, `VOUCHER_PINST`, `VOUCHER_GENVOC`, `VOUCHER_ROTRO`, `VOUCHER_GENVOCSC`)
   graban `RegisterPrize(1, ...)`, que no cerraba la promo. Tras un reinicio se re-ejecutaban y
   disparaban una segunda llamada real al servidor OC, que vuelve a descontar stock.

Ademas, saltear la promo entera podia dejar sin ejecutar acciones que si correspondian: si el
reinicio caia justo despues del descuento, un voucher posterior de la misma promo no se
entregaba nunca.

### Decision

Se reemplaza el gate por-promocion por un gate **por accion**. Cada renglon `DPromo` que se
graba en `trans.dbf` lleva ahora el indice 1-based de la accion que lo genero, de modo que al
reanudar un ticket interrumpido se retoma exactamente en la accion donde quedo.

**No se cambio el formato de `trans.dbf`.** `trans.dbf` es un DBF real con un unico esquema de
44 columnas (`crStr`) compartido por los 41 tipos de registro, y los structs se serializan campo
por campo contra columnas nombradas. El indice de accion se mapeo a `que_anula`, una columna que
ya existia y que los renglones `func=26` no usaban. Los tickets pendientes grabados por la
version anterior la leen en blanco (`atoi("") == 0`), que es justamente el valor reservado para
"sin indice de accion": esas promos caen al comportamiento viejo (`legacy`), sin crash ni
regresion. Agregar una columna nueva a `crStr` SI hubiera roto: `trDecode` hace `AbnormalEnd`
si un `Get()` falla sobre un `trans.dbf` viejo.

Sobre la reconstruccion de `acumSaldoPromo`: `F_DescTotalIva2()` **asigna** la variable (y la
pone en 0 antes), no la acumula. O sea que vale el ultimo descuento aplicado, no la suma de
todos. La reconstruccion respeta esa semantica (last-writer-wins); sumarlos habria descontado
de mas si un ticket tuviera dos promos de saldocaja.

La marca de "esta accion toca saldocaja" se detecta buscando `DESC_TOTIVACC` en el texto crudo
de la accion, no por `MarcaCli`: hay promos con `MarcaCli > 0` que no tocan `saldocaja`.

### Parche

**`SRC/Include/FUNCS.H`**
- Campo `accionSeq` al final de `struct DPromo_` (157 -> 161 bytes, sigue holgado bajo los 180
  que fuerza `fijo_`) y entrada `{ SZ_INT, 'I', "que_anula" }` al final de `xxDDPromo`. El orden
  struct <-> tabla tiene que coincidir: `trEncode` avanza con `p += xf->ancho`.

**`SRC/Include/promo.h`**
- `PromoDef::accionesSaldoCajaMask` (bitmask, hasta 32 acciones) + `AccionUsaSaldoCaja(idx)` y
  `ActionCount()`. Prototipo de `ReconstruirAcumSaldoPromo()`.

**`SRC/Functions/PROMOS.cpp`**
- `PromoAplicadaInfo` ahora guarda el detalle por accion (`AccionAplicadaInfo`) y una marca
  `legacy` para tickets de la version anterior.
- `PromoYaAplicada()` se reemplaza por `AccionYaAplicada(codPromo, seq)` (corte fino) y
  `PromoTotalmenteAplicada(promo)` (corte grueso: solo saltea la promo si TODAS sus acciones
  quedaron grabadas). Los 4 call sites de `CalcularPromosAntesMP()`, `AplicarPromociones()` y
  los dos loops de `ApliPromoCobra()` pasan a usar el corte grueso.
- `PromoDef::ApplyAction()` saltea individualmente las acciones ya grabadas y sella
  `accionActualSeq` antes de cada `action->Run()`.
- `RegisterPrize()` graba `dp.accionSeq`. OJO: `dp` no se memsetea, todo campo nuevo hay que
  inicializarlo a mano ahi.
- `ProcPromo()` asienta la accion concreta de cada renglon, tanto en el reproceso de arranque
  (`fwrite=0`) como en vivo (`fwrite=1`).
- `PromoDef::AddAction()` prende el bit de saldocaja cuando la accion invoca `DESC_TOTIVACC`.
- Nuevas `BuscaPromoDef()` y `ReconstruirAcumSaldoPromo()`.
- **Hardening:** el constructor de `PromoDef` no inicializaba `filterResult`, y `ApplyAction()`
  lo dereferencia sin chequear. `AplicarPromociones()` nunca llama a `Evaluate()`: depende de que
  `CalcularPromosAntesMP()` haya corrido antes en el mismo proceso. Se inicializa a `NULL` y se
  agrega la guarda, para no leer memoria indeterminada.

**`SRC/Kernel/DUMP.CPP`**
- `InitDump()` llama `ReconstruirAcumSaldoPromo()` despues del loop de reproceso de `trans.dbf`.
  El orden es correcto: en `KRNLTBL.CPP` `InitPromos` (que carga las definiciones) corre antes
  que `InitDump`.

### Pendiente / fuera de alcance

- `TRAER_PREMIOSTOCK` / `F_TraeStockPremio` descuenta stock en el servidor y no llama a
  `RegisterPrize` en ningun camino, asi que no deja rastro: el gate por accion solo lo cubre
  cuando esta anidado dentro de una accion que si graba. Cerrarlo requiere memoizar la respuesta
  del servidor para devolverla en el replay.
- `F_TraeStockPremio` y `F_VoucherC2` no cortan por `soloSimular`: le pasan el flag al servidor,
  asi que cada ticket abre el socket dos veces (simula=1 y simula=0). Preexistente.
- Ventana no atomica residual: si el corte cae entre la llamada al servidor y el `RegisterPrize`
  de esa misma accion, la accion se repite. `VOUCHER_DESCTO` es el mas expuesto (genera el premio
  antes de abrir el modal de ingreso de documento).
- `mpago_2()` / `mpago_3()` llaman `AplicarPromociones()` sin pasar nunca por
  `CalcularPromosAntesMP()` y no tienen el resguardo `if(!promosHechas){ promosCalc = false; }`
  que si tiene `mpago_()`. La guarda de `filterResult` evita el comportamiento indefinido, pero
  el flujo en si queda por revisar.

---

## 2026-08-13 - TarjOnlineTouch (autocobro): carga manual de cupon Prisma cuando no llega respuesta de aprobacion

### Contexto / sintoma

En las cajas de autocobro, las operaciones con tarjeta de credito por Prisma Integrado
(no PrismaECR) a veces quedan aprobadas del lado del equipo pero la respuesta no vuelve
por el protocolo serie (comunicacion), dejando el ticket a medias. Hasta ahora la unica
salida era anular el ticket y recargar el cupon a mano en otra caja.

### Decision

Se agregan dos alternativas, habilitadas SOLO cuando falla la comunicacion con el Prisma
(no ante un rechazo real de la operacion) y gateadas por autorizacion puntual de un
supervisor (mismo mecanismo de CAJEROS.DBF + clave que usa `superv()`, pero sin loguinear
al supervisor ni cambiar el modo de la caja):

1. **Carga manual**: el supervisor autoriza y carga a mano lote/cupon/autorizacion.
2. **Consultar ultima operacion**: se consulta al Prisma Integrado (comando ULT) la ultima
   transaccion aprobada y, si el supervisor confirma que corresponde a la tarjeta usada
   (el protocolo no informa monto ni cuotas en esta consulta, solo puede compararse contra
   los ultimos 4 / primeros 6 digitos de la tarjeta), se usa para completar el cupon.

Cada intento de autorizacion (exito, clave invalida, cancelado, intentos agotados) queda
registrado en el log via `WLog`. Maximo 3 intentos antes de cancelar el pedido. El cupon
resultante queda marcado en su mensaje de respuesta como carga manual o consulta, junto
con el nombre del supervisor que autorizo, para poder diferenciarlo despues de una
aprobacion automatica normal.

### Parche

**Archivos nuevos:**
- `SRC/Forms/frmIngCodSuperv.h/.cpp/.resx`: dialogo touch para pedir el codigo de supervisor.
- `SRC/Forms/frmCuponManualPrisma.h/.cpp/.resx`: dialogo touch de 3 campos (lote/cupon/
  autorizacion), con constructor vacio (carga manual) y constructor precargado (consulta).

**Archivos modificados:**
- `SRC/Include/POS.H`, `SRC/Functions/CAJERO.CPP`: nueva funcion
  `AutorizaSupervisorPuntual()`, variante de un solo uso de `superv()` (codigo + nivel>=3 +
  clave via `CheckPassword`), con log de auditoria y limite de 3 intentos.
- `SRC/Forms/TarjOnlineTouch.h`: se quito el scaffold muerto `AcceptSupervisorCode`/
  `DoSupervCode` (estaba atado al lector de codigo de barras SERIE via `ScannerRedirectedFunc`,
  que no aplica en autocobro porque el lector ahi es USB/teclado). Se agregaron los botones
  `btnCargaManual` y `btnConsultarUltima` (ocultos por defecto, visibles solo cuando
  `otroMsg->ConError` en `btOk_Click`) y sus handlers.
- `POS.VCPROJ`: entradas de los 2 forms nuevos.

Pendiente: compilar y validar en el escenario real (Prisma Integrado aprueba sin devolver
respuesta) antes de dar el tema por cerrado.

---

## 2026-08-10 - TicketSync: eliminar envío inmediato al cerrar el ticket, sincronizar solo desde SyncWorker

### Contexto / síntoma

Diagnóstico de la sesión del 2026-08-08: tickets fiscales duplicados en la base del
webapi. Tres causas identificadas en `LibEntidades/Alberdi/`: (1) `MarcaSincronizadoPorSeq()`
buscaba el ticket recién enviado entre los 200 pendientes más antiguos
(`ObtenerPendientes(200)`) y con backlog >200 nunca lo encontraba, dejándolo sin marcar
`SINCRONIZADO`; (2) un timeout de red no prueba que el servidor no haya procesado el POST;
(3) carrera entre el envío inmediato al cerrar el ticket
(`ClienteComprobante.PostComprobante`) y el reintento en background (`SyncWorker`), sin
ningún "claim" de fila entre ambos caminos.

### Decisión

En vez de parchear las dos causas de forma incremental (marcar sincronizado por seq de
forma directa + un estado transitorio tipo ENVIANDO), se optó por una simplificación de
raíz: suprimir el intento de envío síncrono en el cierre del ticket y dejar que solo
el `SyncWorker` en background envíe al webapi. `SyncWorker.ProcesarLote()` corre en un
único hilo secuencial (sin reentrancia) y marca `SINCRONIZADO` por `Id` directo del lote
que ya tiene en mano, no repite el patrón de búsqueda de `MarcaSincronizadoPorSeq()`.
Esto elimina las causas #1 y #3 directamente, no solo las mitiga: ya no hay dos caminos que
puedan enviar el mismo `seq`. La causa #2 (timeouts ambiguos) sigue viva dentro del propio
`SyncWorker`, pero de fondo requiere idempotencia del lado del webapi, fuera del alcance de
este cambio.

### Parche

**Archivo:** `LibEntidades/Alberdi/ClienteComprobante.cs`

`PostComprobante()` ahora solo inserta el comprobante en el buffer SQLite local
(`_syncRepo.InsertarPendiente`) y retorna `true`. Se eliminó el bloque de envío
inmediato (intento HTTP con hasta 2 reintentos y timeout de 3s cada uno) y el helper
`MarcaSincronizadoPorSeq()`, que quedaba sin uso. El único llamador
(`SRC/Kernel/DUMP.CPP:3177`) solo usaba el valor de retorno booleano, que ya era siempre
`true` en la implementación anterior; no cambia su comportamiento visible.

### Nota

Efecto colateral esperado: el cierre de ticket deja de bloquear la caja esperando red
(antes hasta ~6s en el peor caso de 2 reintentos con timeout). A cambio, en el caso feliz
el ticket ya no llega "casi inmediato" al webapi -- ahora depende del próximo ciclo del
`SyncWorker` (cada 30s, lotes de 50). Implementado el 2026-08-10, sin compilar ni validar
todavía.

---


## 2026-08-06 - ApliPromoCobra() sin reguardo de reintento (doble descuento al reenviar a caja cobradora)

### Contexto / síntoma

Pendiente anotado en la entrada del 2026-07-31 (`ApliPromoCobra()`, usada al enviar el
ticket a "caja cobradora" vía `EnviarACobradora()`, `MPAGO.CPP:1269`). Al revisarlo se
encontró además que la rama `#ifdef FACTSQL` de `EnviarACobradora()` **ya no es código
muerto**: `POS.VCPROJ` tiene `FACTSQL` definido en la configuración `Release|Win32` (y
`WFACT` en `Debug|Win32`), a diferencia de lo verificado el 2026-06-18. El cambio data del
2026-07-30, cuando se tocó el `.vcproj` a mano para la implementación de SmartPoint.

### Diagnóstico

`ApliPromoCobra()` (`PROMOS.cpp:590-663`) es una segunda pasada de aplicación de promos
(filtra solo las que no dependen de un medio de pago o tarjeta específico), independiente
del flujo normal (`CalcularPromosAntesMP`/`AplicarPromociones`). A diferencia de esas dos:

- Fuerza `promosCalc = false` en cada entrada, y las líneas que marcarían "ya terminé"
  (`promosCalc = true`, `promosAplicadas = true`, `promosHechas = true`, `WriteCmos(...)`)
  están comentadas desde antes de esta sesión. No había ningún resguardo contra
  reprocesar las mismas promos.
- `EnviarACobradora()` llama `ApliPromoCobra()` y, si el envío falla, borra de
  `trans.dbf` los renglones `DPromo` (`func=26`, auditoría) pero no los renglones de
  descuento ya escritos (`func=4`, negativos, grabados por `CommonDescto()` dentro de
  `ApliPromoCobra()`). Si el cajero reintenta el envío, `ApliPromoCobra()` vuelve a
  aplicar las mismas promos sin ningún chequeo, apilando un `func=4` nuevo sobre el que
  quedó del intento fallido: descuento duplicado real, independiente del reinicio de caja.

### Parche

**Archivo:** `SRC/Functions/PROMOS.cpp`

Se reusó el mecanismo ya validado en la entrada anterior: se agregó
`if(PromoYaAplicada(promo->CodPromo)) continue;` al principio de los dos loops de
`ApliPromoCobra()` (fase de simulación y fase de aplicación real), igual que en
`CalcularPromosAntesMP()`/`AplicarPromociones()`. Como `tablaPromosAplicadas` vive en
memoria durante todo el ticket (se vacía recién en `ResetPOSAcumInternal()`, al cerrar el
ticket) y se llena desde el único funnel `ProcPromo()` sin importar qué código disparó la
promo, un reintento de `EnviarACobradora()` ahora salta las promos cuya acción de
descuento/entrega ya quedó confirmada (`premio==3`) en el intento anterior, aunque
`trans.dbf` ya no tenga el renglón `DPromo` correspondiente (se borró en el cleanup del
intento fallido).

No se tocaron los flags legacy comentados (`promosCalc`/`promosAplicadas`/`promosHechas`
dentro de esta función): son del mecanismo viejo, ya superado, y reactivarlos podría
interferir con el flujo normal de pago que también los usa.

### Nota

Queda sin resolver, aparte, la asimetría del cleanup en el catch de `EnviarACobradora()`
(borra `func=26` pero no `func=4` tras un envío fallido) — con este parche deja de causar
descuento duplicado, pero el renglón de descuento que queda en `trans.dbf` sin su
`DPromo` de auditoría sigue siendo una inconsistencia de trazabilidad menor. No evaluado
en esta sesión.

---

## 2026-08-06 - Descuento de promo faltante tras reinicio a mitad de una promo con varias acciones (secuela del fix de 2026-07-31)

### Contexto / síntoma

Confirmado el límite conocido que había quedado documentado como pendiente en la entrada
del 2026-07-31: se compararon dos tickets reales con los mismos artículos en `trans.dbf`
(uno cobrado en flujo normal, otro simulando un reinicio de caja a mitad de ticket). En el
ticket con reinicio, la promo 17 (20% desc.) no se aplicó, cobrando de más ese ticket.

### Diagnóstico

`PromoDef::ApplyAction()` (`PROMOS.cpp:1080`) ejecuta las acciones de una promo en
secuencia (`actionExps`). La promo 17 tiene dos: un mensaje informativo (`F_Mensaje` ->
`RegisterPrize(2,...)`, graba un renglón `DPromo` con datos de leyenda) y el descuento real
(`CommonDescto` -> `ProcPlu` + `RegisterPrize(3,...)`, graba el renglón `func=4` negativo más
un renglón `DPromo` de confirmación). El reinicio de caja se produjo justo entre esas dos
acciones: el renglón informativo ya estaba en `trans.dbf`, el del descuento no.

`ProcPromo()` marcaba `infoAplic->aplicado = true` en `tablaPromosAplicadas` con **cualquier**
renglón `DPromo` que procesara, sin distinguir de qué acción provenía. Al reanudar el
ticket, `AplicarPromociones()` encontraba la promo 17 ya presente en la tabla (por el
mensaje) y saltea la promo entera (`PROMOS.cpp:377`), incluida la acción del descuento que
nunca se había llegado a ejecutar.

### Parche

**Archivo:** `SRC/Functions/PROMOS.cpp`

- Se agregó `PromoYaAplicada(codPromo)`, que solo devuelve `true` cuando la entrada de la
  tabla tiene `aplicado == true` (antes, `CalcularPromosAntesMP()` y `AplicarPromociones()`
  usaban directamente `BuscaPromoAplicada(...) != NULL`, que da `true` con solo *existir* la
  entrada, sin importar qué acción la creó).
- `ProcPromo()` ahora solo pone `infoAplic->aplicado = true` cuando el renglón `DPromo` es
  el de confirmación de la acción real (`promo->tpremio == 3`, el mismo criterio que usa
  `RegisterPrize(3,...)` tras un `ProcPlu` real en `CommonDescto` y en las demás acciones
  de entrega de premio). Un renglón informativo (`premio` 1 o 2) actualiza el resto de la
  entrada (`montoAplicado`, `descarga`) pero no cierra la promo.
- `CalcularPromosAntesMP()` y `AplicarPromociones()` pasaron a llamar `PromoYaAplicada()`
  en vez de `BuscaPromoAplicada() != NULL`.

### Nota

Con este cambio, si el reinicio interrumpe una promo entre su acción informativa y su
acción de descuento, al reanudar se vuelve a ejecutar la promo completa desde el principio
(`ApplyAction()` no retoma desde la acción pendiente, arranca de nuevo). Esto puede
duplicar el renglón informativo (mensaje/leyenda) en `trans.dbf`, pero no duplica plata: la
acción de descuento/entrega real recién se marca aplicada cuando efectivamente se graba.
Tradeoff aceptado conscientemente (duplicar un renglón cosmético es preferible a perder un
descuento que correspondía). Ver debajo la entrada siguiente: el mismo dia se
resolvio tambien lo pendiente de `ApliPromoCobra()`.

---

## 2026-07-31 - Doble descuento de promociones al reiniciar la caja con ticket pendiente

### Contexto / síntoma

Ocasionalmente, al reiniciar la caja con un ticket sin cerrar, las promociones que ya se
habían aplicado (renglones de descuento `func=4` + registro `DPromo` grabados en
`trans.dbf`) se aplicaban una segunda vez al reanudar el ticket, duplicando el descuento.

### Diagnóstico

Las promociones se manejan en dos fases, cada una con su flag de control (`promosCalc`,
`promosHechas`, `SRC/Functions/PROMOS.cpp`) persistido en `cmos.dat`:

- **Simular** (`CalcularPromosAntesMP`, `PROMOS.cpp:252-308`): no escribe `trans.dbf`, solo
  actualiza el total en memoria.
- **Aplicar** (`AplicarPromociones`, `PROMOS.cpp:325-378`): por cada promo que corresponde,
  escribe en `trans.dbf` el renglón de descuento (`func=4`, vía `CommonDescto` en
  `Plugin.cpp`) y el registro `DPromo` de auditoría. Recién después de terminar de
  recorrer todas las promos se marcaba `promosHechas=true` y se persistía con
  `WriteCmos(PROMOSHECHAS)`.

Si la caja se reiniciaba justo después de grabar una o más promos pero antes de terminar
el resto del ticket, `trans.dbf` quedaba con esos renglones ya escritos, pero `cmos.dat`
seguía diciendo `promosHechas=false`. Al reanudar el ticket, el POS volvía a calcular y
aplicar todas las promos del ticket, duplicando las que ya estaban grabadas.

### Parche

**Archivos:** `SRC/Functions/PROMOS.cpp`, `SRC/Include/promo.h`, `SRC/Kernel/DUMP.CPP`

Se reemplazó el flag único por ticket (`promosHechas`) como mecanismo de bloqueo por una
**tabla en memoria de promociones ya aplicadas** (`tablaPromosAplicadas`, con `codPromo`,
`montoAplicado`, `descarga`, `aplicado`), para no depender de un booleano todo-o-nada ni
tener que re-escanear `trans.dbf` en cada intento:

- `ProcPromo()` (único punto por el que pasa todo renglón `DPromo`, tanto al reprocesar
  el ticket pendiente al iniciar la caja como al aplicarse en vivo) llena/actualiza la tabla.
- `CalcularPromosAntesMP()` y `AplicarPromociones()` consultan la tabla (`BuscaPromoAplicada`)
  y saltean cualquier promo que ya figure aplicada, en vez de recalcularla o reescribirla.
- `ResetPOSAcumInternal()` (`DUMP.CPP`) vacía la tabla al cerrar cada ticket
  (`LimpiarPromosAplicadas()`), junto con el reset existente de `promosCalc`/`promosHechas`.

### Nota

Límite conocido, no cerrado por este parche: si una misma promo dispara varias acciones
(`PromoDef::ApplyAction`, `PROMOS.cpp:1012-1079`) y la caja se reinicia a mitad de esas
acciones (no entre dos promos distintas), la tabla marca la promo como aplicada con la
primera acción ya escrita y no reintenta las que faltan. Caso más angosto que el
reportado (reinicio entre dos promos distintas), pendiente si llega a observarse en
producción.

También se detectó, sin corregir, que `ApliPromoCobra()` (`PROMOS.cpp:502-575`, usada al
enviar el ticket a "caja cobradora") tiene deshabilitadas (comentadas) sus propias líneas
de guardia: recalcula y reaplica descuentos reales sin ningún resguardo cada vez que se
llama, incluso en un reintento normal tras un envío fallido. Queda pendiente como parche
aparte.

## 2026-07-29 - Corrección: ID de pedido Ecommerce/PedidosYa truncado en trans.dbf (ppuntual)

### Contexto / síntoma

`NumPedEcommerce` (número de pedido de Ecommerce o PedidosYa) se había ampliado
recientemente a `long long` para admitir IDs de PedidosYa que superan el rango de un
`long` de 32 bits (p. ej. 2216280879, mayor a `LONG_MAX` = 2147483647). El voucher impreso
mostraba el número correcto, pero en `trans.dbf` el dato quedaba mal.

### Diagnóstico

`trans.dbf` se serializa en `SRC/Kernel/DUMP.CPP` (`trEncode`/`trDecode`) recorriendo cada
struct de volcado byte a byte según la tabla `dDbf_` de `SRC/Include/FUNCS.H` (ancho fijo por
campo: `SZ_ULONG` = 4 bytes tipo `'L'`, `SZ_LLONG` = 8 bytes tipo `'X'`). El ensanche de
`NumPedEcommerce` se había aplicado a la variable global y al struct de pago (`DDmpag_.nticket`,
ya `long long` / `SZ_LLONG`), pero no al struct de la línea de ticket que efectivamente se graba
en `trans.dbf` por artículo:

- `DDplu_.ppuntual` y `DDNcItem_.ppuntual` (`FUNCS.H`) seguían declarados `long` (32 bits, con
  signo), y las tablas `xxDDplu`/`xxDDNcItem` seguían mapeando ese campo como
  `{ SZ_ULONG, 'L', "fecha_z"}`.
- En `PLU.CPP:879`, `dplu.ppuntual = NumPedEcommerce;` truncaba el valor de 64 bits a 32 bits
  con signo. Para un ID como 2216280879 (entra en 32 bits sin signo pero supera `LONG_MAX`), el
  valor quedaba **negativo** en memoria.
- Eso rompía además el chequeo `if (p->ppuntual > 0)` en `PLU.CPP:1250`, que se salteaba
  silenciosamente para estos IDs grandes.
- Al releer el registro (`trDecode`, `DUMP.CPP`), el caso `'L'` usa `atol()`, que en MSVC
  satura en `LONG_MAX` ante overflow en vez de dar la vuelta como bits: otra vía de pérdida
  de dato al reprocesar/reimprimir.

La columna física `FECHA_Z` de `trans.dbf` es numérica de ancho 10 (verificado leyendo el
header del DBF), suficiente para IDs de hasta 10 dígitos sin tocar el esquema.

### Parche

**Archivo:** `SRC/Include/FUNCS.H`

- `DDplu_.ppuntual`: `long` -> `long long` (línea 675).
- `xxDDplu["fecha_z"]`: `{ SZ_ULONG, 'L', "fecha_z"}` -> `{ SZ_LLONG, 'X', "fecha_z"}` (línea 707).
- `DDNcItem_.ppuntual`: `long` -> `long long` (línea 743).
- `xxDDNcItem["fecha_z"]`: `{ SZ_ULONG, 'L', "fecha_z"}` -> `{ SZ_LLONG, 'X', "fecha_z"}` (línea 775).

Todo el bloque de structs está bajo `#pragma pack(1)`, así que el cambio de tamaño no
introduce padding y el recorrido posicional por bytes de `trEncode`/`trDecode` sigue
sincronizado. Con esto queda igualado al tratamiento que ya tenía `DDmpag_.nticket`. No hizo
falta tocar `PLU.CPP` ni el ancho de la columna `FECHA_Z`.

### Nota

Durante la edición se detectaron y repararon 7 acentos de `FUNCS.H` que ya estaban
corrompidos a UTF-8 (mismo patrón de la corrección del 2026-06-19), sin relación con este
fix.

### Verificación

1. Compilar `pos` (Release|Win32).
2. Cargar un pedido de PedidosYa con ID > 2147483647 (p. ej. 2216280879) y confirmar que
   `trans.dbf` graba el número completo y correcto en el campo `fecha_z` del renglón del
   artículo.
3. Confirmar que el voucher sigue mostrando el número correcto (no debería cambiar).
4. Si existe flujo de Nota de Crédito con pedido de ecommerce, confirmar también ahí.

---

## 2026-06-19 - Recuperacion de codificacion: acentos/enie corrompidos a UTF-8

### Contexto / sintoma

Los archivos editados el 2026-06-18 (SRC/Functions/PLU.CPP, MainForm.h,
SRC/Kernel/POS.CPP, SRC/Kernel/DUMP.CPP) habian quedado guardados en UTF-8 en lugar de la codificacion
original del proyecto (Windows-1252 / ANSI, VS2008). Cada caracter acentuado y la enie
del texto en castellano preexistente (comentarios y cadenas) se convirtio en el caracter
de reemplazo U+FFFD, perdiendo el original. La reescritura afecto el archivo completo, no
solo las lineas editadas. POS.VCPROJ no se vio afectado (sin acentos).

### Causa

Las herramientas de edicion leyeron y guardaron los fuentes como UTF-8. Los bytes ANSI de
los acentos no son UTF-8 valido, asi que se sustituyeron por el caracter de reemplazo al
decodificar; el caracter original se perdio (no reversible re-convirtiendo).

### Recuperacion

- Se uso C:\pos (copia mas antigua, ANSI, sin estos fixes) SOLO como referencia de que
  palabra acentuada correspondia a cada caracter perdido (ambiguedades resueltas por
  contexto contra esa copia).
- Se restauro cada caracter a su letra correcta y se regrabaron los tres archivos en
  Windows-1252 sin BOM, dejando intactos los fixes de codigo y el resto del contenido.
- Verificacion: 0 caracteres de reemplazo, 0 secuencias UTF-8, y el conteo de bytes altos
  coincide con los originales de C:\pos (PLU=4, MainForm=17, POS=6, DUMP=18 mas 1 corregido a mano por el usuario).

### Nota para futuras ediciones

Estos fuentes deben editarse y guardarse en Windows-1252 ANSI, no en UTF-8, para no
reintroducir el problema. Este CHANGELOG.md tambien se paso a Windows-1252.

---

## 2026-06-18 - Robustez al reprocesar TRANS.DBF: descripción de artículo ausente

Mejora que complementa el fix de la lectura fuera de rango en `PrintPlu`/`PrintNcPlu`.

### Descripción de respaldo si el artículo no existe en el `mplu` local

**Archivo:** `SRC/Functions/PLU.CPP` (`PrintPlu` y `PrintNcPlu`).

Antes se hacía `dbplu.Seek(...)` sin verificar el resultado y luego `Get("des", usades)`.
Si el código no existía localmente, `usades` quedaba vacío (o basura). Ahora:

```c
if(dbplu.Seek(padl(p->cod, 6)))
    dbplu.Get("des", usades);
else
    sprintf(usades, "ARTICULO %ld (no encontrado)", p->cod);
```

Así la línea se reimprime con una descripción válida y no se opera sobre un buffer vacío.

### Descartado: filtro por número de caja al reprocesar

Se evaluó descartar en `ReprocActualTran`/`PrintActualTran` los registros de `trans.dbf` cuyo
campo `caja` no coincidiera con la caja local. **Se revirtió**: el número de caja de un POS puede
cambiarse por configuración, por lo que un `trans.dbf` con registros bajo un número anterior
**sigue siendo de esta misma máquina** y debe recuperarse. Filtrar por caja descartaría tickets
pendientes legítimos. No es un discriminante válido.

---

## 2026-06-18 - Crash al reprocesar ticket pendiente (lectura fuera de rango en PrintPlu/PrintNcPlu)

### Contexto / síntoma

Crash intermitente al iniciar el POS **solo cuando hay un ticket abierto** en `bin\trans.dbf`
(uno o más registros `func=4` = `DPlu`, línea de artículo). Sin `func=4` no ocurre. En
depuración aparecían además first-chance `0xC0000008` (invalid handle) y, al continuar,
a veces arrancaba; la caída dura es una violación de acceso (`0xC0000005`) intermitente.

### Causa raíz

En `PrintPlu` (y su gemela `PrintNcPlu`), al reimprimir cada línea de artículo:

```c
dbplu.Get("des", usades);     // usades = "char *usades = new char[100]" (global)
Trim(usades);
#ifndef FACTSQL               // FACTSQL NO está definido en este build -> el bloque SÍ compila
char *pTemp = usades + strlen(usades) - 6;
if(_stricmp(pTemp, "x 1 Un") == 0 || _stricmp(pTemp, "x 1 Kg") == 0) *pTemp = 0;
#endif
```

Si `strlen(usades) < 6`, `usades + strlen(usades) - 6` apunta **antes** del buffer y el
`_stricmp` lee (y eventualmente `*pTemp = 0` escribe) fuera de rango ? violación de acceso /
corrupción de heap, **intermitente** según el layout de memoria. Se da cuando la descripción
tiene =5 caracteres (p. ej. "Pan", "Sal") o cuando el artículo **no existe en el `mplu` local**
(`dbplu.Seek` falla ? `Get` devuelve blancos ? `Trim` deja cadena vacía). Esto último es
típico al reprocesar un `trans.dbf` con códigos de **otra caja** (en el caso observado, el
`trans.dbf` tenía registros de caja 3 y 4 en un POS configurado como caja 1).

Solo se ejecuta para líneas `func=4` (`PrintPlu`/`PrintNcPlu`), lo que explica por qué el
crash aparece únicamente con un ticket abierto pendiente.

### Cambio

**Archivo:** `SRC/Functions/PLU.CPP` (`PrintPlu` y `PrintNcPlu`).

- Se calcula `size_t lenDes = strlen(usades)` y el recorte del sufijo `"x 1 Un"`/`"x 1 Kg"`
  solo se hace **si `lenDes >= 6`**, evitando el acceso antes del buffer.

### Notas

- Si el artículo no existe en el `mplu` local, ahora **no crashea** pero la línea se reimprime
  con descripción vacía; mejorar el manejo de `dbplu.Seek` fallido (descripción de respaldo)
  queda como mejora futura.
- Relacionado: revisar por qué `trans.dbf` contenía transacciones de otras cajas (ver entrada
  de captura de excepciones y la sugerencia de validar la caja al reprocesar).

---

## 2026-06-18 - Captura de excepciones no controladas (diagnóstico de crash aleatorio)

### Contexto

Crash intermitente al iniciar el POS cuando hay un ticket pendiente en `bin\trans.dbf`
(ruta "Recuperando ticket pendiente, desde corte de corriente" ? `ReprintDump`). El
`crash.dmp` no se actualizaba (entrada vieja de 2011): el `try/catch(Exception)` de `main`
solo cubre el hilo principal, por lo que las caídas en **hilos de fondo** (tareas `fork`,
eventos `SerialPort.DataReceived`) o las **violaciones de acceso nativas** no quedaban
registradas. El error era, en la práctica, "mudo".

### Cambio

**Archivo:** `SRC/Kernel/POS.CPP` (función `main`, bloque `POSXP`).

- Se agregó la clase `CrashLogger` y se registran, al inicio de `main` (antes de `pos_main`):
  - `AppDomain::CurrentDomain->UnhandledException` — captura excepciones no controladas de
    **cualquier hilo** justo antes de terminar el proceso (en .NET 2.0 también recibe las
    violaciones de acceso nativas como excepción administrada).
  - `Application::ThreadException` — captura excepciones del lazo de mensajes de la UI.
- Ambos vuelcan `Message` + `StackTrace` (e inner exceptions) a `crash.dmp`, con el mismo
  formato que ya usaba `main`, e indicando el origen y si el proceso va a terminar.

### Notas

- Es un parche **de diagnóstico** (no corrige la causa): sirve para que la próxima caída
  aleatoria registre el stack real y el hilo culpable, y así confirmar si la causa es la
  concurrencia durante el reproceso de `trans.dbf` (tareas `fork`/eventos serie vs. manejador
  DBF no thread-safe) o datos de transacciones ajenas (en el caso observado, `trans.dbf` tenía
  registros de caja 3 y 4 en un POS configurado como caja 1).
- `Application::ThreadException` enganchado implica que una excepción en el hilo de UI se
  **registra y la app continúa** (en lugar del diálogo por defecto). Es intencional para no
  perder el evento; revisar `crash.dmp` tras cada incidente.

---

## 2026-06-18 - Build independiente de unidad (compilar fuera de C:\pos)

### Contexto

`POS.VCPROJ` tenía rutas de salida **absolutas** que ataban la compilación a `C:\pos`:

- `OutputDirectory="c:\pos\bin"` (Debug y Release)
- `IntermediateDirectory="c:\TEMP"` (Debug) / `"c:\temp"` (Release)

Esto impedía compilar desde otra unidad (p. ej. `Z:\pos`).

### Cambio

**Archivo:** `POS.VCPROJ` (ambas configuraciones, Debug|Win32 y Release|Win32).

- `OutputDirectory` ? `.\bin`
- `IntermediateDirectory` ? `.\temp`

Rutas **relativas al directorio del proyecto**, consistentes con el resto del `.vcproj`
(que ya usaba `.\BIN`, `.\TEMP`, `.\src\include`). En Windows `.\temp` coincide con las
rutas por-herramienta existentes (`ObjectFile=".\TEMP\"`, `PrecompiledHeaderFile=".\TEMP\pos.pch"`,
etc.) por insensibilidad a mayúsculas. Ahora el proyecto compila desde cualquier unidad.

### Notas

- Los `.csproj` dependientes (`Controles`, `TJOCommon`, `TransSql`, `LibEntidades`) ya usaban
  rutas relativas; las referencias de proyecto en el `.vcproj` también (`.\Controles\...`).
- El `.user` del usuario actual (`POS.vcproj.ALBERDI.joser.user`) usa `WorkingDirectory="$(OutDir)"`,
  por lo que la depuración (F5) también es independiente de unidad. Otros `.user` de otras
  máquinas/usuarios aún tienen `WorkingDirectory="c:\pos\bin"` (solo afecta correr/depurar en
  ESAS máquinas, no la compilación).
- Rutas de **datos en runtime** (p. ej. `pos\\mplu.dbf`, y `c:\\command.com` en `SYNC.CPP`) NO
  se modificaron: son ajenas a la compilación y se resuelven relativas al directorio de trabajo
  del ejecutable.

---

## 2026-06-18 - Corrección de precio unitario erróneo ($1.00) en artículo pesable

### Contexto / síntoma

En producción se reportó que, al **pesar un artículo en la balanza y luego teclear su
código interno**, el ticket fiscal salía con **precio unitario $1,00** y una **cantidad
inflada** (p. ej. `734.230000 Kg x $ 1.00`), aunque el **total cobrado era correcto**.

- Caja afectada: Caja 3. Ticket de ejemplo: 165865 (artículo 8606 "Tomate Perita").
- El mismo artículo, en la misma caja y el mismo día, se facturó **correctamente** minutos
  antes y después (`0.245012 Kg x $ 1578.90`).
- No se había recibido un `mplu.dbf` nuevo en esa franja y la configuración no se tocó.

### Diagnóstico (resumen)

No es un problema de datos, de índice `.idx` ni de configuración: es una **condición de
carrera sobre buffers/variables globales** entre el hilo principal del POS (registración
`pesar_plu` ? `plu_`) y el **evento serial de la balanza `spBalan_DataReceived`**, que
.NET ejecuta en un **hilo de fondo** (`SerialDataReceived`).

`plu_` leía el precio del artículo en el **buffer global `msg`** y lo parseaba un instante
después (`dbplu.Get(..., msg)` ? `BDecimal::Parse(msg)`). Si una trama de peso de la balanza
llegaba en esa ventana, sobrescribía `msg` (visible en el `BOOT.LOG` como valores "1.000"
intercalados que reemplazaban las líneas de log esperadas), y `basePrec` terminaba valiendo
$1,00. Como la cantidad se deriva de `monto / basePrec`, salía cantidad enorme y unitario $1,00;
el total se mantenía bien porque proviene del monto pre-calculado (`ForcePrice`).

### Parche 1 (puntual) - Buffer local para el precio en `plu_`

**Archivo:** `SRC/Functions/PLU.CPP` (función `plu_`, bloque de determinación de precio).

- Se reemplazó el uso del **buffer global `msg`** por un **buffer local en la pila
  `precBuf[128]`** para toda la lectura y parseo del precio: lecturas de `precio`/`precioN`,
  parseos de `basePrec` y `precio1`, el override de e-commerce/parking (`STRCPY`) y el
  truncado por balanza (`CantDigDecBal == 1`).
- Como `precBuf` vive en la pila de `plu_`, ningún otro contexto (logging ni el hilo serial
  de la balanza) puede pisarlo. Esto corta exactamente la ventana que producía `basePrec = 1.00`.
- Endurecimiento adicional: el truncado `precBuf[strlen(precBuf)-1] = '\0'` ahora valida
  `strlen(precBuf) > 0` (el código original hacía `msg[strlen(msg)-1]` sin chequear; con campo
  vacío escribía en `msg[-1]`).
- Sin cambios de lógica de precios ni dependencias aguas abajo (se verificó que después del
  bloque nadie lee el precio desde `msg`).

### Parche 2 (raíz) - Sincronización del evento serial de la balanza

**Archivo:** `MainForm.h` (handler `spBalan_DataReceived`).

- El handler corre en un hilo de fondo de .NET. Ahora se **serializa con el lazo principal**
  usando el mismo lock que toma `plu()` / `pago` / registración
  (`GCStatic::LockMainLoop("balanza")` / `ReleaseLockMainLoop`, mutex `PosProc`).
- Estructura en tres fases para evitar interbloqueo y pérdida de datos:
  1. `spBalan->ReadExisting()` **antes** de tomar el lock (drena el frame actual de inmediato).
  2. Toma el lock **solo** para actualizar el estado compartido (`LeePesoBalanza` /
     `LeePesoBalanzaSystel`, que escriben `pesoLeido`); se libera en un `finally`.
  3. `ActualizarPeso()` se llama **fuera del lock**, porque hace `Invoke` al hilo de UI y
     sostener el lock durante el marshalling podría provocar un **deadlock cruzado** con el
     hilo de interfaz.
- `PosProc` es un `Mutex` de .NET (reentrante por hilo); `WaitOne`/`ReleaseMutex` ocurren en
  el mismo hilo serial dentro de la misma invocación, y la balanza solo toma este lock (sin
  anidar otros) ? sin riesgo de orden de bloqueo.

**Fuera de alcance (no modificado):** `spControlBal_DataReceived` (báscula de control /
self-checkout) solo escribe `pesoBalanControl` con buffer local; no toca `pesoLeido`, `msg`
ni el camino de precio.

### Notas

- Los dos parches son independientes y acumulativos: el 1 corrige el síntoma exacto del
  ticket; el 2 elimina la condición de carrera para toda la clase de casos.
- Causa de fondo aún latente (mejora futura sugerida): el patrón de **globales compartidos**
  (`msg`, `usacod`, `xReg`, `yReg`, `pesoLeido`) entre el hilo principal y los hilos de fondo
  de .NET puede afectar otros campos; conviene ir migrando esos buffers a locales / protegerlos.

### Verificación

1. Compilar `pos` (Release|Win32).
2. Reproducir: dejar el producto sobre la balanza enviando peso continuo y teclear el código
   interno ? el precio unitario debe ser el real.
3. Revisar `BOOT.LOG`: ya no deben intercalarse valores espurios ("1.000") dentro de las
   secuencias `PLU-S1` / `PLU-S2`.
4. Si la caja usa self-checkout con `spControlBal`, confirmar que la báscula de control sigue
   refrescando con normalidad.

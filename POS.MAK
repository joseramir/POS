PROJ	=POS
DEBUG	=0
CC	=qcl
AS	=qcl
CFLAGS_G	= /AM /W3 /Ze /DINPOS 
CFLAGS_D	= /Zi /Od 
CFLAGS_R	= /O /Ot /Gs /DNDEBUG 
CFLAGS	=$(CFLAGS_G) $(CFLAGS_R)
AFLAGS_G	=/Cx /P1
AFLAGS_D	=/Zi
AFLAGS_R	=/DNDEBUG
AFLAGS	=$(AFLAGS_G) $(AFLAGS_R)
LFLAGS_G	= /CP:0xffff /NOI /SE:0x80 /ST:0x4e20 /SEGMENTS:150 /MAP 
LFLAGS_D	= /CO 
LFLAGS_R	= 
LFLAGS	=$(LFLAGS_G) $(LFLAGS_R)
RUNFLAGS	=
OBJS_EXT = 	obj\qsort.obj obj\bsearch.obj 
LIBS_EXT = 	

.asm.obj: ; $(AS) $(AFLAGS) -c $*.asm

all:	$(PROJ).EXE

alert.obj:	krnl\alert.c $(H)

compat.obj:	krnl\compat.c $(H)

digits.obj:	krnl\digits.c $(H)

dump.obj:	krnl\dump.c $(H)

dumperr.obj:	krnl\dumperr.c $(H)

enum.obj:	krnl\enum.c $(H)

horafec.obj:	krnl\horafec.c $(H)

messg.obj:	krnl\messg.c $(H)

mymath.obj:	krnl\mymath.c $(H)

opc.obj:	krnl\opc.c $(H)

pos.obj:	krnl\pos.c $(H)

cmos.obj:	posd\cmos.c $(H)

emax.obj:	posd\emax.c $(H)

p300.obj:	posd\p300.c $(H)

p930.obj:	posd\p930.c $(H)

posdisp.obj:	posd\posdisp.c $(H)

printer.obj:	posd\printer.c $(H)

cajero.obj:	posf\cajero.c $(H)

chmode.obj:	posf\chmode.c $(H)

corri.obj:	posf\corri.c $(H)

depto.obj:	posf\depto.c $(H)

envases.obj:	posf\envases.c $(H)

factu.obj:	posf\factu.c $(H)

ivas.obj:	posf\ivas.c $(H)

miscs.obj:	posf\miscs.c $(H)

mpago.obj:	posf\mpago.c $(H)

plu.obj:	posf\plu.c $(H)

premios.obj:	posf\premios.c $(H)

subtot.obj:	posf\subtot.c $(H)

db.obj:	..\db\db.c $(H)

idx.obj:	..\db\idx.c $(H)

elige.obj:	..\krnl\elige.c $(H)

event.obj:	..\krnl\event.c $(H)

ini.obj:	..\krnl\ini.c $(H)

kbd.obj:	..\krnl\kbd.c $(H)

mtask.obj:	..\krnl\mtask.c $(H)

serial.obj:	..\krnl\serial.c $(H)

wsup.obj:	..\krnl\wsup.c $(H)

scanner.obj:	posd\scanner.c $(H)

iofar.obj:	..\db\iofar.asm $(INC)

sleep.obj:	..\krnl\sleep.c $(H)

host.obj:	..\net\host.c $(H)

netproto.obj:	..\net\netproto.c $(H)

queue.obj:	..\net\queue.c $(H)

bitmap.obj:	..\krnl\bitmap.c $(H)

mozos.obj:	posf\mozos.c $(H)

$(PROJ).EXE:	alert.obj compat.obj digits.obj dump.obj dumperr.obj enum.obj horafec.obj \
	messg.obj mymath.obj opc.obj pos.obj cmos.obj emax.obj p300.obj p930.obj posdisp.obj \
	printer.obj cajero.obj chmode.obj corri.obj depto.obj envases.obj factu.obj ivas.obj \
	miscs.obj mpago.obj plu.obj premios.obj subtot.obj db.obj idx.obj elige.obj event.obj \
	ini.obj kbd.obj mtask.obj serial.obj wsup.obj scanner.obj iofar.obj sleep.obj host.obj \
	netproto.obj queue.obj bitmap.obj mozos.obj $(OBJS_EXT)
	echo >NUL @<<$(PROJ).crf
alert.obj +
compat.obj +
digits.obj +
dump.obj +
dumperr.obj +
enum.obj +
horafec.obj +
messg.obj +
mymath.obj +
opc.obj +
pos.obj +
cmos.obj +
emax.obj +
p300.obj +
p930.obj +
posdisp.obj +
printer.obj +
cajero.obj +
chmode.obj +
corri.obj +
depto.obj +
envases.obj +
factu.obj +
ivas.obj +
miscs.obj +
mpago.obj +
plu.obj +
premios.obj +
subtot.obj +
db.obj +
idx.obj +
elige.obj +
event.obj +
ini.obj +
kbd.obj +
mtask.obj +
serial.obj +
wsup.obj +
scanner.obj +
iofar.obj +
sleep.obj +
host.obj +
netproto.obj +
queue.obj +
bitmap.obj +
mozos.obj +
$(OBJS_EXT)
$(PROJ).EXE

$(LIBS_EXT);
<<
	qlink $(LFLAGS) @$(PROJ).crf

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


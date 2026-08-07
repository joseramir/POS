PROJ     = DBTEST
DEBUG    = 0
CC       = sc
AS       = qcl
CFLAGS_G = -mx -wx -NV -D_CONSOLE -w2 -C
CFLAGS_D = -g
CFLAGS_R = -DNDEBUG
CFLAGS   = $(CFLAGS_G) $(CFLAGS_R)
AFLAGS_G = /Cx /P1
AFLAGS_D = /Zi
AFLAGS_R = /DNDEBUG
AFLAGS   = $(AFLAGS_G) $(AFLAGS_R)
LFLAGS_G = /NOI /STACK:50000
LFLAGS_D = /co /map
LFLAGS_R = 
LFLAGS   = $(LFLAGS_G) $(LFLAGS_R)
RUNFLAGS =
OBJS_EXT = krnl\cx.obj
LIBS_EXT =  

all:	$(PROJ).EXE

dbtest.obj  : dbtest\dbtest.cpp 
            $(CC) $(CFLAGS) -c $? -o$@

db.obj      : db\db.cpp 
            $(CC) $(CFLAGS) -c $? -o$@

idx.obj     : db\idx.cpp 
            $(CC) $(CFLAGS) -c $? -o$@

xfile.obj   : db\xfile.cpp 
            $(CC) $(CFLAGS) -c $? -o$@

index_on.obj : db\index_on.cpp
            $(CC) $(CFLAGS) -c $? -o$@

$(PROJ).EXE : dbtest.obj db.obj idx.obj xfile.obj index_on.obj \
    $(OBJS_EXT)
    @echo $(OBJS_EXT)+ >linker.rsp
    @echo dbtest.obj+db.obj+idx.obj+xfile.obj+index_on.obj >>linker.rsp
    @echo dbtest.exe,,,pos.def, >>linker.rsp
    +link386 @linker.rsp $(LFLAGS)

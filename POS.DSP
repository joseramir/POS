# Microsoft Developer Studio Project File - Name="pos" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=pos - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pos.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pos.mak" CFG="pos - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pos - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "pos - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pos - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "\pos\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "INPOS" /D "POSDEBUG" /D "__VSC__" /YX /FD /c
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "pos - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "TEMP"
# PROP Intermediate_Dir "TEMP"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "\pos\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "INPOS" /D "POSDEBUG" /D "__VSC__" /FR /YX /FD /c
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x1f400 /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "pos - Win32 Release"
# Name "pos - Win32 Debug"
# Begin Group "Fuentes"

# PROP Default_Filter ""
# Begin Group "Kernel"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Krnl\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Dump.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Dumperr.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Enum.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Event.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Horafec.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Ini.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Kbd.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\KrnlTbl.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Messg.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Mtask.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Mymath.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Mystr.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Opc.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Pos.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\schedul.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Serial.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Sleep.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\Stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\sync.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\xstring.cpp
# End Source File
# End Group
# Begin Group "Screen Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\scrsup\aget.cpp
# End Source File
# Begin Source File

SOURCE=.\scrsup\Alert.cpp
# End Source File
# Begin Source File

SOURCE=.\Scrsup\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\scrsup\Digits.cpp
# End Source File
# Begin Source File

SOURCE=.\scrsup\Elige.cpp
# End Source File
# Begin Source File

SOURCE=.\Krnl\ssaver.cpp
# End Source File
# Begin Source File

SOURCE=.\Scrsup\termo.cpp
# End Source File
# Begin Source File

SOURCE=.\scrsup\Wsup.cpp
# End Source File
# Begin Source File

SOURCE=.\scrsup\xbrowse.cpp
# End Source File
# End Group
# Begin Group "Pos Functions"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Posf\Cajero.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Chmode.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Corri.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Depto.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Envases.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Factu.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Ivas.cpp
# End Source File
# Begin Source File

SOURCE=.\deftbl\maphand.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Miscs.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Mozos.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Mpago.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\pcomp.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Plu.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Premios.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\programa.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Subtot.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\suspend.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Tickfac.cpp
# End Source File
# Begin Source File

SOURCE=.\Posf\Variab.cpp
# End Source File
# End Group
# Begin Group "Pos Devices"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Posd\Cmos.cpp
# End Source File
# Begin Source File

SOURCE=.\Posd\Emax.cpp
# End Source File
# Begin Source File

SOURCE=.\Posd\P300.cpp
# End Source File
# Begin Source File

SOURCE=.\Posd\P930.cpp
# End Source File
# Begin Source File

SOURCE=.\Posd\Posdisp.cpp
# End Source File
# Begin Source File

SOURCE=.\Posd\PPR4.cpp
# End Source File
# Begin Source File

SOURCE=.\Posd\Printer.cpp
# End Source File
# Begin Source File

SOURCE=.\Posd\Scanner.cpp
# End Source File
# End Group
# Begin Group "xBase Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Db\create.cpp
# End Source File
# Begin Source File

SOURCE=.\Db\Db.cpp
# End Source File
# Begin Source File

SOURCE=.\Db\Idx.cpp
# End Source File
# Begin Source File

SOURCE=.\db\Index_on.cpp
# End Source File
# Begin Source File

SOURCE=.\Db\Xfile.cpp
# End Source File
# End Group
# Begin Group "Windows Support"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\winsup\winsup.cpp
# End Source File
# End Group
# End Group
# Begin Group "Includes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Include\Asigport.h
# End Source File
# Begin Source File

SOURCE=.\Include\Bitmap.h
# End Source File
# Begin Source File

SOURCE=.\Include\Cmos.h
# End Source File
# Begin Source File

SOURCE=.\Include\Compat.h
# End Source File
# Begin Source File

SOURCE=.\Include\Comprob.h
# End Source File
# Begin Source File

SOURCE=.\Include\Db.h
# End Source File
# Begin Source File

SOURCE=.\Include\debug.h
# End Source File
# Begin Source File

SOURCE=.\Include\Deftypes.h
# End Source File
# Begin Source File

SOURCE=.\Include\Error.h
# End Source File
# Begin Source File

SOURCE=.\Include\Event.h
# End Source File
# Begin Source File

SOURCE=.\Include\Funcs.h
# End Source File
# Begin Source File

SOURCE=.\Include\Io.h
# End Source File
# Begin Source File

SOURCE=.\Include\Ipx.h
# End Source File
# Begin Source File

SOURCE=.\Include\Kbd.h
# End Source File
# Begin Source File

SOURCE=.\Include\Limites.h
# End Source File
# Begin Source File

SOURCE=.\Include\maptbl.h
# End Source File
# Begin Source File

SOURCE=.\Include\Memcheck.h
# End Source File
# Begin Source File

SOURCE=.\Include\Mtask.h
# End Source File
# Begin Source File

SOURCE=.\Include\Myio.h
# End Source File
# Begin Source File

SOURCE=.\Include\Opc.h
# End Source File
# Begin Source File

SOURCE=.\Include\Pos.h
# End Source File
# Begin Source File

SOURCE=.\Include\Posdb.h
# End Source File
# Begin Source File

SOURCE=.\Include\Scrobj.h
# End Source File
# Begin Source File

SOURCE=.\Include\Serial.h
# End Source File
# Begin Source File

SOURCE=.\Include\Stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Include\Wsup.h
# End Source File
# Begin Source File

SOURCE=.\Include\xstring.h
# End Source File
# End Group
# End Target
# End Project

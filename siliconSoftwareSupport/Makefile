TOP=..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# Note, the files in this directory were manually copied from the Andor SDK directories 
# for Linux and Windows.  The soft links for Linux were manually created.

#copied from win32 version... assume win64 is same h files.


INC += SisoDisplay.h
INC += clser.h
INC += cxp.h
INC += fgrab_define.h
INC += fgrab_prototyp.h
INC += fgrab_struct.h
INC += gbe.h
INC += gbe_error.h
INC += msinttypes/inttypes.h
INC += os_funcs.h
INC += os_type.h
INC += sisoIo.h
INC += sisoboards.h
INC += msinttypes/stdint.h


ifeq (win32-x86, $(findstring win32-x86, $(T_A)))


LIB_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/lib/visualc/clsersis.lib
LIB_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/lib/visualc/display_lib.lib
LIB_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/lib/visualc/fglib5.lib
LIB_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/lib/visualc/iolibrt.lib
LIB_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/lib/visualc/gbelib.lib

BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/QtCore4.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/QtGui4.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/QtNetwork4.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/QtWebKit4.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/QtXmlPatterns4.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/clallserial.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/clsersis.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/common-logging-dispatcher.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/common-logging-log4cxx.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/display_lib.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/fglib5.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/gbelib.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/haprt.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/iolibrt.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/phonon4.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/qt-mt338.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/siso_hal.dll
BIN_INSTALLS_WIN32    += ../os/WIN32/Runtime5.2.0/bin/siso_hw.dll

else ifeq (windows-x64, $(findstring windows-x64, $(T_A)))

LIB_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/lib/visualc/clsersis.lib
LIB_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/lib/visualc/display_lib.lib
LIB_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/lib/visualc/fglib5.lib
LIB_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/lib/visualc/iolibrt.lib
LIB_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/lib/visualc/gbelib.lib

BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/QtCore4.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/QtGui4.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/QtNetwork4.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/QtWebKit4.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/QtXmlPatterns4.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/clsersis.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/common-logging-dispatcher.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/common-logging-log4cxx.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/display_lib.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/fglib5.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/gbelib.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/haprt.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/iolibrt.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/phonon4.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/qt-mt338.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/siso_hal.dll
BIN_INSTALLS_WIN32    += ../os/WIN64/Runtime5.2.0/bin/siso_hw.dll

#else ifeq (linux-x86_64, $(findstring linux-x86_64, $(T_A)))

#else ifeq (linux-x86, $(findstring linux-x86, $(T_A)))

endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


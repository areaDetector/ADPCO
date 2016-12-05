TOP=../..
include $(TOP)/configure/CONFIG

#----------------------------------------
#  
#
#PCO PCO PCO
#
#
#=============================

INC += pco.h

ifeq (windows-x64, $(findstring windows-x64, $(T_A)))
  LIBRARY_IOC = pco
endif

LIB_SRCS    += PCOSerial.cpp
LIB_SRCS    += pco.cpp
LIB_SRCS    += drvPcoEpics.c

LIB_SRCS += drvpcoEdgePluginEpics.c
LIB_SRCS += pcoEdgePlugin.cpp

#one or the other
#USR_CXXFLAGS += /D USE_SAP
#USR_CXXFLAGS_Linux += -DUSE_SISW

#USR_CXXFLAGS_WIN32 += /D USE_SISW
#USR_CXXFLAGS_WIN32 += /D USE_SAP

#to use internal serial port code, do not define this
#define to use seperate asynDruver for seruial port.
USR_CXXFLAGS_Linux += -D USEASYNSERIAL
USR_CXXFLAGS_WIN32 += /D USEASYNSERIAL

#define to use the log_file class in camLinkSrc2, else it uses a null sort of class that does notbhgin
USR_CXXFLAGS_WIN32 += /D USELOGFILE
USR_CXXFLAGS_Linux += -D USELOGFILE

# Note that the order in which .dbd files are added to a list variable
# doesn't matter, but the order of libraries does.

DBD += PCOSupport.dbd
DBD += pcoEdgePluginSupport.dbd

LIB_LIBS += cameralink
LIB_LIBS += NDPlugin

include $(ADCORE)/ADApp/commonLibraryMakefile

include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

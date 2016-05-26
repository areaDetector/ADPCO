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

#USR_INCLUDES += $(ADCAMERALINK)

LIBRARY_IOC = pco

#LIB_SRCS    +=legacystubs.c

LIB_SRCS    += PCOSerial.cpp
LIB_SRCS    += pco.cpp
LIB_SRCS    +=  drvPcoEpics.c


LIB_SRCS +=drvpcoEdgePluginEpics.c
LIB_SRCS +=pcoEdgePlugin.cpp



#one or the other
#USR_CXXFLAGS += /D USE_SAP
USR_CXXFLAGS_Linux += -DUSE_SISW

USR_CXXFLAGS_WIN32 += /D USE_SISW
#USR_CXXFLAGS_WIN32 += /D USE_SAP

#to use internal serial port code, do not define this
#define to use seperate asynDruver for seruial port.
USR_CXXFLAGS_Linux += -D USEASYNSERIAL
USR_CXXFLAGS_WIN32 += /D USEASYNSERIAL



#define to use the log_file class in camLinkSrc2, else it uses a null sort of class that does notbhgin
USR_CXXFLAGS_WIN32 += /D USELOGFILE
USR_CXXFLAGS_Linux += -D USELOGFILE





#when we build debug.. we need this line?
#!! for debvug only
PROD_LDFLAGS_WIN32 = /NODEFAULTLIB:LIBCMT.lib




PROD_LDFLAGS_WIN32 += /NODEFAULTLIB:msvcrt.lib


PROD_LDFLAGS_WIN32 += /VERBOSE:LIB

# Note that the order in which .dbd files are added to a list variable
# doesn't matter, but the order of libraries does.


#DBD += caputRecorder.dbd

#!!DBD += PCOApp.dbd
#DBD += PCOAppInclude.dbd

DBD += PCOSupport.dbd
DBD += pcoEdgePluginSupport.dbd

LIB_LIBS += cameralink caputRecorder

include $(ADCORE)/ADApp/commonLibraryMakefile


include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

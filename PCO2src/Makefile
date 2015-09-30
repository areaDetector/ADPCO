TOP=..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  
#
#PCO PCO PCO
#
#
#=============================


LIBRARY_IOC = pco

LIB_SRCS    += PCOSerial.cpp
LIB_SRCS    += pco.cpp
LIB_SRCS    +=  drvPcoEpics.c

#one or the other
#USR_CXXFLAGS += /D USE_SAP
USR_CXXFLAGS_LINUX += -D USE_SISW

USR_CXXFLAGS_WIN32 += /D USE_SISW
#USR_CXXFLAGS_WIN32 += /D USE_SAP

#to use internal serial port code, do not define this
#define to use seperate asynDruver for seruial port.
USR_CXXFLAGS_LINUX += -D USEASYNSERIAL
USR_CXXFLAGS_WIN32 += /D USEASYNSERIAL



PROD_IOC=PCOApp


#when we build debug.. we need this line?
#!! for debvug only
PROD_LDFLAGS_WIN32 = /NODEFAULTLIB:LIBCMT.lib




PROD_LDFLAGS_WIN32 += /NODEFAULTLIB:msvcrt.lib


PROD_LDFLAGS_WIN32 += /VERBOSE:LIB

# Note that the order in which .dbd files are added to a list variable
# doesn't matter, but the order of libraries does.


#DBD += caputRecorder.dbd

DBD += PCOApp.dbd
#DBD += PCOAppInclude.dbd
DBD += PCOSupport.dbd





#had to make with this....in the O.linux 64 dir... then tyope make in pco2src
#/usr/bin/g++ -o PCOApp -L/home/beams8/USER2BMB/tmadden/synApps_5_5/support/ADIOCs/lib/linux-x86_64 -L/home/beams/USER2BMB/tmadden/baseR12.1/lib/linux-x86_64 -L/home/beams/USER2BMB/tmadden/synApps_5_5/support/asyn4-21/lib/linux-x86_64 -L/home/beams/USER2BMB/tmadden/synApps_5_5/support/autosave-4-7/lib/linux-x86_64 -L/home/beams/USER2BMB/tmadden/synApps_5_5/support/busy-1-3/lib/linux-x86_64 -L/home/beams/USER2BMB/tmadden/synApps_5_5/support/calc-2-8/lib/linux-x86_64 -L/home/beams/USER2BMB/tmadden/synApps_5_5/support/sscan-2-6-6/lib/linux-x86_64 -Wl,-rpath,/home/beams8/USER2BMB/tmadden/synApps_5_5/support/ADIOCs/lib/linux-x86_64 -Wl,-rpath,/home/beams/USER2BMB/tmadden/baseR12.1/lib/linux-x86_64 -Wl,-rpath,/home/beams/USER2BMB/tmadden/synApps_5_5/support/asyn4-21/lib/linux-x86_64 -Wl,-rpath,/home/beams/USER2BMB/tmadden/synApps_5_5/support/autosave-4-7/lib/linux-x86_64 -Wl,-rpath,/home/beams/USER2BMB/tmadden/synApps_5_5/support/busy-1-3/lib/linux-x86_64 -Wl,-rpath,/home/beams/USER2BMB/tmadden/synApps_5_5/support/calc-2-8/lib/linux-x86_64 -Wl,-rpath,/home/beams/USER2BMB/tmadden/synApps_5_5/support/sscan-2-6-6/lib/linux-x86_64       -m64        -l display -l  gbe  -l clserme4 -l fglib5  -l haprt -L/opt/siso/lib        PCOApp_registerRecordDeviceDriver.o PCOAppMain.o   -lpco -lMaddenPlugins -lcamLinkSerial -lADCameralink  -lrecIoc -lsoftDevIoc -lmiscIoc -lrsrvIoc -ldbtoolsIoc -lasIoc -ldbIoc -lregistryIoc -ldbStaticIoc -lca -lCom -lNDPlugin -lADBase -lnetCDF -lcalc -lbusy -lsscan -lautosave -lasyn -lrecIoc -lsoftDevIoc -lmiscIoc -lrsrvIoc -ldbtoolsIoc -lasIoc -ldbIoc -lregistryIoc -ldbStaticIoc -lca -lCom -lPvAPI -lNeXus -lhdf5 -lsz -ltiff -ljpeg -lz -Wl,-Bdynamic  -lpthread   -lreadline -lm -lrt -ldl -lgcc



PROD_SRCS    += PCOApp_registerRecordDeviceDriver.cpp PCOAppMain.cpp   


PROD_LDFLAGS_WIN32 += /STACK:12000000,12000000



# for Dalsa card
#PROD_LDFLAGS_WIN32 += /LIBPATH:C:/DALSA/Sapera/Lib
#PROD_LDFLAGS_WIN32 += SapClassBasic.lib  corapi.lib 
#USR_INCLUDES_WIN32 += -I C:/DALSA/Sapera/Include
#USR_INCLUDES_WIN32 += -I C:/DALSA/Sapera/Classes/Basic

#for silicon SW card

PROD_LDFLAGS_WIN32 += /LIBPATH:$(TOP)/siliconSoftwareSupport/os/WIN64/Runtime5.2.0/lib/visualc
PROD_LDFLAGS_WIN32 +=  display_lib.lib  gbelib.lib  clsersis.lib  fglib5.lib  iolibrt.lib




#PROD_LDFLAGS_LINUX += -L/opt/siso/lib
PROD_LDFLAGS_LINUX +=  -l display -l  gbe  -l clserserme4 -l fglib5  -l haprt


PROD_LDFLAGS += /LIBPATH:C:\\Program\ Files\\HDF5-1.8.7_win_x64\\lib
PROD_LDFLAGS += hdf5.lib hdf5_cpp.lib zlib.lib libszip.lib

#PROD_LDFLAGS_WIN32 += /LIBPATH:C:\\Program\ Files\\HDF5-1.8.7_win_x64\\lib

PROD_LDFLAGS_LINUX += -L/opt/siso/lib


#PROD_LDFLAGS_WIN32 += hdf5.lib hdf5_cpp.lib zlib.lib libszip.lib

PROD_LIBS += pco

LIBS_LIBS += caputRecorder
PROD_LIBS += caputRecorder


PROD_LIBS += MaddenPlugins 


#PROD_LIBS        += PvAPI 



# Put additional areaDetector libraries here
PROD_LIBS +=  ADCameralink
PROD_LIBS +=  camLinkSerial


#PROD_LIBS +=display_lib
#PROD_LIBS +=gbelib
#PROD_LIBS +=clsersis
#PROD_LIBS +=fglib5
#PROD_LIBS +=iolibrt



PROD_LIBS += $(EPICS_BASE_IOC_LIBS)



USR_INCLUDES += -I $(AREA_DETECTOR)/ADApp/ADSrc
USR_INCLUDES += -I $(AREA_DETECTOR)/ADApp/

USR_INCLUDES += -I $(TOP)/cameraLinkSrc2



#USR_INCLUDES += -I $(TOP)/siliconSoftwareSupport/os/WIN64/Runtime5.2.0/include

include $(AREA_DETECTOR)/ADApp/commonDriverMakefile


include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

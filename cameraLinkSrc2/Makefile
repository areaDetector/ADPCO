TOP=..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  
#
#ADCameralink ADCameralink ADCameralink
#
#
#=============================


ifeq (windows-x64, $(findstring windows-x64, $(T_A)))

USR_CXXFLAGS_WIN32 += -D _WIN64



endif


#if linux and SISO
USR_CXXFLAGS_LINUX += -D USE_SISW

#one or the other
#USR_CXXFLAGS_WIN32 += /D USE_SAP
USR_CXXFLAGS_WIN32 += /D USE_SISW
#this is used for null software grabber for testing w/ no camera. define this AND USESAP to use sw grabber
#you define usesap on the pco camera driver, but not sw grab. onlu define this HERE.
#USR_CXXFLAGS_WIN32 += /D USE_SWGRAB


LIBRARY_IOC = ADCameralink
LIB_SRCS += ccd_exception.cpp
LIB_SRCS    += coreco.cpp
LIB_SRCS    += siswSerialPort.cpp


LIB_SRCS    += stopWatch.cpp
LIB_SRCS    += tifflib.cpp
LIB_SRCS    += siliconSoftware.cpp

LIB_SRCS    += softwareGrabber.cpp

LIB_SRCS    += image_ram2.cpp
LIB_SRCS    +=  image_file.cpp
LIB_SRCS    += cl_com_port2.cpp




LIB_SRCS    += logfile.cpp
LIB_SRCS    += ADCameralink.cpp





USR_INCLUDES += -I $(AREA_DETECTOR)/ADApp/ADSrc
USR_INCLUDES += -I $(AREA_DETECTOR)/ADApp/


#!!Never need this
USR_INCLUDES += -I $(TOP)/siliconSoftwareSupport/os/WIN64/Runtime5.2.0/include


#if building w/ dalsa
USR_INCLUDES += -I C:/DALSA/Sapera/Include
USR_INCLUDES += -I C:/DALSA/Sapera/Classes/Basic



include $(AREA_DETECTOR)/ADApp/commonDriverMakefile


include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

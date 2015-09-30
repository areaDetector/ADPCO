TOP=..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  
#
#camLinkSerial 
#
#
#=============================


LIBRARY_IOC = camLinkSerial

LIB_SRCS    += camLinkSerial.cpp



DBD += camLinkSerialSupport.dbd


#one or the other
USR_CXXFLAGS_WIN32 += /D USE_SISW
#USR_CXXFLAGS_WIN32 += /D USE_SAP


USR_CXXFLAGS_LINUX += -D USE_SISW



#USR_INCLUDES += -I C:/DALSA/Sapera/Include
#USR_INCLUDES += -I C:/DALSA/Sapera/Classes/Basic



USR_INCLUDES += -I $(TOP)/cameraLinkSrc2



#USR_INCLUDES += -I $(TOP)/siliconSoftwareSupport/os/WIN64/Runtime5.2.0/include

include $(AREA_DETECTOR)/ADApp/commonDriverMakefile


include $(TOP)/configure/RULES

#----------------------------------------
#  ADD RULES AFTER THIS LINE

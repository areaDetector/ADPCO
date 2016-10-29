TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE






PROD_NAME = pcoApp
ifeq (windows-x64, $(findstring windows-x64, $(T_A)))
  PROD_IOC_WIN32 += $(PROD_NAME)
endif
#PROD_IOC_Linux += $(PROD_NAME)


# <name>.dbd will be created from <name>Include.dbd
DBD += $(PROD_NAME).dbd
$(PROD_NAME)_DBD += PCOSupport.dbd
$(PROD_NAME)_DBD  += camLinkSerialSupport.dbd
$(PROD_NAME)_DBD  += pcoEdgePluginSupport.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
PROD_SRCS_DEFAULT += $(PROD_NAME)_registerRecordDeviceDriver.cpp $(PROD_NAME)Main.cpp

# Add locally compiled object code
PROD_LIBS += pco clsersis display_lib fglib5 gbelib iolibrt 

PROD_LIBS += cameralink caputRecorder

#!!$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))



include $(ADCORE)/ADApp/commonDriverMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


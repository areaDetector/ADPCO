TOP=..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# The following gets rid of the -fno-implicit-templates flag on vxWorks, 
# so we get automatic template instantiation.
# This is what we want for the source files that define template functions.
ifeq (vxWorks,$(findstring vxWorks, $(T_A)))
CODE_CXXFLAGS=
endif




#USR_INCLUDES += -I $(TOP)/cameraLinkSrc2



# These flags are needed by PvApi.h, the Prosilca library used for Bayer to RGB color conversion
USR_CXXFLAGS_Linux += -D_LINUX -D_x86 -O0

USR_CXXFLAGS_WIN32 += /D_WINDOWS


#define to use the log_file class in camLinkSrc2, else it uses a null sort of class that does notbhgin
USR_CXXFLAGS_WIN32 += /D USELOGFILE
USR_CXXFLAGS_LINUX += -D USELOGFILE



# These flags are used for Linux and WIN32 to build the Bayer conversion in NDPluginColorConvert
#USR_CXXFLAGS_Linux += -DHAVE_PVAPI
USR_CXXFLAGS_Darwin += -DHAVE_PVAPI
USR_CXXFLAGS_WIN32 += /DHAVE_PVAPI

# This flag is used to indicate that the TIFF library was built statically
USR_CXXFLAGS_WIN32 += /DLIBTIFF_STATIC


##!!TJM
#DBD += NDPluginDLL.dbd
#DBD += NDPluginJava.dbd
DBD += IMMpluginSupport.dbd
DBD += TinyTIFFpluginSupport.dbd
#DBD += HDF5XMLpluginSupport.dbd
DBD += simPluginSupport.dbd
#DBD += pcoEdgePluginSupport.dbd

DBD += immViewPluginSupport.dbd


##!!ENDTJM



LIBRARY_IOC = MaddenPlugins



##!!TJM

#USR_CXXFLAGS_WIN32 += -ID:/Java/jdk1.7.0/include/win32
#USR_CXXFLAGS_WIN32 += -ID:/Java/jdk1.7.0/include




LIB_SRCS+= NDFileIMM.cpp
LIB_SRCS+= compressed_file2.cpp
LIB_SRCS+= image_file2.cpp
LIB_SRCS+= drvNDFileIMMEpics.c
LIB_SRCS+= NDFileTinyTIFF.cpp
LIB_SRCS+= tinytiff.cpp
LIB_SRCS+= drvNDFileTinyTIFFEpics.c

LIB_SRCS+= imm.cpp

LIB_SRCS+= drvimmViewPluginEpics.c
LIB_SRCS+= immViewPlugin.cpp


#LIB_SRCS += NDPluginDLL.cpp
#LIB_SRCS += CJAreaDetector.cpp
#LIB_SRCS += NDPluginJava.cpp

#LIB_SRCS += hdf5Visitor.cpp
#LIB_SRCS += NDFileHDF5XML.cpp
#LIB_SRCS += drvNDFileHDF5XMLEpics.c

LIB_SRCS +=drvSimPluginEpics.c
LIB_SRCS +=simPlugin.cpp

#LIB_SRCS +=drvpcoEdgePluginEpics.c
#LIB_SRCS +=pcoEdgePlugin.cpp

##!!ENDTJM

USR_INCLUDES += -I $(AREA_DETECTOR)/ADApp/ADSrc
USR_INCLUDES += -I $(AREA_DETECTOR)/ADApp/

USR_INCLUDES += -I C:/"Program Files"/HDF5-1.8.7_win_x64/include


include $(AREA_DETECTOR)/ADApp/commonDriverMakefile

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE


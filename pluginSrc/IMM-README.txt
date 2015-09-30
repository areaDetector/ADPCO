Files needed for IMM plugin

NDFileIMM.cpp  
ccd_exception.cpp  
compressed_file2.cpp  
image_file2.cpp
NDFileIMM.h  
ccd_exception.h  
compressed_file2.h  
drvNDFileIMM.h  
image_file2.h
IMMpluginSupport.dbd



IMM requirements for Make file


DBD += IMMpluginSupport.dbd


LIB_SRCS+= NDFileIMM.cpp
LIB_SRCS+= compressed_file2.cpp
LIB_SRCS+= image_file2.cpp
LIB_SRCS+= drvNDFileIMMEpics.c



Gui and Records:

Put in the op/adl dir
NDFileIMM.adl

Put in the DB dir
NDFileIMM.template


Add thisline to NDPluginSupport.dbd
include "IMMpluginSupport.dbd"
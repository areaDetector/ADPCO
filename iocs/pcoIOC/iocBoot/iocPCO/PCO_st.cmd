# Must have loaded envPaths via st.cmd.linux or st.cmd.win32
< envPaths
errlogInit(20000)

dbLoadDatabase("$(AREA_DETECTOR)/dbd/pcoApp.dbd")
pcoApp_registerRecordDeviceDriver(pdbbase)




PCOConfig("PCOIOC", 2560, 2160, 3, 5000, 800715200,50,10000)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/coreco.template","P=PCOIOC:,R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")


dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/ADBase.template",     "P=PCOIOC:,R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/pco.template",     "P=PCOIOC:,R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")




# Create a standard arrays plugin, set it to get data from first PCO driver.
NDStdArraysConfigure("PCOIOCImage", 3, 0, "PCOIOC", 0, 30000000)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC:,R=image1:,PORT=PCOIOCImage,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDStdArrays.template", "P=PCOIOC:,R=image1:,PORT=PCOIOCImage,ADDR=0,TIMEOUT=1,SIZE=16,TYPE=Int16,FTVL=SHORT,NELEMENTS=6000000")



drvNDFileTinyTIFFConfigure("TIFF1", 3000, 0,"PCOIOC",0)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFile.template",      "P=PCOIOC:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFileTIFF.template",  "P=PCOIOC:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFileTinyTIFF.template",  "P=PCOIOC:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1")

dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/pco_metarecs.template",     "P=PCOIOC:,C=cam1:,R=TIFF1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")




# Create a NeXus file saving plugin
drvNDFileHDF5XMLConfigure("HDF5", 450, 0, "PCOIOC", 0)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC:,R=HDF5:,PORT=HDF5,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFile.template",      "P=PCOIOC:,R=HDF5:,PORT=HDF5,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFileNexus.template", "P=PCOIOC:,R=HDF5:,PORT=HDF5,ADDR=0,TIMEOUT=1")



dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/pco_metarecs.template",     "P=PCOIOC:,C=cam1:,R=HDF5:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")



# Create 4 ROI plugins
NDROIConfigure("ROI1", 20, 0, "PCOIOC", 0, -1, -1)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC:,R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDROI.template",       "P=PCOIOC:,R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1")



#setDllName "D:/Madden/convolutionFFT2D/Debug/FFTDLL.dll"
#NDDLLConfigure("DLL", 20, 0, "PCOIOC", 0, -1,-1,50,8000000)
#dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC:,R=DLL:,PORT=DLL,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
#dbLoadRecords("D:/Madden/convolutionFFT2D/FFTDLL/FFTDLL.template","P=PCOIOC:,R=DLL:,PORT=DLL,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")

#NDJavaConfigure("Java", 20, 0, "PCOIOC", 0, -1,-1,50,8000000)
#dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC:,R=Java:,PORT=Java,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")

# BEGIN save_restore.cmd ------------------------------------------------------

### save_restore setup
#
# This file does not require modification for standard use, but...

# status PVs
#save_restoreSet_UseStatusPVs(1)
save_restoreSet_status_prefix("PCOIOC:")

dbLoadRecords("D:/EPICS/ADEpics/synApps_5_5/support/autosave-4-7/asApp/Db/save_restoreStatus.db", "P=PCOIOC:, DEAD_SECONDS=5")

# Ok to save/restore save sets with missing values (no CA connection to PV)?
save_restoreSet_IncompleteSetsOk(1)

# Save dated backup files?
save_restoreSet_DatedBackupFiles(1)

# Number of sequenced backup files to write
save_restoreSet_NumSeqFiles(3)

# Time interval between sequenced backups
save_restoreSet_SeqPeriodInSeconds(300)

# specify where save files should be
set_savefile_path("D:/EPICS/ADEpics/iocs/PCO", "autosave")

###
# specify what save files should be restored.  Note these files must be
# in the directory specified in set_savefile_path(), or, if that function
# has not been called, from the directory current when iocInit is invoked
set_pass0_restoreFile("auto_settings.sav")
set_pass1_restoreFile("auto_settings.sav")

# Note that you can restore a .sav file without also autosaving to it.
#set_pass0_restoreFile("myInitData.sav")
#set_pass1_restoreFile("myInitData.sav")

###
# specify directories in which to to search for included request files
set_requestfile_path("./")
set_requestfile_path("../")
set_requestfile_path("D:/EPICS/ADEpics/iocs/PCO", "")
set_requestfile_path("D:/EPICS/ADEpics/iocs/PCO", "autosave")
set_requestfile_path("D:/EPICS/ADEpics/synApps_5_5/support/areaDetector-1-6", "ADApp/Db")
set_requestfile_path("D:/EPICS/ADEpics/synApps_5_5/support/areaDetector-1-6", "iocBoot")
set_requestfile_path("D:/EPICS/ADEpics/synApps_5_5/support/autosave-4-7", "asApp/Db")
#set_requestfile_path("$(CALC)", "calcApp/Db")
#set_requestfile_path("$(MCA)", "mcaApp/Db")
#set_requestfile_path("$(SSCAN)", "sscanApp/Db")
#set_requestfile_path("$(STD)", "stdApp/Db")
#set_requestfile_path("D:/EPICS/ADEpics", "SIMx86App/Db")

# Debug-output level
save_restoreSet_Debug(0) 


iocInit()

create_monitor_set("auto_settings.req", 30, "P=PCOIOC:")


dbpf "PCOIOC:cam1:EnableCallbacks","Yes"
dbpf "PCOIOC:cam1:ArrayCallbacks","Enable"
dbpf "PCOIOC:image1:EnableCallbacks","Yes"
dbpf "PCOIOC:image1:MinCallbackTime","0.2"




dbpf "PCOIOC:cam1:pco_comport_number","3"


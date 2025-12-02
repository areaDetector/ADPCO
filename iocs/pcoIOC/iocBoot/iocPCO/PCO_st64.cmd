< envPaths64


epicsEnvSet("QSIZE",  "5000")
errlogInit(20000)

dbLoadDatabase("$(TOP)/dbd/pcoApp.dbd")
PCOApp_registerRecordDeviceDriver(pdbbase)


drvCamlinkSerialConfigure("SERIAL","COM2");

PCOConfig("PCOIOC", "SERIAL",$(QSIZE), -1,50,100)

dbLoadRecords("$(ADIOCs)/Db/coreco.template","P=PCOIOC2:,R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")


dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/ADBase.template",     "P=PCOIOC2:,R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADIOCs)/Db/pco.template",     "P=PCOIOC2:,R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFile.template",      "P=PCOIOC2:,R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")




# Create a standard arrays plugin, set it to get data from first PCO driver.
NDStdArraysConfigure("PCOIOCImage", 3, 0, "PCOIOC", 0, 30000000)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC2:,R=image1:,PORT=PCOIOCImage,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDStdArrays.template", "P=PCOIOC2:,R=image1:,PORT=PCOIOCImage,ADDR=0,TIMEOUT=1,SIZE=16,TYPE=Int16,FTVL=SHORT,NELEMENTS=6000000")



drvNDFileTinyTIFFConfigure("TIFF1", $(QSIZE), 0,"PCOIOC",0)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC2:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFile.template",      "P=PCOIOC2:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFileTIFF.template",  "P=PCOIOC2:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADIOCs)/Db/NDFileTinyTIFF.template",  "P=PCOIOC2:,R=TIFF1:,PORT=TIFF1,ADDR=0,TIMEOUT=1")
#Turn off metarecs for tiff because we are using the nick software
#dbLoadRecords("$(ADIOCs)/Db/pco_metarecs.template",     "P=PCOIOC2:,C=cam1:,R=TIFF1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")




# Create a NeXus file saving plugin
drvNDFileHDF5XMLConfigure("HDF5", $(QSIZE), 0, "PCOIOC", 0)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC2:,R=HDF5:,PORT=HDF5,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFile.template",      "P=PCOIOC2:,R=HDF5:,PORT=HDF5,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADIOCs)/Db/NDFileHDF5XML.template", "P=PCOIOC2:,R=HDF5:,PORT=HDF5,ADDR=0,TIMEOUT=1")
#dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDFileNexus.template", "P=PCOIOC2:,R=HDF5:,PORT=HDF5,ADDR=0,TIMEOUT=1")




#dbLoadRecords("$(ADIOCs)/Db/pco_metarecs.template",     "P=PCOIOC2:,C=cam1:,R=HDF5:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")



# Create 4 ROI plugins
NDROIConfigure("ROI1", 20, 0, "PCOIOC", 0, -1, -1)
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDPluginBase.template","P=PCOIOC2:,R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(AREA_DETECTOR)/ADApp/Db/NDROI.template",       "P=PCOIOC2:,R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1")





# BEGIN save_restore.cmd ------------------------------------------------------

### save_restore setup
#
# This file does not require modification for standard use, but...

# status PVs
#save_restoreSet_UseStatusPVs(1)
save_restoreSet_status_prefix("PCOIOC2:")

dbLoadRecords("D:/EPICS/ADEpics/synApps_5_5/support/autosave-4-7/asApp/Db/save_restoreStatus.db", "P=PCOIOC2:, DEAD_SECONDS=5")

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

create_monitor_set("auto_settings.req", 30, "P=PCOIOC2:")


dbpf "PCOIOC2:cam1:EnableCallbacks","Yes"
dbpf "PCOIOC2:cam1:ArrayCallbacks","Enable"
dbpf "PCOIOC2:image1:EnableCallbacks","Yes"
dbpf "PCOIOC2:image1:MinCallbackTime","0.2"





dbpf "PCOIOC2:cam1:pco_comport_number","2"


epicsThreadSleep 5







#dbpf "PCOIOC2:cam1:pco_reset_default_settings","1"
#dbpf "PCOIOC2:cam1:cor_ccf_filename","D:/corecofiles/P_Edge_5120_2160_.ccf"
dbpf "PCOIOC2:cam1:cor_ccf_filename","D:/corecofiles/dimaxSISW.mcf"

#needed for sisw grabber . set to 0 for coreco grabber. hasto do with successive calls to serial_port->write
dbpf "PCOIOC2:cam1:pco_ser_waitms","10"


dbpf "PCOIOC2:cam1:w_is_sleep","1"
 dbpf "PCOIOC2:cam1:w_sleep_ms","50"
 
dbpf "PCOIOC2:cam1:pco_grab_waittime","5.0"


dbpf "PCOIOC2:cam1:pco_baudrate","9600"

dbpf "PCOIOC2:cam1:w_open_com","1"

dbpf "PCOIOC2:cam1:cor_num_coreco_buffers","16"


#dbpf "PCOIOC2:cam1:SizeX","1000"
#dbpf "PCOIOC2:cam1:SizeY","1000"
dbpf "PCOIOC2:cam1:cor_use_image_mode","1"

dbpf "PCOIOC2:cam1:pco_reconfig_grabber","1"






dbpf "PCOIOC2:cam1:AcquireTime_RBV.PREC","6"
dbpf "PCOIOC2:cam1:AcquireTime.PREC","6"

dbpf "PCOIOC2:cam1:AcquirePeriod_RBV.PREC","6"
dbpf "PCOIOC2:cam1:AcquirePeriod.PREC","6"

dbpf "PCOIOC2:cam1:pco_delay_time.PREC","6"
dbpf "PCOIOC2:cam1:pco_delay_time_RBV.PREC","6"

dbpf "PCOIOC2:cam1:pco_set_frame_rate.PREC","6"
dbpf "PCOIOC2:cam1:pco_set_frame_rate_RBV.PREC","6"


dbpf "PCOIOC2:HDF5:EZ_is_makedirs","1"

dbpf "PCOIOC2:TIFF1:EZ_is_makedirs","1"






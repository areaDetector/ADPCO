< envPaths64

epicsEnvSet("PREFIX",  "PCOIOC2:")


epicsEnvSet("QSIZE",  "5000")

errlogInit(20000)

dbLoadDatabase("$(PCO_IOC)/dbd/pcoApp.dbd")
pcoApp_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("$(PCO_APP)/Db/debug.template",     "P=$(PREFIX),R=cam1:")




drvCamlinkSerialConfigure("SERIAL","COM2");

PCOConfig("PCOIOC", "SERIAL",$(QSIZE), -1,50,100)
dbLoadRecords("$(ADCAMERALINK)/db/coreco.template","P=$(PREFIX),R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADAPP)/Db/ADBase.template",     "P=$(PREFIX),R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(PCO_APP)/Db/pco.template",     "P=$(PREFIX),R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")
dbLoadRecords("$(ADAPP)/Db/NDFile.template",      "P=$(PREFIX),R=cam1:,PORT=PCOIOC,ADDR=0,TIMEOUT=1")




drvpcoEdgePluginConfigure("EDGEDSC",$(QSIZE),0,"PCOIOC",0,50,0);
dbLoadRecords("$(ADAPP)/Db/NDPluginBase.template","P=$(PREFIX),R=EDGEDSC:,PORT=EDGEDSC,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(PCO_APP)/Db/pcoEdgePlugin.template",     "P=$(PREFIX),R=EDGEDSC:,PORT=EDGEDSC,ADDR=0,TIMEOUT=1")



# Create a standard arrays plugin, set it to get data from first PCO driver.
NDStdArraysConfigure("PCOIOCImage", 3, 0, "PCOIOC", 0, 30000000)
dbLoadRecords("$(ADAPP)/Db/NDPluginBase.template","P=$(PREFIX),R=image1:,PORT=PCOIOCImage,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(ADAPP)/Db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=PCOIOCImage,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,SIZE=16,TYPE=Int16,FTVL=SHORT,NELEMENTS=6000000")



# Create 4 ROI plugins
NDROIConfigure("ROI1", 20, 0, "PCOIOC", 0, -1, -1)
dbLoadRecords("$(ADAPP)/Db/NDPluginBase.template","P=$(PREFIX),R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(ADAPP)/Db/NDROI.template",       "P=$(PREFIX),R=ROI1:,  PORT=ROI1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC")

NDFileHDF5Configure("HDF5",10000,0,"PCOIOC",0,50,0)
dbLoadRecords("$(ADAPP)/Db/NDPluginBase.template","P=$(PREFIX),R=HDF5:,  PORT=HDF5,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC,NDARRAY_ADDR=0")
dbLoadRecords("$(ADAPP)/Db/NDFileHDF5.template",       "P=$(PREFIX),R=HDF5:,  PORT=HDF5,ADDR=0,TIMEOUT=1,NDARRAY_PORT=PCOIOC")




#
# caput recorder stuff
#

asSetFilename("$(PCO_IOC)/iocBoot/iocPCO/accessSecurity.acf")
dbLoadRecords("$(CAPUTRECORDER)/caputRecorderApp/Db/caputPoster.db","P=$(PREFIX),N=300")



# BEGIN save_restore.cmd ------------------------------------------------------

### save_restore setup
#
# This file does not require modification for standard use, but...
save_restoreSet_status_prefix("$(PREFIX)")

dbLoadRecords("$(SUPPORT)/autosave-4-7/asApp/Db/save_restoreStatus.db", "P=$(PREFIX), DEAD_SECONDS=5")

# Ok to save/restore save sets with missing values (no CA connection to PV)?
save_restoreSet_IncompleteSetsOk(1)

# Save dated backup files?
save_restoreSet_DatedBackupFiles(1)

# Number of sequenced backup files to write
save_restoreSet_NumSeqFiles(3)

# Time interval between sequenced backups
save_restoreSet_SeqPeriodInSeconds(300)

# specify where save files should be
set_savefile_path("$(REQPATH)/PCOEdge", "autosave")

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
set_requestfile_path("$(REQPATH)/PCOEdge", "")
set_requestfile_path("$(REQPATH)/PCOEdge", "autosave")

set_requestfile_path("$(REQPATH)/PCO", "")
set_requestfile_path("$(REQPATH)/PCO", "autosave")
set_requestfile_path("$(ADAPP)/", "Db")
set_requestfile_path("$(AUTOSAVE)", "asApp/Db")

# Debug-output level
save_restoreSet_Debug(0) 


iocInit()

create_monitor_set("auto_settings.req", 30, "P=$(PREFIX)")


registerCaputRecorderTrapListener("$(PREFIX)caputRecorderCommand")


dbpf "$(PREFIX)cam1:EnableCallbacks","Yes"
dbpf "$(PREFIX)cam1:ArrayCallbacks","Enable"
dbpf "$(PREFIX)image1:EnableCallbacks","Yes"
dbpf "$(PREFIX)image1:MinCallbackTime","0.2"





dbpf "$(PREFIX)cam1:pco_comport_number","2"


epicsThreadSleep 5



dbpf "$(PREFIX)cam1:pco_reset_default_settings","1"
#dbpf "$(PREFIX)cam1:cor_ccf_filename","pcoEdge.ccf"
dbpf "$(PREFIX)cam1:cor_ccf_filename","edgeRollShutter.mcf"

#needed for edge- for rolling / global shutter mode
dbpf "$(PREFIX)cam1:pco_rollshut_mcfname","edgeRollShutter.mcf"
dbpf "$(PREFIX)cam1:pco_globshut_mcfname","edgeGlobShutter.mcf"



#needed for sisw grabber . set to 0 for coreco grabber. hasto do with successive calls to serial_port->write
dbpf "$(PREFIX)cam1:pco_ser_waitms","10"





dbpf "$(PREFIX)cam1:w_is_sleep","1"
 dbpf "$(PREFIX)cam1:w_sleep_ms","50"
 
dbpf "$(PREFIX)cam1:pco_grab_waittime","5.0"


dbpf "$(PREFIX)cam1:pco_baudrate","9600"

dbpf "$(PREFIX)cam1:w_open_com","1"

dbpf "$(PREFIX)cam1:cor_num_coreco_buffers","16"

dbpf "$(PREFIX)cam1:cor_use_image_mode","1"

dbpf "$(PREFIX)cam1:pco_reconfig_grabber","1"






dbpf "$(PREFIX)cam1:AcquireTime_RBV.PREC","6"
dbpf "$(PREFIX)cam1:AcquireTime.PREC","6"

dbpf "$(PREFIX)cam1:AcquirePeriod_RBV.PREC","6"
dbpf "$(PREFIX)cam1:AcquirePeriod.PREC","6"

dbpf "$(PREFIX)cam1:pco_delay_time.PREC","6"
dbpf "$(PREFIX)cam1:pco_delay_time_RBV.PREC","6"

dbpf "$(PREFIX)cam1:pco_set_frame_rate.PREC","6"
dbpf "$(PREFIX)cam1:pco_set_frame_rate_RBV.PREC","6"














dbpf "$(PREFIX)cam1:w_is_sleep","1"
 dbpf "$(PREFIX)cam1:w_sleep_ms","50"
 
dbpf "$(PREFIX)cam1:pco_grab_waittime","5.0"


dbpf "$(PREFIX)cam1:pco_baudrate","9600"

dbpf "$(PREFIX)cam1:w_open_com","1"

dbpf "$(PREFIX)cam1:cor_num_coreco_buffers","16"



dbpf "$(PREFIX)cam1:pco_reconfig_grabber","1"


#epicsThreadSleep(10.0)
#set global shutter
dbpf "$(PREFIX)cam1:pco_global_shutter","Global"


dbpf "$(PREFIX)EDGEDSC:EnableCallbacks","Enable"

#enable plugin to do descrambling
dbpf "$(PREFIX)EDGEDSC:is_enable","1"
# handle descrambling in a plugin and not driver.
dbpf "$(PREFIX)cam1:pco_disable_descramble","1"
#tell plugin to get control settings from img atrrubutes, from driver. if you control driver
# you control plugin
dbpf "$(PREFIX)cam1:is_use_attr","1"

dbpf "$(PREFIX)cam1:AcquireTime_RBV.PREC","6"
dbpf "$(PREFIX)cam1:AcquireTime.PREC","6"

dbpf "$(PREFIX)cam1:AcquirePeriod_RBV.PREC","6"
dbpf "$(PREFIX)cam1:AcquirePeriod.PREC","6"

dbpf "$(PREFIX)cam1:pco_delay_time.PREC","6"
dbpf "$(PREFIX)cam1:pco_delay_time_RBV.PREC","6"

dbpf "$(PREFIX)cam1:pco_set_frame_rate.PREC","6"
dbpf "$(PREFIX)cam1:pco_set_frame_rate_RBV.PREC","6"


dbpf "$(PREFIX)HDF5:NDArrayPort","EDGEDSC"
dbpf "$(PREFIX)image1:NDArrayPort","EDGEDSC"




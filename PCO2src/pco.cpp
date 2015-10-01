#include "pco.h"

static const char *driverName = "pco";


//debugging flags
extern "C" int pco_blocking=0;
extern "C" int pco_pr_mutex=0;
extern "C" int pco_pr_mutex2=0;




static void pcoTaskC(void *drvPvt)
{
    pco *pPvt = (pco *)drvPvt;

    pPvt->pcoTask();
}

static void pcoTaskC2(void *drvPvt)
{
    pco *pPvt = (pco *)drvPvt;

    pPvt->pcoTask2();
}




void pco::setDebuggingMessage(int level)
{
    dbg_msg_level=level;

}






/** Called when asyn clients call pasynInt32->write().
  * This function performs actions for some parameters, including ADAcquire, ADBinX, etc.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus pco::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
   
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   char mesx[256];
   char *mesx2;
    

   status = setIntegerParam(function, value);
   
   
   getParamName(function,(const char**) &mesx2);
   
     sprintf(mesx,"pco::writeInt32, param=%s, value=%d",mesx2,value);
   lf.log(mesx);

   
   
  if (function == pco_kill_ioc && value ==1)
    exit(0);

//ret if we actulayyl updated so we can call super if necessary
   updateParameters(
        pasynUser, 
        value,
        0,
        pco_paramint);
        
        
    //call super class to update pawram

    ADCameralink::writeInt32(pasynUser, value);


    callParamCallbacks();
    

        status=asynSuccess;
    return(status);

}







/** Called when asyn clients call pasynFloat64->write().
  * This function performs actions for some parameters.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus pco::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   
   status = setDoubleParam(function, value);
   
   char mesx[256];
   char *mesx2;
   
   getParamName(function, (const char**)&mesx2);
   
     sprintf(mesx,"pco::writeFloat64, param=%s, value=%f",mesx2,value);
   lf.log(mesx);

//ret if we actulayyl updated so we can call super if necessary
   updateParameters(
        pasynUser, 
        0,
        value,
        pco_paramdoub);
        
        
    //call super class to update pawram
   ADCameralink::writeFloat64(pasynUser,  value);
    callParamCallbacks();

    
    return(status);
 }



/** Report status of the driver.
  * Prints details about the driver if details>0.
  * It then calls the ADDriver::report() method.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details If >0 then driver details are printed.
  */
void pco::report(FILE *fp, int details)
{
    fprintf(fp, "MAR-CCD detector %s\n", this->portName);
    if (details > 0) {
        int nx, ny;
        getIntegerParam(ADSizeX, &nx);
        getIntegerParam(ADSizeY, &ny);
        fprintf(fp, "  NX, NY:            %d  %d\n", nx, ny);
    }
    /* Invoke the base class method */
    ADDriver::report(fp, details);
}

extern "C" int PCOConfig(const char *portName, const char *serverPort,
                            int maxBuffers, size_t maxMemory,
                            int priority, int stackSize)
{
    size_t maxmemBytes;
    
    maxmemBytes=1024*1024*maxMemory;
    
    new pco(portName, serverPort, maxBuffers, maxmemBytes, priority, stackSize);
    return(asynSuccess);
}

/** Constructor for pco driver; most parameters are simply passed to ADDriver::ADDriver.
  * After calling the base class constructor this method creates a thread to collect the detector data,
  * and sets reasonable default values the parameters defined in this class, asynNDArrayDriver, and ADDriver.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] serverPort The name of the asyn port driver previously created with drvAsynIPPortConfigure
  *            connected to the pco_server program.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
pco::pco(const char *portName, const char *serverPort,
                                int maxBuffers, size_t maxMemory,
                                int priority, int stackSize)

    : ADCameralink(
        portName, 
        serverPort,
        NUM_pco_PARAMS,
        maxBuffers,  
        maxMemory,
        priority,  
        stackSize),
        time1(),
        time2(),
        grab_delay(),
        image_timer()
  {


    
    int status;
    
    char mesg[256];
    
    int err;
    lf.log("PCO construcitng");

    
    sprintf(mesg,"PCO Compile date %s, Time %s",__DATE__,__TIME__);
    lf.log(mesg);

    
    
    strcpy(this->myServerPort,serverPort);

        //enough for edge dimax and thensome
    temp_image=(unsigned short*)malloc(2560*2200*sizeof(short));

    

    createParam("pco_dimax_nimgs_fudge",asynParamInt32,&pco_dimax_nimgs_fudge);
    
    createParam("pco_conv_12to16",asynParamInt32,&pco_conv_12to16);

    createParam("pco_rollshut_mcfname",asynParamOctet,&pco_rollshut_mcfname);
    createParam("pco_globshut_mcfname",asynParamOctet,&pco_globshut_mcfname);
 
    
    createParam("pco_global_shutter",asynParamInt32,&pco_global_shutter);


    createParam("pco_disable_descramble",asynParamInt32,&pco_disable_descramble);

    createParam("pco_favor_dlytime",asynParamInt32,&pco_favor_dlytime);



    createParam("pco_ready2acquire",asynParamInt32,&pco_ready2acquire);
    createParam("pco_grab_waittime",asynParamFloat64,&pco_grab_waittime);

    createParam("pco_dbg_serwrite",asynParamInt32,&pco_dbg_serwrite);

    createParam("pco_dbg_serread",asynParamInt32,&pco_dbg_serread);
    createParam("pco_dbg_serstr",asynParamOctet,&pco_dbg_serstr);

    createParam("pco_do_camcontrol",asynParamInt32,&pco_do_camcontrol);

    createParam("pco_dbg_serprint",asynParamInt32,&pco_dbg_serprint);
    createParam("pco_cancel_dump",asynParamInt32,&pco_cancel_dump);
    //kB/sec
    createParam("pco_dump_maxdatarate",asynParamInt32,&pco_dump_maxdatarate);
    createParam("pco_frame_rate",asynParamFloat64,&pco_frame_rate);
    createParam("pco_reconfig_grabber",asynParamInt32,&pco_reconfig_grabber);
    createParam("pco_check_time",asynParamInt32,&pco_check_time);

     createParam("pco_play_stop",asynParamInt32,&pco_play_stop);
     createParam("pco_play_slow",asynParamInt32,&pco_play_slow);

     createParam("pco_is_frame_rate_mode",asynParamInt32,&pco_is_frame_rate_mode);

     createParam("pco_cdi_mode",asynParamInt32,&pco_cdi_mode);
     createParam("pco_dnsu_mode",asynParamInt32,&pco_dnsu_mode);
     createParam("pco_dnsu_init_mode",asynParamInt32,&pco_dnsu_init_mode);

     createParam("pco_set_frame_rate",asynParamFloat64,&pco_set_frame_rate);
        createParam("pco_delay_time",asynParamFloat64,&pco_delay_time);
     // set callback to do nothing default....
    //putPtrParam(serial_callback,(const void*);genCamController::defaultCallback);
    createParam("pco_baudrate",asynParamInt32,&pco_baudrate);
    createParam("pco_trigger_mode",asynParamInt32,&pco_trigger_mode);
    createParam("pco_force_check",asynParamInt32,&pco_force_check);
    createParam("pco_camlink_pixfreq",asynParamInt32,&pco_camlink_pixfreq);
    createParam("pco_cclines",asynParamInt32,&pco_cclines);
    createParam("pco_camlink_pixperclk",asynParamInt32,&pco_camlink_pixperclk);
    createParam("pco_camlink_cont",asynParamInt32,&pco_camlink_cont);
    createParam("pco_reset_memory",asynParamInt32,&pco_reset_memory);

    createParam("pco_force_trigger",asynParamInt32,&pco_force_trigger);
    createParam("pco_dump_camera_memory",asynParamInt32,&pco_dump_camera_memory);
    createParam("pco_imgs2dump",asynParamInt32,&pco_imgs2dump);
    createParam("pco_memreq_cnt",asynParamInt32,&pco_memreq_cnt);

    createParam("pco_live_view",asynParamInt32,&pco_live_view);
    createParam("pco_dump_counter",asynParamInt32,&pco_dump_counter);
    createParam("pco_dump_waitms",asynParamInt32,&pco_dump_waitms);

    createParam("pco_health_error",asynParamInt32,&pco_health_error);
    createParam("pco_health_warning",asynParamInt32,&pco_health_warning);
    createParam("pco_health_status",asynParamInt32,&pco_health_status);
    createParam("pco_check_time2",asynParamInt32,&pco_check_time2);

            createParam("pco_reset_default_settings",asynParamInt32,&pco_reset_default_settings);
            createParam("pco_init_selftest",asynParamInt32,&pco_init_selftest);

        

            createParam("pco_sensor_format",asynParamInt32,&pco_sensor_format);
            createParam("pco_pixelrate",asynParamInt32,&pco_pixelrate);
            createParam("pco_doub_img_mode",asynParamInt32,&pco_doub_img_mode);
            createParam("pco_adc_mode",asynParamInt32,&pco_adc_mode);
            createParam("pco_temp_setpt",asynParamInt32,&pco_temp_setpt);
            createParam("pco_offset_mode",asynParamInt32,&pco_offset_mode);
            createParam("pco_noise_filt_mode",asynParamInt32,&pco_noise_filt_mode);

            createParam("pco_exp_timebase",asynParamInt32,&pco_exp_timebase);
            createParam("pco_dly_timebase",asynParamInt32,&pco_dly_timebase);
            createParam("pco_fps_mode",asynParamInt32,&pco_fps_mode);
            createParam("pco_busy",asynParamInt32,&pco_busy);
            createParam("pco_exp_trig_stat",asynParamInt32,&pco_exp_trig_stat);

            createParam("pco_camera_tot_ram_size",asynParamInt32,&pco_camera_tot_ram_size);
            createParam("pco_camera_ram_npages",asynParamInt32,&pco_camera_ram_npages);
            createParam("pco_camera_ram_page_size",asynParamInt32,&pco_camera_ram_page_size);
            createParam("pco_camera_seg_size0",asynParamInt32,&pco_camera_seg_size0);
            createParam("pco_camera_seg_size1",asynParamInt32,&pco_camera_seg_size1);
            createParam("pco_camera_seg_size2",asynParamInt32,&pco_camera_seg_size2);
            createParam("pco_camera_seg_size3",asynParamInt32,&pco_camera_seg_size3);
            createParam("pco_clear_ram_seg",asynParamInt32,&pco_clear_ram_seg);
            createParam("pco_active_seg",asynParamInt32,&pco_active_seg);

            createParam("pco_storage_mode",asynParamInt32,&pco_storage_mode);
            createParam("pco_rec_submode",asynParamInt32,&pco_rec_submode);
            createParam("pco_rec_status",asynParamInt32,&pco_rec_status);
            createParam("pco_acq_mode",asynParamInt32,&pco_acq_mode);
            createParam("pco_acqen_sig_stat",asynParamInt32,&pco_acqen_sig_stat);
            createParam("pco_timestamp_mode",asynParamInt32,&pco_timestamp_mode);
            createParam("pco_arm_camera",asynParamInt32,&pco_arm_camera);
            createParam("pco_rec_stop_event_mode",asynParamInt32,&pco_rec_stop_event_mode);
            createParam("pco_rec_stop_event_nimgs",asynParamInt32,&pco_rec_stop_event_nimgs);
            createParam("pco_stop_record",asynParamInt32,&pco_stop_record);



            createParam("pco_read_which_seg",asynParamInt32,&pco_read_which_seg);
            createParam("pco_read_st_img",asynParamInt32,&pco_read_st_img);
            createParam("pco_read_end_img",asynParamInt32,&pco_read_end_img);
            createParam("pco_do_read_imgs",asynParamInt32,&pco_do_read_imgs);
            createParam("pco_req_img",asynParamInt32,&pco_req_img);
            createParam("pco_rpt_img",asynParamInt32,&pco_rpt_img);
            createParam("pco_cancel_img",asynParamInt32,&pco_cancel_img);
            createParam("pco_bit_alignment",asynParamInt32,&pco_bit_alignment);

            createParam("pco_num_imgs_seg0",asynParamInt32,&pco_num_imgs_seg0);
            createParam("pco_max_imgs_seg0",asynParamInt32,&pco_max_imgs_seg0);
            createParam("pco_num_imgs_seg1",asynParamInt32,&pco_num_imgs_seg1);
            createParam("pco_max_imgs_seg1",asynParamInt32,&pco_max_imgs_seg1);
            createParam("pco_num_imgs_seg2",asynParamInt32,&pco_num_imgs_seg2);
            createParam("pco_max_imgs_seg2",asynParamInt32,&pco_max_imgs_seg2);
            createParam("pco_num_imgs_seg3",asynParamInt32,&pco_num_imgs_seg3);
            createParam("pco_max_imgs_seg3",asynParamInt32,&pco_max_imgs_seg3);

    createParam("pco_ser_waitms",asynParamInt32,&pco_ser_waitms);

    createParam("pco_hotpix_corr",asynParamInt32,&pco_hotpix_corr);
    createParam("pco_1612_lookup",asynParamInt32,&pco_1612_lookup);
    createParam("pco_edge_fastscan",asynParamInt32,&pco_edge_fastscan);


    /// init all params or else the client will not see them...they will be undefined...
    createParam("pco_is_running",asynParamInt32,&pco_is_running);
    //putPtrParam(log_file_ptr, (void*);&lf);
    createParam("pco_is_log",asynParamInt32,&pco_is_log);
    createParam("pco_run_counter",asynParamInt32,&pco_run_counter);
    createParam("w_sleep_ms",asynParamInt32,&pco_sleep_ms);
    createParam("w_is_sleep",asynParamInt32,&pco_is_sleep);
    //putPtrParam(coreco_control_ptr,0);// need to set this up leter...

    createParam("pco_comport_number",asynParamInt32,&pco_comport_number);
    
    createParam("com_port_name",asynParamOctet,&com_port_name);
    createParam("w_open_com",asynParamInt32,&open_com);
    createParam("w_close_com",asynParamInt32,&close_com);
    createParam("w_is_com_open",asynParamInt32,&is_com_open);

    createParam("pco_setallparams",asynParamInt32,&pco_setallparams);

    createParam("pco_descramble",asynParamInt32,&pco_descramble);

    createParam("pco_which_camera",asynParamInt32,&pco_which_camera);

    createParam("pco_array_counter",asynParamInt32,&pco_array_counter);
    createParam("pco_test_pixels",asynParamInt32,&pco_test_pixels);

    createParam("pco_image_rate",asynParamFloat64,&pco_image_rate);
createParam("pco_com_err_max",asynParamInt32,&pco_com_err_max);

    createParam("pco_kill_ioc",asynParamInt32,&pco_kill_ioc);

    createParam("pco_double_image_error",asynParamInt32,&pco_double_image_error);
    
    

///
///
///
///
///


    
setStringParam(pco_globshut_mcfname,"D:/corecofiles/edgeGlobShutter.mcf");

setStringParam(pco_rollshut_mcfname,"D:/corecofiles/timmaddenedge.mcf");



setIntegerParam(pco_dimax_nimgs_fudge,1);


setIntegerParam(pco_conv_12to16,0);

setIntegerParam(pco_global_shutter,0);

setIntegerParam(pco_disable_descramble,0);
setIntegerParam(pco_kill_ioc,0);

setIntegerParam(pco_com_err_max,2);

setDoubleParam(asynParamFloat64,0.0);

setIntegerParam(pco_test_pixels,0);

setIntegerParam(pco_test_pixels,0);
setIntegerParam(pco_array_counter,0);

    
    setIntegerParam(pco_setallparams,0);


    setIntegerParam(pco_favor_dlytime,0);



    setIntegerParam(pco_ready2acquire,0);
    setDoubleParam(pco_grab_waittime,5.0);

    setIntegerParam(pco_dbg_serwrite,0);

    setIntegerParam(pco_dbg_serread,0);
    setStringParam(pco_dbg_serstr,"00");

    setIntegerParam(pco_do_camcontrol,1);

    setIntegerParam(pco_dbg_serprint,0);
    setIntegerParam(pco_cancel_dump,0);
    //kB/sec
    setIntegerParam(pco_dump_maxdatarate,50000);
    setDoubleParam(pco_frame_rate,0.0);
    setIntegerParam(pco_reconfig_grabber,0);
    setIntegerParam(pco_check_time2,5000);

     setIntegerParam(pco_play_stop,0);
     setIntegerParam(pco_play_slow,0);

     setIntegerParam(pco_is_frame_rate_mode,1);

     setIntegerParam(pco_cdi_mode,0);
     setIntegerParam(pco_dnsu_mode,0);
     setIntegerParam(pco_dnsu_init_mode,0);

     setDoubleParam(pco_set_frame_rate,1.0);
        setDoubleParam(pco_delay_time,0.0);
     // set callback to do nothing default....
    //putPtrParam(serial_callback,(const void*);genCamController::defaultCallback);
    //callbackFunction = (int (*);(callbackReasons reason, void*););getPtrParam(serial_callback);
    setIntegerParam(pco_baudrate,9600);
    setIntegerParam(pco_trigger_mode,0);
    setIntegerParam(pco_force_check,0);
    setIntegerParam(pco_camlink_pixfreq,0);
    setIntegerParam(pco_cclines,0);
    setIntegerParam(pco_camlink_pixperclk,0);
    setIntegerParam(pco_camlink_cont,0);
    setIntegerParam(pco_reset_memory,0);

    setIntegerParam(pco_force_trigger,0);
    setIntegerParam(pco_dump_camera_memory,0);
    setIntegerParam(pco_imgs2dump,0);
    setIntegerParam(pco_memreq_cnt,0);

    setIntegerParam(pco_live_view,0);
    setIntegerParam(pco_dump_counter,0);
    setIntegerParam(pco_dump_waitms,50);

    setIntegerParam(pco_health_error,0);
    setIntegerParam(pco_health_warning,0);
    setIntegerParam(pco_health_status,0);
    setIntegerParam(pco_check_time,1000);

            setIntegerParam(pco_reset_default_settings,0);
            setIntegerParam(pco_init_selftest,0);

            setIntegerParam(NDDataType,3);

            setIntegerParam(pco_sensor_format,0);
            setIntegerParam(pco_pixelrate,0);
            setIntegerParam(pco_doub_img_mode,0);
            setIntegerParam(pco_adc_mode,0);
            setIntegerParam(pco_temp_setpt,0);
            setIntegerParam(pco_offset_mode,0);
            setIntegerParam(pco_noise_filt_mode,0);

            setIntegerParam(pco_exp_timebase,0);
            setIntegerParam(pco_dly_timebase,0);
            setIntegerParam(pco_fps_mode,0);
            setIntegerParam(pco_busy,0);
            setIntegerParam(pco_exp_trig_stat,0);

            setIntegerParam(pco_camera_tot_ram_size,0);
            setIntegerParam(pco_camera_ram_npages,0);
            setIntegerParam(pco_camera_ram_page_size,0);
            setIntegerParam(pco_camera_seg_size0,0);
            setIntegerParam(pco_camera_seg_size1,0);
            setIntegerParam(pco_camera_seg_size2,0);
            setIntegerParam(pco_camera_seg_size3,0);
            setIntegerParam(pco_clear_ram_seg,0);
            setIntegerParam(pco_active_seg,0);

            setIntegerParam(pco_storage_mode,0);
            setIntegerParam(pco_rec_submode,0);
            setIntegerParam(pco_rec_status,0);
            setIntegerParam(pco_acq_mode,0);
            setIntegerParam(pco_acqen_sig_stat,0);
            setIntegerParam(pco_timestamp_mode,0);
            setIntegerParam(pco_arm_camera,0);
            setIntegerParam(pco_rec_stop_event_mode,0);
            setIntegerParam(pco_rec_stop_event_nimgs,0);
            setIntegerParam(pco_stop_record,0);



            setIntegerParam(pco_read_which_seg,0);
            setIntegerParam(pco_read_st_img,0);
            setIntegerParam(pco_read_end_img,0);
            setIntegerParam(pco_do_read_imgs,0);
            setIntegerParam(pco_req_img,0);
            setIntegerParam(pco_rpt_img,0);
            setIntegerParam(pco_cancel_img,0);
            setIntegerParam(pco_bit_alignment,0);

            setIntegerParam(pco_num_imgs_seg0,0);
            setIntegerParam(pco_max_imgs_seg0,0);
            setIntegerParam(pco_num_imgs_seg1,0);
            setIntegerParam(pco_max_imgs_seg1,0);
            setIntegerParam(pco_num_imgs_seg2,0);
            setIntegerParam(pco_max_imgs_seg2,0);
            setIntegerParam(pco_num_imgs_seg3,0);
            setIntegerParam(pco_max_imgs_seg3,0);

    setIntegerParam(pco_ser_waitms,0);

    setIntegerParam(pco_hotpix_corr,0);
    setIntegerParam(pco_1612_lookup,0);
    setIntegerParam(pco_edge_fastscan,0);


    /// init all params or else the client will not see them...they will be undefined...
    setIntegerParam(pco_is_running ,1);
    //putPtrParam(log_file_ptr, (void*);&lf);
    setIntegerParam(pco_is_log,1);
    setIntegerParam(pco_run_counter,0);
    setIntegerParam(pco_sleep_ms,10);
    setIntegerParam(pco_is_sleep,1);// sleep...
    //putPtrParam(coreco_control_ptr,0);// need to set this up leter...

    setIntegerParam(pco_comport_number,3);
    sprintf(cpstr,"COM%d",getIntParam(pco_comport_number));

    setStringParam(com_port_name,cpstr);
    setIntegerParam(open_com,0);
    setIntegerParam(close_com,0);
    setIntegerParam(is_com_open,0);


    setIntegerParam(ADSizeX,2560);
    setIntegerParam(ADSizeY,2160);
        setStringParam(ADManufacturer,"PCO-General");
    setStringParam(ADModel,"PCO");
    setDoubleParam(ADGain,1.0);
    setIntegerParam(ADBinX,1);
    setIntegerParam(ADBinY,1);
    setIntegerParam(ADMinX,1);
    setIntegerParam(ADMinY,1);
    setIntegerParam(ADSizeX,2560);
    setIntegerParam(ADSizeY,2160);
    setIntegerParam(ADMaxSizeX,2560);
    setIntegerParam(ADMaxSizeY,2160);


    setIntegerParam(pco_descramble,1);

    setIntegerParam(pco_which_camera,pco_undef);

    
    setIntegerParam(ADAcquirePeriod,1000000);




    #ifdef USEASYNSERIAL

   status = pasynOctetSyncIO->connect(
    serverPort, 
    0, 
    &this->pasynUserSerial,
    (const char*)NULL);
    
    
    if (status!=asynSuccess)
    {
        printf("ERROR---Could not connect to serial port asyn driver %s\n",serverPort);
        exit(-1);
    }
 
  #else
    #ifndef USE_SISW
    serial_port=new cl_com_port(getStringParam(com_port_name));
    #else
    serial_port = new siswSerialPort(0);
    #endif
#endif
    setIntegerParam(ADStatus,ADStatusWaiting);


    
            setIntegerParam(ADStatus,ADStatusIdle);
            setIntegerParam(open_com,0);
            //force baud to fastest rate at startup
            setIntegerParam(pco_baudrate,115200);
            //force largest img size in camera and grabber at startup
            setIntegerParam(ADSizeX,2560);
            setIntegerParam(ADSizeY,2160);
            
            

            dbg_msg_level=0;

            com_error_counter=0;
            last_which_camera=0;
            is_enable_which_camera=false;
            is_allow_kill_acquire=false;
            is_reset_dump_counters=false;
            is_reset_memory=true;

lastxsize==0;
lastysize=0;


 status = (epicsThreadCreate("CameraLinkSerial",
                epicsThreadPriorityMedium,
                epicsThreadGetStackSize(epicsThreadStackMedium),
                (EPICSTHREADFUNC)pcoTaskC,
                this) == NULL);
    if (status) {
        printf("pco construct epicsThreadCreate failure for serial port task\n");
        return;
    }


            
 }

 
 void pco::processNewImage(void* img_ptr)
 {
    double elapsed_time;
    unsigned short *shrt_img;
    int xs;
    int ys;
    int rowa;
    int rowb;
    int rows;
    int kx;
    void *dst,*src;
    

            unsigned short diff;
        int diff_tot;
    int rowbytes;
 int attrval=0;

char mesgx[256];


 NDArray *nimg=(NDArray*)img_ptr;

 // add attribute here ...
    if (getIntParam(ADFrameType)==ADFrameNormal) //normal
        nimg->pAttributeList->add("Data","Data Attr",NDAttrInt32,&attrval);
    else if (getIntParam(ADFrameType)==ADFrameBackground)//dark
        nimg->pAttributeList->add("Dark","Dark Attr",NDAttrInt32,&attrval);
    else if (getIntParam(ADFrameType)==ADFrameFlatField)//white flood
        nimg->pAttributeList->add("White","White Attr",NDAttrInt32,&attrval);


    setIntegerParam(NDArrayCounter,getIntParam(NDArrayCounter)+1);
    setIntegerParam(pco_array_counter,getIntParam(pco_array_counter)+1);

    elapsed_time=image_timer.toc();
    if (elapsed_time>0.0)
        setDoubleParam(pco_image_rate,1.0/elapsed_time);

    image_timer.tic();




//    if (getIntParam(pco_test_pixels)==1 && getIntParam(pco_which_camera)==pco_dimax)
    if (getIntParam(pco_test_pixels)==1 )
    {
        //start 1k into the array.



        shrt_img=(unsigned short*)(nimg->pData);
        shrt_img+=1024;
        diff_tot=0;
        for (kx=0;kx<16;kx++)
        {
            diff=test_line[kx]-shrt_img[kx];
            diff_tot+=(int)diff;

            //store to compare with next img.
            test_line[kx]=shrt_img[kx];
        }
        if (diff_tot==0)
        {
            lf.log("WARNING: 2 images have same data. Read in same img twice?");
            setIntegerParam(pco_double_image_error,getIntParam(pco_double_image_error)+1);
        }

    }


//temp_image
//img_ptr
    if (getIntParam(pco_disable_descramble)==0)
    {
    if (getIntParam(pco_which_camera)==pco_edge)
    {
        xs=getIntParam(ADSizeX);
        ys=getIntParam(ADSizeY);
        rowa=0;

        if (xs!=lastxsize || ys!= lastysize)
        {
            printf("pcoImgCam, descramble, img size changed: %d %d\n",xs,ys);
        }

        lastxsize=xs;
        lastysize = ys;
        rowb=ys-1;
        rows=0;
        shrt_img=(unsigned short*)nimg->pData;
        rowbytes=xs*2;
        
        if (xs>2560 || xs<0 || ys>2160 || ys<0)
        {
            printf("ERROR: PCOImgCam::processNewImage() ADSIZEX or ADSizeY Illegal\n");
            return;
        }
        
                    
        if (getIntParam(pco_conv_12to16)==1)
        {
            // this means that we have 12 bit encoded data and we have to extract to 16bit 
            //lengh of line in ints of xs pixels for 12 bit data.
            // xs is in words of 12 bits. 
            // to find number of chars in same amount of memory, mult by 1.5.
            // to find number of ints, we then divide by 4.
            double nibbles = (double)xs * 3.0;
            double shorts_in=nibbles/4.0;
            
            
            if ((shorts_in - floor(shorts_in))>0.0001)
            {
                sprintf(mesgx,"ERROR- Decompression shorts in= %f,   img size= %d",shorts_in,xs);
                lf.log(mesgx);
            }    
            
            int len_in = (int)floor(shorts_in);
            int len_out = xs;
            
            
            
            
            
            
            // so 3*32 bits is 8 pixels
            //we will count by groups of 3 ints to get one line.
            
            unsigned short *iptr = (unsigned short*)(nimg->pData);
            unsigned short *optr = (unsigned short*)(temp_image);
                            
            
            //
            for (rows = 0 ;rows<ys; rows++)
            {
                // go across one line.
                
        
                int k=0;
                int m=0;
   
                while(k<len_in)
                {
                    
                    optr[m]= (iptr[k] & 0xfff0)>>4;
                    
                    m=m+1;

                    optr[m]=(iptr[k]&0xf)<<8;
                    k=k+1;

                    optr[m]=optr[m] + ((iptr[k]&0xff00)>>8);
                    m=m+1;

                    optr[m]=(iptr[k]&0xff)<<4;
                    k=k+1;

                    optr[m]=optr[m] + ((iptr[k]&0xf000)>>12);
                    m=m+1;

                    optr[m]=(iptr[k]&0xfff);

                    m=m+1;
                    k=k+1;

        
                }
                
                iptr+=k;
                optr+=m;
                
                
            }
        }
        else
        {
            // if no 12 to 16 bit extraction, then copy raw to temp img
            memcpy((void*)temp_image,(void*)shrt_img,xs*ys*sizeof(short));
        }    
            
            
        if (getIntParam(pco_descramble)==1 )
        {        
            
            //descramnle from temp img to output img, called shrt image.
            for (rows=0;rows<ys;rows=rows+2)
            {
                // odds rows on bottom , bottom 2 top. we are calc in shorts...
                dst = (void*)(shrt_img+rowb*xs);
                src = (void*)(temp_image+(rows+1)*xs);
                memcpy(dst,src,rowbytes);
                //even rows on top, top 2 bottom
                dst=(void*)(shrt_img+rowa*xs);
                src = (void*)(temp_image+(rows)*xs);
                memcpy(dst,src,rowbytes);

                rowa++;
                rowb--;

            }
        checkEdgeDescramble(nimg);
        }

    
    }//if edge


    
    

    }//if disable descram

    int attrx=getIntParam(pco_descramble);
    nimg->pAttributeList->add("pco_descramble", "pco_descramble", NDAttrInt32, &attrx);
     attrx=getIntParam(pco_edge_fastscan);
    nimg->pAttributeList->add("pco_edge_fastscan", "pco_edge_fastscan", NDAttrInt32, &attrx);
     attrx=getIntParam(pco_conv_12to16);
    nimg->pAttributeList->add("pco_conv_12to16", "pco_conv_12to16", NDAttrInt32, &attrx);

     attrx=getIntParam(pco_disable_descramble);
    nimg->pAttributeList->add("pco_disable_descramble", "pco_disable_descramble", NDAttrInt32, &attrx);

    
    //shut off camera if ADAcquire was turned off 
    if (getIntParam(pco_which_camera)==pco_edge)
    {
        if (getIntParam(ADAcquire)==0)
        {

            printf("Turning off camera, \n");
            //we already have the grabber mutex-- we may interlock if we atempt to get both...
            releaseGrabberMutex();
            grabSerialMutex();
            doHighLevelParams(ADAcquire);
            //may be lots of useless frames in grabber memory- so we delete them
            // the frames came in after we turned off grab but before we turned off camera
            //reconfigGrabber();
            releaseSerialMutex();
            grabGrabberMutex();

        }

    }




 }


 
 void pco::keepHouse(void)
{

    
}

 
int pco::intFlEndian(unsigned int in)
{
    char a[4];
    char b[4];
    
    unsigned int *aptr = (unsigned int*)a;
    
    *aptr = in;
    
    b[0] = a[3];
    b[1]=a[2];
    b[2]=a[3];
    b[3]=a[0];
    
    unsigned int *bptr = (unsigned int*)b;
    
    int ans=*bptr;
    return(ans);
    
}
 

void pco::checkEdgeDescramble(NDArray* img_ptr)
{

    double elapsed_time;
    unsigned short *shrt_img;
    int xs;
    int ys;
    
    int kx;
    void *dst,*src;
    int dftsum;
    int sign;
    int pixval;

    sign=1;


            unsigned short diff;
        int diff_tot;
    int rowbytes;
    int musum;


    xs=getIntParam(ADSizeX);
            ys=getIntParam(ADSizeY);


        shrt_img=(unsigned short*)img_ptr->pData;
        shrt_img+=1024;
        dftsum=0;
        musum=0;

        for (kx=0;kx<16;kx++)
        {
            //take a vertiacl line down image of len 16.
            pixval = (int)shrt_img[kx*xs];

            musum+=pixval;
        

        }
        musum=musum/16;
        
        for (kx=0;kx<16;kx++)
        {
            //take a vertiacl line down image of len 16.
            pixval = (int)shrt_img[kx*xs];
            pixval = pixval - musum;
            dftsum+= sign* pixval;
            sign=-1*sign;


        }
    

        dftsum=dftsum/16;
        dftsum=abs(dftsum);

        if (dftsum>600)
        {
            printf("Probable descrambling error\n");
        }


}

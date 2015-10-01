/* ADCameralink.cpp
 *
 * Author: TJ Madden
 *
 *
 */

#ifndef _ADCAMERALINK_H
#define _ADCAMERALINK_H
 
 
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsStdlib.h>
#include <epicsString.h>
#include <epicsStdio.h>
#include <epicsMutex.h>
#include <cantProceed.h>
#include <iocsh.h>
#include <epicsExport.h>

#include <asynOctetSyncIO.h>

#include "ADDriver.h"
#define _ADIOC_
#include "grabberInterface.h"

#include "comportInterface.h"
#include "logfile.h"
#include "image_ram2.h"
#include "ccd_exception.h"
#include "coreco.h"
#include "siliconSoftware.h"
#include "stopWatch.h"
#include "siswSerialPort.h"


#include <math.h>
#include "time.h"



#ifndef _WIN32
#define NULL reinterpret_cast<void *>(0)
#endif

/** Driver for ADCameralink (Rayonix) CCD detector; communicates with the ADCameralink program over a TCP/IP
  * socket with the ADCameralink_server_socket program that they distribute.
  * The ADCameralink program must be set into Acquire/Remote Control/Start to use this driver.
  */
  
  
  //   putIntParam\((?<param>[a-z_]*).*
  // createParam\("$+{param}",asynInt32,$+{param}\)
  
class ADCameralink : public ADDriver 
{
public:
    ADCameralink(const char *portName, const char *ADCameralinkPort,int numparams,
           int maxBuffers, size_t maxMemory,
           int priority, int stackSize);

    /* These are the methods that we override from ADDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

    virtual void report(FILE *fp, int details);
    void getImageTask(int arg);          /**< This should be private but is called from C, must be public */
    

    epicsEventId stopEventId;   /**< This should be private but is accessed from C, must be public */

    


        // handla cameralink stuff. called by main()
        virtual void oneLoopImage(void);
        // after we get image from card, we call this. It does nothing, but can be overridden
        virtual void processNewImage(void* img_ptr);
        virtual void keepHouse(void);
    void grabberSetup(int function);


    void loadCCF();

    //image_ram ram0;

    //cameralinkCCDClass *cameralink_card;
     grabberInterface* cameralink_card;
    int words_read;


    double elapsed_time, last_time;

    unsigned int last_timestamp;
    unsigned int ts_diff_last,ts_diff;

    



    
    
protected:
    int ADCameralink_first;
    #define FIRST_ADCameralink_PARAM ADCameralink_first
    

    int        cor_is_running;
    int cor_is_log;        
    int        is_new_image;
    int        cor_run_counter;
    int        cor_sleep_ms;
    int        cor_is_sleep;
    int        is_grab;
    int        is_abort;
    int        is_snap;

    int        is_freeze;
    int        is_acquire;
    int        is_acquire_num_frames;
    int        cor_ccf_filename;
    int        is_loadccf;
    int        is_buffer_mode;
    int        is_dbg_viewer;
    int        is_trigpin0;
    int        is_trigpin1;
    int        is_trigpin2;
    
    int        is_trigpin3;
    int        copy_memory;
    int        collect_status;
    int        size_x;
    int        size_y;
    int        operation_count;
    int        total_missed_frames;
    int        recent_missed_frames;
    int        num_coreco_buffers;//number of buffers to request
    int        is_force_img_size;
    
    int        is_mult_width2;//used for 8bit data
    int        cor_check_timestamps;//causes check of time stamps. otherwize ignore time stamps
    int        cor_num_free_buffers;//number of free buffers in grabber
    int        cor_num_buffers;//number of buffers we actially have in grabber
    int        cor_cant_get_ndarray;//counts num of tims we cant get nd array
    int        cor_max_ndbuffers;// limit on buffers in pool
    int        cor_max_ndmemory;// limit on memory for array pool
    int        cor_free_ndbuffers;//alloc but free nd arrays in pool
    int        cor_num_ndbuffers;//alloc nd arrays in array pool
    int        cor_alloc_ndmemory;//alloc mem in bytes in array pool

    int        cor_est_buffers_left;// mamory left counted in buffgers; before we run out of mem
    int        cor_nd_datasize;//size in bytes of nd array
    int        cor_num_repeat_timestamp;//counters for missed and repeated time stamps; from corecoticks
    int        cor_num_missed_timestamp;
    int        cor_missed_ts_wait;// time between shots where we dont count as missed shot- say if we stop and start acq
    int        cor_use_image_mode;// causes grabber to not grab more then ADNumImages if in multi mode; or single mode. otheruse grab all shots
    int        cor_reset_buff_index;// reset buffer index to clear untransfered frames...
    int        cor_grabber_type;
    
    //got frame but  threw away- counter
    int     cor_frame_to_null;
    
    int        last_coreco_param;

        

    
    
    int ADCameralink_last;
    #define LAST_ADCameralink_PARAM ADCameralink_last

    
    
    
    
    enum{ 
        ADCameralink_paramint,
        ADCameralink_paramdoub};
        
        
        

        virtual void grabSerialMutex(void);
        virtual void releaseSerialMutex(void);
        virtual void grabGrabberMutex(void);
        virtual void releaseGrabberMutex(void);


        virtual void setDebuggingMessage(int level);
    
        virtual int getSemaphore(void);
    
        virtual void incSemaphore(void);
        
        volatile int dbg_msg_level;
        
        


    int getIntParam(int param);
    double getDoubleParam(int param);

char* getStringParam(int param);

 //int function;
 
char cpstr[256];
    /* Our data */
    epicsEventId startEventId;
    epicsEventId imageEventId;
    epicsTimeStamp acqStartTime;
    epicsTimeStamp acqEndTime;
    epicsTimerId timerId;
    NDArray *pImage;
    NDArray *pImageBitBucket;
    
    
    asynUser *pasynUserServer;
NDArrayPool *array_pool;
size_t dims[2];
//mutex protexted counter
        volatile int semaphore_counter;
    epicsMutexId cntMutex;
    epicsMutexId serialMutex;
    epicsMutexId grabberMutex;

    log_file lf;

};


#define NUM_ADCameralink_PARAMS (&LAST_ADCameralink_PARAM - &FIRST_ADCameralink_PARAM + 1)

#endif

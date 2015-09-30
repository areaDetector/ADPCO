/* pco.cpp
 *
 * Author: TJ Madden
 *
 *
 */

 
 #ifndef PCO_H_
#define PCO_H_

 
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

#ifdef USEASYNSERIAL
#include <asynShellCommands.h>
#endif


#include "ADDriver.h"
#define _ADIOC_
#include "grabberInterface.h"

#include "comportInterface.h"
#include "logfile.h"
#include "image_ram2.h"
#include "ccd_exception.h"
#include "coreco.h"
#include "stopWatch.h"
#include "pco_structures.h"

#ifdef USE_SISW
#include "siswSerialPort.h"
#endif

#ifdef USE_SAP
#include "cl_com_port2.h"
#endif

#include "ADCameralink.h"
#include <math.h>
#include "time.h"




/** Driver for pco (Rayonix) CCD detector; communicates with the pco program over a TCP/IP
  * socket with the pco_server_socket program that they distribute.
  * The pco program must be set into Acquire/Remote Control/Start to use this driver.
  */
  
  
  //   putIntParam\((?<param>[a-z_]*).*
  // createParam\("$+{param}",asynInt32,$+{param}\)
  
class pco : public ADCameralink {
public:
    pco(const char *portName, const char *pcoPort,
           int maxBuffers, size_t maxMemory,
           int priority, int stackSize);

    /* These are the methods that we override from ADDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

    virtual void report(FILE *fp, int details);
    void pcoTask();          /**< This should be private but is called from C, must be public */
    void pcoTask2();          /**< This should be private but is called from C, must be public */

    epicsEventId stopEventId;   /**< This should be private but is accessed from C, must be public */

protected:
	int pco_first;
    #define FIRST_pco_PARAM pco_first
	
	
int			pco_is_running;

	int		serial_callback;
	int		pco_run_counter;
	int		pco_sleep_ms;
	int		pco_is_sleep;
	int 	pco_is_log;
	int		com_port_name;
	int		open_com;

	int		close_com;
	int		is_com_open;
	int		pco_health_warning;
	int		pco_health_error;
	int		pco_health_status;
	int		pco_reset_default_settings;//!!
	int		pco_init_selftest;//!!
	int		pco_sensor_format;//!! need to put into cpp- int
	int		pco_pixelrate;
	int		pco_doub_img_mode;//on or off

	int		pco_adc_mode;
	int		pco_temp_setpt;
	int		pco_offset_mode;
	int		pco_noise_filt_mode;
	int		pco_exp_timebase;//!! need to put into cpp- double
	int		pco_dly_timebase;//double
	int		pco_fps_mode;//int
	int		pco_busy;
	int		pco_exp_trig_stat;
	int		pco_camera_tot_ram_size;

	int		pco_camera_ram_npages;
	int		pco_camera_ram_page_size;
	int		pco_camera_seg_size0;
	int		pco_camera_seg_size1;
	int		pco_camera_seg_size2;
	int		pco_camera_seg_size3;
	int		pco_clear_ram_seg;
	int		pco_active_seg;
	int		pco_storage_mode;
	int		pco_rec_submode;

	int		pco_rec_status;
	int		pco_acq_mode;
	int		pco_acqen_sig_stat;
	int		pco_timestamp_mode;
	int		pco_arm_camera;
	int		pco_rec_stop_event_mode;
	int		pco_rec_stop_event_nimgs;
	int		pco_stop_record;
	int		pco_read_which_seg;
	int		pco_read_st_img;

	int	pco_read_end_img;
	int	pco_do_read_imgs;
	int	pco_req_img;
	int	pco_rpt_img;
	int	pco_cancel_img;
	int	pco_bit_alignment;
	int	pco_num_imgs_seg0;
	int	pco_max_imgs_seg0;
	int	pco_num_imgs_seg1;
	int	pco_max_imgs_seg1;

	int		pco_num_imgs_seg2;
	int		pco_max_imgs_seg2;
	int		pco_num_imgs_seg3;
	int		pco_max_imgs_seg3;
	int		pco_check_time;
	int		pco_force_check;
	int		pco_camlink_pixfreq;
	int		pco_cclines;
	int		pco_camlink_pixperclk;
	int		pco_camlink_cont;

	int		pco_trigger_mode;
	int		pco_baudrate;
	int		pco_reset_memory;
	int		pco_dump_camera_memory;
	int		pco_imgs2dump;
	int		pco_memreq_cnt;
	int		pco_force_trigger;
	int		pco_cancel_dump;
	int		pco_play_slow;
	int		pco_play_stop;

	int		pco_live_view;
	int		pco_dump_counter;
	int		pco_dump_waitms;
	int		pco_check_time2;
	int		pco_frame_rate;
	int		pco_cdi_mode;
	int		pco_dnsu_mode;
	int		pco_dnsu_init_mode;
	int		pco_reconfig_grabber;
	int		pco_set_frame_rate;
    
	int		pco_delay_time;
	int		pco_dump_maxdatarate;
	int		pco_favor_dlytime;
	int		pco_hotpix_corr;
	int		pco_1612_lookup;
	int		pco_edge_fastscan;
	int		pco_comport_number;
	int		pco_is_frame_rate_mode;
    
	int		pco_dbg_serwrite;
	int		pco_dbg_serread;
	int		pco_dbg_serstr;
	int		pco_dbg_serprint;//set to int value of serial code to pco- then printing turns on for that code pnly
    
	int		pco_ser_waitms;//wait between read and write in serial transaction. coreco need none. sisw need a few ms
	int		pco_do_camcontrol;//1 to run camera contol loop- so we can turn it off in debigg
	int		pco_grab_waittime;
    
	int		pco_ready2acquire;
		
    
	
	
	// pco grabber paramers
	
	int		pco_array_counter;
	int		 pco_image_rate;
	int		 pco_test_pixels;
	int		 pco_double_image_error;
	int		 pco_which_camera;
	int		 pco_descramble;
	int 	pco_setallparams;// for setting a large group of settings in one pass
	
	int pco_com_err_max;
	
    
	int pco_kill_ioc;
	int pco_disable_descramble;
	
	int pco_global_shutter;
	
	int pco_rollshut_mcfname;
	int pco_globshut_mcfname;
	int pco_conv_12to16;

	//dimax needs extra mem for images. if we need 1 img, we need mem for 3 according
	// to manual. this adds to num images when we declar mem. in dimax
	int pco_dimax_nimgs_fudge;
	int pco_last;
    #define LAST_pco_PARAM pco_last


	
	
	
	enum{ 
		pco_paramint,
		pco_paramdoub};
		
		
		

		virtual void setDebuggingMessage(int level);
	
		
		volatile int dbg_msg_level;
		
	

// this is included in the pco.h file


	stopWatch time1;
	stopWatch time2;
	stopWatch grab_delay;

	void reconfigGrabber(void);
void checkEdgeDescramble(NDArray* img_ptr);

		// after we get image from card, we call this. It does nothing, but can be overridden
		virtual void processNewImage(void* img_ptr);
		virtual void keepHouse(void);
int updateParameters(
	asynUser *pasynUser, 
		epicsInt32 ivalue,
		epicsFloat64 dvalue,
		int paramtype);
	
		virtual int doSerialTransactions(
			int function, 
			 int ivalue,
			 double dvalue,
			int paramtype);
		
		
	virtual void clearPipe(FILE *fp);

	// load PCO DLL
	int getlib(void);
   int OpenCamera(void);
int Disconnected(int code,char *usrmsg);
void resetDimaxMemory(void);



int checkCameraMessages(void);
int setPcoBaudrate(int function);
int ReportError(int code,char *usrmsg);
int doSerialCommand(
	pco_command &cmd,
	pco_response &rsp,
	unsigned char* obj,
	int len);

int getPcoStatusParams(void);

int setPcoCameraLinkParams(int function);
int getPcoCameraLinkParams(void);

int getPcoGeneralParams(void);
int setPcoGeneralParams(int function);

int getPcoSensorParams(void);
int setPcoSensorParams(int function);

int getPcoTimingParams(void);
int setPcoTimingParams(int function);

int getPcoStorageParams(void);
int setPcoStorageParams(int function);

int getPcoRecordingParams(void);
int setPcoRecordingParams(int function);

int setPcoImageReadParams(int function);
int getPcoImageReadParams(void);


int dumpOneFrame(int k);

void dumpCameraMemory(void);


int doHighLevelParams(int function);
static char dll_name[];
	
	#ifndef USEASYNSERIAL
   comportInterface *serial_port;
#endif

void dbgSerial(void);

int com_error_counter;

	xPCO_Description pco_desc;
int last_which_camera;
bool is_enable_which_camera;

// avoids race condition- allows camera stats to shut down ADACquire setting to 0, if no recording is happening.
// user turns on acquire, camera turns it off when acq is done. 
bool is_allow_kill_acquire;

// if trye then dump[ counters get set to 1 and num of imgs in memory. 
bool is_reset_dump_counters;

//true to reset mem on acquire.
bool is_reset_memory;

//how may written to port
size_t mynwrite;
size_t mynread;
int myeomreason;
// asyn user for the serial port
asynUser *pasynUserSerial;

//interface to serial port octet
asynInterface *paynOctet;
asynInterface *paynOption;

char myServerPort[255];
//
// camera grabber stuff
//




	//enums to choose which camera we are using...
		enum whichPCOCam
		{
			pco_dimax,
			pco_edge,
			pco_other,
			pco_undef

		};
		


// we have one overridden function. it is called when we get a frame

	stopWatch image_timer;
	//space for 128 pixels- we test image to see if its same as last..
	unsigned short test_line[128];

	//enough for 1 edge imgae. pco imges are smaller...
	unsigned short *temp_image;


	//for debugging....

	int lastxsize, lastysize;


	void checkEdgeDescramble(void* img_ptr);


int intFlEndian(unsigned int in);




	
private:


 //int function;
 
char cpstr[256];
    /* Our data */
    epicsEventId startEventId;
    epicsEventId imageEventId;
    epicsTimeStamp acqStartTime;
    epicsTimeStamp acqEndTime;
    epicsTimerId timerId;
    NDArray *pData;
    asynUser *pasynUserServer;
NDArrayPool *array_pool;


};
#endif


#define NUM_pco_PARAMS (&LAST_pco_PARAM - &FIRST_pco_PARAM + 1)


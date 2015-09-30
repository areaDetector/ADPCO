#include "ADCameralink.h"
#include "softwareGrabber.h"


static const char *driverName = "ADCameralink";




/** This function is called when the exposure time timer expires */
extern "C"  void timerCallbackC(void *drvPvt)
{
    ADCameralink *pPvt = (ADCameralink *)drvPvt;

   epicsEventSignal(pPvt->stopEventId);

}



static void ADCameralinkGetImageTask(void *drvPvt)
{
    ADCameralink *pPvt = (ADCameralink *)drvPvt;

    pPvt->getImageTask(0);
}




void ADCameralink::setDebuggingMessage(int level)
{
	dbg_msg_level=level;

}



 void ADCameralink::grabSerialMutex(void)
{
	

	epicsMutexLock(serialMutex);
		

 }
void ADCameralink::releaseSerialMutex(void)
{

epicsMutexUnlock(serialMutex);
}


 void ADCameralink::grabGrabberMutex(void)
{
	

	epicsMutexLock(grabberMutex);
		

 }
void ADCameralink::releaseGrabberMutex(void)
{

epicsMutexUnlock(grabberMutex);
}



int ADCameralink::getSemaphore(void)
{

	int i;

	epicsMutexLock(cntMutex);
	i=semaphore_counter;
	
 epicsMutexUnlock(cntMutex);
	return(i);
}
void ADCameralink::incSemaphore(void)
{
	epicsMutexLock(cntMutex);
	semaphore_counter++;
	epicsMutexUnlock(cntMutex);

}
 




/** Called when asyn clients call pasynInt32->write().
  * This function performs actions for some parameters, including ADAcquire, ADBinX, etc.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus ADCameralink::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
   
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   
   if (function==ADAcquire)
		printf("ADCameralink::writeInt32  ADacquire %d\n ",value);


   status = setIntegerParam(function, value);
   
   
   char mesx[256];
   char *mesx2;
   
   getParamName(function, (const char**)&mesx2);
   
     sprintf(mesx,"ADCameralink::writeInt32, param=%s, value=%d",mesx2,value);
   lf.log(mesx);
   
   
   
//ret if we actulayyl updated so we can call super if necessary
  
		grabberSetup( function);
		
	//call super class to update pawram
    
		ADDriver::writeInt32(pasynUser,  value);


		//make sure we dont have any mutexes...
		//releaseSerialMutex();
		//releaseGrabberMutex();

	callParamCallbacks();
	
		status=asynSuccess;
	return(status);

}


int ADCameralink::getIntParam(int param)
{
	int saveparamx;
	getIntegerParam(param, &saveparamx);
	
	
	
	return(saveparamx);

}


double ADCameralink::getDoubleParam(int param)
{
	double saveparamx;
	ADDriver::getDoubleParam(param, &saveparamx);
	
	
	
	return(saveparamx);

}


char* ADCameralink::getStringParam(int param)
{
	static char saveparamx[256];
	ADDriver::getStringParam(param, 255,saveparamx);
	//!! need to implement this funct
	return(saveparamx);

}







/** Called when asyn clients call pasynFloat64->write().
  * This function performs actions for some parameters.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus ADCameralink::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   
   status = setDoubleParam(function, value);
   
   char mesx[256];
   char *mesx2;
   
   getParamName(function, (const char**)&mesx2);
   
     sprintf(mesx,"ADCameralink::writeFloat64, param=%s, value=%f",mesx2,value);
   lf.log(mesx);
   
//ret if we actulayyl updated so we can call super if necessary
  
		
		grabberSetup( function);
	//call super class to update pawram
    
	callParamCallbacks();
	ADDriver::writeFloat64(pasynUser,  value);
	
	return(status);
 }



/** Report status of the driver.
  * Prints details about the driver if details>0.
  * It then calls the ADDriver::report() method.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details If >0 then driver details are printed.
  */
void ADCameralink::report(FILE *fp, int details)
{
    fprintf(fp, "ADCameralink %s\n", this->portName);
    if (details > 0) {
        int nx, ny;
        getIntegerParam(ADSizeX, &nx);
        getIntegerParam(ADSizeY, &ny);
        fprintf(fp, "  NX, NY:            %d  %d\n", nx, ny);
    }
    /* Invoke the base class method */
    ADDriver::report(fp, details);
}

extern "C" int ADCameralinkConfig(const char *portName, const char *serverPort,
                            int maxBuffers, size_t maxMemory,
                            int priority, int stackSize)
{
    new ADCameralink(portName, serverPort, 300,maxBuffers, maxMemory, priority, stackSize);
    return(asynSuccess);
}

/** Constructor for ADCameralink driver; most parameters are simply passed to ADDriver::ADDriver.
  * After calling the base class constructor this method creates a thread to collect the detector data,
  * and sets reasonable default values the parameters defined in this class, asynNDArrayDriver, and ADDriver.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] serverPort The name of the asyn port driver previously created with drvAsynIPPortConfigure
  *            connected to the ADCameralink_server program.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
ADCameralink::ADCameralink(const char *portName, const char *serverPort,
                                int numparams, int maxBuffers, size_t maxMemory,
                                int priority, int stackSize)

    : ADDriver(portName, 1, numparams+NUM_ADCameralink_PARAMS, maxBuffers, maxMemory,
               0, 0,             /* No interfaces beyond those set in ADDriver.cpp */
               ASYN_CANBLOCK, 1, /* ASYN_CANBLOCK=1, ASYN_MULTIDEVICE=0, autoConnect=1 */
               priority, stackSize),
      pImage((NDArray*)0),
	  lf("ADCameralink_logfile.txt")

{


	
	
	
	char mesg[256];
	
	int err;

	int status = asynSuccess;

	
	
	lf.enablePrintf(true);
	lf.log("ADCameralink construcitng");
	
	sprintf(mesg,"ADCameralink Compile date %s, Time %s",__DATE__,__TIME__);
	lf.log(mesg);

	
	
	createParam("collect_status",asynParamInt32,&collect_status);

	
	
	
	createParam("cor_frame_to_null",asynParamInt32,&cor_frame_to_null);
	

	createParam("cor_reset_buff_index",asynParamInt32,&cor_reset_buff_index);
	createParam("cor_is_log",asynParamInt32,&cor_is_log);


	createParam("cor_use_image_mode",asynParamInt32,&cor_use_image_mode);
	createParam("cor_num_repeat_timestamp",asynParamInt32,&cor_num_repeat_timestamp);
	createParam("cor_num_missed_timestamp",asynParamInt32,&cor_num_missed_timestamp);
	createParam("cor_missed_ts_wait",asynParamInt32,&cor_missed_ts_wait);




	createParam("cor_est_buffers_left",asynParamInt32,&cor_est_buffers_left);
	createParam("cor_nd_datasize",asynParamInt32,&cor_nd_datasize);

	
	createParam("cor_grabber_type",asynParamInt32,&cor_grabber_type);


	createParam("cor_max_ndbuffers",asynParamInt32,&cor_max_ndbuffers);
	createParam("cor_max_ndmemory",asynParamInt32,&cor_max_ndmemory);
	createParam("cor_free_ndbuffers",asynParamInt32,&cor_free_ndbuffers);
	createParam("cor_num_ndbuffers",asynParamInt32,&cor_num_ndbuffers);
	createParam("cor_alloc_ndmemory",asynParamInt32,&cor_alloc_ndmemory);

	createParam("cor_sleep_ms",asynParamInt32,&cor_sleep_ms);
	createParam("cor_cant_get_ndarray",asynParamInt32,&cor_cant_get_ndarray);

	createParam("cor_num_free_buffers",asynParamInt32,&cor_num_free_buffers);
	createParam("cor_num_buffers",asynParamInt32,&cor_num_buffers);

	createParam("cor_check_timestamps",asynParamInt32,&cor_check_timestamps);

	createParam("cor_is_force_img_size",asynParamInt32,&is_force_img_size);
	createParam("cor_is_running",asynParamInt32,&cor_is_running);
 	createParam("cor_is_new_image",asynParamInt32,&is_new_image);

	createParam("cor_is_mult_width2",asynParamInt32,&is_mult_width2);
	//callbackFunction = (int (*);(callbackReasons reason, void*););getPtrParam(callback_ptr);;

	createParam("cor_run_counter",asynParamInt32,&cor_run_counter);
	createParam("cor_operation_count",asynParamInt32,&operation_count);


	
	createParam("cor_is_sleep",asynParamInt32,&cor_is_sleep);
	//clearChanges();;


createParam("cor_num_coreco_buffers",asynParamInt32,&num_coreco_buffers);
	createParam("cor_is_trigpin0",asynParamInt32,&is_trigpin0);
	createParam("cor_is_trigpin1",asynParamInt32,&is_trigpin1);
	createParam("cor_is_trigpin2",asynParamInt32,&is_trigpin2);
	createParam("cor_is_trigpin3",asynParamInt32,&is_trigpin3);
	

	createParam("cor_is_loadccf",asynParamInt32,&is_loadccf);
	createParam("cor_ccf_filename",asynParamOctet,&cor_ccf_filename);
 
	createParam("cor_is_grab",asynParamInt32,&is_grab);
	

	createParam("cor_is_abort",asynParamInt32,&is_abort);
	createParam("cor_is_freeze",asynParamInt32,&is_freeze);
	createParam("cor_is_snap",asynParamInt32,&is_snap);


	//is there an error here?
	createParam("cor_size_x",asynParamInt32,&size_x);
	createParam("cor_size_y",asynParamInt32,&size_y);

	//createParam("ADSizeX",asynParamInt32,&ADSizeX);
	//createParam("ADSizeY",asynParamInt32,&ADSizeY);

	createParam("cor_total_missed_frames",asynParamInt32,&total_missed_frames);
	createParam("cor_recent_missed_frames",asynParamInt32,&recent_missed_frames);

	createParam("is_dbg_viewer",asynParamInt32,&is_dbg_viewer);

	
	
	
	setIntegerParam(is_dbg_viewer,0);;
	
	setIntegerParam(cor_is_log,0);;

	setIntegerParam(cor_reset_buff_index,0);;
	setIntegerParam(cor_use_image_mode,1);;
	setIntegerParam(cor_num_repeat_timestamp,0);;
	setIntegerParam(cor_num_missed_timestamp,0);;
	//in us
	setIntegerParam(cor_missed_ts_wait,1000000);;

setIntegerParam(cor_frame_to_null,0);;



	setStringParam(ADManufacturer,"APS Detector Group");;
	setStringParam(ADModel,"Coreco Card");;

	setIntegerParam(cor_est_buffers_left,0);;
	setIntegerParam(cor_nd_datasize,0);;

	#ifdef USE_SAP
	setIntegerParam(cor_grabber_type,grabberInterface::gDALSA);;
	#else
	setIntegerParam(cor_grabber_type,grabberInterface::gSISW);;
	#endif

	setIntegerParam(cor_max_ndbuffers,0);;
	setIntegerParam(cor_max_ndmemory,0);;
	setIntegerParam(cor_free_ndbuffers,0);;
	setIntegerParam(cor_num_ndbuffers,0);;
	setIntegerParam(cor_alloc_ndmemory,0);;

	setIntegerParam(cor_sleep_ms,10);;
	setIntegerParam(cor_cant_get_ndarray,0);;

	setIntegerParam(cor_num_free_buffers,0);;
	setIntegerParam(cor_num_buffers,0);;

	setIntegerParam(cor_check_timestamps,1);;

	setIntegerParam(is_force_img_size,1);;
	setIntegerParam(cor_is_running,true);;
	
 	setIntegerParam(is_new_image,false);;

	setIntegerParam(is_mult_width2,0);;
	//callbackFunction = (int (*);(callbackReasons reason, void*););getPtrParam(callback_ptr);;

	setIntegerParam(cor_run_counter,0);;
	setIntegerParam(operation_count,0);;



	setIntegerParam(cor_is_sleep,1);;// 0 to suspend thread, 1 to sleep
	//clearChanges();;


setIntegerParam(num_coreco_buffers,2);;
	setIntegerParam(is_trigpin0,0);;
	setIntegerParam(is_trigpin1,0);;
	setIntegerParam(is_trigpin2,0);;
	setIntegerParam(is_trigpin3,0);;

	setIntegerParam(is_loadccf,0);;
	setStringParam(cor_ccf_filename,"D:/corecofiles/c1k1k16bit.ccf");;
 

//
	setIntegerParam(is_grab,0);;


	setIntegerParam(is_abort,0);;
	setIntegerParam(is_freeze,0);;
	setIntegerParam(is_snap,0);;


	setIntegerParam(size_x,0);;
	setIntegerParam(size_y,0);;

	setIntegerParam(ADSizeX,1024);;
	setIntegerParam(ADSizeY,1024);;

	setIntegerParam(total_missed_frames,0);;
	setIntegerParam(recent_missed_frames,0);;







	
			setIntegerParam(ADStatus,ADStatusIdle);
			
			//force baud to fastest rate at startup
			
			//force largest img size in camera and grabber at startup
			setIntegerParam(ADSizeX,2560);
			setIntegerParam(ADSizeY,2160);
			
			
			
	cntMutex=epicsMutexCreate();
	semaphore_counter=0;

	serialMutex=epicsMutexCreate();
	grabberMutex=epicsMutexCreate();
 

	//cameralinkCCDClass *cameralink_card;
	cameralink_card=0;
	words_read=0;


	 elapsed_time=0.0;
	 last_time=0.0;

	 last_timestamp=0;

	 ts_diff_last=0;
		 ts_diff=0;

array_pool=0;
		 
		 
		 
		 
		

	int dd;
	size_t dims[2];
		getIntegerParam(ADSizeX, &dd);
		dims[0] = dd;
							  
		getIntegerParam(ADSizeY, &dd);
		dims[1] = dd;
		this->pImageBitBucket = this->pNDArrayPool->alloc(2, (size_t*)dims, NDUInt16, (size_t)0, NULL);
							
	

 status = (epicsThreadCreate("CameraLinkTask",
                epicsThreadPriorityMedium,
                epicsThreadGetStackSize(epicsThreadStackMedium),
                (EPICSTHREADFUNC)ADCameralinkGetImageTask,
                this) == NULL);
    if (status) {
		printf("ADCameralink construct epicsThreadCreate failure for data collection task\n");
        return;
    }

}



 





void ADCameralink::getImageTask(int arg)
{
	double exp_elapsedtime;
	double last_exptime = 0.0;
	char szMsg[128];

	int sleepy,sleepytime;
	void *fptr;



	lf.log("cameralinkcam entering main function");

	// update callback function ptr
	
	
	//cameralink_card = 0;
	//!! where is cameralink_card declared?
//	putPtrParam(image_mem_ptr,ram0.getMem());

	while (getIntParam(cor_is_running))
	{
				//make sure we dont have any mutexes...
	//	releaseSerialMutex();
	//	releaseGrabberMutex();

		try
		{
				epicsThreadSleep(getIntParam(cor_sleep_ms)/1000.0);
						setIntegerParam(cor_run_counter, getIntParam(cor_run_counter) + 1);
			//inc mutex protected counter so we can sort of sync threads

				oneLoopImage();
			

		}//try
		catch (ccd_exception err)
		{
			lf.log(err.err_mess());
			//(*callbackFunction)(call_error,this);

			

		}//catch
		catch (...)
		{
			lf.log("Stack Overflow Error? Unknown exception");
		}



	}//while

	lf.log("cameralinkCam leaving main funtionc");
	//(*callbackFunction)(exiting,this);
	

	//cameralink_card->CleanUp();
	//delete(cameralink_card);
	
	
}//main





void ADCameralink::oneLoopImage(void)
{
	char mesx[256];

			
  int is_send_img=1;
				

			




			//if (cameralink_card != 0)
			//{
				if (this->pNDArrayPool)
				{
					size_t memsize;
					int imemsize;
					//!! where is array_pool set up? need to look at all constructors and mains in older code.
							imemsize=2*getIntParam(ADSizeX)*getIntParam(ADSizeY);
							imemsize=imemsize>>20;
							
							setIntegerParam(cor_nd_datasize,imemsize);
							setIntegerParam(cor_max_ndbuffers,this->pNDArrayPool->maxBuffers());
							setIntegerParam(cor_num_ndbuffers,this->pNDArrayPool->numBuffers());
							memsize=this->pNDArrayPool->maxMemory();
							memsize=memsize>>20;
							
							setIntegerParam(cor_max_ndmemory,(int)memsize);
							memsize=this->pNDArrayPool->memorySize();
							memsize=memsize>>20;
							setIntegerParam(cor_alloc_ndmemory,(int)memsize);
							setIntegerParam(cor_free_ndbuffers,this->pNDArrayPool->numFree());

							if (getIntParam(cor_nd_datasize)>0)
							{
								setIntegerParam(cor_est_buffers_left,
								getIntParam(cor_free_ndbuffers)+
								((getIntParam(cor_max_ndmemory)- getIntParam(cor_alloc_ndmemory))/getIntParam(cor_nd_datasize)  )     );
							}

				}
			//}








			
			if (cameralink_card != 0)
			{
				
				if (cameralink_card->isMissedFrame() &&getIntParam(is_grab) ==1)
				{
					lf.log("Missed Frames");
					grabGrabberMutex();
					setIntegerParam(total_missed_frames,cameralink_card->getTotalMissedFrames());
					setIntegerParam(recent_missed_frames,cameralink_card->getRecentMissedFrames());
					printf("cameralinkCam: ERROR: TOTAL MISSED FRAMES = %d\n",getIntParam(total_missed_frames));
					cameralink_card->clearMissedFrames();
					releaseGrabberMutex();
				}
				
			}
			

			grabGrabberMutex();
			if (cameralink_card != 0)
			{
				int fcnt;

				int nbytes;

				

				nbytes = getIntParam(ADSizeX)*getIntParam(ADSizeY)*2;
				fcnt=0;
				if (getIntParam(is_grab)==1)
				{
					while (

							(getIntParam(cor_use_image_mode)==0 ||
							(getIntParam(cor_use_image_mode)&&
								(
									(getIntParam(ADImageMode)==2) ||
									(getIntParam(ADImageMode)==1 &&
											getIntParam(ADNumImagesCounter) < getIntParam(ADNumImages)) ||
									(getIntParam(ADImageMode)==0 && getIntParam(ADNumImagesCounter) <1 )
									)
							)
							)
						&&

						(cameralink_card->isFrameAvailable() && fcnt<100)
						)

					{
						

						void *cm;
						unsigned short *cms;
						int status;
						int k;
						unsigned int cameralinkticks;
						int attrx;
						double attrxd;
						int memtries;
						fcnt++;

						//lf.log("captiured image");

						if (getIntParam(is_abort))
							break;
						
						setIntegerParam(cor_num_free_buffers,cameralink_card->getNumFreeBuffers());


							//(*callbackFunction)(before_image,this);
							
							//!! need to alloc an image somewhere


							int dd;
							  getIntegerParam(ADSizeX, &dd);
							  dims[0] = dd;
							  
							getIntegerParam(ADSizeY, &dd);
							dims[1] = dd;
							this->pImage = this->pNDArrayPool->alloc(2, (size_t*)dims, NDUInt16, (size_t)0, NULL);
							

							
						//	sprintf(mesx,"image ptr %i\n",(long)cm);
						//	lf.log(mesx);

							memtries=0;
							releaseGrabberMutex();
						

							
							grabGrabberMutex();

							
							is_send_img=1;
							//
							// if we cant get ndarray just puit into the reserved ndarray as a bitbucket.
							// in this way the grabber gets read, but we do not send image.
							//
							
							if (this->pImage==0)
							{
								this->pImage=this->pImageBitBucket;
								is_send_img=0;
								cameralink_card->incMissedFrames();
							}
							
							
							 array_pool= this->pImage->pNDArrayPool;
							
							  cm=this->pImage->pData;
							  
							
							
							setIntegerParam(cor_nd_datasize,(this->pImage->dataSize)>>20);
							setIntegerParam(cor_max_ndbuffers,array_pool->maxBuffers());
							setIntegerParam(cor_num_ndbuffers,array_pool->numBuffers());
							setIntegerParam(cor_max_ndmemory,(int)((array_pool->maxMemory())>>20));
							setIntegerParam(cor_alloc_ndmemory,(int)((array_pool->memorySize())>>20));
							setIntegerParam(cor_free_ndbuffers,array_pool->numFree());
							if (getIntParam(cor_nd_datasize)!=0)
							{
								setIntegerParam(cor_est_buffers_left,
								getIntParam(cor_free_ndbuffers)+
								((getIntParam(cor_max_ndmemory)- getIntParam(cor_alloc_ndmemory))/getIntParam(cor_nd_datasize)  )     );
							}

							try
							{
							status = cameralink_card->getFrame(
								cm,
								&cameralinkticks,
								nbytes);
							}
							catch(...)
							{

								lf.log("cameralink_card->getFrame threw exception");
							}

							this->pImage->timeStamp = 1e-6*((double)cameralinkticks);

							if (getIntParam(cor_check_timestamps))
							{
								ts_diff = cameralinkticks - last_timestamp;
								if (ts_diff > (ts_diff_last*10)  && ts_diff<getIntParam(cor_missed_ts_wait))
								{

									setIntegerParam(cor_num_missed_timestamp,getIntParam(cor_num_missed_timestamp)+1);
									//printf("ERROR - MISSED FRAME based on TimeStamps\n");
										lf.log("ERROR - MISSED FRAME based on TimeStamps\n");
								}

								if (cameralinkticks==last_timestamp)
								{
									//printf("ERROR-Repeated Frame based on timestamps\n");
									lf.log("ERROR-Repeated Frame based on timestamps\n");

									setIntegerParam(cor_num_repeat_timestamp,getIntParam(cor_num_repeat_timestamp)+1);

								}


							}
							ts_diff_last = ts_diff;

							last_timestamp=cameralinkticks;


						


							setIntegerParam(collect_status,status);
							cms=(unsigned short*)cm;
							

							
							setIntegerParam(
								ADNumImagesCounter,
								getIntParam(ADNumImagesCounter) + 1);
							

							if (getIntParam(cor_use_image_mode))
							{
								if (
									(getIntParam(ADImageMode)==1 &&
									(getIntParam(ADNumImagesCounter) == getIntParam(ADNumImages))) ||
									(getIntParam(ADImageMode)==0 &&
									(getIntParam(ADNumImagesCounter) == 1))

									)
								{
									setIntegerParam(ADAcquire,0);
							
								}
							}



							this->pImage->uniqueId=getIntParam(NDUniqueId);
							
							
							setIntegerParam(NDUniqueId,getIntParam(NDUniqueId)+1);

							setIntegerParam(is_new_image,1);


							getAttributes(pImage->pAttributeList);
							processNewImage(this->pImage);
							
							
							if (is_send_img==1)
							{
							  doCallbacksGenericPointer(this->pImage, NDArrayData, 0);
							  this->pImage->release();
							}
							callParamCallbacks();
					}//while  lots of stuff &&
				}//if (getIntParam(is_grab)==1)
				else
				{
					fcnt=0;
					//here grab is off.
					// we may have frames hanging in the buffers, so we should clear them out.
					while(cameralink_card->isFrameAvailable() && fcnt<100)
					{
						fcnt++;
						//copy the frame to nothing
						bool statx=cameralink_card->getFrame(0);
						if (statx)
							setIntegerParam(cor_frame_to_null,getIntParam(cor_frame_to_null)+1);
						else
							break;
		
					}
					
					if (fcnt>1)
						lf.log("ADCameralink.cpp-  Got throw away frames");
					//cameralink_card->clearMissedFrames();

				}	

			}//if cameralink_card
		
			releaseGrabberMutex();


}




void ADCameralink::processNewImage(void* img_ptr)
{

}
void ADCameralink::keepHouse(void)
{


}
	

void ADCameralink::grabberSetup(int function)
{
	char mesx[256];

			


	grabGrabberMutex();



			if (function == (cor_reset_buff_index))
			{
				
				if (cameralink_card != 0)
				{
					if (getIntParam(cor_reset_buff_index))
					{
						cameralink_card->resetBufferCount();
						lf.log("resetBufferCount");
					}

				}
				//setIntegerParam(operation_count, getIntParam(operation_count) + 1);
				//(*callbackFunction)(wakeup_everyone,this);
				
			}


			if (function == (is_grab))
			{
				
				if (cameralink_card != 0)
				{
					if (getIntParam(is_grab))
					{
						lf.log("aborting grab, turning grab ON");
						cameralink_card->abort();
						cameralink_card->resetBufferCount();
						cameralink_card->grab();
						lf.log("Grab ON");
					}
					else
					{
						//cameralink_card->abort();
						
						lf.log("Grab OFF, no abort");
					}
				}
				//setIntegerParam(operation_count, getIntParam(operation_count) + 1);
				//(*callbackFunction)(wakeup_everyone,this);
				
			}

			if (function == (is_abort))
			{
				lf.log("ABORT");
				setIntegerParam(is_grab,0);
				setIntegerParam(is_abort,0);

				//(*callbackFunction)(general,this);
				

				callParamCallbacks();	
				if (cameralink_card != 0)
				{
					cameralink_card->abort();


				}
				//setIntegerParam(operation_count, getIntParam(operation_count) + 1);
				//(*callbackFunction)(wakeup_everyone,this);
				
			}
			if (function == (is_snap))
			{
				lf.log("Snao");


				setIntegerParam(is_snap,0);
				
				if (cameralink_card != 0)
				{
					cameralink_card->snap();


				}
				//setIntegerParam(operation_count, getIntParam(operation_count) + 1);
				//(*callbackFunction)(wakeup_everyone,this);
				
			}


			if (function == (is_freeze))
			{
				lf.log("Freeze");
				setIntegerParam(is_grab,0);

				//(*callbackFunction)(general,this);
				
				callParamCallbacks();
				
				if (cameralink_card != 0)
				{
					cameralink_card->freeze();


				}
				//setIntegerParam(operation_count, getIntParam(operation_count) + 1);
				//(*callbackFunction)(wakeup_everyone,this);
				
			}


///

			if (getIntParam(is_dbg_viewer)==1 && function == (is_dbg_viewer))
			{
				
				if (cameralink_card!= 0)
					cameralink_card->makeView();

			}


			if (function == (is_trigpin0))
			{
				lf.log("setting CC pin 0");

				if (cameralink_card != 0)
				{
					cameralink_card->setPin("CC4", getIntParam(is_trigpin0));

				}
				
				//setIntegerParam(operation_count, getIntParam(operation_count) + 1);

				//(*callbackFunction)(wakeup_everyone,this);
				


			}





			


			if (getIntParam(is_loadccf))
			{

				setIntegerParam(is_loadccf,0);
				loadCCF();


			}

		releaseGrabberMutex();
}




void ADCameralink::loadCCF()
{
char mesx[256];

	lf.log("Shut down cameralink Card");
	
	if (cameralink_card != 0)
	{
		cameralink_card->DestroyObjects();
		delete(cameralink_card);
		cameralink_card = 0;
	}

	if (strcmp(getStringParam(cor_ccf_filename), "null") != 0)
	{
	
	sprintf(mesx,"ADCameralink::loadCCF- loading CCD file: %s",getStringParam(cor_ccf_filename));
    lf.log(mesx);

	
	#ifdef USE_SWGRAB
	cameralink_card = new softwareGrabber();
	
	#else
	#ifdef USE_SAP
	cameralink_card = new coreco(false);


	#else

	#ifdef USE_SISW
	cameralink_card = new siSoftware(false);
	#else
	#error NeedToDefineGrabber
	cameralink_card=new softwareGrabber();
	lf.log("Using Software Virtual Grabber");
	#endif
    #endif
	#endif

	setIntegerParam(cor_grabber_type,cameralink_card->getGrabberType());
		
	cameralink_card->setConfigFileName(getStringParam(cor_ccf_filename));
	
	
	cameralink_card->setNumBuffers(getIntParam(num_coreco_buffers));
	//for 10tap mode, width based on 8 bit data, but we stuff 12 or 16 bit data.
	//so for 16  bit data, we muilt wid by2, and for 12 birt, by 1.5.
	// is_mult_width2=0 for no mult, not 10 tap.
	// is_mult_width2=1 for mult by 2, in 10tap mode
	// is_mult_width2=2 for mult by 1.5 in 10 tap mode, 12 bit data
	cameralink_card->setDoubleWidth(getIntParam(is_mult_width2));
	if (! (
		cameralink_card->initialize(
		getIntParam(ADSizeX),
		getIntParam(ADSizeY),
		(bool)getIntParam(is_force_img_size))
		))
	{
		delete(cameralink_card);
		cameralink_card=0;
		lf.log("ADCameralink::  grabber->initialize cound not open the grabber");

	}

	if (cameralink_card)
	{
	setIntegerParam(cor_num_free_buffers,cameralink_card->getNumFreeBuffers());
	setIntegerParam(cor_num_buffers,cameralink_card->getNumBuffers());



	setIntegerParam(size_x,cameralink_card->getWidth());
	setIntegerParam(size_y,cameralink_card->getHeight());

	
	}

		setIntegerParam(operation_count, getIntParam(operation_count) + 1);
		if (cameralink_card)
			cameralink_card->setCamController(this);

		lf.log("loaded CCF");
	//	setIntegerParam(is_load_fpga_regs,1);
		
		//(*callbackFunction)(wakeup_everyone,this);
		}
		else
		{
			lf.log("null ccf file, cameralink card shut down");
			
		}
}






//#endif

 
 
 
/* Code for iocsh registration */
static const iocshArg ADCameralinkConfigArg0 = {"Port name", iocshArgString};
static const iocshArg ADCameralinkConfigArg1 = {"server port name", iocshArgString};
static const iocshArg ADCameralinkConfigArg2 = {"maxBuffers", iocshArgInt};
static const iocshArg ADCameralinkConfigArg3 = {"maxMemory", iocshArgInt};
static const iocshArg ADCameralinkConfigArg4 = {"priority", iocshArgInt};
static const iocshArg ADCameralinkConfigArg5 = {"stackSize", iocshArgInt};
static const iocshArg * const ADCameralinkConfigArgs[] =  {&ADCameralinkConfigArg0,
                                                     &ADCameralinkConfigArg1,
                                                     &ADCameralinkConfigArg2,
                                                     &ADCameralinkConfigArg3,
                                                     &ADCameralinkConfigArg4,
                                                     &ADCameralinkConfigArg5};
static const iocshFuncDef configADCameralink = {"ADCameralinkConfig", 6, ADCameralinkConfigArgs};
static void configADCameralinkCallFunc(const iocshArgBuf *args)
{
    ADCameralinkConfig(args[0].sval, args[1].sval, args[2].ival,
                 args[3].ival, args[4].ival, args[5].ival);
}


static void ADCameralink_ADRegister(void)
{
    iocshRegister(&configADCameralink, configADCameralinkCallFunc);
}

extern "C" {
epicsExportRegistrar(ADCameralink_ADRegister);
}




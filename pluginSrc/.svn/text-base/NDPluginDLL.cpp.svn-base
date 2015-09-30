/*
 * NDPluginDLL.cpp
 *
 * DLL plugin
 * Author: MADDEN
 *
 * Created Feb 21, 2011
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <epicsString.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsString.h>
#include <epicsStdio.h>
#include <epicsMutex.h>
#include <cantProceed.h>

#include <windows.h>


#include <iocsh.h>
#include "ADDriver.h"
#include "asynNDArrayDriver.h"


#include "NDArray.h"
#include "NDPluginDLL.h"

#include <epicsExport.h>




// store names of up to 64 threads
typedef void (__cdecl *OBJMAKER)(int*, genCamController*, void*, void**);


static char *thread_name[64];
static epicsEventId resume_event[64];
static volatile int thread_suspend_counters[64];

static bool is_debug_mess = false;
static char dll_name[256];




NDPluginDLL* NDPluginDLL::madSimPtr = 0;



    /** Callback function that is called by the NDArray driver with new NDArray data.
      * Grabs the current NDArray and applies the selected transforms to the data.  Apply the transforms in order.
      * \param[in] pArray  The NDArray from the callback.
      */
    void NDPluginDLL::processCallbacks(NDArray *pArray)
    {
        NDDimension_t dimsIn[ND_ARRAY_MAX_DIMS];
        NDArray *transformedArray;
        int ii;
        int colorMode;
        int k;
        const char* functionName = "processCallbacks";

        /* Call the base class method */
        NDPluginDriver::processCallbacks(pArray);


	if (controllers[0]->getPtrParam(genCamControllerEnum::gpu_ret_new_img) )
	{
        /* Previous version of the array was held in memory.  Release it and reserve a new one. */
        if (this->pArrays[0]) {
            this->pArrays[0]->release();
            this->pArrays[0] = NULL;
        }

		     pArrayOut = this->pNDArrayPool->copy(pArray, NULL, 1);
			 controllers[0]->putPtrParam(genCamControllerEnum::image_mem_ptr2,pArrayOut->pData);
	}

	//for (int k=0;k<num_controllers;k++)
	//{
		k=0;

		controllers[k]->putPtrParam(genCamControllerEnum::image_mem_ptr,pArray->pData);
		

		controllers[k]->putIntParam(genCamControllerEnum::gpu_img_sizex,pArray->dims[0].size);
		controllers[k]->putIntParam(genCamControllerEnum::gpu_img_sizey,pArray->dims[1].size);

		controllers[k]->processCallback(0);
	//}


		if (controllers[0]->getPtrParam(genCamControllerEnum::gpu_ret_new_img) )
		{

			this->pArrays[0] = pArrayOut;



	        doCallbacksGenericPointer(pArrayOut, NDArrayData,0);
		}

        callParamCallbacks();
    }





    /** Called when asyn clients call pasynInt32->write().
      * This function performs actions for some parameters, including transform type and origin.
      * For other parameters it calls NDPluginDriver::writeInt32 to see if that method understands the parameter.
      * For all parameters it sets the value in the parameter library and calls any registered callbacks..
      * \param[in] pasynUser pasynUser structure that encodes the reason and address.
      * \param[in] value Value to write. */

    asynStatus NDPluginDLL::writeInt32(asynUser *pasynUser, epicsInt32 value){
        int function = pasynUser->reason;
        asynStatus status = asynSuccess;
        int transformIndex;
        int enum_param;
        int k;

        status = setIntegerParam(function, value);



		for (k=0; k<num_controllers; k++)
		{

		if ((*mapAsyn2DLL).find(function)!= (*mapAsyn2DLL).end())
		{
			enum_param = (*mapAsyn2DLL)[function];

			controllers[k]->putIntParam(enum_param,(int)value);
		}

		}


        callParamCallbacks();
        return status;
    }






/*********************************************************************************
 *
 *
 *********************************************************************************/

asynStatus NDPluginDLL::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
	int k;
	int enum_param;

    /* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
     * status at the end, but that's OK */
    status = setDoubleParam(function, value);


	for (k=0; k<num_controllers; k++)
	{
		if ((*mapAsyn2DLL).find(function)!= (*mapAsyn2DLL).end())
		{
			enum_param = (*mapAsyn2DLL)[function];

			controllers[k]->putDoubleParam(enum_param,(double)value);
		}

	}


    /* Do callbacks so higher layers see any changes */
    callParamCallbacks();
    return status;
}



asynStatus NDPluginDLL::writeOctet(asynUser *pasynUser, const char *value,
                                    size_t nChars, size_t *nActual)
{
    int addr=0;
    int k;
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    const char *functionName = "writeOctet";
	int enum_param;

   // status = getAddress(pasynUser, functionName, &addr); if (status != asynSuccess) return(status);

	    /* Set the parameter in the parameter library. */
    status = (asynStatus)setStringParam(0, function, (char *)value);

//	int enum_param = (*mapAsyn2DLL)[function];
	for (k=0; k<num_controllers; k++)
	{

		if ((*mapAsyn2DLL).find(function)!= (*mapAsyn2DLL).end())
		{
			enum_param = (*mapAsyn2DLL)[function];


			controllers[k]->putStringParam(enum_param,(char*)value);
		}

	}

	printf("STR ctrls %i param %i val %i\n",num_controllers, function, value);


     /* Do callbacks so higher layers see any changes */
    status = (asynStatus)callParamCallbacks(addr, addr);

    *nActual = nChars;
    return status;
}



static void trapHere(void)
{
	int a = 1;
}




/** Constructor for NDPluginDLL; most parameters are simply passed to NDPluginDriver::NDPluginDriver.
  * After calling the base class constructor this method sets reasonable default values for all of the
  * Transform parameters.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] queueSize The number of NDArrays that the input queue for this plugin can hold when
  *            NDPluginDriverBlockingCallbacks=0.  Larger queues can decrease the number of dropped arrays,
  *            at the expense of more NDArray buffers being allocated from the underlying driver's NDArrayPool.
  * \param[in] blockingCallbacks Initial setting for the NDPluginDriverBlockingCallbacks flag.
  *            0=callbacks are queued and executed by the callback thread; 1 callbacks execute in the thread
  *            of the driver doing the callbacks.
  * \param[in] NDArrayPort Name of asyn port driver for initial source of NDArray callbacks.
  * \param[in] NDArrayAddr asyn port driver address for initial source of NDArray callbacks.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
NDPluginDLL::NDPluginDLL(const char *portName, int queueSize, int blockingCallbacks,
                         const char *NDArrayPort, int NDArrayAddr, int maxBuffers, size_t maxMemory,
                         int priority, int stackSize)
    /* Invoke the base class constructor */
    : NDPluginDriver(portName, queueSize, blockingCallbacks,
                   NDArrayPort, NDArrayAddr, 1, 1024, maxBuffers, maxMemory,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   asynInt32ArrayMask | asynFloat64ArrayMask | asynGenericPointerMask,
                   ASYN_MULTIDEVICE, 1, priority, stackSize)
{
    asynStatus status;
    const char *functionName = "NDPluginDLL";
    int ii, jj;

    int dims[2];
    HMODULE hMod;
	OBJMAKER makeControllerObjects;
	int gg;
	int_string *params;





	// map the params from asyn assign param to the enumerated param in the dll.

	mapAsyn2DLL = new std::map<int,int>;
	mapDLL2Asyn = new std::map<int,int>;

 dims[0] =1024;
    dims[1] = 1024;
    this->pRaw = this->pNDArrayPool->alloc(2, dims, NDUInt16, 0, NULL);

	printf("Made NDArray, X = %i  Y = %i Type = %i \n",1024, 1024,3);

	printf("Image data pointer = %i \n", (long)(pRaw->pData));


	hMod = LoadLibrary (dll_name);
	if (NULL == hMod)
	{
		printf("Load DLL failed\n");
			exit(0);
	}

	 makeControllerObjects = (OBJMAKER) GetProcAddress (hMod, "makeControllerObjects");
	if (NULL == makeControllerObjects)
	{
		long erx=GetLastError();
		printf("Cannot get Proc Address. Error= %i\n",erx);
	}

	makeControllerObjects(
		(int*)&num_controllers,
		(genCamController*)controllers,
		(void*)madSimCallback,
		&(this->pRaw->pData));



	printf("Controllers Found: %i \n",num_controllers);


	madSimPtr=this;
			// start threads per object
//	for (int k = 0; k<num_controllers; k++)
//	{
//		Sleep(2000);
//		madSimCallback(genCamControllerEnum::make_new_thread,controllers[k]);
//	}


	//
	// get param lists from the DLL and make all params.
	//
	int asyn_param;
	int enum_param;
	int ptype;
	int nparam;

	for (int k = 0; k<num_controllers; k++)
	{
		params=controllers[k]->getCompleteParamIntStrArray();
		nparam=controllers[k]->getTotalNumParams();

		for (int pp=0;pp<nparam;pp++)
		{
			if (pp==12)
				trapHere();

			enum_param=params[pp].x;
			ptype= controllers[k]->getParamType(enum_param);

			switch(ptype)
			{

				case putGetParameters::paramInt:
					createParam(params[pp].str,asynParamInt32,&asyn_param);
					(*mapAsyn2DLL)[asyn_param] = params[pp].x;
					(*mapDLL2Asyn)[params[pp].x] = asyn_param;


					break;

				case putGetParameters::paramDouble:
					createParam(params[pp].str,asynParamFloat64,&asyn_param);
					(*mapAsyn2DLL)[asyn_param] = params[pp].x;
					(*mapDLL2Asyn)[params[pp].x] = asyn_param;
				break;

				case putGetParameters::paramString:
					createParam(params[pp].str,asynParamOctet,&asyn_param);
					(*mapAsyn2DLL)[asyn_param] = params[pp].x;
					(*mapDLL2Asyn)[params[pp].x] = asyn_param;
				break;

				case putGetParameters::paramPtr:
					createParam(params[pp].str,asynParamInt32,&asyn_param);
					(*mapAsyn2DLL)[asyn_param] = params[pp].x;
					(*mapDLL2Asyn)[params[pp].x] = asyn_param;

				break;

				default:
					printf("ERROR: NDPluginDLL::NDPluginDLL: Create Undef Param\n");
				break;


			}

		}
	// copy ALL parameters from detector to this local Area Detector object
	//so EPICS can relect what detector is doing.
	lock();
	copyParamsFromDet(0);
	callParamCallbacks();
	unlock();


	}
//    createParam(NDPluginDLLNameString,         asynParamOctet, &NDPluginDLLName);
//    createParam(NDPluginDLL1TypeString,        asynParamInt32, &NDPluginDLL1Type);

    /* Set the plugin type string */
//    setStringParam(NDPluginDriverPluginType, "NDPluginDLL");
//    setStringParam(NDPluginDLLName, "");
//    setIntegerParam(NDPluginDLL1Type, 0);


	NDArrayDataPub=NDArrayData;

    /* Try to connect to the array port */
    status = connectToArrayPort();
}

/*********************************************************************************
 *
 *
 *********************************************************************************/

static void madTaskC(void *drvPvt)
{
    genCamController *pPvt = (genCamController *)drvPvt;

    pPvt->main(0);
}


/*********************************************************************************
 *
 *
 *********************************************************************************/


static int img_dumb_count = 0;


 int NDPluginDLL::madSimCallback(genCamControllerEnum::callbackReasons reason, void *pp)
{

	genCamController* cam=(genCamController*)pp;
	int k;
	unsigned short valx;
	unsigned short *datax;

    NDDataType_t dataType;
    int binX, binY, minX, minY, sizeX, sizeY, reverseX, reverseY;
    int maxSizeX, maxSizeY;
    NDDimension_t dimsOut[2];
    NDArrayInfo_t arrayInfo;

	// which thread object is calling back.
	// need wither several callbacks, or we need to make this one threadsafe.
	int id;
    int status;
		NDArray *pImage;
		   int dims[2];

	//madSimPtr->lock();
	id=cam->getThisID();




//	pImage = madSimPtr->pArrays[0];

	switch (reason)
	{


		case genCamControllerEnum::general:
			madSimPtr->lock();
			madSimPtr->copyParamsFromDet(1);
			madSimPtr->callParamCallbacks();
			madSimPtr->unlock();
		break;

		case genCamControllerEnum::before_image:
			madSimPtr->lock();

			if (is_debug_mess)
				printf("before image callback\n");

    		madSimPtr->getIntegerParam(genCamControllerEnum::ADSizeX, &dims[0]);
    		madSimPtr->getIntegerParam(genCamControllerEnum::ADSizeY, &dims[1]);
    		madSimPtr->pRaw->release();

			if (is_debug_mess)
    			printf("released praw\n");

			do
			{
				madSimPtr->pRaw =
					madSimPtr->
						pNDArrayPool->
							alloc(2, dims, NDUInt16, 0, NULL);

				if (madSimPtr->pRaw == 0)
				{
			//		printf("Could not alloc NDArray, try again afte 1s\n");
					Sleep(1000);
					madSimPtr->setIntegerParam(
						genCamControllerEnum::ADStatus,
						genCamControllerEnum::ADStatusError);
				}

			} while((madSimPtr->pRaw)==0);


			if (is_debug_mess)
			{
				printf("allocated praw %i \n",(long)madSimPtr->pRaw->pData);
				//printf("references coiunts %i \n",madSimPtr->pRaw->referenceCount);
			}
			cam->putPtrParam(genCamControllerEnum::image_mem_ptr,madSimPtr->pRaw->pData);

			madSimPtr->unlock();

		break;

		case genCamControllerEnum::add_image_attribute:

			// must have called before_image callback to set pRaw to new NDARRAY
			madSimPtr->pRaw->pAttributeList->add(
				(const char *)cam->attribute_name,
				(const char *)cam->attribute_description,
                (NDAttrDataType_t)cam->attribute_datatype,
                cam->attribute_pvalue);


		break;



		case genCamControllerEnum::new_image:
			madSimPtr->lock();

			if (is_debug_mess)
			  printf("new image callback\n");

		      madSimPtr->getIntegerParam(genCamControllerEnum::ADBinX,         &binX);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADBinY,         &binY);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADMinX,         &minX);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADMinY,         &minY);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADSizeX,        &sizeX);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADSizeY,        &sizeY);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADReverseX,     &reverseX);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADReverseY,     &reverseY);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADMaxSizeX,     &maxSizeX);
		      madSimPtr->getIntegerParam(genCamControllerEnum::ADMaxSizeY,     &maxSizeY);
		      madSimPtr->getIntegerParam(genCamControllerEnum::NDDataType,     (int *)&dataType);


		     /* Extract the region of interest with binning.
		       * If the entire image is being used (no ROI or binning) that's OK because
		       * convertImage detects that case and is very efficient */
		      madSimPtr->pRaw->initDimension(&dimsOut[0], sizeX);
		      dimsOut[0].binning = binX;
		      dimsOut[0].offset = minX;
		      dimsOut[0].reverse = reverseX;
		      madSimPtr->pRaw->initDimension(&dimsOut[1], sizeY);
		      dimsOut[1].binning = binY;
		      dimsOut[1].offset = minY;
		      dimsOut[1].reverse = reverseY;
		      /* We save the most recent image buffer so it can be used in the read() function.
		       * Now release it before getting a new version. */
		     // if (madSimPtr->pArrays[0]) madSimPtr->pArrays[0]->release();
			if (is_debug_mess)
				printf("mem ptr %i\n",(long)(madSimPtr->pRaw->pData));

			if (is_debug_mess)
				for (k=0;k<16;k++)
				{
					datax= (unsigned short *)(madSimPtr->pRaw->pData);
					valx = datax[k * sizeX*sizeY / 16];
					printf("pRaw data %i %i \n", k * sizeX*sizeY / 16, valx);
				}

			madSimPtr->pRaw->dims[0].size=sizeX;
			madSimPtr->pRaw->dims[0].offset=minX;
			madSimPtr->pRaw->dims[0].binning=binX;
			madSimPtr->pRaw->dims[0].reverse=reverseX;

			madSimPtr->pRaw->dims[1].size=sizeY;
			madSimPtr->pRaw->dims[1].offset=minY;
			madSimPtr->pRaw->dims[1].binning=binY;
			madSimPtr->pRaw->dims[1].reverse=reverseY;
//			madSimPtr->pRaw->dataSize = sizeY*sizeX*sizeof(unsigned short);
			madSimPtr->pRaw->dataType = NDUInt16;
			madSimPtr->pRaw->uniqueId=img_dumb_count;
			img_dumb_count++;




			//epicsMutexUnlock(madSimPtr->mutexId);
		// madSimPtr->doCallbacksGenericPointer(madSimPtr->pArrays[0], NDArrayData, 0);
		 madSimPtr->doCallbacksGenericPointer(madSimPtr->pRaw, madSimPtr->NDArrayDataPub, 0);

		//for (k=0; k<10; k++)
		//   printf(" %i ", * ((unsigned short *)(pImage->pData) +k));
			madSimPtr->callParamCallbacks();

		if (is_debug_mess)
			 printf("image callback done\n");

			madSimPtr->unlock();

		break;

		case genCamControllerEnum::make_new_thread:
			madSimPtr->lock();
			thread_suspend_counters[id] = 0;
		    thread_name[id]=new char[128];
		    sprintf(thread_name[id],"Thread %i",id);
		 /* Create the thread that updates the images */
		    status = (epicsThreadCreate(thread_name[id],
		                                epicsThreadPriorityMedium,
		                                epicsThreadGetStackSize(epicsThreadStackMedium),
		                                (EPICSTHREADFUNC)madTaskC,
		                                cam) == NULL);
		    if (status)
		        {
		        printf("epicsThreadCreate failure for image task\n");

    			}
    		else
    		 //printf("Thread Created\n");



			resume_event[id]= epicsEventCreate(epicsEventEmpty);

			if (!resume_event[id])
		        printf("epicsEventCreate failure for stop event\n");
       		else
    		 //printf("Epics Event Created\n");

			madSimPtr->unlock();
		break;

		case genCamControllerEnum::exiting:

		break;

		case genCamControllerEnum::grab_mutex:
				madSimPtr->lock();
//		        epicsMutexLock(madSimPtr->mutexId);


		break;

		case genCamControllerEnum::release_mutex:
			madSimPtr->unlock();
//                epicsMutexUnlock(madSimPtr->mutexId);

		break;

		case genCamControllerEnum::call_error:

		break;

		case genCamControllerEnum::thread_suspend:
			// a resume event incs counter. if the thread is not suspended it does not
			// need to be woken up./ If we get resume event, then thread suspends it may suspend
			// forever. IN this way we dont miss resume events.
		 // thread_suspend_counters[id]=thread_suspend_counters[id]-1;

		  //if (thread_suspend_counters[id] < 0)
		  //	thread_suspend_counters[id] = 0;

		  //if (thread_suspend_counters[id]<=0)// this way we dont miss wake up events
		     //epicsEventWait(resume_event[id]);
		     // cannot suspend for more than 20sec
		     epicsEventWaitWithTimeout(resume_event[id],  ((double)(cam->sleepy_time))/1000.0 );

		break;

		case genCamControllerEnum::thread_resume:
			//thread_suspend_counters[id] = thread_suspend_counters[id]+1;
			epicsEventSignal(resume_event[id]);

		break;

		case genCamControllerEnum::wakeup_everyone:

			for (k = 0; k<madSimPtr->num_controllers; k++)
			{
				epicsEventSignal(resume_event[ madSimPtr->controllers[k]->getThisID()  ]);
				//thread_suspend_counters[ madSimPtr->controllers[k]->getThisID()] =
				//	thread_suspend_counters[ madSimPtr->controllers[k]->getThisID() ]+1;
			}


		break;

		case genCamControllerEnum::sleep:
			// need the epics sleep!!
			epicsThreadSleep( ((double)(cam->sleepy_time))/1000.0 );
			// in this way it can be awakened by a signal.
			//epicsEventWaitWithTimeout(resume_event[id],((double)(cam->sleepy_time))/1000.0 );


		default:
		break;

	}
	//madSimPtr->unlock();

  return 0;
}
/*********************************************************************************
 *
 *
 *********************************************************************************/

void NDPluginDLL::copyParamsFromDet(int which_ones)
{

	int k;
	int t;
	int m;
	int asyn_param;
	int enum_param;
	int_string *params;
	int nparams;
	// which ones
	// 0 for all params
	// 1 for changed params

	for (m=0;m<num_controllers; m++)
	{
		nparams=controllers[m]->getTotalNumParams();
		params=controllers[m]->getCompleteParamIntStrArray();
		for (k=0;k<nparams; k++)
		{
			enum_param = params[k].x;

			t=controllers[m]->getParamType(enum_param);
			if (t != putGetParameters::paramUndef)
			{

				if (which_ones==0 || (which_ones==1 && controllers[m]->getIsChanged(enum_param)) )
				{

					//controllers[m]->clearChanges(k);
					//string
					if (t==putGetParameters::paramString)
					{
						asyn_param = (*mapDLL2Asyn)[enum_param];
						setStringParam(asyn_param, controllers[m]->getStringParam(enum_param));
					}
					else if (t==putGetParameters::paramInt)
					{
						asyn_param = (*mapDLL2Asyn)[enum_param];
						setIntegerParam(asyn_param, controllers[m]->getIntParam(enum_param));
					}
					else if (t==putGetParameters::paramDouble )
					{
						asyn_param = (*mapDLL2Asyn)[enum_param];
						setDoubleParam(asyn_param,(double)(controllers[m]->getDoubleParam(enum_param)));
					}
					else
						{}
				}
			}

		}
	}

}


/** Configuration command */
extern "C" int NDDLLConfigure(const char *portName, int queueSize, int blockingCallbacks,
                                 const char *NDArrayPort, int NDArrayAddr,
                                 int maxBuffers, size_t maxMemory,
                                 int priority, int stackSize)
{
    NDPluginDLL *pPlugin =
    new NDPluginDLL(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
                    maxBuffers, maxMemory, priority, stackSize);
    pPlugin = NULL;  /* This is just to eliminate compiler warning about unused variables/objects */
    return(asynSuccess);
}




/* Code for iocsh registration */

static const iocshArg dllPlgDllName = {"Dll Name", iocshArgString};
static const iocshArg * const dllPlgDllArgs[] =  {&dllPlgDllName};
static const iocshFuncDef dllNamedllPlg = {"setDllName", 1, dllPlgDllArgs};
static void dllPlgDllNameCallFunc(const iocshArgBuf *args)
{
	 strcpy(dll_name,args[0].sval);
}



/* EPICS iocsh shell commands */
static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg initArg1 = { "frame queue size",iocshArgInt};
static const iocshArg initArg2 = { "blocking callbacks",iocshArgInt};
static const iocshArg initArg3 = { "NDArrayPort",iocshArgString};
static const iocshArg initArg4 = { "NDArrayAddr",iocshArgInt};
static const iocshArg initArg5 = { "maxBuffers",iocshArgInt};
static const iocshArg initArg6 = { "maxMemory",iocshArgInt};
static const iocshArg initArg7 = { "priority",iocshArgInt};
static const iocshArg initArg8 = { "stackSize",iocshArgInt};
static const iocshArg * const initArgs[] = {&initArg0,
                                            &initArg1,
                                            &initArg2,
                                            &initArg3,
                                            &initArg4,
                                            &initArg5,
                                            &initArg6,
                                            &initArg7,
                                            &initArg8};
static const iocshFuncDef initFuncDef = {"NDDLLConfigure",9,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    NDDLLConfigure(args[0].sval, args[1].ival, args[2].ival,
                   args[3].sval, args[4].ival, args[5].ival,
                   args[6].ival, args[7].ival, args[8].ival);
}

extern "C" void NDDLLRegister(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
    iocshRegister(&dllNamedllPlg, dllPlgDllNameCallFunc);

}

extern "C" {
epicsExportRegistrar(NDDLLRegister);
}

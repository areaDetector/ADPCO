

/*
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>

#include <epicsStdio.h>
#include <epicsExport.h>
#include <iocsh.h>

#include <epicsTime.h>
#include <epicsThread.h>


#include "NDPluginFile.h"
#include "immViewPlugin.h"
#include "drvimmViewPlugin.h"
#include "asynNDArrayDriver.h"

static const char *driverName = "immViewPlugin";


// Section for Custom Parameters
//



//
//End Section for Custom Params
//



/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e) {asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, \
                "%s:%s error=%s\n", \
                driverName, functionName, nc_strerror(e)); \
                return(asynError);}

/* NDArray string attributes can be of any length, but netCDF requires a fixed maximum length
 * which we define here. */
#define MAX_ATTRIBUTE_STRING_SIZE 256


asynStatus immViewPlugin::openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray)
{
/*    nc_type ncType=NC_NAT;*/
	char str0[256];
	char str1[256];
	int fn0,fn1;
	char fullfname[256];
	int is_update;
    static const char *functionName = "openFile";

	
	double wt;
	
	

    return(asynSuccess);
}

/** This is called to write data a single NDArray to the file.  It can be called multiple times
 *  to add arrays to a single file in stream or capture mode */
asynStatus immViewPlugin::writeFile(NDArray *pArray)
{
	unsigned long int stripsize;
    static const char *functionName = "writeFile";
	int sizex, sizey;
	int is_update;
	int fnx;
	int ii0;
	int imgc,nimg;
	double wt;
	
	
	
	return(asynSuccess);
}



int immViewPlugin::getIntParam(int param)
{
	int saveparamx;
	getIntegerParam(param, &saveparamx);
	
	
	
	return(saveparamx);

}

/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void immViewPlugin::processCallbacks(NDArray *pArray)
{
double elapsed_time;
	//unsigned short *shrt_img;
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
	
int dd;
size_t dims[2];
							 

	NDArrayPool *mypool;
	
	mypool=pArray->pNDArrayPool;

	///
	///
	///

	// numAttributes = pArray->numAttributes();
	
	if (getIntParam(print_attr)==1)
	{
	 numAttributes = pArray->pAttributeList->count();

	printf("Num Attributes %i \n", numAttributes);
	pAttribute = pArray->pAttributeList->next(NULL);
		for (attrCount=0; attrCount<numAttributes; attrCount++)
		{

			pAttribute->getValueInfo(&attrDataType, &attrSize);
			//strcpy(name, pAttribute->pName);
			//strcpy(description, pAttribute->pDescription);
					// pAttribute->getDescription(description, sizeof(description));
			//pAttribute->getValue(attrDataType, void *pValue, attrSize);

			printf("Attr Name %s, Addr Disc %s\n",pAttribute->pName, pAttribute->pDescription);


				pAttribute = pArray->pAttributeList->next(pAttribute);
			// pAttribute = pArray->nextAttribute(pAttribute);

		}
	}
	

    //if (getIntParam(is_decompress)==1 || getIntParam(is_descramble)==1)


    xs = pArray->dims[0].size;
    ys = pArray->dims[1].size;

    rowa=0;

    rowb=ys-1;
    rows=0;

    rowbytes=xs*2;
    dims[0] = xs;
    dims[1] = ys;
    NDArray *nimg = mypool->alloc(2, (size_t*)dims, NDInt16, (size_t)0, NULL);


     if (nimg==0)
     {
         printf("ERROR: imm viewer plugin could not alloc nd array\n");
    }
    else
	{

          this->unlock();

         setIntegerParam(is_did_convert,0);


        mypool->copy(pArray,nimg,1);

        unsigned short *shrt_img = (unsigned short*)(nimg->pData);
		
		
         unsigned char* cimg=(unsigned char*)(pArray->pData);

		
		int sx,sy,np,tk;

		immHeader *immh = (immHeader*)cimg;
		if (immh->mode==2 && \
                immh->rows==pArray->dims[1].size && \
        		immh->cols==pArray->dims[0].size)
		{
        	    //imm file
        	    myimm.IMMtoRaw(
                		(unsigned char*)cimg,
                		pArray->dataSize,
                		(unsigned short*)shrt_img,
                		&sx,
                		&sy,
                		&np,
                		&tk);


                nimg->dims[0].size = sx;
                nimg->dims[1].size = sy;
                nimg->ndims = 2;
               //printf("immViewPligin:: converted IMM to raw\n");

                setIntegerParam(num_converted,1+ getIntParam(num_converted));
                setIntegerParam(is_did_convert,1);

		}

         nimg->uniqueId=arycnt++;

		nimg->timeStamp= pArray->timeStamp;
		nimg->uniqueId=getIntParam(NDUniqueId);
							
							
		setIntegerParam(NDUniqueId,getIntParam(NDUniqueId)+1);

		getAttributes(nimg->pAttributeList);
		
		doCallbacksGenericPointer(nimg, NDArrayData, 0);
		nimg->release();
		callParamCallbacks();

        this->lock();
	}
	

	
	
	
	// check for bad FPGA headers, get stats etc...
	// call base class function...
	NDPluginFile::processCallbacks(pArray);

	


}

asynStatus immViewPlugin::readFile(NDArray **pArray)
{
    //static const char *functionName = "readFile";

    return asynError;
}


asynStatus immViewPlugin::closeFile()
{
    static const char *functionName = "closeFile";
	int fnx;
	int is_update;

 

			return asynSuccess;
}

/*


/** Constructor for NDPluginFile; all parameters are simply passed to NDPluginDriver::NDPluginDriver.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] queueSize The number of NDArrays that the input queue for this plugin can hold when 
  *            NDPluginDriverBlockingCallbacks=0.  Larger queues can decrease the number of dropped arrays,
  *            at the expense of more NDArray buffers being allocated from the underlying driver's NDArrayPool.
  * \param[in] blockingCallbacks Initial setting for the NDPluginDriverBlockingCallbacks flag.
  *            0=callbacks are queued and executed by the callback thread; 1 callbacks execute in the thread
  *            of the driver doing the callbacks.
  * \param[in] NDArrayPort Name of asyn port driver for initial source of NDArray callbacks.
  * \param[in] NDArrayAddr asyn port driver address for initial source of NDArray callbacks.
  * \param[in] maxAddr The maximum  number of asyn addr addresses this driver supports. 1 is minimum.
  * \param[in] numParams The number of parameters supported by the derived class calling this constructor.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is 
  *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is 
  *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] interfaceMask Bit mask defining the asyn interfaces that this driver supports.
  * \param[in] interruptMask Bit mask definining the asyn interfaces that can generate interrupts (callbacks)
  * \param[in] asynFlags Flags when creating the asyn port driver; includes ASYN_CANBLOCK and ASYN_MULTIDEVICE.
  * \param[in] autoConnect The autoConnect flag for the asyn port driver.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  *
NDPluginFile::NDPluginFile(const char *portName, int queueSize, int blockingCallbacks, 
                           const char *NDArrayPort, int NDArrayAddr, int maxAddr, int numParams,
                           int maxBuffers, size_t maxMemory, int interfaceMask, int interruptMask,
                           int asynFlags, int autoConnect, int priority, int stackSize)

     Invoke the base class constructor.
     * We allocate 1 NDArray of unlimited size in the NDArray pool.
     * This driver can block (because writing a file can be slow), and it is not multi-device.  
     * Set autoconnect to 1.  priority and stacksize can be 0, which will use defaults. 
    : NDPluginDriver(portName, queueSize, blockingCallbacks, 
                     NDArrayPort, NDArrayAddr, maxAddr, numParams+NUM_NDPLUGIN_FILE_PARAMS, maxBuffers, maxMemory, 
                     asynGenericPointerMask, asynGenericPointerMask,
                     asynFlags, autoConnect, priority, stackSize)
{

*/


/* Configuration routine.  Called directly, or from the iocsh function in drvNDFileEpics */

extern "C" int drvimmViewPluginConfigure(const char *portName, int queueSize, int blockingCallbacks,
                                     const char *NDArrayPort, int NDArrayAddr,
                                     int priority, int stackSize)
{
    new immViewPlugin(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
                     priority, stackSize);
    return(asynSuccess);
}

/* The constructor for this class */
immViewPlugin::immViewPlugin(const char *portName, int queueSize, int blockingCallbacks,
                           const char *NDArrayPort, int NDArrayAddr,
                           int priority, int stackSize)
    : NDPluginFile(portName, queueSize, blockingCallbacks,
                  NDArrayPort, NDArrayAddr,
		   1, NUM_immViewPlugin_PARAMS, 200000, -1, asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize),
		   myimm(8000000)
{
	int i;

 //wait in sec
	
	
	createParam("print_attr",asynParamInt32,&print_attr);
	
    createParam("is_enable",asynParamInt32,&is_enable);

    createParam("is_did_convert",asynParamInt32,&is_did_convert);
    createParam("num_converted",asynParamInt32,&num_converted);


    arycnt=0;
	
		//enough for edge dimax and thensome
	temp_image=(unsigned short*)malloc(2560*2200*sizeof(short));

	setIntegerParam(is_enable, 0);
    setIntegerParam(is_did_convert,0);
    setIntegerParam(num_converted, 0);
	
    this->supportsMultipleArrays = 1;
    this->pAttributeId = NULL;






}





/*
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netcdf.h>

#include <epicsStdio.h>
#include <epicsExport.h>
#include <iocsh.h>

#include <epicsTime.h>
#include <epicsThread.h>


#include "NDPluginFile.h"
#include "simPlugin.h"
#include "drvSimPlugin.h"
#include "asynNDArrayDriver.h"

static const char *driverName = "simPlugin";


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


asynStatus simPlugin::openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray)
{
/*    nc_type ncType=NC_NAT;*/
    char str0[256];
    char str1[256];
    int fn0,fn1;
    char fullfname[256];
    int is_update;
    static const char *functionName = "openFile";

    
    double wt;
    
    getDoubleParam(open_waittime,&wt);
    epicsThreadSleep(wt);
    

    return(asynSuccess);
}

/** This is called to write data a single NDArray to the file.  It can be called multiple times
 *  to add arrays to a single file in stream or capture mode */
asynStatus simPlugin::writeFile(NDArray *pArray)
{
    unsigned long int stripsize;
    static const char *functionName = "writeFile";
    int sizex, sizey;
    int is_update;
    int fnx;
    int ii0;
    int imgc,nimg;
    double wt;
    
    
    getDoubleParam(write_waittime,&wt);
    epicsThreadSleep(wt);
    
    return(asynSuccess);
}
/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void simPlugin::processCallbacks(NDArray *pArray)
{



    NDArrayPool *mypool;
    
    mypool=pArray->pNDArrayPool;
    
    setIntegerParam(max_buffers,mypool->maxBuffers());
    setIntegerParam(num_buffers,mypool->numBuffers());
    setDoubleParam(max_memory,(double)(mypool->maxMemory()) / 1.0e6);
    setDoubleParam(mem_size,(double)(mypool->memorySize()) / 1.0e6);
    setIntegerParam(num_free,mypool->numFree());
    
    

    // numAttributes = pArray->numAttributes();
    numAttributes = pArray->pAttributeList->count();

//    printf("Num Attributes %i \n", numAttributes);
    pAttribute = pArray->pAttributeList->next(NULL);
    for (attrCount=0; attrCount<numAttributes; attrCount++)
    {

        pAttribute->getValueInfo(&attrDataType, &attrSize);
        strcpy(name, pAttribute->pName);
        strcpy(description, pAttribute->pDescription);
                // pAttribute->getDescription(description, sizeof(description));
        //pAttribute->getValue(attrDataType, void *pValue, attrSize);

        


            pAttribute = pArray->pAttributeList->next(pAttribute);
        // pAttribute = pArray->nextAttribute(pAttribute);

    }




    /*


    */


    // check for bad FPGA headers, get stats etc...
    // call base class function...
    NDPluginFile::processCallbacks(pArray);





}

asynStatus simPlugin::readFile(NDArray **pArray)
{
    //static const char *functionName = "readFile";

    return asynError;
}


asynStatus simPlugin::closeFile()
{
    static const char *functionName = "closeFile";
    int fnx;
    int is_update;

 

            return asynSuccess;
}




/* Configuration routine.  Called directly, or from the iocsh function in drvNDFileEpics */

extern "C" int drvSimPluginConfigure(const char *portName, int queueSize, int blockingCallbacks,
                                     const char *NDArrayPort, int NDArrayAddr,
                                     int priority, int stackSize)
{
    new simPlugin(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
                     priority, stackSize);
    return(asynSuccess);
}

/* The constructor for this class */
simPlugin::simPlugin(const char *portName, int queueSize, int blockingCallbacks,
                           const char *NDArrayPort, int NDArrayAddr,
                           int priority, int stackSize)
    : NDPluginFile(portName, queueSize, blockingCallbacks,
                  NDArrayPort, NDArrayAddr,
           1, NUM_SIMPLUGIN_PARAMS, 1, -1, asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize)
{
    int i;


    createParam("open_waittime",asynParamFloat64,&open_waittime);
    createParam("write_waittime",asynParamFloat64,&write_waittime);

    createParam("max_buffers",asynParamInt32,&max_buffers);
    createParam("num_buffers",asynParamInt32,&num_buffers);
    createParam("max_memory",asynParamFloat64,&max_memory);
    createParam("mem_size",asynParamFloat64,&mem_size);
    createParam("num_free",asynParamInt32,&num_free);
    
    setDoubleParam(open_waittime,0.0);
    setDoubleParam(write_waittime,0.0);
    
    
    
    this->supportsMultipleArrays = 1;
    this->pAttributeId = NULL;






}


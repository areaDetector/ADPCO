#ifndef NDPluginDLL_H
#define NDPluginDLL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <map>


#include <epicsTypes.h>
#include <asynStandardInterfaces.h>

#include "NDPluginDriver.h"
#include "genCamController.h"


/* The following enum is for each of the Transforms */
#define NDPluginDLLFirstTransformNParam NDPluginDriverLastParam




/** Map parameter enums to strings that will be used to set up EPICS databases
  */

//typedef NDTransformIndex_t (*transformFunctions_t) (NDTransformIndex_t, int, int) ;

static const char* pluginName = "NDPluginDLL";


/** Perform transformations (rotations, flips) on NDArrays.   */
class NDPluginDLL : public NDPluginDriver {
public:
    NDPluginDLL(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int maxBuffers, size_t maxMemory,
                 int priority, int stackSize);
    /* These methods override the virtual methods in the base class */
    void processCallbacks(NDArray *pArray);


    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual asynStatus writeOctet(
		asynUser *pasynUser,
		const char *value,
	    size_t nChars,
	    size_t *nActual);

    /* These methods are unique to this class */


	NDArray *pArrayOut;


	void copyParamsFromDet(int which_ones);


	genCamController *controllers[64];
	int num_controllers;


    NDArray *pRaw;

   static int madSimCallback(genCamControllerEnum::callbackReasons reason, void *pp);
	static NDPluginDLL	*madSimPtr;

	int NDArrayDataPub;


	// map the params from asyn assign param to the enumerated param in the dll.
	std::map<int,int> *mapAsyn2DLL;
	std::map<int,int> *mapDLL2Asyn;


protected:
 //   int NDPluginDLLName;           /* (asynOctet;   r/w) Name of this Transform */
   // #define FIRST_TRANSFORM_PARAM NDPluginDLLName
  //INTS HERE
   // #define LAST_TRANSFORM_PARAM NDPluginDLLArraySize2








private:

//    int userDims[ND_ARRAY_MAX_DIMS];
//    int realDims[ND_ARRAY_MAX_DIMS];
//    int transformFlipsAxes(int);
//    void setMaxSizes(int);
//    int maxTransforms;
//    NDTransform_t *pTransforms;    /* Array of NDTransform structures */
//    int originLocation;
//    epicsInt32 *totalArray;
//    epicsInt32 *netArray;


};
//#define NUM_TRANSFORM_PARAMS (&LAST_TRANSFORM_PARAM - &FIRST_TRANSFORM_PARAM + 1)

#endif

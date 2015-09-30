/*
 * simPlugin.h
  *Tim Madden
 */

#ifndef simPlugin_H
#define simPlugin_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
using std::string;
//#include <direct.h>

#include "NDPluginFile.h"








/** Enums for plugin-specific parameters. There are currently no specific parameters for this driver yet.
  * It uses the ADStdDriverParams and NDPluginDriver params. */


class simPlugin : public NDPluginFile {
public:
    simPlugin(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int priority, int stackSize);

	virtual void processCallbacks(NDArray *pArray);
    /* The methods that this class implements */
    virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray);
    virtual asynStatus readFile(NDArray **pArray);
    virtual asynStatus writeFile(NDArray *pArray);
    virtual asynStatus closeFile();

    
 //virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
 //                            const char **pptypeName, size_t *psize);
protected:



	
    int arrayDataId;
    int uniqueIdId;
    int timeStampId;
    int nextRecord;
    int *pAttributeId;


	NDAttribute *pAttribute;
    char name[256];
    char description[256];
    char tempString[256];
    NDAttrDataType_t attrDataType;
    size_t attrSize;
    int numAttributes, attrCount;

	//
	// ////////////   PARAMETERS ////////////////////
	//
	
	int sim_first;
	 #define FIRST_SIMPLUGIN_PARAM sim_first

	 //wait in sec
	int open_waittime;
	//wait in sec
	int write_waittime;
	
	
	int max_buffers;
	int num_buffers;
	int max_memory;
	int mem_size;
	int num_free;
	
	int sim_last;
	 #define LAST_SIMPLUGIN_PARAM sim_last
	
	
	//
	//////////////// END PARAMETERS ///////////////////
	//
	


};


#define NUM_SIMPLUGIN_PARAMS (&LAST_SIMPLUGIN_PARAM - &FIRST_SIMPLUGIN_PARAM + 1)

#endif

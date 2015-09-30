/*
 * immViewPlugin.h
  *Tim Madden
 */

#ifndef immViewPlugin_H
#define immViewPlugin_H


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



#include "imm.h"




/** Enums for plugin-specific parameters. There are currently no specific parameters for this driver yet.
  * It uses the ADStdDriverParams and NDPluginDriver params. */


class immViewPlugin : public NDPluginFile {
public:
    immViewPlugin(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int priority, int stackSize);

	virtual void processCallbacks(NDArray *pArray);
    /* The methods that this class implements */
    virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray);
    virtual asynStatus readFile(NDArray **pArray);
    virtual asynStatus writeFile(NDArray *pArray);
    virtual asynStatus closeFile();

	int getIntParam(int param);

    
 //virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
 //                            const char **pptypeName, size_t *psize);
protected:

	imm myimm;

//enough for 1 edge imgae. pco imges are smaller...
	unsigned short *temp_image;

	
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
	 #define FIRST_immViewPlugin_PARAM sim_first

	 //wait in sec
	
	int print_attr;
	
	int is_enable;
	
    int is_did_convert;
    int num_converted;

    int arycnt;
	
	
	int sim_last;
	 #define LAST_immViewPlugin_PARAM sim_last
	
	
	//
	//////////////// END PARAMETERS ///////////////////
	//
	


};


#define NUM_immViewPlugin_PARAMS (&LAST_immViewPlugin_PARAM - &FIRST_immViewPlugin_PARAM + 1)

#endif

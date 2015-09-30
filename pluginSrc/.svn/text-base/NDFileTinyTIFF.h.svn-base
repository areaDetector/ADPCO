/*
 * NDFileTinyTIFF.h
 * Writes NDArrays to TIFF files.
 * Tim Madden
 *use tinytiff code- home grown.
 * will support streaming tiff
 *supports new tags, supports 4k header section for metadata.
 */

#ifndef NDFileTinyTIFF_H
#define NDFileTinyTIFF_H


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <string>
using std::string;
#include <direct.h>

#include "NDPluginFile.h"
//#include "tiffio.h"
#include "tinytiff.h"

using namespace tinytiff_plugin;






/* This version number is an attribute in the TIFF file to allow readers
 * to handle changes in the file contents */
//#define NDTIFFFileVersion 1.0

/* Max String Param Length */

/* Max String Param Length */
struct param_type_str
{

	param_type_str(int *ix, int ip, char* sx){
		str_ptr=new char[128];
		int_ptr=ix;
		param_type = ip;
		strcpy(str_ptr,sx);
		};
	~param_type_str(){delete(str_ptr);};

	int *int_ptr;
	int param_type;
	char *str_ptr;
};


/** Enums for plugin-specific parameters. There are currently no specific parameters for this driver yet.
  * It uses the ADStdDriverParams and NDPluginDriver params. */


/** Writes NDArrays in the TIFF file format.
    Tagged Image File Format is a file format for storing images.  The format was originally created by Aldus corporation and is
    currently developed by Adobe Systems Incorporated.  This plugin was developed using the libtiff library to write the file.
    The current version is only capable of writes 2D images with 1 image per file.
    */

class NDFileTinyTIFF : public NDPluginFile {
public:
    NDFileTinyTIFF(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int priority, int stackSize);

	virtual void processCallbacks(NDArray *pArray);
    /* The methods that this class implements */
    virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray);
    virtual asynStatus readFile(NDArray **pArray);
    virtual asynStatus writeFile(NDArray *pArray);
    virtual asynStatus closeFile();

    asynStatus writeOctet(asynUser *pasynUser, const char *value,
                                    size_t nChars, size_t *nActual);

 //virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
 //                            const char **pptypeName, size_t *psize);
protected:



	int recursePath(char *pathstr, bool is_makedirs);

    int arrayDataId;
    int uniqueIdId;
    int timeStampId;
    int nextRecord;
    int *pAttributeId;

	tinytiff *f_tiff;

	NDAttribute *pAttribute;
    char name[256];
    char description[256];
    char tempString[256];
    NDAttrDataType_t attrDataType;
    size_t attrSize;
    int numAttributes, attrCount;

	int ii0,ii1;

	int threshold, is_fpga_comp;
		double acq_time;
	unsigned int corecoticks;
	unsigned int num_bad_fpgaheads;
	int fpga_pixels;
	unsigned int fpga_timestamp;
	int fpga_comp_frames;


	int bytesperpix;
	int cam_type;
	int fileformat;

	int last_filenumber;
	bool is_valid_head;

	NDFileOpenMode_t openModesave;
	int capture_trig;
	int filenum_kludge;




	int NDFileTinyTIFF_threshold;

	int ND_makedirs;

	int	NDFileTinyTIFF_is_recapture;

	enum {num_params=3};
	param_type_str *paramStrings[num_params];


};

#endif

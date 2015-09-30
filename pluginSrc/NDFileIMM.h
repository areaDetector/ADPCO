/*
 * NDFileTIFF.h
 * Writes NDArrays to TIFF files.
 * John Hammonds
 * April 17, 2009
 */

#ifndef NDFileIMM_H
#define NDFileIMM_H


#ifdef _WIN32
#include <windows.h>

#endif


#ifndef _WIN32
#define _stat stat
#define _chmod chmod

#endif


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
#include "compressed_file2.h"

/* This version number is an attribute in the TIFF file to allow readers
 * to handle changes in the file contents */
//#define NDTIFFFileVersion 1.0

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








using namespace nd_imm_plugin;

/** Writes NDArrays in the TIFF file format.
    Tagged Image File Format is a file format for storing images.  The format was originally created by Aldus corporation and is
    currently developed by Adobe Systems Incorporated.  This plugin was developed using the libtiff library to write the file.
    The current version is only capable of writes 2D images with 1 image per file.
    */

class NDFileIMM : public NDPluginFile {
public:
    NDFileIMM(const char *portName, int queueSize, int blockingCallbacks,
                 const char *NDArrayPort, int NDArrayAddr,
                 int priority, int stackSize);

	virtual void processCallbacks(NDArray *pArray);
    /* The methods that this class implements */
    virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray);
    virtual asynStatus readFile(NDArray **pArray);
    virtual asynStatus writeFile(NDArray *pArray);
    virtual asynStatus closeFile();

// virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
//                             const char **pptypeName, size_t *psize);



	int recursePath(char *pathstr, bool is_makedirs);


asynStatus writeOctet(
	asynUser *pasynUser, 
	const char *value,
      	size_t nChars, 
	size_t *nActual);

protected:
    int arrayDataId;
    int uniqueIdId;
    int timeStampId;
    int nextRecord;
    int *pAttributeId;

		bool is_open_good;

	compressed_file *cf;

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
	comp_img_header *fpga_header;

    int is_already_imm;
    int pipe_num_shorts;

	int bytesperpix;
	int cam_type;
	int fileformat;

	int last_filenumber;
	bool is_valid_head;


	int last_unique_id;
	double last_timestamp;
	int timestamp_reset_counter;

	// if fpga or coreco attrubute,we use this timestamp.
	bool is_coreco_timestamp;
	double general_timestamp;


	enum which_timestamp {
		xcorecoticks,
		xfpga_ts,
		xarray_timestamp,
		xarray_unique_id

	};

	NDArray *my_array;

	//
	// Params
	//
	int NDFileIMM_threshold;
	int NDFileIMM_is_fpga_comp;
	int NDFileIMM_fpga_timestamp;
	int NDFileIMM_grabber_timestamp;
	int NDFileIMM_num_bad_fpgaheads;
	int NDFileIMM_num_fpga_pixels;
	int NDFileIMM_num_comp_frames;
    int NDFileIMM_timestamp;
    int NDFileIMM_uniqueID;
    int NDFileIMM_fileCorecoticks;
    int NDFileIMM_fileSysticks;
    int NDFileIMM_fileElapsed;
    int NDFileIMM_printAttributes;
    int NDFileIMM_NmissedTimeStamps;
    int NDFileIMM_framePeriod;
    int NDFileIMM_NmissedIDs;
	int NDFileIMM_Nimg_rst_ts;
	int NDFileIMM_throw_images;
	int NDFileIMM_fileevent;

	enum {num_params=19};
	param_type_str *paramStrings[num_params];

		unsigned int file_coreco_ts;
		double file_elapsed_ts;
		unsigned int file_systick_ts;


	int getIntParam(int param){
		int output;
		getIntegerParam(param,&output);
		return(output);
	};


	double getDoubParam(int param){
		double output;
		getDoubleParam(param,&output);
		return(output);
	};

};

#endif

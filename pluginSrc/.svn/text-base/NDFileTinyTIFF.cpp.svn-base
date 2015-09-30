

/*
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netcdf.h>

#include <epicsStdio.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "NDPluginFile.h"
#include "NDFileTinyTIFF.h"
#include "drvNDFileTinyTIFF.h"
// #include "tiffio.h"
#include "tinytiff.h"
#include "asynNDArrayDriver.h"

static const char *driverName = "NDFileTinyTIFF";


using namespace tinytiff_plugin;
//
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



/** Called when asyn clients call pasynOctet->write().
  * This function performs actions for some parameters, including NDPluginDriverArrayPort.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Address of the string to write.
  * \param[in] nChars Number of characters to write.
  * \param[out] nActual Number of characters actually written. */
asynStatus NDFileTinyTIFF::writeOctet(asynUser *pasynUser, const char *value,
                                    size_t nChars, size_t *nActual)
{
    int addr=0;
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;
    char pathstr[512];
    int statx;
    int is_makedirs;

    const char *functionName = "writeOctet";

    status = getAddress(pasynUser, &addr); if (status != asynSuccess) return(status);
    /* Set the parameter in the parameter library. */
    status = (asynStatus)setStringParam(addr, function, (char *)value);

    status = NDPluginFile::writeOctet(pasynUser, value, nChars, nActual);


    if (function == NDFilePath) {


			getStringParam(NDFilePath, sizeof(pathstr), pathstr);
			getIntegerParam(ND_makedirs,&is_makedirs);
			statx = recursePath(pathstr, (bool)is_makedirs);
			printf("Recurse path: statis = %d\n",statx);

			if (statx==0)
				setIntegerParam(NDFilePathExists, 1);
			else
				setIntegerParam(NDFilePathExists, 0);


    }
	

     /* Do callbacks so higher layers see any changes */
    status = (asynStatus)callParamCallbacks(addr, addr);

    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, value=%s",
                  driverName, functionName, status, function, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, value=%s\n",
              driverName, functionName, function, value);
    *nActual = nChars;
    return status;
}



/** This is called to open a TIFF file.
*/
asynStatus NDFileTinyTIFF::openFile(const char *fileName, NDFileOpenMode_t openMode, NDArray *pArray)
{
    /* When we create TIFF variables and dimensions, we get back an
     * ID for each one. */
/*    nc_type ncType=NC_NAT;*/
	char str0[256];
	char str1[256];
	int fn0,fn1;
	char fullfname[256];
	int is_update;
    static const char *functionName = "openFile";


	//enable retrig capture strat
	capture_trig=1;

	this->openModesave = openMode;

    /* We don't support reading yet */
    if (openMode & NDFileModeRead) return(asynError);

    /* We don't support opening an existing file for appending yet */
    if (openMode & NDFileModeAppend) return(asynError);

    /* Set the next record in the file to 0 */

	num_bad_fpgaheads=0;





				getStringParam(NDFileName,255,str0);
				getStringParam(NDFilePath,255,str1);
				getIntegerParam(NDFileNumber,&fn0);
				getIntegerParam(NDFileNumCapture,&fn1);

	// this is a kludge because base class uincs the NDFileNumber for us. we are negating that so
				// we can keep track ourselves here.
	getIntegerParam(NDAutoIncrement,&is_update);
	if (is_update==1 && filenum_kludge==0)
	{
		fn0=fn0-1;
		setIntegerParam(NDFileNumber,fn0);
	}

// keep track of filenumber so if base class messes it up we are not consused...
	//			last_filenumber = fn0;

	this->nextRecord = 0;
	try {

	if (openMode&NDFileModeMultiple)
	{
		f_tiff->setMultiFrames(fn1);
		sprintf(fullfname,"%s%s_%05d_%05d.tif",str1,str0,fn0,fn1);
	}
	else
	{
		sprintf(fullfname,"%s%s_%05d.tif",str1,str0,fn0);
		f_tiff->setMultiFrames(1);
	}


	// if filename is "pipe" then we are ariting to named pipe...
	//
	if (strcmp(str0,"pipe")==0)
		f_tiff->is_use_pipe = true;
	else
		f_tiff->is_use_pipe = false;



	f_tiff->open_w(fullfname);



		}
		catch (tinytiff_exception ex)
		{
			printf("%s\n",ex.err_mess());
		}

	setStringParam(NDFullFileName,f_tiff->file_name);

    return(asynSuccess);
}

/** This is called to write data a single NDArray to the file.  It can be called multiple times
 *  to add arrays to a single file in stream or capture mode */
asynStatus NDFileTinyTIFF::writeFile(NDArray *pArray)
{
	unsigned long int stripsize;
    static const char *functionName = "writeFile";
	int sizex, sizey;
	int is_update;
	int fnx;
	int ii0;
	int imgc,nimg;


	//
	// For splitting CAPTURE mode saving into several large files.
	//
	//getIntegerParam(NDFileTinyTIFF_nimages_in_file,&nimg);
	//getIntegerParam(NDFileTinyTIFF_img_counter,&imgc);

	//if (imgc>=nimg)
	//{
	//	closeFile();
	//	openFile(0, openModesave, pArray);

	//
	//}
	//imgc++;
	//setIntegerParam(NDFileTinyTIFF_img_counter,imgc);


		getIntegerParam(NDFileNumber,&fnx);

		getIntegerParam(NDAutoIncrement,&is_update);



	sizex = pArray->dims[0].size;
	sizey = pArray->dims[1].size;




//NDFileFormat

	getIntegerParam(NDFileFormat,&fileformat);


	//setIntegerParam(NDFileTinyTIFF_threshold,threshold);
// need to determint what type of TinyTIFF to wrute:
//raw data, FPGA compressed data, raw data written as compressed
// for now just do raw...

		//printf("saveFileTinyTIFFRaw\n");
	//	cf->saveFileTinyTIFFRaw(
	//	corecoticks,//timestamp- ise ndarray param
	//	threshold,//thresh
	//	sizex,
	//	sizey,
	//	bytesperpix,// bytes per pix-00 need to get this from NDArray... use short for now
	//	pArray->pData,
	//	this->nextRecord,//fiklenumber- where do we get it?
	//	cam_type,// camtype should be a ndarray param
	//	acq_time//acq period- do we read the param or use an ndarray param?
	//	);

		try
		{

		f_tiff->tifWr((unsigned short*)pArray->pData,sizex,sizey);
		}
		catch (tinytiff_exception ex)
		{
			printf("%s\n",ex.err_mess());
		}
		this->nextRecord++;


		if (is_update)
			setIntegerParam(NDFileNumber,fnx+1);



	setIntegerParam(NDFileNumCaptured,this->nextRecord);
    getIntegerParam(NDFileNumCapture,&ii0);
	if (this->nextRecord >= ii0)
	{
		setIntegerParam(NDFileNumCaptured,ii0);
		closeFile();
		setIntegerParam(NDFileCapture, 0);


	}
	return(asynSuccess);
}
/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void NDFileTinyTIFF::processCallbacks(NDArray *pArray)
{


	threshold = 0;
	is_fpga_comp = 0;
	acq_time = 0.0;
	corecoticks=0;






	// enable the automatic restarting of capture. In this way we can write tons of files with Ncapture images in each
		int capture_forever,cap;

		getIntegerParam(NDFileTinyTIFF_is_recapture,&capture_forever);
		getIntegerParam(NDFileCapture,&cap);

		if (capture_forever==0)
			capture_trig=0;

		if ( cap==0 && capture_forever>0 && capture_trig==1)
		{
			setIntegerParam(NDFileCapture,1);
			setIntegerParam(NDFileNumCaptured, 0);
					//setIntegerParam(NDWriteFile, 1);
					filenum_kludge=1;
					openFile(0,openModesave, pArray);
					filenum_kludge=0;

		}




	// numAttributes = pArray->numAttributes();
	numAttributes = pArray->pAttributeList->count();

//	printf("Num Attributes %i \n", numAttributes);
	pAttribute = pArray->pAttributeList->next(NULL);
	for (attrCount=0; attrCount<numAttributes; attrCount++)
	{

        pAttribute->getValueInfo(&attrDataType, &attrSize);
        strcpy(name, pAttribute->pName);
        strcpy(description, pAttribute->pDescription);
                // pAttribute->getDescription(description, sizeof(description));
		//pAttribute->getValue(attrDataType, void *pValue, attrSize);

		// TinyTIFF_threshold
		// TinyTIFF_fpga_compressed
		// TinyTIFF_acq_time
		// TinyTIFF_coreco_ticks


	        pAttribute = pArray->pAttributeList->next(pAttribute);
		// pAttribute = pArray->nextAttribute(pAttribute);

	}




	/*


	*/


	fpga_timestamp=0;
	fpga_pixels=0;
	fpga_comp_frames=0;



	// check for bad FPGA headers, get stats etc...
	// call base class function...
	NDPluginFile::processCallbacks(pArray);





}

asynStatus NDFileTinyTIFF::readFile(NDArray **pArray)
{
    //static const char *functionName = "readFile";

    return asynError;
}


asynStatus NDFileTinyTIFF::closeFile()
{
    static const char *functionName = "closeFile";
	int fnx;
	int is_update;

   // printf("TinyTIFF closeFile\n");
	f_tiff->close(f_tiff->file_pointer);


			return asynSuccess;
}




/* Configuration routine.  Called directly, or from the iocsh function in drvNDFileEpics */

extern "C" int drvNDFileTinyTIFFConfigure(const char *portName, int queueSize, int blockingCallbacks,
                                     const char *NDArrayPort, int NDArrayAddr,
                                     int priority, int stackSize)
{
    new NDFileTinyTIFF(portName, queueSize, blockingCallbacks, NDArrayPort, NDArrayAddr,
                     priority, stackSize);
    return(asynSuccess);
}

/* The constructor for this class */
NDFileTinyTIFF::NDFileTinyTIFF(const char *portName, int queueSize, int blockingCallbacks,
                           const char *NDArrayPort, int NDArrayAddr,
                           int priority, int stackSize)
    : NDPluginFile(portName, queueSize, blockingCallbacks,
                  NDArrayPort, NDArrayAddr,
		   1, NDFileTinyTIFF::num_params, 1, -1, asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize)
{
	int i;


	paramStrings[0]=new param_type_str(&NDFileTinyTIFF_threshold,asynParamInt32,"NDFileTinyTIFF_threshold");
	paramStrings[1]=new param_type_str(&NDFileTinyTIFF_is_recapture,asynParamInt32,"NDFileTinyTIFF_is_recapture");
	paramStrings[2]=new param_type_str(&ND_makedirs,asynParamInt32,"EZ_is_makedirs");

	for (i=0;i<num_params;i++)
	{

		  createParam(
		    paramStrings[i]->str_ptr,
		  	(asynParamType)(paramStrings[i]->param_type),
		  	paramStrings[i]->int_ptr);

		  	 printf("Create %d Param: Str %s  , Param %d \n",
		  	 	paramStrings[i]->param_type,
				paramStrings[i]->str_ptr,
		  		*(paramStrings[i]->int_ptr));

	}

    this->supportsMultipleArrays = 1;
    this->pAttributeId = NULL;

    	f_tiff= new tinytiff();


		num_bad_fpgaheads=0;
		last_filenumber=0;
			//setIntegerParam(NDFileTinyTIFF_max_filesize,1000000);
			//setIntegerParam(NDFileTinyTIFF_nimages_in_file,1);
			//setIntegerParam(NDFileTinyTIFF_img_counter,0);

			//hits capture START on fileclose
			setIntegerParam(NDFileTinyTIFF_is_recapture,0);
			capture_trig=0;
			filenum_kludge=0;
			setIntegerParam(ND_makedirs,0);



}

// searches path part by part.
// path is of the form part/part/part/part
// return
// -64 if drive not exist.
// -1 if 1st path not exist, -2 if 2md part not exist.
// -128| part if cannot create dir. ex -128-2 if cant create 2nd part.

int NDFileTinyTIFF::recursePath(char *pathstr, bool is_makedirs)
{

#ifndef _WIN32
#define _stat stat
#endif


	struct _stat buff;
	int status;
	int slash_pos=0;
	int last_slash_pos = 0;
	int start = 0;
	int part_number = 1;

	string part_path, path_word;

	if (strlen(pathstr)==0)
		return(-1);

	string full_path(pathstr);

	// make sure last char is /

	
	if (full_path[full_path.size()-1] != '/')
		full_path.append("/",1);

	//search thru path name part by part
	slash_pos = full_path.find(string("/"),0);
	while (slash_pos!=string::npos)
	{
		// path up to / not incl /
		//here we are actually calling the copy constructor...of part path. The returned str from substr?
		// prob in memory inside fullpath,,, it will go out of scope and destruct?
		part_path = full_path.substr(start, slash_pos);
	

		// this part of path... say path is D:/aaa/bbb/ccc   then worrd is bbb for eample
		if (full_path[last_slash_pos] == '/')
			path_word = full_path.substr(last_slash_pos+1, (slash_pos -last_slash_pos-1));
		else
			path_word = full_path.substr(last_slash_pos, (slash_pos  -last_slash_pos));


		//check if we are looking at a drive, check for : at pos 1
		if (path_word[1] == ':')
		{
			//a drive
			//somehow make sure drive exists.

			// for drive need / at end.
			part_path.append("/",1);
			status = _stat(part_path.c_str(), &buff);
			if (status!=0)
				return(-64-part_number);



			// if not exist return(-64 -part_num);
		}
		else
		{

			status = _stat(part_path.c_str(), &buff);

			if (!is_makedirs)
			{
				if (status!=0)
					return(0-part_number);
			}
			else
			{
				if (status!=0)
				{
					
#ifdef _WIN32					
					status= _mkdir(part_path.c_str());
#else
					status= mkdir(part_path.c_str(),777);

#endif

					if (status!=0)
					{
						printf("ERROR: Could Not Dir %s\n",part_path.c_str());
						return(-128-part_number);
					}
					else
						printf("Created Dir %s\n",part_path.c_str());
				}
			}


		}

		//search thru path name part by part
		last_slash_pos = slash_pos;
		slash_pos = full_path.find(string("/"),slash_pos+1);
		part_number++;

	}

	return(0);


}

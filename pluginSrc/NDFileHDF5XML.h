/*
 * NDFileHDF5XML.h
 * Writes NDArrays to TIFF files.
 * Tim Madden
 *use tinytiff code- home grown.
 * will support streaming tiff
 *supports new tags, supports 4k header section for metadata.
 */

#ifndef NDFileHDF5XML_H
#define NDFileHDF5XML_H

#include "NDPluginFile.h"
//#include "tiffio.h"
#include "hdf5Visitor.h"

#ifdef _WIN32
#include <windows.h>

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


#ifdef USELOGFILE
#include "logfile.h"
#else
// if we done link against the log file bin...
class _log_file
{
public:
  log_file(char* x){}
  void log(char *x){printf("%s\n",x);}
  void enablePrintf(bool is_pr){}

};

#define log_file _log_file

#endif



class epics2hdf : public hdfEpicsInterface
{
public:
   epics2hdf(asynUser* pasynUser);
   virtual pArraySpecs* getNDArraySpecs(pArraySpecs *a);
   // you must provide memory for all output pointers, pvval, attr_val, datatype
   // provide pvname or attr name, return data and datatype

   virtual void* epicsCaGet(const char* pvname, void* pvvalue, hid_t *datatype);
   virtual void* getNDAttr(const char* attr_name, void* attr_val,
         hid_t *datatype);

   NDArray *pArray;
   
   hid_t type_nd2hdf(NDDataType_t datatype);
   hid_t type_ndatr2hdf(NDAttrDataType_t datatype);

   char scratch[256];
};

/* This version number is an attribute in the TIFF file to allow readers
 * to handle changes in the file contents */
//#define NDTIFFFileVersion 1.0
/* Max String Param Length */

/* Max String Param Length */

/*
struct param_type_str
{

   param_type_str(int *ix, int ip, char* sx)
   {
      str_ptr = new char[128];
      int_ptr = ix;
      param_type = ip;
      strcpy(str_ptr, sx);
   }
   ;
   ~param_type_str()
   {
      delete (str_ptr);
   }
   ;

   int *int_ptr;
   int param_type;
   char *str_ptr;
};
*/
/** Enums for plugin-specific parameters. There are currently no specific parameters for this driver yet.
 * It uses the ADStdDriverParams and NDPluginDriver params. */

/** Writes NDArrays in the TIFF file format.
 Tagged Image File Format is a file format for storing images.  The format was originally created by Aldus corporation and is
 currently developed by Adobe Systems Incorporated.  This plugin was developed using the libtiff library to write the file.
 The current version is only capable of writes 2D images with 1 image per file.
 */

class NDFileHDF5XML : public NDPluginFile
{
public:
   NDFileHDF5XML(const char *portName, int queueSize, int blockingCallbacks,
         const char *NDArrayPort, int NDArrayAddr, int priority, int stackSize);
   virtual ~NDFileHDF5XML();

   virtual void processCallbacks(NDArray *pArray);
   /* The methods that this class implements */
   virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode,
         NDArray *pArray);
   virtual asynStatus readFile(NDArray **pArray);
   virtual asynStatus writeFile(NDArray *pArray);
   virtual asynStatus closeFile();

   
   virtual int recursePath(char *pathstr, bool is_makedirs);
   virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
   asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t nChars,
         size_t *nActual);
   asynStatus lockConfigFile();
   asynStatus unlockConfigFile();
   //virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
   //                            const char **pptypeName, size_t *psize);
protected:
   int arrayDataId;
   int uniqueIdId;
   int timeStampId;
   int nextRecord;
   int *pAttributeId;

   void loadTemplateFile(void);

   TiXmlDocument *configDoc;
   TiXMLhdf5SetupNew *hdfSetup;
   TiXMLhdf5WriteData *hdf5Writer;
   TiXMLhdf5Shutdown *hdf5Closer;

   epics2hdf *hdf_interface;

   NDAttribute *pAttribute;
   char name[256];
   char description[256];
   char tempString[256];
   NDAttrDataType_t attrDataType;
   size_t attrSize;
   int numAttributes, attrCount;

   int ii0, ii1;

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

   int NDFileHDF5XML_threshold;

   int NDFileHDF5XML_is_recapture;
   int NDFileHDF5XML_templatefile;
   int NDFileHDF5XML_templatepath;
   int NDFileHDF5XML_TemplateValid;
   
   int NDFileHDF5XML_createDirs;
   
   enum
   {
      maxfilenamelen = 128
   };
   //the file name needs to be longer to allow writing the XML straight
   //into the waveform record
   enum
   {
      maxfilenamelen2 = 102400
   };

   char template_fullname[maxfilenamelen2];
   char template_path[maxfilenamelen];
   char template_file[maxfilenamelen2];
   char hdf_fullname[maxfilenamelen2];

   enum
   {
      num_params = 6
   };
   //param_type_str *paramStrings[num_params];

private:

log_file lf;
	NDArray *dummyArray;
   epicsMutexId configFileMutex;
   bool is_file_open;
   
   char longmsg[65536];
};

#endif

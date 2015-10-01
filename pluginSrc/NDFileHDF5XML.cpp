/*
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netcdf.h>

#include <epicsStdio.h>
#include <epicsExport.h>
#include <iocsh.h>
#include <epicsMutex.h>

#include "NDPluginFile.h"
#include "NDFileHDF5XML.h"
#include "drvNDFileHDF5XML.h"A
#include "asynNDArrayDriver.h"

#include <time.h>
static const char *driverName = "NDFileHDF5XML";

/*
 ISO Date time format

 * Date and time represents a specified time of a specified day. When use is made of the calendar date the representation is:

 YYYY-MM-DDThh:mm:ss

 where the capital letter T is used to separate the date and time components. Thus, for a very precise date and time, look at this:

 Example: 2003-04-01T13:01:02 represents one minute and two seconds after one o'clock in the afternoon of 2003-04-01.


 */

//
// There are 2 classes here:
// NDPluginFile derived class, AD1-6 version.
// hdfEpicsInterface. This is a class to interface epics to hdf.
// hdf uses the Visitor/Composite Object design. Use tinyxml, to make a composite object based on xml file.
// timyxml accepts a visitor object to do ops on each element in the tree.
// this is hdf5Visitor class. To seperate hdf5 writing from epics, we have a helper class to interface epics to visitor.
// we construct the hdf5Visitor, hdfEpicsInterface, then pass hdfEpicsInterface to the hdf5Visitor
//we consturct the tinyxml, , then call tinyxml.accept(my visitor) to write hdf 5.
// Then if the hdf needs an attrivute or PV value, it can get it from the hdfEpicsInterface
//
// hdfEpicsInterface functions...
//
/**
 *
 */
epics2hdf::epics2hdf(asynUser* pasynUser)
{
   this->pasynUser = pasynUser;
}

/**
 *
 */
pArraySpecs* epics2hdf::getNDArraySpecs(pArraySpecs *a)
{
   a->rank = 2;
   a->dims[0] = pArray->dims[0].size;
   a->dims[1] = pArray->dims[1].size;

   a->nd_datatype = pArray->dataType;
   a->hdf_datatype = type_nd2hdf(pArray->dataType);

   a->hdf_rank = 3;
   a->hdf_dims[2] = a->dims[0];
   a->hdf_dims[1] = a->dims[1];
   a->hdf_dims[0] = 0;

   //H5D_UNLIMITED undefined... compile err.r... so use 1000000
   a->hdf_maxdims[1] = a->hdf_dims[1];
   a->hdf_maxdims[2] = a->hdf_dims[2]; //H5S_UNLIMITED
   a->hdf_maxdims[0] = H5S_UNLIMITED;

   a->hdf_chunkdims[1] = a->hdf_dims[1];
   a->hdf_chunkdims[2] = a->hdf_dims[2]; //H5S_UNLIMITED
   a->hdf_chunkdims[0] = 1;

   a->data = pArray->pData;
   return (a);

}

void* epics2hdf::epicsCaGet(const char* pvname, void* pvvalue, hid_t *datatype)
{
   *datatype = 0;
   strcpy((char*) pvvalue, "Error: EpicsPV in xml file Not Supported");
   return (pvvalue);
}

void* epics2hdf::getNDAttr(const char* attr_name, void* attr_val,
      hid_t *datatype)
{
   NDAttrDataType_t attrDataType;
   NDAttribute *pAttr;
   size_t attrDataSize;
   hid_t nd_hdf_type;

   if (strcmp(attr_name, "sysclock") == 0)
   {
      *datatype = H5T_NATIVE_DOUBLE;
      double *x;
      x = (double*) attr_val;
      *x = clock() / CLOCKS_PER_SEC;
   }
   else if (strcmp(attr_name, "timestamp") == 0)
   {
      *datatype = H5T_NATIVE_DOUBLE;
      double *x;
      x = (double*) attr_val;
      *x = pArray->timeStamp;
   }
   else if (strcmp(attr_name, "uniqueId") == 0)
   {
      *datatype = H5T_NATIVE_INT;
      int *x;
      x = (int*) attr_val;
      *x = pArray->uniqueId;
   }
   else if (strcmp(attr_name, "datetime") == 0)
   {
      //2003-04-01T13:01:02 is ISO format.
      time_t rawtime;

      struct tm *today;
      char tstr[128];
      time(&rawtime);
      today = localtime(&rawtime);
      sprintf(tstr, "%04i-%02i-%02iT%02i:%02i:%02i", today->tm_year + 1900,
            today->tm_mon + 1, today->tm_mday, today->tm_hour, today->tm_min,
            today->tm_sec);

      *datatype = H5T_STR_NULLTERM;
      strcpy((char*) attr_val, tstr);

   }
   else
   {
      pAttr = pArray->pAttributeList->find(attr_name);

      if (pAttr != NULL)
      {
         pAttr->getValueInfo(&attrDataType, &attrDataSize);
         nd_hdf_type = type_ndatr2hdf(attrDataType);

         *datatype = nd_hdf_type;

         if (nd_hdf_type >= 0)
         {

            pAttr->getValue(attrDataType, (char *) attr_val, attrDataSize);
         }
         else
         {
            printf("epics2hdf::getNDAttr bad NDAttr datatype %s\n", attr_name);
            *datatype = 0;
            strcpy((char*) attr_val, "Error: Unknown Attribute Datatype");

         }
      } //pAttr
      else
      {
         int attrCount;
         printf("epics2hdf::getNDAttr Could not find NDAttr %s\n", attr_name);
         *datatype = 0;
         strcpy((char*) attr_val, "Error: Unknown Attribute");
         // list the attributes
         printf("List of attributes in image\n");
         pAttr = pArray->pAttributeList->next(NULL);
         for (attrCount = 0; attrCount < pArray->pAttributeList->count();
               attrCount++)
         {

            printf("Attr: %s \n", pAttr->pName);
            pAttr = pArray->pAttributeList->next(pAttr);
         }

      } //pAttr
   }

   return (attr_val);
}

/** Translate the NDArray datatype to HDF5 datatypes */
hid_t epics2hdf::type_nd2hdf(NDDataType_t datatype)
{
   hid_t result;

   switch (datatype)
      {
   case NDInt8:
      result = H5T_NATIVE_INT8;
      break;
   case NDUInt8:
      result = H5T_NATIVE_UINT8;
      break;
   case NDInt16:
      result = H5T_NATIVE_INT16;
      break;
   case NDUInt16:
      result = H5T_NATIVE_UINT16;
      break;
   case NDInt32:
      result = H5T_NATIVE_INT32;
      break;
   case NDUInt32:
      result = H5T_NATIVE_UINT32;
      break;
   case NDFloat32:
      result = H5T_NATIVE_FLOAT;
      break;
   case NDFloat64:
      result = H5T_NATIVE_DOUBLE;
      break;
   default:
      result = -1;
      }
   return result;
}

/** Translate the NDArray datatype to HDF5 datatypes */
hid_t epics2hdf::type_ndatr2hdf(NDAttrDataType_t datatype)
{
   hid_t result;

   switch (datatype)
      {
   case NDAttrInt8:
      result = H5T_NATIVE_INT8;
      break;
   case NDAttrUInt8:
      result = H5T_NATIVE_UINT8;
      break;
   case NDAttrInt16:
      result = H5T_NATIVE_INT16;
      break;
   case NDAttrUInt16:
      result = H5T_NATIVE_UINT16;
      break;
   case NDAttrInt32:
      result = H5T_NATIVE_INT32;
      break;
   case NDAttrUInt32:
      result = H5T_NATIVE_UINT32;
      break;
   case NDAttrFloat32:
      result = H5T_NATIVE_FLOAT;
      break;
   case NDAttrFloat64:
      result = H5T_NATIVE_DOUBLE;
      break;
   case NDAttrString:
      result = H5T_STR_NULLTERM;
      break;
   default:
      result = -1;
      }
   return result;
}

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

/** This is called to open a TIFF file.
 */
asynStatus NDFileHDF5XML::openFile(const char *fileName,
      NDFileOpenMode_t openMode, NDArray *pArray)
{
   char str0[256];
   char str1[256];
   int fn0, fn1;
   int is_update;
   static const char *functionName = "openFile";
/*   if (lockConfigFile() != asynSuccess)
   {
      return asynError;
   }
*/
   //enable retrig capture strat
   capture_trig = 1;

   
   lf.log("NDFileHDF5XML::openFile");
   
   
   this->openModesave = openMode;

   /* We don't support reading yet */
   if (openMode & NDFileModeRead)
      return (asynError);

   /* We don't support opening an existing file for appending yet */
   if (openMode & NDFileModeAppend)
      return (asynError);

   /* Set the next record in the file to 0 */

   //num_bad_fpgaheads=0;
   this->nextRecord = 0;

#if 0
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
   //            last_filenumber = fn0;

   if (openMode&NDFileModeMultiple)
   {

      sprintf(hdf_fullname,"%s%s_%05d_%05d.hdf",str1,str0,fn0,fn1);
   }
   else
   {
      sprintf(hdf_fullname,"%s%s_%05d.hdf",str1,str0,fn0);
   }

#endif
   strcpy(hdf_fullname, fileName);

   loadTemplateFile();

   hdf_interface->pArray = pArray;

   hdfSetup->setFileName(hdf_fullname);
   hdfSetup->setLeaveOpen(true);
  //!!  lock();
   if (!configDoc->Accept(hdfSetup)){
      return (asynError);
      is_file_open = false;
   }
   else{
      is_file_open = true;
   }
   //!!unlock();
   hdf5Writer->setFileName(hdf_fullname);
   hdf5Writer->setLeaveOpen(true);
   hdf5Writer->setHDFFilePtr(hdfSetup->getHDFFilePtr());
   hdf5Writer->setHDFAccess(hdf_append);

   hdf5Closer->setFileName(hdf_fullname);
   hdf5Closer->setLeaveOpen(false);
   hdf5Closer->setHDFFilePtr(hdfSetup->getHDFFilePtr());
   hdf5Closer->setHDFAccess(hdf_append);

   setStringParam(NDFullFileName, hdf_fullname);

   return (asynSuccess);
}

/** This is called to write data a single NDArray to the file.  It can be called multiple times
 *  to add arrays to a single file in stream or capture mode */
asynStatus NDFileHDF5XML::writeFile(NDArray *pArray)
{
   unsigned long int stripsize;
   static const char *functionName = "writeFile";
   int sizex, sizey;
   int numToCapture;
   int imgc, nimg;
   int writeMode;

   sizex = pArray->dims[0].size;
   sizey = pArray->dims[1].size;

//NDFileFormat

   getIntegerParam(NDFileFormat, &fileformat);
   getIntegerParam(NDFileWriteMode, &writeMode);

//   pArray->pAttributeList->report(20);
   getAttributes(pArray->pAttributeList);
   hdf_interface->pArray = pArray;



   //This will write the image data...
   //!!lock();
   configDoc->Accept(hdf5Writer);
   //!!unlock();
   this->nextRecord++;

   setIntegerParam(NDFileNumCaptured, this->nextRecord);
   getIntegerParam(NDFileNumCapture, &numToCapture);
/*   if ((((writeMode == NDFileModeStream) || (writeMode == NDFileModeCapture)) &&
         this->nextRecord >= numToCapture) )
   {
      setIntegerParam(NDFileNumCaptured, numToCapture);
      hdf_interface->pArray = pArray;


      closeFile();

   }
*/
   return (asynSuccess);
}
/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void NDFileHDF5XML::processCallbacks(NDArray *pArray)
{

   threshold = 0;
   is_fpga_comp = 0;
   acq_time = 0.0;
   corecoticks = 0;

   // enable the automatic restarting of capture. In this way we can write tons of files with Ncapture images in each
   int capture_forever, cap;

   getIntegerParam(NDFileHDF5XML_is_recapture, &capture_forever);
   getIntegerParam(NDFileCapture, &cap);

   if (capture_forever == 0)
      capture_trig = 0;

   if (cap == 0 && capture_forever > 0 && capture_trig == 1)
   {
      setIntegerParam(NDFileCapture, 1);
      setIntegerParam(NDFileNumCaptured, 0);
      //setIntegerParam(NDWriteFile, 1);
      filenum_kludge = 1;
      openFile(0, openModesave, pArray);
      filenum_kludge = 0;

   }

   // numAttributes = pArray->numAttributes();
   numAttributes = pArray->pAttributeList->count();

//    printf("Num Attributes %i \n", numAttributes);
   pAttribute = pArray->pAttributeList->next(NULL);
   for (attrCount = 0; attrCount < numAttributes; attrCount++)
   {

      pAttribute->getValueInfo(&attrDataType, &attrSize);
      strcpy(name, pAttribute->pName);
      strcpy(description, pAttribute->pDescription);

      pAttribute = pArray->pAttributeList->next(pAttribute);

   }

   /*


    */

   fpga_timestamp = 0;
   fpga_pixels = 0;
   fpga_comp_frames = 0;

   // check for bad FPGA headers, get stats etc...
   // call base class function...
   NDPluginFile::processCallbacks(pArray);

}

/**
 *
 */
asynStatus NDFileHDF5XML::readFile(NDArray **pArray)
{
   //static const char *functionName = "readFile";

   return asynError;
}

/**
 *
 */
void NDFileHDF5XML::loadTemplateFile(void)
{
   bool loadStatus = false;
   int status = asynSuccess;
   int addr = 0;
   static const char *functionName = "loadTemplateFile";

   
   lf.log("NDFileHDF5XML::loadTemplateFile");
/*   if (lockConfigFile() ==asynSuccess){
      /* get the filename to be used for nexus template */
      status = getStringParam(addr, NDFileHDF5XML_templatepath,
            sizeof(template_path), template_path);
      status = getStringParam(addr, NDFileHDF5XML_templatefile,
            sizeof(template_file), template_file);

      delete (configDoc);
      configDoc = new TiXmlDocument();

      if ( strstr(template_file, "<?xml") == NULL) {
         sprintf(template_fullname, "%s%s", template_path, template_file);
         if (strlen(template_fullname) == 0)
            return;

         /* Load the HDF template file */
        //!! lock();
         loadStatus = configDoc->LoadFile(template_fullname);
         
         lf.log("Load template from file");
       //!!  unlock();
      }
      else {
         //printf("Template contents:\n%s\n", template_file);
       //!! lock();
         configDoc->Parse(template_file, 0, TIXML_ENCODING_UTF8);
         lf.log("Parse xml in the pv");
       //!!  unlock();
         if (configDoc->Error()){
            loadStatus = false;
            lf.log("parsing error in xml");
         }
         else {
            loadStatus = true;
            lf.log("parsed xml correctly");
         }
      }
      if (loadStatus != true)
      {
         lf.log("Cound not load xml");
         asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
               "%s:%s: Parameter file %s is invalid\n", driverName, functionName,
               template_fullname);
         asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
               "%s:%s: XML parsing message on (row, col) (%d,%d)\n%s\n", driverName, functionName,
               configDoc->ErrorRow(), configDoc->ErrorCol(), configDoc->ErrorDesc());
         setIntegerParam(addr, NDFileHDF5XML_TemplateValid, 0);
         callParamCallbacks(addr, addr);

         asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
               "xml not found- use default\n");
         loadStatus = configDoc->Parse(default_xml);

         return;
      }
      else
      {
         lf.log("loaded xml");
         asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
               "%s:%s: Parameter file %s was successfully loaded\n", driverName,
               functionName, template_fullname);
         setIntegerParam(addr, NDFileHDF5XML_TemplateValid, 1);
         callParamCallbacks(addr, addr);
      }
/*      unlockConfigFile();
   }
   else
   {
      asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
            "%s:%s: Parameter file %s is locked.  Cannot load a new file now.\n", driverName,
            functionName, template_fullname);
      setIntegerParam(addr, NDFileHDF5XML_TemplateValid, 0);
      callParamCallbacks(addr, addr);
   }
*/
}

asynStatus NDFileHDF5XML::closeFile()
{
   static const char *functionName = "closeFile";
   //This will write the image data...
//   setIntegerParam(NDFileNumCaptured, numToCapture);
  //!!tjm lock();// took lock out,. we are not altering the pimage, so no lock needed.
   //!! we also have a reference to i, as we have not called release() yet.
   
   lf.log("call NDFileHDF5XML::closeFile");
   
   if (is_file_open)
   {
    lf.log("Close HDF file");
       // we can get attr. for ONCloseFile in xml file. so attr are written on file close.
       // we cant wrute valid img data on close, but that is life. Deal with it.
       //!! above comment in 2012. Now its 2013...
       //!! it seems that processCallbacks grabs latest pArray, then stores it in pArrays[0],
       //!! that is, a pointer to it. it won't release it until the next process callbacks.
       //!! close file is called by writefile or readfile in superclass, so pArrays[0] should
       //!! be valid. image data should be valid. the way it would work to write image on close
       //!! is that we get img from detector, send to plugins.
       //!! xml file tells where to write the file.. close file is called by write file, so
       //!! as far as AD is concerned, who cares. the file just gets written in another funciton.
       getAttributes(this->pArrays[0]->pAttributeList);
       hdf_interface->pArray = this->pArrays[0];
      configDoc->Accept(hdf5Closer);
      is_file_open = false;
   }
  //!! tjm unlock();
   setIntegerParam(NDFileCapture, 0);

   return asynSuccess;
}



/** Called when asyn clients call pasynInt32->write().
  * This function performs actions for some parameters, including ADAcquire, ADBinX, etc.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus NDFileHDF5XML::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
   
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   
  

   status = setIntegerParam(function, value);
   
   
   char mesx[256];
   char *mesx2;
   
   getParamName(function, (const char**)&mesx2);
   
     sprintf(mesx,"NDFileHDF5XML::writeInt32, param=%s, value=%d",mesx2,value);
   lf.log(mesx);
   
   
   
        NDPluginFile::writeInt32(pasynUser,  value);


        //make sure we dont have any mutexes...
        //releaseSerialMutex();
        //releaseGrabberMutex();

    callParamCallbacks();
    
        status=asynSuccess;
    return(status);

}







/** Called when asyn clients call pasynOctet->write().
 * Catch parameter changes.  If the user changes the path or name of the template file
 * load the new template file.
 * \param[in] pasynUser pasynUser structure that encodes the reason and address.
 * \param[in] value Address of the string to write.
 * \param[in] nChars Number of characters to write.
 * \param[out] nActual Number of characters actually written. */
asynStatus NDFileHDF5XML::writeOctet(asynUser *pasynUser, const char *value,
      size_t nChars, size_t *nActual)
{
   int addr = 0;
   int function = pasynUser->reason;
   asynStatus status = asynSuccess;
   const char *functionName = "writeOctet";
char pathstr[512];

   status = getAddress(pasynUser, &addr);
   if (status != asynSuccess)
      return (status);
   /* Set the parameter in the parameter library. */
   status = (asynStatus) setStringParam(addr, function, (char *) value);

   char mesx[256];
   char *mesx2;
   
   getParamName(function, (const char**)&mesx2);
   
     sprintf(longmsg,"NDFileHDF5XML::writeOctet, param=%s, value=%s",mesx2,value);
   lf.log(longmsg);
   
  
    if (function == NDFilePath) {

            int is_makedirs;
            int statx;
            
            getStringParam(NDFilePath, sizeof(pathstr), pathstr);
            getIntegerParam(NDFileHDF5XML_createDirs,&is_makedirs);
            statx = recursePath(pathstr, (bool)is_makedirs);
            printf("HDF5 Recurse path: statis = %d\n",statx);

            if (statx==0)
                setIntegerParam(NDFilePathExists, 1);
            else
                setIntegerParam(NDFilePathExists, 0);


    } 
   
   if (function == NDFileHDF5XML_templatepath)
   {
      loadTemplateFile();
   }
   if (function == NDFileHDF5XML_templatefile)
   {
      loadTemplateFile();
   }
   else
   {
      /* If this parameter belongs to a base class call its method */
      status = NDPluginFile::writeOctet(pasynUser, value, nChars, nActual);
   }

   /* Do callbacks so higher layers see any changes */
   status = (asynStatus) callParamCallbacks(addr, addr);

   if (status)
      epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
            "%s:%s: status=%d, function=%d, value=%s", driverName, functionName,
            status, function, value);
   else
      asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
            "%s:%s: function=%d, value=%s\n", driverName, functionName,
            function, value);
   *nActual = nChars;
   return status;
}

/* Configuration routine.  Called directly, or from the iocsh function in drvNDFileEpics */

extern "C" int drvNDFileHDF5XMLConfigure(const char *portName, int queueSize,
      int blockingCallbacks, const char *NDArrayPort, int NDArrayAddr,
      int priority, int stackSize)
{
   new NDFileHDF5XML(portName, queueSize, blockingCallbacks, NDArrayPort,
         NDArrayAddr, priority, stackSize);
   return (asynSuccess);
}
//maxbuffers is 10 so we can declare a dummy array...

/* The constructor for this class */
NDFileHDF5XML::NDFileHDF5XML(const char *portName, int queueSize,
      int blockingCallbacks, const char *NDArrayPort, int NDArrayAddr,
      int priority, int stackSize) :
      NDPluginFile(portName, queueSize, blockingCallbacks, NDArrayPort,
            NDArrayAddr, 1, NDFileHDF5XML::num_params, 10, -1,
            asynGenericPointerMask, asynGenericPointerMask, ASYN_CANBLOCK, 1,
            priority, stackSize), 
            is_file_open(false),
            lf("HDF5XMLPlug_log.txt")
{
   int i;
   static const char *functionName = "NDFileHDF5XML";

int dims[3];
   dims[0]=10;
   dims[1]=10;
char mesg[256];

    //dummyArray=pNDArrayPool->alloc(2, dims, NDInt16, 0, NULL);

/*
   paramStrings[0] = new param_type_str(&NDFileHDF5XML_threshold,
         asynParamInt32, "NDFileHDF5XML_threshold");
   paramStrings[1] = new param_type_str(&NDFileHDF5XML_is_recapture,
         asynParamInt32, "NDFileHDF5XML_is_recapture");
   paramStrings[2] = new param_type_str(&NDFileHDF5XML_templatefile,
         asynParamOctet, "TEMPLATE_FILE_NAME");
   paramStrings[3] = new param_type_str(&NDFileHDF5XML_templatepath,
         asynParamOctet, "TEMPLATE_FILE_PATH");
   paramStrings[4] = new param_type_str(&NDFileHDF5XML_TemplateValid,
         asynParamInt32, "TEMPLATE_FILE_VALID");
*/


    lf.enablePrintf(true);

    lf.log("NDFileHDF5XML constructing");
    sprintf(mesg,"Compile date %s, Time %s",__DATE__,__TIME__);
    lf.log(mesg);
    
    
    createParam("NDFileHDF5XML_threshold",asynParamInt32,&NDFileHDF5XML_threshold);    
       createParam("NDFileHDF5XML_is_recapture",asynParamInt32,&NDFileHDF5XML_is_recapture);    
   createParam("TEMPLATE_FILE_NAME",asynParamOctet,&NDFileHDF5XML_templatefile);    
   createParam("TEMPLATE_FILE_PATH",asynParamOctet,&NDFileHDF5XML_templatepath);    
   createParam("TEMPLATE_FILE_VALID",asynParamInt32,&NDFileHDF5XML_TemplateValid);    
   createParam("EZ_is_makedirs",asynParamInt32,&NDFileHDF5XML_createDirs);    
  



   this->supportsMultipleArrays = 1;
   this->pAttributeId = NULL;
   this->configFileMutex = epicsMutexCreate();
   if (!this->configFileMutex) {
       printf("%s::%s ERROR: epicsMutexCreate failure\n", driverName, functionName);
       return;
   }


   hdfSetup = new TiXMLhdf5SetupNew();
   hdf5Writer = new TiXMLhdf5WriteData();
   hdf5Closer = new TiXMLhdf5Shutdown();

   configDoc = new TiXmlDocument();

   hdf_interface = new epics2hdf(this->pasynUserSelf);

   hdfSetup->setEpicsInterface(hdf_interface);
   hdf5Writer->setEpicsInterface(hdf_interface);
   hdf5Closer->setEpicsInterface(hdf_interface);

   num_bad_fpgaheads = 0;
   last_filenumber = 0;

   //hits capture START on fileclose
   setIntegerParam(NDFileHDF5XML_is_recapture, 0);
   capture_trig = 0;
   filenum_kludge = 0;

}

NDFileHDF5XML::~NDFileHDF5XML()
{
   delete(hdfSetup);
   delete(hdf5Writer);
   delete(hdf5Closer);
   delete(hdf_interface);
   delete(configDoc);
}

/**
 * Place a lock on the configuration file for the HDF.  If this is not
 * not locked and loadTempleteFile runs while the file is being written
 * the configDoc is changed in the middle of the write.  This should be
 * locked on file open and unlocked on file close.
 */
asynStatus NDFileHDF5XML::lockConfigFile()
{
   int status;
   status = epicsMutexLock(this->configFileMutex);
   if (status) return(asynError);
   else return(asynSuccess);
}

/**
 * Unlock the config file.  See lock() for more information.
 */
asynStatus NDFileHDF5XML::unlockConfigFile()
{
   epicsMutexUnlock(this->configFileMutex);
   return(asynSuccess);
}






// searches path part by part.
// path is of the form part/part/part/part
// return
// -64 if drive not exist.
// -1 if 1st path not exist, -2 if 2md part not exist.
// -128| part if cannot create dir. ex -128-2 if cant create 2nd part.

int NDFileHDF5XML::recursePath(char *pathstr, bool is_makedirs)
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



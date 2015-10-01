#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyxml.h"
#include "hdf5.h"


/**************************************************************************************************
 <?xml version="1.0" encoding="UTF-8"?>

 <root>

 <AttrName1 hdfObjType="Attr" source="CONST" datatype="H5T_STR_NULLTERM" when="OnFileOpen">This string gets assocatited with HDF Attr named AttrName1</AttrName1>
 <AttrName2 hdfObjType="Attr" source="CONST" datatype="H5T_STR_NULLTERM" when="OnFileOpen">We assyne 80 chars in str. Const means it is in XML file.</AttrName2>
 <UserName hdfObjType="Attr" source="CONST" datatype="H5T_STR_NULLTERM" when="OnFileOpen">Dr. Maddog</UserName>
 <Facility hdfObjType="Attr" source="CONST" datatype="H5T_STR_NULLTERM" when="OnFileOpen">PPS (Primative Photon Source)</Facility>
 <City hdfObjType="Attr" source="CONST" datatype="H5T_STR_NULLTERM" when="OnFileOpen">Paris, Illinois</City>

 <SomeLargeNumber hdfObjType="Attr" source="CONST" datatype="H5T_NATIVE_INT" when="OnFileOpen">478392837</SomeLargeNumber>

 <dsx hdfObjType="DataSet" source="EpicsPV"  when="OnFileOpen" source_name="ioc:pv:name"/>
 <dsx2 hdfObjType="DataSet" source="EpicsPV"  when="OnImage" source_name="ioc:pv:name"/>

 <dsx3 hdfObjType="DataSet" source="EpicsPV" when="OnFileOpen" source_name="ioc:pv:name" />
 <dsx4 hdfObjType="DataSet" source="EpicsPV"  when="OnImage" source_name="ioc:pv:name" />

 <BeamCurrent hdfObjType="Attr" source="EpicsPV"  when="OnFileOpen" source_name="ioc:pv:name" />

 <MyDataDir  hdfObjType="Group" >
 <MyDataDir2  hdfObjType="Group">

 <City hdfObjType="Attr" source="CONST" datatype="H5T_STR_NULLTERM" when="OnFileOpen">Paris, Illinois</City>

 <SomeLargeNumber hdfObjType="Attr" source="CONST" datatype="H5T_NATIVE_INT" when="OnFileOpen">478392837</SomeLargeNumber>
 <BeamCurrent2 hdfObjType="Attr" source="NDAttr" when="OnFileOpen" source_name="ioc:pv:name" />


 <TomoShots hdfObjType="DataSet" source="Images" when="OnImage" />


 </MyDataDir2>

 </MyDataDir>

 <MyDataDir5  hdfObjType="Group" >

 <TomoShots hdfObjType="DataSet" source="Images" when="OnFileOpen" />

 </MyDataDir5>

 <MyDataDir6  hdfObjType="Group" >

 <TomoNumbers hdfObjType="DataSet" source="NDAttr" when="OnImage" source_name="ioc:pv:name" />

 </MyDataDir6>


 <TomoNumbersShut hdfObjType="DataSet" source="NDAttr" when="OnFileClose" source_name="imgattrname" />
 <TomoNumbersPVShut hdfObjType="DataSet" source="EpicsPV" when="OnFileClose" source_name="imgattrname"  />
 <AttrNameShut hdfObjType="Attr" source="CONST" datatype="H5T_STR_NULLTERM" when="OnFileClose">Written on fioe close</AttrNameShut>

 <Times  hdfObjType="Group" >
 <StartTime hdfObjType="Attr" source="NDAttr"  when="OnFileOpen" source_name="sysclock" />
 <EndTime hdfObjType="Attr" source="NDAttr"  when="OnFileClose" source_name="sysclock" />

 <ImageTimes hdfObjType="DataSet" source="NDAttr" when="OnImage" source_name="sysclock" />
 </Times>


 </root>





 *************************************************************************************************/

#ifndef HDF5VISITOR_H
#define HDF5VISITOR_H


// Define this if you are building outside of EPICS.
//#define _NOEPICS

#ifdef _NOEPICS

#include <stdarg.h>


#define ASYN_TRACE_ERROR     0x0001
#define ASYN_TRACEIO_DEVICE  0x0002
#define ASYN_TRACEIO_FILTER  0x0004
#define ASYN_TRACEIO_DRIVER  0x0008
#define ASYN_TRACE_FLOW      0x0010

//set to one of the defines above or OR of several of them.
extern int printlevel;

struct asynUser 
{
  
    char          *errorMessage;
    int            errorMessageSize;
    /* timeout must be set by the user */
    double         timeout;  /*Timeout for I/O operations*/
    void          *userPvt; 
    void          *userData; 
    /*The following is for user to/from driver communication*/
    void          *drvUser;
    /*The following is normally set by driver*/
    int            reason;
    int     timestamp;
    /* The following are for additional information from method calls */
    int            auxStatus; /*For auxillary status*/
};


int asynPrint(asynUser* asyn, int level,char *format,...);



#else

#include "asynDriver.h"

#endif



enum
{
   HDF5_str_length = 1024
};

extern char *default_xml;
union pMemValue
{
   double a;
   int b;
   char c;
   short d;
   float f;
   unsigned char g;
   unsigned short h;
   unsigned int i;

};

class hdfStack
{
public:
   hdfStack(int length);
   ~hdfStack();

   int push(hid_t a);

   hid_t pop(void);

private:
   hid_t *stacklist;
   int stackPtr;
   int len;

};

struct pArraySpecs
{
   void *data;
   int rank;
   int dims[32];
   int nd_datatype;
   int hdf_datatype;
   int hdf_rank;
   hsize_t hdf_dims[32];
   hsize_t hdf_maxdims[32];
   hsize_t hdf_chunkdims[32];
};

class hdfEpicsInterface
{
public:
   //provide empty pArraySpecs, then function fills it in.
   virtual pArraySpecs* getNDArraySpecs(pArraySpecs *a)=0;
   // you must provide memory for all pointers.
   // provide pvname or attr name. returns datatype and pvvalue or attr val
   // datatyep < 0 for error.

   virtual void* epicsCaGet(const char* pvname, void* pvvalue,
         hid_t *datatype)=0;
   virtual void* getNDAttr(const char* attr_name, void* attr_val,
         hid_t *datatype)=0;
   asynUser* pasynUser;

};

class defaultEpicsInterface : public hdfEpicsInterface
{
public:
   virtual pArraySpecs* getNDArraySpecs(pArraySpecs *a);
   virtual void* epicsCaGet(const char* pvname, void* pvvalue, hid_t *datatype);
   virtual void* getNDAttr(const char* attr_name, void* attr_val,
         hid_t *datatype);

   virtual void* genTestImage(int itype, int sizex, int sizey);

   void* constTextToDataType(const char *inText, const int dataType,
         pMemValue *pValue);

   unsigned short idata[1500000];

};

enum hdf_data_access
{
   hdf_none, hdf_read, hdf_append, hdf_overwrite
};

//called on hdf file open, when new file.
class TiXMLhdf5SetupNew : public TiXmlVisitor
{
public:

   TiXMLhdf5SetupNew();

   virtual void setFileName(char* fname);
   virtual char* getFileName(void);

   virtual hid_t getHDFFilePtr(void);

   virtual void setEpicsInterface(hdfEpicsInterface* e);

   virtual void setLeaveOpen(bool is_lv_opn);

   virtual bool VisitEnter(const TiXmlDocument &);
   virtual bool VisitExit(const TiXmlDocument &);

   virtual bool VisitEnter(const TiXmlElement &, const TiXmlAttribute *);
   virtual bool VisitExit(const TiXmlElement &);

   virtual bool Visit(const TiXmlDeclaration &);

   virtual bool Visit(const TiXmlText &);

   virtual bool Visit(const TiXmlComment &);

   virtual bool Visit(const TiXmlUnknown &);

   //virtual bool writeData(void);
   virtual bool writeData(void* the_data2_write);
   void writeArrayDataFromString(hsize_t dim0, hsize_t dim1, char *textValue, int & nodeDataTypeInt, hid_t dataset);

protected:

   hdfEpicsInterface* my_epics;

   int typeStringToVal(const char * typeStr);

   void* constTextToDataType(char *inText, int dataType, pMemValue *pValue);

//    void getNdArraySpecs(pArraySpecs *a);

   // have a stack of open hdf obhects we keep track up.
   hdfStack stack;

   hid_t file_ptr;

   char file_name[256];

   bool is_leave_open;

   defaultEpicsInterface def_epics;

   hdf_data_access hdf_access;

//need to be able store a 1024 string.
   char scratch_str[2048];

   bool write_xml_text;

   const char* className;
};

class TiXMLhdf5WriteData : public TiXMLhdf5SetupNew
{

public:

   TiXMLhdf5WriteData();

   virtual bool VisitEnter(const TiXmlDocument &);
   //virtual bool     VisitExit (const TiXmlDocument &);

   virtual bool VisitEnter(const TiXmlElement &, const TiXmlAttribute *);
   //virtual bool     VisitExit (const TiXmlElement &);

   //virtual bool     Visit (const TiXmlDeclaration &);

   //virtual bool     Visit (const TiXmlText &);

//    virtual bool     Visit (const TiXmlComment &);

   //virtual bool     Visit (const TiXmlUnknown &);

   virtual void setHDFFilePtr(hid_t fp);

   virtual void setHDFAccess(hdf_data_access acs);

protected:

   bool doImageTypesMatch(const char *typeName, int typeNumber);
   bool is_already_open;

};

class TiXMLhdf5Shutdown : public TiXMLhdf5WriteData
{

public:

   TiXMLhdf5Shutdown();

//    virtual bool     VisitEnter (const TiXmlDocument &);
   //virtual bool     VisitExit (const TiXmlDocument &);

   virtual bool VisitEnter(const TiXmlElement &, const TiXmlAttribute *);
   //virtual bool     VisitExit (const TiXmlElement &);

   //virtual bool     Visit (const TiXmlDeclaration &);

   //virtual bool     Visit (const TiXmlText &);

//    virtual bool     Visit (const TiXmlComment &);

   //virtual bool     Visit (const TiXmlUnknown &);

protected:

};

#endif

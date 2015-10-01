#include "hdf5Visitor.h"
#include <time.h>
#include <string>

/**************************************************************************************************/
char *default_xml =
      "\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "\n<root>\n"
            "<MyDataDir  hdfObjType=\"Group\" >\n "
            "<TomoShots hdfObjType=\"DataSet\" source=\"Images\" when=\"OnImage\" />\n\n "
            "</MyDataDir>\n\n\n  "
            "\n</root>\n\n";

/*************************************************************************************************/






#ifdef _NOEPICS
int printlevel=0;

int asynPrint(asynUser* asyn, int level,char *format,...)
{
 va_list args;
    if (level&printlevel > 0)
    {
         
        va_start (args, format);
        vprintf (format, args);
        va_end (args);
    

    }
return(0);
}

#endif









/******************************************************************************************************
 *
 *****************************************************************************************************/

hdfStack::hdfStack(int length)
{
   stacklist = new hid_t[length];
   stackPtr = 0;
   len = length;
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

hdfStack::~hdfStack()
{
   delete (stacklist);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

hid_t hdfStack::pop(void)
{
   if (stackPtr > 0)
   {
      stackPtr--;
      return (stacklist[stackPtr]);
   }
   return (-1);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

int hdfStack::push(hid_t a)
{
   if (stackPtr < len)
   {
      stacklist[stackPtr] = a;
      stackPtr++;
      return (0);
   }
   return (-1);
}

/******************************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************************************/

/******************************************************************************************************
 *
 *****************************************************************************************************/
pArraySpecs*
defaultEpicsInterface::getNDArraySpecs(pArraySpecs *a)
{
   a->rank = 2;
   a->dims[0] = 1024;
   a->dims[1] = 1024;

   a->nd_datatype = 0;
   a->hdf_datatype = H5T_NATIVE_USHORT;

   a->hdf_rank = 3;
   //save as z,y x, where z is image number.
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

   a->data = genTestImage(0, 1024, 1024);

   return (a);
}
/******************************************************************************************************
 *
 *****************************************************************************************************/
void*
defaultEpicsInterface::epicsCaGet(const char* pvname, void* pvvalue,
      hid_t *datatype)
{
   char zz[128];

   sprintf(zz, "%f", 100.0 * rand());

   *datatype = H5T_NATIVE_FLOAT;

   constTextToDataType(zz, *datatype, (pMemValue*) pvvalue);

   return (pvvalue);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/
void*
defaultEpicsInterface::getNDAttr(const char* pvname, void* pvvalue,
      hid_t *datatype)
{

   char zz[256];

   if (std::string(pvname) == "sysclock")
   {
      // sprintf(zz,"%f",clock()  );
      *datatype = H5T_NATIVE_DOUBLE;
      double *x;
      x = (double*) pvvalue;
      *x = clock() / CLOCKS_PER_SEC;

   }
   else
   {
      sprintf(zz, "%f", 100.0 * rand());
      *datatype = H5T_NATIVE_INT;
      constTextToDataType(zz, *datatype, (pMemValue*) pvvalue);
   }

   return (pvvalue);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

void *
defaultEpicsInterface::genTestImage(int itype, int sizex, int sizey)
{

   for (int k = 0; k < sizex * sizey; k++)
   {
      idata[k] = (unsigned short) (4000 + 100.0 * rand());
   }

   return ((void*) idata);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

void*
defaultEpicsInterface::constTextToDataType(const char *inText,
      const int dataType, pMemValue *pValue)
{
   double dval;
   int ival;
   int ii;

   if (dataType == H5T_NATIVE_CHAR)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->c = (char) ival;
   }
   if (dataType == H5T_NATIVE_UCHAR)
   {
      sscanf((const char *) inText, "%d ", &ival);
      pValue->g = (unsigned char) ival;
   }
   if (dataType == H5T_NATIVE_SHORT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->d = (short) ival;
   }
   if (dataType == H5T_NATIVE_USHORT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->h = (unsigned short) ival;
   }
   if (dataType == H5T_NATIVE_INT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->b = ival;
   }
   if (dataType == H5T_NATIVE_UINT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->i = (unsigned int) ival;
   }
   if (dataType == H5T_NATIVE_FLOAT)
   {
      sscanf((const char *) inText, "%lf", &dval);
      pValue->f = (float) dval;
   }
   if (dataType == H5T_NATIVE_DOUBLE)
   {
      // Note here that the format code %lf may not work on all systems
      //it does seem to be the most common though.
      sscanf((const char *) inText, "%lf", &dval);
      pValue->a = dval;
   }

   return (pValue);
}

/******************************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************************************/

TiXMLhdf5SetupNew::TiXMLhdf5SetupNew() :
      stack(1024), def_epics()
{

   file_ptr = -1;
   is_leave_open = false;
   my_epics = &def_epics;
   hdf_access = hdf_append;
   write_xml_text = false;
   className = "TiXMLhdf5SetupNew";

}

/******************************************************************************************************
 *
 *****************************************************************************************************/
void TiXMLhdf5SetupNew::setFileName(char *fname)
{
   strcpy(file_name, fname);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/
char*
TiXMLhdf5SetupNew::getFileName(void)
{
   return (file_name);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/
hid_t TiXMLhdf5SetupNew::getHDFFilePtr(void)
{
   return (file_ptr);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

void TiXMLhdf5SetupNew::setLeaveOpen(bool is_lv_opn)
{

   is_leave_open = is_lv_opn;
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5SetupNew::VisitEnter(const TiXmlDocument &a)
{
   bool retVal = false;
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
         "%s::VisitEnter(doc) * start****\n", className);
   hid_t fp = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
         "%s::VisitEnter(doc) * start****\n", className);
   if (fp > 0)
   {
      stack.push(fp);
      file_ptr = fp;
      retVal = true;
   }
   else
   {
      asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
            "TiXMLhdf5SetupNew::VisitEnter Trouble opening %s\n", file_name);

      retVal = false;
   }
   return (true);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/
bool TiXMLhdf5SetupNew::VisitExit(const TiXmlDocument &a)
{
   hid_t fp = stack.pop();

   if (!is_leave_open)
   {
      file_ptr = -1;
      asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
            "%s::VisitExit(Doc)* start****\n", className);
      H5Fclose(fp);
      asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
            "%s::VisitExit(Doc)* end*****\n", className);
   }
   return (true);

}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5SetupNew::VisitEnter(const TiXmlElement& xml_element,
      const TiXmlAttribute* attr)
{
   const char *nodeValue;
   const char *nodeHdfObjType;
   const char *nodeDatatype;
   const char *nodeWhen;
   const char *nodeDataRank;

   const char *nodeDims;
   const char *nodeSource;
   const char *nodeSourceName;
   const char *nodeSourceCond;

   const TiXmlElement *xml_parent;
   const char *parentHdfType;

   hid_t hdfparent, hdfa, hdfb, hdfc;
   hid_t hdfnode;

   nodeValue = xml_element.Value();

   nodeHdfObjType = xml_element.Attribute("hdfObjType");
   nodeDatatype = xml_element.Attribute("datatype");
   nodeWhen = xml_element.Attribute("when");
   nodeDataRank = xml_element.Attribute("dataRank");
   nodeDims = xml_element.Attribute("Dims");
   nodeSource = xml_element.Attribute("source");
   nodeSourceName = xml_element.Attribute("source_name");
   nodeSourceCond = xml_element.Attribute("source_condition");

   xml_parent = xml_element.Parent()->ToElement();
   if (xml_parent)
      parentHdfType = xml_parent->Attribute("hdfObjType");
   else
      parentHdfType = NULL;

   //if parent is dataset we pop 4 things...
   if (parentHdfType && std::string(parentHdfType) == "DataSet")
   {

      hdfa = stack.pop();
      hdfb = stack.pop();
      hdfc = stack.pop();
      hdfparent = stack.pop();

      stack.push(hdfparent);
      stack.push(hdfc);
      stack.push(hdfb);
      stack.push(hdfa);

   }
   else
   {
      //if file or group is parent we pop 1 thing..
      hdfparent = stack.pop();
      stack.push(hdfparent);

   }

   write_xml_text = false;

   if (nodeWhen && std::string(nodeWhen) == "OnFileOpen")
      write_xml_text = true;

   if (nodeHdfObjType)
   {
      if (std::string(nodeHdfObjType) == "Group")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
               "Enter %s %s\n", nodeHdfObjType, nodeValue);
         hdfnode = H5Gcreate(hdfparent, nodeValue, H5P_DEFAULT, H5P_DEFAULT,
               H5P_DEFAULT);
         stack.push(hdfnode);
      }

      if (std::string(nodeHdfObjType) == "DataSet")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                          "Enter %s %s\n", nodeHdfObjType, nodeValue);

         if (strcmp(nodeValue, "data") == 0)
         {
            //trap here
            int aa;
            aa = 0;
         }

         hid_t dataset, datatype, dataspace, plist;
         //hsize_t dimsf[2];
         pArraySpecs img_specs;
         int fillval = 0;

         if (nodeSource == NULL)
         {
            asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                           "DataSet %s needs source\n", nodeValue);
            return (false);
         }
         if (std::string(nodeSource) == "Images")
         {
            my_epics->getNDArraySpecs(&img_specs);
            //should save as z,x,y, where z is image number. x,y are coord in image.
            dataspace = H5Screate_simple(img_specs.hdf_rank, img_specs.hdf_dims,
                  img_specs.hdf_maxdims);
            datatype = H5Tcopy(img_specs.hdf_datatype);

            //H5Tset_order(datatype, H5T_ORDER_LE);

            plist = H5Pcreate(H5P_DATASET_CREATE);
            H5Pset_chunk(plist, img_specs.hdf_rank, img_specs.hdf_chunkdims);
            //H5Pset_fill_value(plist, datatype, &fillval);

            dataset = H5Dcreate2(hdfparent, nodeValue, datatype, dataspace,
                  H5P_DEFAULT, plist, H5P_DEFAULT);

            stack.push(dataset);
            stack.push(datatype);
            stack.push(dataspace);
            stack.push(plist);

            if (std::string(nodeWhen) == "OnFileOpen")
               writeData(img_specs.data);
         }
         else if (std::string(nodeSource) == "EpicsPV"
               || std::string(nodeSource) == "NDAttr"
               || std::string(nodeSource) == "CONST")
         {
            int nodeDataTypeInt;

            if (nodeSourceName == NULL && std::string(nodeSource) != "CONST")
            {
               stack.push(-1);
               stack.push(-1);
               stack.push(-1);
               stack.push(-1);

               asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                              "Dataset %s missing source_name\n", nodeValue);
               return (false);
            }
            if (nodeSource && std::string(nodeSource) == "CONST")
               nodeDataTypeInt = typeStringToVal(nodeDatatype);
            else if (nodeSource && std::string(nodeSource) == "EpicsPV"
                  && nodeSourceName)
               my_epics->epicsCaGet(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);
            else if (nodeSource && std::string(nodeSource) == "NDAttr"
                  && nodeSourceName)
               my_epics->getNDAttr(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);
            else
            {
               asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                              "%s Attr has invalid source %s or source name %s\n",
                     nodeValue, nodeSource, nodeSourceName);
               stack.push(-1);
               stack.push(-1);
               stack.push(-1);
               stack.push(-1);
               return (false);
            }

            if (nodeDataTypeInt == H5T_STR_NULLTERM)
            {
               hsize_t strdims[2], strmaxdims[2], strchunk[2];
               strdims[1] = 1;
               strdims[0] = 0;
               strchunk[0] = 1;
               strchunk[1] = 1;
               strmaxdims[1] = 1;
               strmaxdims[0] = -1; //unlimited
               asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                     "%s::VisitEnter(el, attr)** start %s **\n", className, nodeValue);
               dataspace = H5Screate_simple(2, strdims, strmaxdims);
               //datatype = H5Tcopy(H5T_NATIVE_UCHAR);
               datatype = H5Tcopy(H5T_C_S1);
               //make string HDF5_str_length len
               H5Tset_size(datatype, HDF5_str_length);
               H5Tset_strpad(datatype, H5T_STR_NULLTERM);

               plist = H5Pcreate(H5P_DATASET_CREATE);
               H5Pset_chunk(plist, 2, strchunk);

               dataset = H5Dcreate2(hdfparent, nodeValue, datatype, dataspace,
                     H5P_DEFAULT, plist, H5P_DEFAULT);
               asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                              "%s::VisitEnter(el, attr)** end %s **\n", className, nodeValue);

            }
            else
            {
               hsize_t numdims[3], nummaxdims[3], chunkdims[3];
               numdims[0] = 0;
               nummaxdims[0] = -1; //unlimited
               int rank = 1;
               if (nodeDataRank == NULL){
                  rank = 1;
               }
               else
               {
                  pMemValue anytype_data;
                  sscanf((const char *) nodeDataRank, "%d", &rank);
                  if (rank > 3){
                     rank = 3;
                     asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                                     "WARNING: Node %s specifies a rank > 3"
                           ". Rank is limited to 3\n", nodeValue);
                  }
               }
               if (nodeDims == NULL){
                  numdims[0] = 0;
                  nummaxdims[0] = -1;
                  chunkdims[0] = 4096;
               }
               else {
                  char *tokValue = strtok((char *) nodeDims, ",");
                  for (int ii=0; ii < rank; ii++){
                     pMemValue anytype_data;
                     if (tokValue != NULL)
                     {
                        sscanf((const char *) tokValue, "%d", &numdims[ii]);
                        nummaxdims[ii] = numdims[ii];
                        chunkdims[ii] = numdims[ii];
                        tokValue = strtok(NULL, ",");
                     }
                  }
               }
               dataspace = H5Screate_simple(rank, numdims, nummaxdims);
               datatype = H5Tcopy(nodeDataTypeInt);
               plist = H5Pcreate(H5P_DATASET_CREATE);
               H5Pset_chunk(plist, rank, chunkdims);

               dataset = H5Dcreate2(hdfparent, nodeValue, datatype, dataspace,
                     H5P_DEFAULT, plist, H5P_DEFAULT);
            }

            stack.push(dataset);
            stack.push(datatype);
            stack.push(dataspace);
            stack.push(plist);

            if (std::string(nodeWhen) == "OnFileOpen")
            {
               if (std::string(nodeSource) == "NDAttr")
               {

                  if (nodeDatatype != NULL)
                     asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                                    "Dataset datatype ignored-derived from NDAttribute\n");

                  my_epics->getNDAttr(nodeSourceName, (void*) scratch_str,
                        &nodeDataTypeInt);

                  writeData((void*) scratch_str);

               }
               else if (std::string(nodeSource) == "EpicsPV")
               {
                  if (nodeDatatype != NULL)
                     asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                                    "Dataset datatype ignored-derived from PV\n");

                  my_epics->epicsCaGet(nodeSourceName, (void*) scratch_str,
                        &nodeDataTypeInt);

                  writeData((void*) scratch_str);

               }
               else if (std::string(nodeSource) == "CONST")
               {
               }
               else
               {
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                                 "DataSet %s unrecognized source\n", nodeValue);
               }

            }

         }

      }

      if (std::string(nodeHdfObjType) == "Attr")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "Enter %s %s\n", nodeHdfObjType, nodeValue);

         hid_t hdfattrdataspace, hdfdatatype, hdfstatus, hdfattr;
         int nodeDataTypeInt = -1;

         //grandparent=stack.pop();
         //stack.push(grandparent);

         if (nodeSource && std::string(nodeSource) == "CONST")
            nodeDataTypeInt = typeStringToVal(nodeDatatype);
         else if (nodeSource && std::string(nodeSource) == "EpicsPV"
               && nodeSourceName)
            my_epics->epicsCaGet(nodeSourceName, (void*) scratch_str,
                  &nodeDataTypeInt);
         else if (nodeSource && std::string(nodeSource) == "NDAttr"
               && nodeSourceName)
            my_epics->getNDAttr(nodeSourceName, (void*) scratch_str,
                  &nodeDataTypeInt);
         else
         {
            asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                           "%s Attr has invalid source %s or source name %s\n",
                  nodeValue, nodeSource, nodeSourceName);
            return (true);
         }

         if (nodeDataTypeInt < 0)
         {
            asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                           "%s Attr has invalid type %s\n", nodeValue, nodeDataTypeInt);
            return (true);
         }
         if (nodeDataTypeInt == H5T_STR_NULLTERM)
         {
            hdfattrdataspace = H5Screate(H5S_SCALAR);

            hdfdatatype = H5Tcopy(H5T_C_S1);
            //make string HDF5_str_length len
            hdfstatus = H5Tset_size(hdfdatatype, HDF5_str_length);
            hdfstatus = H5Tset_strpad(hdfdatatype, H5T_STR_NULLTERM);
            hdfattr = H5Acreate2(hdfparent, nodeValue, hdfdatatype,
                  hdfattrdataspace, H5P_DEFAULT, H5P_DEFAULT);
         }
         else
         {
            hsize_t hdfattrdims;
            hdfattrdims = 1;

            hdfattrdataspace = H5Screate_simple(1, &hdfattrdims, NULL);
            hdfdatatype = H5Tcopy(nodeDataTypeInt);

            hdfattr = H5Acreate2(hdfparent, nodeValue, hdfdatatype,
                  hdfattrdataspace, H5P_DEFAULT, H5P_DEFAULT);

         }

//hdfstatus        = H5Awrite(hdfattr, H5T_NATIVE_INT32, (void*)&hdfattrval);
         if (nodeWhen && std::string(nodeWhen) == "OnFileOpen")
         {

            if (nodeSource == NULL || std::string(nodeSource) == "CONST")
            {
               // we write the const in the xml text

               //if (nodeDataTypeInt==H5T_STR_NULLTERM)
               //    H5Awrite(hdfattr, hdfdatatype, xml_text.Value());
               //else
               //    H5Awrite(hdfattr, hdfdatatype, constTextToDataType((char*)xml_text.Value(),nodeDataTypeInt,&anytype_data) );
            } //Const
            else if (std::string(nodeSource) == "NDAttr")
            {

               my_epics->getNDAttr(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               H5Awrite(hdfattr, hdfdatatype, (void*) scratch_str);

            } //NDAttr
            else if (std::string(nodeSource) == "EpicsPV")
            {

               my_epics->epicsCaGet(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               H5Awrite(hdfattr, hdfdatatype, (void*) scratch_str);

            } //epicsPV

         } //if nodeWhen

         stack.push(hdfattr);
         stack.push(hdfdatatype);
         stack.push(hdfattrdataspace);

         //H5Aclose (hdfattr);//attr
         //H5Tclose(hdfdatatype);
         //H5Sclose(hdfattrdataspace);

      } //Attr

   }
   return (true);
}
/******************************************************************************************************
 *
 *****************************************************************************************************/
bool TiXMLhdf5SetupNew::VisitExit(const TiXmlElement &xml_element)
{
   const char *nodeValue;
   const char *nodeHdfObjType;
   const char *nodeDatatype;
   const char *nodeWhen;
   const char *nodeDataRank;
   const char *nodeDims;
   const char *nodeSource;

   hid_t hdfparent;

   hid_t hdfnode;

   nodeValue = xml_element.Value();

   nodeHdfObjType = xml_element.Attribute("hdfObjType");
   nodeDatatype = xml_element.Attribute("datatype");
   nodeWhen = xml_element.Attribute("when");
   nodeDataRank = xml_element.Attribute("dataRank");
   nodeDims = xml_element.Attribute("Dims");
   nodeSource = xml_element.Attribute("source");

   if (nodeHdfObjType)
   {
      if (std::string(nodeHdfObjType) == "Group")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "Exit %s %s\n", nodeHdfObjType, nodeValue);
         hdfparent = stack.pop();
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitExit(el)* start** Group* %s **\n", className, nodeValue);
         H5Gclose(hdfparent);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitExit(el)* end** Group* %s **\n", className, nodeValue);
      }

      if (std::string(nodeHdfObjType) == "Attr")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "Exit %s %s\n", nodeHdfObjType, nodeValue);

         hid_t hdfattr, hdfdatatype, hdfattrdataspace;

         hdfattrdataspace = stack.pop();
         hdfdatatype = stack.pop();
         hdfattr = stack.pop();

         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitExit(el)* start** Attr* %s **\n", className, nodeValue);
         H5Aclose(hdfattr); //attr
         H5Tclose(hdfdatatype);
         H5Sclose(hdfattrdataspace);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitExit(el)* end** Attr* %s **\n", className, nodeValue);
      }

      if (std::string(nodeHdfObjType) == "DataSet")
      {
         hid_t dataset, datatype, dataspace, plist;
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "Exit %s %s\n", nodeHdfObjType, nodeValue);

         plist = stack.pop();
         dataspace = stack.pop();
         datatype = stack.pop();
         dataset = stack.pop();
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitExit(el)* start** Dataset* %s **\n", className, nodeValue);

         H5Dclose(dataset);
         H5Tclose(datatype);
         H5Sclose(dataspace);
         H5Pclose(plist);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitExit(el)* end ** Dataset* %s **\n", className, nodeValue);

      }

   }
   return (true);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5SetupNew::Visit(const TiXmlDeclaration &a)
{
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "Visit Declaration %s\n", a.Value());
   return (true);

}

void TiXMLhdf5SetupNew::writeArrayDataFromString(hsize_t dim0, hsize_t dim1, char *textValue,
      int & nodeDataTypeInt, hid_t dataset)
{
    asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,"%s::writeArray 1 dims: %d, %d\n", className, dim0, dim1);
    pMemValue anytype_data;
    asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,"%s::writeArray 2 dims: %d, %d\n", className, dim0, dim1);
   char **outerArray = new char*[dim1];
    asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,"%s::writeArray 3 dims: %d, %d\n", className, dim0, dim1);
   for (unsigned int kx = 0; kx < dim1; kx++)
   {
      outerArray[kx] = new char[dim0];
   }
   outerArray[0] = strtok(textValue, ":");
   for (unsigned int outerIndex = 1; outerIndex < dim1; outerIndex++)
   {
      outerArray[outerIndex] = strtok(NULL, ":");
   }

   char ***array_string = new char**[dim1];
   for (unsigned int kx = 0; kx < dim1; kx++)
   {
      array_string[kx] = new char*[dim0];
   }
   for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
   {
      array_string[outerIndex][0] = strtok(outerArray[outerIndex], ",");
      for (unsigned int innerIndex = 1; innerIndex < dim0; innerIndex++)
      {
         array_string[outerIndex][innerIndex] = strtok(NULL, ",");
      }
   }
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "%s::WriteArraDataFromString* start*** **\n", className);

   if (nodeDataTypeInt == H5T_NATIVE_CHAR)
   {
      char **cArray = new char*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx ++)
      {
         cArray[kx] =  new char[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (unsigned int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%d",
                  &cArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, cArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete cArray[kx];
      }
      delete cArray;
   }
   else if (nodeDataTypeInt == H5T_NATIVE_UCHAR)
   {
      unsigned char **cArray = new unsigned char*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx ++)
      {
         cArray[kx] = new unsigned char[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (unsigned int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%d",
                  &cArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, cArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete cArray[kx];
      }
      delete cArray;
   }
   else if (nodeDataTypeInt == H5T_NATIVE_SHORT)
   {
      short **sArray = new short*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx ++)
      {
         sArray[kx] =  new short[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (unsigned int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%d",
                  &sArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, sArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete sArray[kx];
      }
      delete sArray;
   }
   else if (nodeDataTypeInt == H5T_NATIVE_USHORT)
   {
      unsigned short **sArray = new unsigned short*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx ++)
      {
         sArray[kx] =  new unsigned short[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%d",
                  &sArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, sArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete sArray[kx];
      }
      delete sArray;
   }
   else if (nodeDataTypeInt == H5T_NATIVE_INT)
   {
      int **iArray = new int*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx++)
      {
         iArray[kx] = new int[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (unsigned int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%d",
                  &iArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, iArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete iArray[kx];
      }
      delete iArray;
   }
   else if (nodeDataTypeInt == H5T_NATIVE_UINT)
   {
      unsigned int **iArray = new unsigned int*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx++)
      {
         iArray[kx] = new unsigned int[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (unsigned int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%d",
                  &iArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, iArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete iArray[kx];
      }
      delete iArray;
   }
   else if (nodeDataTypeInt == H5T_NATIVE_FLOAT)
   {
      float **fArray = new float*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx++)
      {
         fArray[kx] = new float[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (unsigned int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%lf",
                  &fArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, fArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete fArray[kx];
      }
      delete fArray;
   }
   else if (nodeDataTypeInt == H5T_NATIVE_DOUBLE)
   {
      double **dArray = new double*[dim0];
      for (unsigned int kx = 0; kx < dim0; kx++)
      {
         dArray[kx] = new double[dim1];
      }
      for (unsigned int outerIndex = 0; outerIndex < dim1; outerIndex++)
      {
         for (unsigned int innerIndex = 0; innerIndex < dim0; innerIndex++)
         {
            sscanf((const char *) array_string[outerIndex][innerIndex], "%lf",
                  &dArray[innerIndex][outerIndex]);
         }
      }
      H5Dwrite(dataset, nodeDataTypeInt, H5S_ALL, H5S_ALL, H5P_DEFAULT, dArray);
      for (unsigned int kx = 0; kx < dim0; kx++){
         delete dArray[kx];
      }
      delete dArray;
   }
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "%s::WriteArraDataFromString* start*** **\n", className);

   for(unsigned int kx=0; kx<dim1; kx++)
   {
      delete(array_string[kx]);
   }
   delete(array_string);
   delete(outerArray);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/
bool TiXMLhdf5SetupNew::Visit(const TiXmlText &xml_text)
{
   int nodeDataTypeInt = 0;
   hid_t attr, datatype, dataset, dataspace, plist;

   herr_t hdfstatus;
   hid_t hdfdatatype;
   hid_t hdfattr;
   hid_t hdfattrdataspace;
   hid_t grandparent;

   pMemValue anytype_data;

   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "Visit Text %s\n", xml_text.Value());

   //xml_text.Parent();
   const TiXmlElement *parent = (xml_text.Parent())->ToElement();

   const char* parentHdfObjType = parent->Attribute("hdfObjType");
   const char* nodeValue = parent->Value();
   const char* nodeDatatype = parent->Attribute("datatype");
   const char* nodeSource = parent->Attribute("source");

   const char* nodeWhen = parent->Attribute("when");

   if (write_xml_text == false)
      return (true);

   if (nodeDatatype)
      nodeDataTypeInt = typeStringToVal(nodeDatatype);

   if (parentHdfObjType)
   {
      if (std::string(parentHdfObjType) == "Attr")
      {

         hdfattrdataspace = stack.pop();
         hdfdatatype = stack.pop();
         hdfattr = stack.pop();

         stack.push(hdfattr);
         stack.push(hdfdatatype);
         stack.push(hdfattrdataspace);

         if (nodeSource && std::string(nodeSource) == "CONST")
         {
            nodeDataTypeInt = typeStringToVal(nodeDatatype);

            if (nodeDataTypeInt < 0)
            {
               asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                              "%s Attr has invalid type %s\n", parent->Value(),
                     nodeSource);
               return (true);
            }

            asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                           "%s::Visit(text)* start*** Attr CONST %s **\n", className, nodeValue);
            if (nodeDataTypeInt == H5T_STR_NULLTERM)
            {
                memset(scratch_str,0,1024);
                strcpy(scratch_str, xml_text.Value());
               H5Awrite(hdfattr, hdfdatatype, scratch_str);
           }
            else
               H5Awrite(
                     hdfattr,
                     hdfdatatype,
                     constTextToDataType((char*) xml_text.Value(),
                           nodeDataTypeInt, &anytype_data));
            asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                           "%s::Visit(text)* start*** Attr CONST %s **\n", className, nodeValue);

         }
         else
         {
            asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                           "%s Attr has invalid source %d\n", parent->Value(),
                  nodeDataTypeInt);
            return (true);
         }

      } //Attr
      else if (std::string(parentHdfObjType) == "DataSet")
      {
         char cstrx[HDF5_str_length];

         plist = stack.pop();
         dataspace = stack.pop();
         datatype = stack.pop();
         dataset = stack.pop();

         stack.push(dataset);
         stack.push(datatype);
         stack.push(dataspace);
         stack.push(plist);

         if (nodeSource && std::string(nodeSource) == "CONST")
         {
            nodeDataTypeInt = typeStringToVal(nodeDatatype);

            if (nodeDataTypeInt < 0)
            {
               asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                     "%s Dataset has invalid type %s\n", parent->Value(),
                     nodeSource);
               return (true);
            }
            else if (nodeDataTypeInt == H5T_STR_NULLTERM){
               asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                              "%s::Visit(Text)*************************\n", className);
                memset(scratch_str,0,1024);
                strcpy(scratch_str, xml_text.Value());

               writeData((char *)scratch_str);
               asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                              "%s::Visit(Text)*************************\n", className);

            }
            else
            {
               hsize_t dims[256], maxdims[256];
               int rank = H5Sget_simple_extent_dims(dataspace, dims, maxdims);
               if (rank == 1)
               {
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                                 "Rank =1\n");
                  char *tokValue;
                  tokValue = strtok((char *) xml_text.Value(), ",");
                  while (tokValue != NULL)
                  {
                     writeData(constTextToDataType(tokValue,
                           nodeDataTypeInt, &anytype_data));
                     tokValue = strtok(NULL, ",");

                  }
               }
               else if (rank == 2)
               {
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "Rank =2\n");
                  char *textValue = (char *)(xml_text.Value());
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                                 "%s::dims: %d, %d\n", className, dims[0], dims[1]);
                   writeArrayDataFromString(dims[0], dims[1], textValue, nodeDataTypeInt, dataset);
               }
            }
         }
         else
         {
            asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                           "%s DataSet has invalid source %d\n", parent->Value(),
                  nodeDataTypeInt);
            return (true);
         }

         //    asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
         //"TiXMLhdf5SetupNew::Visit (const TiXmlText &xml_text) : DataSet, Ignoring Text\n");

      } //dataset
      else
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
               "TiXMLhdf5SetupNew::Visit (const TiXmlText &xml_text) : invalid hdf obj type- need DataSet or Attr\n");
      }
   } ///if parentHdfObjType

   return (true);

}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5SetupNew::Visit(const TiXmlComment &a)
{
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "Visit Comment %s\n", a.Value());
   return (true);

}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5SetupNew::Visit(const TiXmlUnknown &a)
{
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "Visit Unknown %s\n", a.Value());
   return (true);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

int TiXMLhdf5SetupNew::typeStringToVal(const char * typeStr)
{

   if (strcmp(typeStr, "H5T_NATIVE_CHAR") == 0)
   {
      return H5T_NATIVE_CHAR;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_SCHAR") == 0)
   {
      return H5T_NATIVE_SCHAR;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_UCHAR") == 0)
   {
      return H5T_NATIVE_UCHAR;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_SHORT") == 0)
   {
      return H5T_NATIVE_SHORT;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_USHORT") == 0)
   {
      return H5T_NATIVE_USHORT;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_INT") == 0)
   {
      return H5T_NATIVE_INT;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_UINT") == 0)
   {
      return H5T_NATIVE_UINT;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_FLOAT") == 0)
   {
      return H5T_NATIVE_FLOAT;
   }
   else if (strcmp(typeStr, "H5T_NATIVE_DOUBLE") == 0)
   {
      return H5T_NATIVE_DOUBLE;
   }
   else if (strcmp(typeStr, "H5T_STR_NULLTERM") == 0)
   {
      return H5T_STR_NULLTERM;
   }
   else
      return -1;
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

void*
TiXMLhdf5SetupNew::constTextToDataType(char *inText, int dataType,
      pMemValue *pValue)
{
   double dval;
   int ival;
   int ii;

   if (dataType == H5T_NATIVE_CHAR)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->c = (char) ival;
   }
   else if (dataType == H5T_NATIVE_UCHAR)
   {
      sscanf((const char *) inText, "%d ", &ival);
      pValue->g = (unsigned char) ival;
   }
   else if (dataType == H5T_NATIVE_SHORT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->d = (short) ival;
   }
   else if (dataType == H5T_NATIVE_USHORT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->h = (unsigned short) ival;
   }
   else if (dataType == H5T_NATIVE_INT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->b = ival;
   }
   else if (dataType == H5T_NATIVE_UINT)
   {
      sscanf((const char *) inText, "%d", &ival);
      pValue->i = (unsigned int) ival;
   }
   else if (dataType == H5T_NATIVE_FLOAT)
   {
      sscanf((const char *) inText, "%lf", &dval);
      pValue->f = (float) dval;
   }
   else if (dataType == H5T_NATIVE_DOUBLE)
   {
      // Note here that the format code %lf may not work on all systems
      //it does seem to be the most common though.
      sscanf((const char *) inText, "%lf", &dval);
      pValue->a = dval;
   }

   return (pValue);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

void TiXMLhdf5SetupNew::setEpicsInterface(hdfEpicsInterface* e)
{
   my_epics = e;
}

/******************************************************************************************************
 *
 *****************************************************************************************************/
#if 0
bool TiXMLhdf5SetupNew::writeData(void)
{

   hid_t dataset, datatype, dataspace, plist,selectspace;
   hsize_t dims[256],maxdims[256],offset[256],framesize[256],start[256],end[256];

   plist=stack.pop();
   dataspace=stack.pop();
   datatype=stack.pop();
   dataset=stack.pop();

   int rank = H5Sget_simple_extent_dims(dataspace, dims, maxdims);

   //n images in file...
   int n_images = dims[rank-1];
   int k;

   //size of 1 frame
   for (k=0;k<rank;k++)
   framesize[k] = dims[k];

   framesize[rank-1]=1;

   // for x y corred of image, start at x=1, y=1 etc. set which image later...
   for (k=0;k<rank;k++)
   offset[k] = 0;

   H5Sclose(dataspace);

   if (hdf_access==hdf_overwrite)
   {
      //seledt 1st image and write there...
      offset[rank-1]=0;
   }
   if (hdf_access==hdf_append)
   {
      // we will add a new image.
      offset[rank-1]=n_images;
      dims[rank-1]=n_images+1;
      H5Dset_extent(dataset, dims);
   }

   dataspace = H5Dget_space(dataset);
   rank = H5Sget_simple_extent_dims(dataspace, dims, maxdims);

   selectspace = H5Screate_simple(rank, framesize, NULL);

   //increase dataset size and write data..
   H5Sselect_hyperslab(dataspace, H5S_SELECT_SET,
         offset, NULL,
         framesize, NULL);

   H5Sget_select_bounds(dataspace, start, end );
   H5S_sel_type s_type =H5Sget_select_type(dataspace);

   hid_t tpx=H5Tget_native_type( datatype, H5T_DIR_ASCEND );
   size_t tps=H5Tget_size( datatype );

   void *imgdata=genTestImage(0,dims[0],dims[1]);
   H5Dwrite(dataset, datatype, selectspace, dataspace,
         H5P_DEFAULT, imgdata);

   delete(imgdata);
   H5Sclose(selectspace);

   stack.push(dataset);
   stack.push(datatype);
   stack.push(dataspace);
   stack.push(plist);

   return(true);
}
#endif

/******************************************************************************************************
 *
 *****************************************************************************************************/
bool TiXMLhdf5SetupNew::writeData(void *the_data2_write)
{

   hid_t dataset, datatype, dataspace, plist, selectspace;
   hsize_t dims[256], maxdims[256], offset[256], framesize[256], start[256],
         end[256];
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "%s::writeData start ************************\n", className);
   plist = stack.pop();
   dataspace = stack.pop();
   datatype = stack.pop();
   dataset = stack.pop();

   int rank = H5Sget_simple_extent_dims(dataspace, dims, maxdims);

   //rang should be 3. for z x yn for images. it is 2 for strigs.

   //n images in file...
   //saved as z,x,y. so dims[0] is n images
   int n_images = dims[0];
   int k;

   //size of 1 frame
   for (k = 0; k < rank; k++)
      framesize[k] = dims[k];

   framesize[0] = 1;

   // for x y corred of image, start at x=1, y=1 etc. set which image later...
   for (k = 0; k < rank; k++)
      offset[k] = 0;

   H5Sclose(dataspace);

   if (hdf_access == hdf_overwrite)
   {
      //seledt 1st image and write there...
      offset[0] = 0;
   }
   if (hdf_access == hdf_append)
   {
      // we will add a new image.
      offset[0] = n_images;
      dims[0] = n_images + 1;
      H5Dset_extent(dataset, dims);
   }

   dataspace = H5Dget_space(dataset);
   rank = H5Sget_simple_extent_dims(dataspace, dims, maxdims);

   selectspace = H5Screate_simple(rank, framesize, NULL);

   //increase dataset size and write data..
   H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, framesize,
         NULL);

   H5Sget_select_bounds(dataspace, start, end);
   H5S_sel_type s_type = H5Sget_select_type(dataspace);

   hid_t tpx = H5Tget_native_type(datatype, H5T_DIR_ASCEND);
   size_t tps = H5Tget_size(datatype);

   H5Dwrite(dataset, datatype, selectspace, dataspace, H5P_DEFAULT,
         the_data2_write);

   H5Sclose(selectspace);

   stack.push(dataset);
   stack.push(datatype);
   stack.push(dataspace);
   stack.push(plist);
   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "%s::writeData start ************************\n", className);

   return (true);
}

/******************************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************************************/

bool TiXMLhdf5WriteData::VisitEnter(const TiXmlDocument &)
{
   if (is_already_open == false)
   {
      asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                     "%s::VisitEnter(Document)* start*** \n", className);
      hid_t fp = H5Fopen(file_name, H5F_ACC_RDWR, H5P_DEFAULT);
      asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                     "%s::VisitEnter(Document)* start*** \n", className);

      stack.push(fp);
      file_ptr = fp;

   }
   else
      stack.push(file_ptr);

   return (true);
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5WriteData::doImageTypesMatch(const char *typeName, int typeNumber)
{
   bool result = false;
   if (((strcmp(typeName, "Normal") == 0) && (typeNumber == 0)) ||
         ((strcmp(typeName, "Background") == 0) && (typeNumber == 1)) ||
         ((strcmp(typeName, "FlatField") == 0) && (typeNumber == 2)) ||
         ((strcmp(typeName, "DblCorrelation") == 0) && (typeNumber == 3))) {
      result = true;
   }
   else
   {
      result = false;
   }

   return result;

}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5WriteData::VisitEnter(const TiXmlElement &xml_element,
const TiXmlAttribute *alist)
{
   const char *nodeValue;
   const char *nodeHdfObjType;
   const char *nodeDatatype;
   const char *nodeWhen;
   const char *nodeDataRank;
   const char *nodeDims;
   const char *nodeSource;
   const char *nodeSourceName;
   const char *nodeSourceCond;

   const TiXmlElement *xml_parent;
   const char *parentHdfType;
   bool is_write_cond_true = false;

   hid_t hdfparent, hdfa, hdfb, hdfc;
   hid_t hdfnode;

   nodeValue = xml_element.Value();

   nodeHdfObjType = xml_element.Attribute("hdfObjType");
   nodeDatatype = xml_element.Attribute("datatype");
   nodeWhen = xml_element.Attribute("when");
   nodeDataRank = xml_element.Attribute("dataRank");
   nodeDims = xml_element.Attribute("Dims");
   nodeSource = xml_element.Attribute("source");

   nodeSourceName = xml_element.Attribute("source_name");
   nodeSourceCond = xml_element.Attribute("source_condition");

   xml_parent = xml_element.Parent()->ToElement();
   if (xml_parent)
      parentHdfType = xml_parent->Attribute("hdfObjType");
   else
      parentHdfType = NULL;

   //if parent is dataset we pop 4 things...
   if (parentHdfType && std::string(parentHdfType) == "DataSet")
   {

      hdfa = stack.pop();
      hdfb = stack.pop();
      hdfc = stack.pop();
      hdfparent = stack.pop();

      stack.push(hdfparent);
      stack.push(hdfc);
      stack.push(hdfb);
      stack.push(hdfa);

   }
   else
   {
      //if file or group is parent we pop 1 thing..
      hdfparent = stack.pop();
      stack.push(hdfparent);

   }

   write_xml_text = false;

   if (nodeWhen != NULL && std::string(nodeWhen) == "OnImage")
      write_xml_text = true;

   if (nodeHdfObjType)
   {
      if (std::string(nodeHdfObjType) == "Group")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                           "Enter %s %s\n", nodeHdfObjType, nodeValue);
         //hdfnode = H5Gcreate(hdfparent,    nodeValue,   H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
         hdfnode = H5Gopen(hdfparent, nodeValue, H5P_DEFAULT);
         stack.push(hdfnode);
      }

      if (std::string(nodeHdfObjType) == "DataSet")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                           "Enter %s %s\n", nodeHdfObjType, nodeValue);
         hid_t dataset, datatype, dataspace, plist;
         //hsize_t dimsf[2];
         pArraySpecs img_specs;
         int fillval = 0;
         int nodeDataTypeInt;

         // if (std::string(nodeSource)=="Images")
         //{
         //getNdArraySpecs(&img_specs);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitEnter(el, attr)* start*** DataSet %s **\n", className, nodeValue);

         dataset = H5Dopen(hdfparent, nodeValue, H5P_DEFAULT);
         dataspace = H5Dget_space(dataset);

         datatype = H5Dget_type(dataset);
         plist = H5Dget_access_plist(dataset);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitEnter(el, attr)* end*** DataSet %s **\n", className, nodeValue);

         stack.push(dataset);
         stack.push(datatype);
         stack.push(dataspace);
         stack.push(plist);
         //}

         if (std::string(nodeWhen) == "OnNDAttrExist" && nodeSourceCond)
         {
            int cond_datatype = 0;
            my_epics->getNDAttr(nodeSourceCond, (void*) scratch_str,
                  &cond_datatype);

            if (cond_datatype > 0)
               is_write_cond_true = true;

         }
         else if (std::string(nodeWhen) == "OnImageType" && nodeSourceCond)
         {
            int cond_datatype = 0;
            int *image_type =(int*) scratch_str;
            my_epics->getNDAttr("FrameType", (void*)image_type,
                  &cond_datatype);

            if (cond_datatype > 0){

               if (doImageTypesMatch(nodeSourceCond, *image_type)){
                  is_write_cond_true = true;
               }
               else
               {
                  is_write_cond_true = false;
               }
            }

         }

         // we can write data ONImage- whenever an image comes form detector.
         // we can write data on NDAttrExist, whenever the image has some attribute,
         // example= write only images with "Dark" attribute. or write NDAttitubte from sourcename
         // if source_cond == White

         if (std::string(nodeWhen) == "OnImage"
               || (std::string(nodeWhen) == "OnNDAttrExist"
                     && is_write_cond_true)
            || (std::string(nodeWhen) == "OnImageType"
                  && is_write_cond_true))
         {
            if (std::string(nodeSource) == "NDAttr")
            {

               if (nodeDatatype != NULL)
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                                    "Dataset datatype ignored-derived from NDAttribute\n");

               my_epics->getNDAttr(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               writeData((void*) scratch_str);

            }
            else if (std::string(nodeSource) == "EpicsPV")
            {
               if (nodeDatatype != NULL)
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                                    "Dataset datatype ignored-derived from PV\n");

               my_epics->epicsCaGet(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               writeData((void*) scratch_str);

            }
            else if (std::string(nodeSource) == "Images")
            {
               writeData(my_epics->getNDArraySpecs(&img_specs)->data);
            }
            else if (std::string(nodeSource) == "CONST")
            {
            }
            else
            {
               asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                                 "DataSet %s unrecognized source\n", nodeValue);
            }

         }

      }

      if (std::string(nodeHdfObjType) == "Attr")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
               "Enter %s %s\n", nodeHdfObjType, nodeValue);

         herr_t hdfstatus;
         hid_t hdfdatatype;
         hid_t hdfattr;
         hid_t hdfattrdataspace;
         int nodeDataTypeInt;

         // nodeDataTypeInt = typeStringToVal(nodeDatatype);

         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitEnter(el, attr)* start*** Attr %s **\n", className, nodeValue);
         hdfattr = H5Aopen(hdfparent, nodeValue, H5P_DEFAULT);
         hdfattrdataspace = H5Aget_space(hdfattr);
         hdfdatatype = H5Aget_type(hdfattr);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitEnter(el, attr)* end*** Attr %s **\n", className, nodeValue);

         if (std::string(nodeWhen) == "OnImage")
         {
            write_xml_text = false;
            asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                  "Cannot write HDF Attr OnImage\n");
         }

         stack.push(hdfattr);
         stack.push(hdfdatatype);

         stack.push(hdfattrdataspace);

      }

   }
   return (true);

}

#if 0

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5WriteData::Visit (const TiXmlText &xml_text)
{
   int nodeDataTypeInt=0;
   hid_t attr, datatype, dataset,dataspace,plist;
   pMemValue anytype_data;

   asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                  "%s::Visit Text %s\n",className, xml_text.Value());

   xml_text.Parent();
   const TiXmlElement *parent = (xml_text.Parent())->ToElement();

   const char* parentHdfObjType = parent->Attribute("hdfObjType");

   const char* nodeDatatype = parent->Attribute("datatype");
   const char* nodeSource = parent->Attribute("source");
   const char* nodeWhen = parent->Attribute("when");

   if (nodeWhen && std::string(nodeWhen)!="OnImage")
   return(true);

   if (nodeDatatype)
   nodeDataTypeInt = typeStringToVal(nodeDatatype);

   if (parentHdfObjType)
   {
      if (std::string(parentHdfObjType)=="Attr")
      {
         int

         attr= stack.pop();
         datatype= stack.pop(); //datayupe
         dataset= stack.pop();//dataset

         stack.push(dataset);
         stack.push(datatype);
         stack.push(attr);

//hdfstatus        = H5Awrite(hdfattr, H5T_NATIVE_INT32, (void*)&hdfattrval);

         if (nodeSource==NULL || std::string(nodeSource)=="CONST")
         {
            if (nodeDataTypeInt==H5T_STR_NULLTERM)
            {
                    memset(scratch_str,0,1024);
                strcpy(scratch_str, xml_text.Value());
            H5Awrite(attr, datatype, scratch_str);
            }
            else
            H5Awrite(attr, nodeDataTypeInt, constTextToDataType((char*)xml_text.Value(),nodeDataTypeInt,&anytype_data) );
         } //Const
         else if (std::string(nodeSource)=="NDAttr")
         {
            if (nodeDataTypeInt==H5T_STR_NULLTERM)
            H5Awrite(attr, datatype, my_epics->getNDAttr(xml_text.Value(),scratch_str));
            else
            H5Awrite(
                  attr,
                  nodeDataTypeInt,
                  constTextToDataType(
                        my_epics->getNDAttr(xml_text.Value(),scratch_str),
                        nodeDataTypeInt,
                        &anytype_data) );
         } //NDAttr
         else if (std::string(nodeSource)=="EpicsPV")
         {
            if (nodeDataTypeInt==H5T_STR_NULLTERM)
            H5Awrite(attr, datatype, my_epics->epicsCaGet(xml_text.Value(),scratch_str));

            else
            H5Awrite(
                  attr,
                  nodeDataTypeInt,
                  constTextToDataType(
                        my_epics->epicsCaGet(xml_text.Value(),scratch_str),
                        nodeDataTypeInt,
                        &anytype_data) );
         } //epicsPV
      } //Attr
      else if (std::string(parentHdfObjType)=="DataSet")
      {
         char cstrx[HDF5_str_length];

         plist = stack.pop();
         dataspace = stack.pop();
         datatype = stack.pop();
         dataset = stack.pop();

         stack.push(dataset);
         stack.push(datatype);
         stack.push(dataspace);
         stack.push(plist);

         if (std::string(nodeSource)=="NDAttr")
         {
            if (nodeDataTypeInt==H5T_STR_NULLTERM)
            {
               strcpy(cstrx,my_epics->getNDAttr(xml_text.Value(),scratch_str));
               writeData((void*)cstrx);
            }
            else
            writeData(
                  constTextToDataType(
                        my_epics->getNDAttr(xml_text.Value(),scratch_str),
                        nodeDataTypeInt,
                        &anytype_data) );

         }
         else if (std::string(nodeSource)=="EpicsPV")
         {
            //writeData((void *)the_data2_write)
            if (nodeDataTypeInt==H5T_STR_NULLTERM)
            {
               strcpy(cstrx,my_epics->epicsCaGet(xml_text.Value(),scratch_str));
               writeData((void*)cstrx);
            }
            else
            writeData(
                  constTextToDataType(
                        my_epics->epicsCaGet(xml_text.Value(),scratch_str),
                        nodeDataTypeInt,
                        &anytype_data) );

         }
         else
         {
            asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                           "%s::Visit (const TiXmlText &xml_text) : DataSet, Unrecogniced source\n", className);
         }

      } //dataset
      else
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::Visit (const TiXmlText &xml_text) : invalid hdf obj type- need DataSet or Attr\n", className);
      }
   } ///if parentHdfObjType

   return(true);

}

#endif

/******************************************************************************************************
 *
 *****************************************************************************************************/

void TiXMLhdf5WriteData::setHDFFilePtr(hid_t fp)
{
   is_already_open = true;
   file_ptr = fp;
}

/******************************************************************************************************
 *
 *****************************************************************************************************/
void TiXMLhdf5WriteData::setHDFAccess(hdf_data_access acs)
{
   hdf_access = acs;
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

TiXMLhdf5WriteData::TiXMLhdf5WriteData() :
      TiXMLhdf5SetupNew()
{
   hdf_access = hdf_none;
   is_already_open = false;
   className = "TiXMLhdf5WriteData";
}

/******************************************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *****************************************************************************************************/

TiXMLhdf5Shutdown::TiXMLhdf5Shutdown() :
      TiXMLhdf5WriteData()
{
   setLeaveOpen(false);
   className = "TiXMLhdf5Shutdown";
}

/******************************************************************************************************
 *
 *****************************************************************************************************/

bool TiXMLhdf5Shutdown::VisitEnter(const TiXmlElement &xml_element,
      const TiXmlAttribute *attr)
{
   const char *nodeValue;
   const char *nodeHdfObjType;
   const char *nodeDatatype;
   const char *nodeWhen;
   const char *nodeDataRank;
   const char *nodeDims;
   const char *nodeSource;

   const char *nodeSourceName;
   const char *nodeSourceCond;

   const TiXmlElement *xml_parent;
   const char *parentHdfType;

   hid_t hdfparent, hdfa, hdfb, hdfc;
   hid_t hdfnode;

   nodeValue = xml_element.Value();

   nodeHdfObjType = xml_element.Attribute("hdfObjType");
   nodeDatatype = xml_element.Attribute("datatype");
   nodeWhen = xml_element.Attribute("when");
   nodeDataRank = xml_element.Attribute("dataRank");
   nodeDims = xml_element.Attribute("Dims");
   nodeSource = xml_element.Attribute("source");
   nodeSourceName = xml_element.Attribute("source_name");
   nodeSourceCond = xml_element.Attribute("source_condition");

   xml_parent = xml_element.Parent()->ToElement();
   if (xml_parent)
      parentHdfType = xml_parent->Attribute("hdfObjType");
   else
      parentHdfType = NULL;

   //if parent is dataset we pop 4 things...
   if (parentHdfType && std::string(parentHdfType) == "DataSet")
   {

      hdfa = stack.pop();
      hdfb = stack.pop();
      hdfc = stack.pop();
      hdfparent = stack.pop();

      stack.push(hdfparent);
      stack.push(hdfc);
      stack.push(hdfb);
      stack.push(hdfa);

   }
   else
   {
      //if file or group is parent we pop 1 thing..
      hdfparent = stack.pop();
      stack.push(hdfparent);

   }

   write_xml_text = false;

   if (nodeWhen && std::string(nodeWhen) == "OnFileClose")
      write_xml_text = true;

   if (nodeHdfObjType)
   {
      if (std::string(nodeHdfObjType) == "Group")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
               "Enter %s %s\n", nodeHdfObjType, nodeValue);
         //hdfnode = H5Gcreate(hdfparent,    nodeValue,   H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
         hdfnode = H5Gopen(hdfparent, nodeValue, H5P_DEFAULT);
         stack.push(hdfnode);
      }

      if (std::string(nodeHdfObjType) == "DataSet")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
               "Enter %s %s\n", nodeHdfObjType, nodeValue);
         hid_t dataset, datatype, dataspace, plist;
         //hsize_t dimsf[2];
         pArraySpecs img_specs;
         int fillval = 0;
         int nodeDataTypeInt;

         // if (std::string(nodeSource)=="Images")
         //{
         //getNdArraySpecs(&img_specs);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitEnter(el, attr)* start*** Attr %s **\n", className, nodeValue);
         dataset = H5Dopen(hdfparent, nodeValue, H5P_DEFAULT);
         dataspace = H5Dget_space(dataset);

         datatype = H5Dget_type(dataset);

         plist = H5Dget_access_plist(dataset);
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "%s::VisitEnter(el, attr)* end*** Attr %s **\n", className, nodeValue);

         stack.push(dataset);
         stack.push(datatype);
         stack.push(dataspace);
         stack.push(plist);

         //}
         if (std::string(nodeWhen) == "OnFileClose")
         {
            if (std::string(nodeSource) == "NDAttr")
            {

               if (nodeDatatype != NULL)
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                        "Dataset datatype ignored-derived from NDAttribute\n");

               my_epics->getNDAttr(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               writeData((void*) scratch_str);

            }
            else if (std::string(nodeSource) == "EpicsPV")
            {
               if (nodeDatatype != NULL)
                  asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                        "Dataset datatype ignored-derived from PV\n");

               my_epics->epicsCaGet(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               writeData((void*) scratch_str);

            }
            else if (std::string(nodeSource) == "Images")
            {
               writeData(my_epics->getNDArraySpecs(&img_specs)->data);
            }
            else
            {
               asynPrint(my_epics->pasynUser, ASYN_TRACE_ERROR,
                     "DataSet %s unrecognized source\n", nodeValue);
            }

         }

      }

      if (std::string(nodeHdfObjType) == "Attr")
      {
         asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
               "Enter %s %s\n", nodeHdfObjType, nodeValue);

         herr_t hdfstatus;
         hid_t hdfdatatype;
         hid_t hdfattr;
         hid_t hdfattrdataspace;
         int nodeDataTypeInt;

         /// nodeDataTypeInt = typeStringToVal(nodeDatatype);

         hdfattr = H5Aopen(hdfparent, nodeValue, H5P_DEFAULT);
         hdfattrdataspace = H5Aget_space(hdfattr);
         hdfdatatype = H5Aget_type(hdfattr);
         stack.push(hdfattr);
         stack.push(hdfdatatype);

         stack.push(hdfattrdataspace);

//hdfstatus        = H5Awrite(hdfattr, H5T_NATIVE_INT32, (void*)&hdfattrval);
         if (nodeWhen && std::string(nodeWhen) == "OnFileClose")
         {
            if (nodeSourceName == 0)
            {
               asynPrint(my_epics->pasynUser, ASYN_TRACE_FLOW,
                              "Attr %s needs source_name\n", nodeValue);
               return (true);
            }

            if (nodeSource == NULL || std::string(nodeSource) == "CONST")
            {
               // we write the const in the xml text

               //if (nodeDataTypeInt==H5T_STR_NULLTERM)
               //    H5Awrite(hdfattr, hdfdatatype, xml_text.Value());
               //else
               //    H5Awrite(hdfattr, hdfdatatype, constTextToDataType((char*)xml_text.Value(),nodeDataTypeInt,&anytype_data) );
            } //Const
            else if (std::string(nodeSource) == "NDAttr")
            {

               my_epics->getNDAttr(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               H5Awrite(hdfattr, hdfdatatype, (void*) scratch_str);

            } //NDAttr
            else if (std::string(nodeSource) == "EpicsPV")
            {

               my_epics->epicsCaGet(nodeSourceName, (void*) scratch_str,
                     &nodeDataTypeInt);

               H5Awrite(hdfattr, hdfdatatype, (void*) scratch_str);

            } //epicsPV

         } //if nodeWhen

      }

   }
   return (true);

}

/******************************************************************************************************
 *
 *****************************************************************************************************/

#if 0
bool TiXMLhdf5Shutdown::Visit (const TiXmlText &txt)
{
   TiXMLhdf5WriteData::Visit(txt);
   return(true);

}

#endif

/******************************************************************************************************
 *
 *****************************************************************************************************/
/******************************************************************************************************
 *
 *****************************************************************************************************/

/******************************************************************************************************
 *
 *****************************************************************************************************/

/******************************************************************************************************
 *
 *****************************************************************************************************/

/******************************************************************************************************
 *
 *****************************************************************************************************/
/******************************************************************************************************
 *
 *****************************************************************************************************/
/******************************************************************************************************
 *
 *****************************************************************************************************/
/******************************************************************************************************
 *
 *****************************************************************************************************/

/******************************************************************************************************
 *
 *****************************************************************************************************/

/******************************************************************************************************
 *
 *****************************************************************************************************/

/******************************************************************************************************
 *
 *****************************************************************************************************/
/******************************************************************************************************
 *
 *****************************************************************************************************/
/******************************************************************************************************
 *
 *****************************************************************************************************/
/******************************************************************************************************
 *
 *****************************************************************************************************/

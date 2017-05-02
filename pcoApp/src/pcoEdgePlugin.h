/*
 * pcoEdgePlugin.h
  *Tim Madden
 */

#ifndef pcoEdgePlugin_H
#define pcoEdgePlugin_H

/** Enums for plugin-specific parameters. There are currently no specific
 * parameters for this driver yet.
  * It uses the ADStdDriverParams and NDPluginDriver params. */

#include "NDPluginFile.h"

class epicsShareClass pcoEdgePlugin : public NDPluginFile {
 public:
  pcoEdgePlugin(const char *portName, int queueSize, int blockingCallbacks,
                const char *NDArrayPort, int NDArrayAddr, int priority,
                int stackSize);

  virtual void processCallbacks(NDArray *pArray);
  /* The methods that this class implements */
  virtual asynStatus openFile(const char *fileName, NDFileOpenMode_t openMode,
                              NDArray *pArray);
  virtual asynStatus readFile(NDArray **pArray);
  virtual asynStatus writeFile(NDArray *pArray);
  virtual asynStatus closeFile();

  int getIntParam(int param);

  // virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
  //                            const char **pptypeName, size_t *psize);
 protected:
  void sort_lines_A(int width, int height, void *adr_out, void *adr_in);
  void sort_lines_B(int width, int height, void *adr_out, void *adr_in);
  void sort_lines_C(int width, int height, void *adr_out, void *adr_in);
  void sort_lines_D(int width, int height, void *adr_out, void *adr_in);

  // enough for 1 edge imgae. pco imges are smaller...
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
#define FIRST_pcoEdgePlugin_PARAM sim_first

  // wait in sec
  int is_descramble;
  int is_decompress;
  int print_attr;
  int is_use_attr;
  int is_pr_pco_attr;
  int is_enable;

  int edge_nd_datasize;
  int edge_max_ndbuffers;
  int edge_num_ndbuffers;
  int edge_max_ndmemory;
  int edge_alloc_ndmemory;
  int edge_free_ndbuffers;
  int edge_est_buffers_left;

  int sim_last;

  //
  //////////////// END PARAMETERS ///////////////////
  //
};


#endif

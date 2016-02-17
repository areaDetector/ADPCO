

/*
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>

#include <epicsStdio.h>
#include <epicsExport.h>
#include <iocsh.h>

#include <epicsTime.h>
#include <epicsThread.h>

#include "NDPluginFile.h"
#include "pcoEdgePlugin.h"
#include "drvpcoEdgePlugin.h"
#include "asynNDArrayDriver.h"

static const char *driverName = "pcoEdgePlugin";

// Section for Custom Parameters
//

//
// End Section for Custom Params
//

/* Handle errors by printing an error message and exiting with a
 * non-zero status. */
#define ERR(e)                                                           \
  {                                                                      \
    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "%s:%s error=%s\n", \
              driverName, functionName, nc_strerror(e));                 \
    return (asynError);                                                  \
  }

/* NDArray string attributes can be of any length, but netCDF requires a fixed
 * maximum length
 * which we define here. */
#define MAX_ATTRIBUTE_STRING_SIZE 256

asynStatus pcoEdgePlugin::openFile(const char *fileName,
                                   NDFileOpenMode_t openMode, NDArray *pArray) {
  /*    nc_type ncType=NC_NAT;*/
  char str0[256];
  char str1[256];
  int fn0, fn1;
  char fullfname[256];
  int is_update;
  static const char *functionName = "openFile";

  double wt;

  return (asynSuccess);
}

/** This is called to write data a single NDArray to the file.  It can be called
 * multiple times
 *  to add arrays to a single file in stream or capture mode */
asynStatus pcoEdgePlugin::writeFile(NDArray *pArray) {
  unsigned long int stripsize;
  static const char *functionName = "writeFile";
  int sizex, sizey;
  int is_update;
  int fnx;
  int ii0;
  int imgc, nimg;
  double wt;

  return (asynSuccess);
}

int pcoEdgePlugin::getIntParam(int param) {
  int saveparamx;
  getIntegerParam(param, &saveparamx);

  return (saveparamx);
}

/*******************************************************************************************
 *
 *
 *
 ******************************************************************************************/

void pcoEdgePlugin::processCallbacks(NDArray *pArray) {
  double elapsed_time;
  // unsigned short *shrt_img;
  int xs;
  int ys;
  int rowa;
  int rowb;
  int rows;
  int kx;
  void *dst, *src;

  unsigned short diff;
  int diff_tot;
  int rowbytes;
  int attrval = 0;

  char mesgx[256];
  int pco_descramble;
  int pco_edge_fastscan;
  int pco_conv_12to16;
  int pco_disable_descramble;

  int dd;
  size_t dims[2];

  NDArrayPool *mypool;

  mypool = pArray->pNDArrayPool;

  this->unlock();

  ///
  ///
  ///

  // numAttributes = pArray->numAttributes();

  if (getIntParam(print_attr) == 1) {
    numAttributes = pArray->pAttributeList->count();

    printf("Num Attributes %i \n", numAttributes);
    pAttribute = pArray->pAttributeList->next(NULL);
    for (attrCount = 0; attrCount < numAttributes; attrCount++) {
      pAttribute->getValueInfo(&attrDataType, &attrSize);
      // strcpy(name, pAttribute->pName);
      // strcpy(description, pAttribute->pDescription);
      // pAttribute->getDescription(description, sizeof(description));
      // pAttribute->getValue(attrDataType, void *pValue, attrSize);

      printf("Attr Name %s, Addr Disc %s\n", pAttribute->getName(),
             pAttribute->getDescription());

      pAttribute = pArray->pAttributeList->next(pAttribute);
      // pAttribute = pArray->nextAttribute(pAttribute);
    }
  }

  if (getIntParam(is_enable) > 0) {
    // numAttributes = pArray->numAttributes();
    numAttributes = pArray->pAttributeList->count();

    NDAttribute *atrx;

    atrx = pArray->pAttributeList->find("pco_descramble");

    if (atrx != 0) {
      atrx->getValue(NDAttrInt32, &pco_descramble, sizeof(int));
      if (getIntParam(is_use_attr) == 1)
        setIntegerParam(is_descramble, pco_descramble);

      if (getIntParam(is_pr_pco_attr))
        printf("pco_descramble = %d\n", pco_descramble);
    } else if (getIntParam(is_pr_pco_attr))
      printf("pco edge plugin: Could not get pcodescramble attr\n");

    atrx = pArray->pAttributeList->find("pco_conv_12to16");

    if (atrx != 0) {
      atrx->getValue(NDAttrInt32, &pco_conv_12to16, sizeof(int));
      if (getIntParam(is_use_attr) == 1) {
        if (pco_conv_12to16 == 1)
          setIntegerParam(is_decompress, 1);
        else
          setIntegerParam(is_decompress, 0);
      }

      if (getIntParam(is_pr_pco_attr))
        printf("pco_conv_12to16 = %d\n", pco_conv_12to16);

    } else if (getIntParam(is_pr_pco_attr))
      printf("pco edge plugin: Could not get pco_conv_12to16 attr\n");

    atrx = pArray->pAttributeList->find("pco_disable_descramble");

    if (atrx != 0) {
      atrx->getValue(NDAttrInt32, &pco_disable_descramble, sizeof(int));
      if (getIntParam(is_use_attr) == 1) {
        if (pco_disable_descramble == 0) {
          setIntegerParam(is_decompress, 0);
          setIntegerParam(is_descramble, 0);
        }
      }

      if (getIntParam(is_pr_pco_attr))
        printf("pco_disable_descramble = %d\n", pco_disable_descramble);

    } else if (getIntParam(is_pr_pco_attr))
      printf("pco edge plugin: Could not get pco_disable_descramble attr\n");

    // if (getIntParam(is_decompress)==1 || getIntParam(is_descramble)==1)

    xs = pArray->dims[0].size;
    ys = pArray->dims[1].size;

    rowa = 0;

    rowb = ys - 1;
    rows = 0;

    rowbytes = xs * 2;

    if (xs > 2560 || xs < 0 || ys > 2160 || ys < 0) {
      printf("ERROR: pco plugin: SIZEX or SizeY Illegal\n");
    }

    //
    //
    //
    if (mypool) {
      //!! where is array_pool set up? need to look at all constructors and
      //! mains in older code.
      setIntegerParam(edge_nd_datasize, 2 * xs * ys);
      setIntegerParam(edge_max_ndbuffers, mypool->maxBuffers());
      setIntegerParam(edge_num_ndbuffers, mypool->numBuffers());
      setIntegerParam(edge_max_ndmemory, mypool->maxMemory());
      setIntegerParam(edge_alloc_ndmemory, mypool->memorySize());
      setIntegerParam(edge_free_ndbuffers, mypool->numFree());

      if (getIntParam(edge_nd_datasize) > 0) {
        setIntegerParam(edge_est_buffers_left,
                        getIntParam(edge_free_ndbuffers) +
                            ((getIntParam(edge_max_ndmemory) -
                              getIntParam(edge_alloc_ndmemory)) /
                             getIntParam(edge_nd_datasize)));
      }
    }

    //
    //
    //

    dims[0] = xs;

    dims[1] = ys;
    NDArray *nimg = mypool->alloc(2, (size_t *)dims, NDInt16, (size_t)0, NULL);

    if (nimg == 0) {
      printf("ERROR: pco plugin could not alloc nd array\n");
      return;
    }

    mypool->copy(pArray, nimg, 0);

    if (getIntParam(is_decompress) == 1) {
      // this means that we have 12 bit encoded data and we have to extract to
      // 16bit
      // lengh of line in ints of xs pixels for 12 bit data.
      // xs is in words of 12 bits.
      // to find number of chars in same amount of memory, mult by 1.5.
      // to find number of ints, we then divide by 4.
      double nibbles = (double)xs * 3.0;
      double shorts_in = nibbles / 4.0;

      unsigned int longpix;
      unsigned short shortpix;

      if ((shorts_in - floor(shorts_in)) > 0.0001) {
        printf("ERROR- Decompression shorts in= %f,   img size= %d", shorts_in,
               xs);
      }

      int len_in = (int)floor(shorts_in);
      int len_out = xs;

      // so 3*32 bits is 8 pixels
      // we will count by groups of 3 ints to get one line.

      unsigned short *iptr = (unsigned short *)(pArray->pData);
      unsigned short *optr = (unsigned short *)(temp_image);

      //
      for (rows = 0; rows < ys; rows++) {
        // go across one line.

        int k = 0;
        int m = 0;

        while (k < len_in) {
          shortpix = (iptr[k] & 0xfff0) >> 4;
          // we assume we are using the 16-12 lut in edge camera. the table is
          // defined
          // as y=x for x<=256
          // y=int(srrt(256*x)+0.5)
          // we implement the inverse of this table
          if (shortpix > 256) {
            longpix = (unsigned int)shortpix;
            longpix = longpix * longpix;
            longpix = longpix >> 8;
            shortpix = (unsigned short)longpix;
          }

          optr[m] = shortpix;

          m = m + 1;

          shortpix = (iptr[k] & 0xf) << 8;
          k = k + 1;

          shortpix = shortpix + ((iptr[k] & 0xff00) >> 8);

          if (shortpix > 256) {
            longpix = (unsigned int)shortpix;
            longpix = longpix * longpix;
            longpix = longpix >> 8;
            shortpix = (unsigned short)longpix;
          }

          optr[m] = shortpix;

          m = m + 1;

          shortpix = (iptr[k] & 0xff) << 4;
          k = k + 1;

          shortpix = shortpix + ((iptr[k] & 0xf000) >> 12);

          if (shortpix > 256) {
            longpix = (unsigned int)shortpix;
            longpix = longpix * longpix;
            longpix = longpix >> 8;
            shortpix = (unsigned short)longpix;
          }

          optr[m] = shortpix;

          m = m + 1;

          shortpix = (iptr[k] & 0xfff);

          if (shortpix > 256) {
            longpix = (unsigned int)shortpix;
            longpix = longpix * longpix;
            longpix = longpix >> 8;
            shortpix = (unsigned short)longpix;
          }

          optr[m] = shortpix;
          m = m + 1;
          k = k + 1;
        }

        iptr += k;
        optr += m;
      }
    } else {
      // if no 12 to 16 bit extraction, then copy raw to temp img

      memcpy((void *)temp_image, (void *)pArray->pData,
             xs * ys * sizeof(short));
    }

    unsigned short *shrt_img = (unsigned short *)nimg->pData;

    if (getIntParam(is_descramble) == 1) {
      sort_lines_A(xs, ys, shrt_img, temp_image);
      // descramnle from temp img to output img, called shrt image.

      /*
      for (rows=0;rows<ys;rows=rows+2)
      {
              // odds rows on bottom , bottom 2 top. we are calc in shorts...
              dst = (void*)(shrt_img+rowb*xs);
              src = (void*)(temp_image+(rows+1)*xs);
              memcpy(dst,src,rowbytes);
              //even rows on top, top 2 bottom
              dst=(void*)(shrt_img+rowa*xs);
              src = (void*)(temp_image+(rows)*xs);
              memcpy(dst,src,rowbytes);

              rowa++;
              rowb--;

      }
      */
    } else if (getIntParam(is_descramble) == 2) {
      sort_lines_B(xs, ys, shrt_img, temp_image);
    } else if (getIntParam(is_descramble) == 3) {
      sort_lines_C(xs, ys, shrt_img, temp_image);
    } else if (getIntParam(is_descramble) == 4) {
      sort_lines_D(xs, ys, shrt_img, temp_image);
    } else {
      memcpy((void *)nimg->pData, (void *)temp_image, xs * ys * sizeof(short));
    }

    nimg->timeStamp = pArray->timeStamp;
    nimg->uniqueId = getIntParam(NDUniqueId);

    setIntegerParam(NDUniqueId, getIntParam(NDUniqueId) + 1);

    getAttributes(nimg->pAttributeList);

    doCallbacksGenericPointer(nimg, NDArrayData, 0);
    nimg->release();
    callParamCallbacks();
  }

  this->lock();

  // check for bad FPGA headers, get stats etc...
  // call base class function...
  NDPluginFile::processCallbacks(pArray);
}

asynStatus pcoEdgePlugin::readFile(NDArray **pArray) {
  // static const char *functionName = "readFile";

  return asynError;
}

asynStatus pcoEdgePlugin::closeFile() {
  static const char *functionName = "closeFile";
  int fnx;
  int is_update;

  return asynSuccess;
}

/*


/** Constructor for NDPluginFile; all parameters are simply passed to
NDPluginDriver::NDPluginDriver.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] queueSize The number of NDArrays that the input queue for this
plugin can hold when
  *            NDPluginDriverBlockingCallbacks=0.  Larger queues can decrease
the number of dropped arrays,
  *            at the expense of more NDArray buffers being allocated from the
underlying driver's NDArrayPool.
  * \param[in] blockingCallbacks Initial setting for the
NDPluginDriverBlockingCallbacks flag.
  *            0=callbacks are queued and executed by the callback thread; 1
callbacks execute in the thread
  *            of the driver doing the callbacks.
  * \param[in] NDArrayPort Name of asyn port driver for initial source of
NDArray callbacks.
  * \param[in] NDArrayAddr asyn port driver address for initial source of
NDArray callbacks.
  * \param[in] maxAddr The maximum  number of asyn addr addresses this driver
supports. 1 is minimum.
  * \param[in] numParams The number of parameters supported by the derived class
calling this constructor.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the
NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited number
of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for
this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited amount
of memory.
  * \param[in] interfaceMask Bit mask defining the asyn interfaces that this
driver supports.
  * \param[in] interruptMask Bit mask definining the asyn interfaces that can
generate interrupts (callbacks)
  * \param[in] asynFlags Flags when creating the asyn port driver; includes
ASYN_CANBLOCK and ASYN_MULTIDEVICE.
  * \param[in] autoConnect The autoConnect flag for the asyn port driver.
  * \param[in] priority The thread priority for the asyn port driver thread if
ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if
ASYN_CANBLOCK is set in asynFlags.
  *
NDPluginFile::NDPluginFile(const char *portName, int queueSize, int
blockingCallbacks,
                           const char *NDArrayPort, int NDArrayAddr, int
maxAddr, int numParams,
                           int maxBuffers, size_t maxMemory, int interfaceMask,
int interruptMask,
                           int asynFlags, int autoConnect, int priority, int
stackSize)

     Invoke the base class constructor.
     * We allocate 1 NDArray of unlimited size in the NDArray pool.
     * This driver can block (because writing a file can be slow), and it is not
multi-device.
     * Set autoconnect to 1.  priority and stacksize can be 0, which will use
defaults.
    : NDPluginDriver(portName, queueSize, blockingCallbacks,
                     NDArrayPort, NDArrayAddr, maxAddr,
numParams+NUM_NDPLUGIN_FILE_PARAMS, maxBuffers, maxMemory,
                     asynGenericPointerMask, asynGenericPointerMask,
                     asynFlags, autoConnect, priority, stackSize)
{

*/

/* Configuration routine.  Called directly, or from the iocsh function in
 * drvNDFileEpics */

extern "C" int drvpcoEdgePluginConfigure(const char *portName, int queueSize,
                                         int blockingCallbacks,
                                         const char *NDArrayPort,
                                         int NDArrayAddr, int priority,
                                         int stackSize) {
  new pcoEdgePlugin(portName, queueSize, blockingCallbacks, NDArrayPort,
                    NDArrayAddr, priority, stackSize);
  return (asynSuccess);
}

/* The constructor for this class */
pcoEdgePlugin::pcoEdgePlugin(const char *portName, int queueSize,
                             int blockingCallbacks, const char *NDArrayPort,
                             int NDArrayAddr, int priority, int stackSize)
    : NDPluginFile(portName, queueSize, blockingCallbacks, NDArrayPort,
                   NDArrayAddr, 1, NUM_pcoEdgePlugin_PARAMS, 200000, -1,
                   asynGenericPointerMask, asynGenericPointerMask,
                   ASYN_CANBLOCK, 1, priority, stackSize) {
  int i;

  // wait in sec

  createParam("is_descramble", asynParamInt32, &is_descramble);
  createParam("is_decompress", asynParamInt32, &is_decompress);
  createParam("print_attr", asynParamInt32, &print_attr);
  createParam("is_use_attr", asynParamInt32, &is_use_attr);

  createParam("is_pr_pco_attr", asynParamInt32, &is_pr_pco_attr);
  createParam("is_enable", asynParamInt32, &is_enable);

  createParam("edge_nd_datasize", asynParamInt32, &edge_nd_datasize);
  createParam("edge_max_ndbuffers", asynParamInt32, &edge_max_ndbuffers);
  createParam("edge_num_ndbuffers", asynParamInt32, &edge_num_ndbuffers);
  createParam("edge_max_ndmemory", asynParamInt32, &edge_max_ndmemory);
  createParam("edge_alloc_ndmemory", asynParamInt32, &edge_alloc_ndmemory);
  createParam("edge_free_ndbuffers", asynParamInt32, &edge_free_ndbuffers);
  createParam("edge_est_buffers_left", asynParamInt32, &edge_est_buffers_left);

  // enough for edge dimax and thensome
  temp_image = (unsigned short *)malloc(2560 * 2200 * sizeof(short));

  setIntegerParam(is_enable, 0);

  setIntegerParam(is_descramble, 1);
  setIntegerParam(is_decompress, 1);
  setIntegerParam(is_use_attr, 1);
  setIntegerParam(print_attr, 0);
  setIntegerParam(is_pr_pco_attr, 0);

  this->supportsMultipleArrays = 1;
  this->pAttributeId = NULL;
}

void pcoEdgePlugin::sort_lines_A(int width, int height, void *adr_out,
                                 void *adr_in) {
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;

  line_1 = (unsigned short *)adr_out;
  line_n = (unsigned short *)adr_out;
  line_n += (height - 1) * width;
  line_in = (unsigned short *)adr_in;

  for (int y = 0; y < height / 2; y++) {
    memcpy(line_1, line_in, width * sizeof(unsigned short));
    line_in += width;
    memcpy(line_n, line_in, width * sizeof(unsigned short));
    line_in += width;

    line_1 += width;
    line_n -= width;
  }
}

void pcoEdgePlugin::sort_lines_B(int width, int height, void *adr_out,
                                 void *adr_in) {
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;

  line_1 = (unsigned short *)adr_out;
  line_1 += (height / 2 - 1) * width;
  line_n = (unsigned short *)adr_out;
  line_n += (height / 2) * width;
  line_in = (unsigned short *)adr_in;

  for (int y = 0; y < height / 2; y++) {
    memcpy(line_1, line_in, width * sizeof(unsigned short));
    line_in += width;
    memcpy(line_n, line_in, width * sizeof(unsigned short));
    line_in += width;

    line_1 -= width;
    line_n += width;
  }
}

void pcoEdgePlugin::sort_lines_C(int width, int height, void *adr_out,
                                 void *adr_in) {
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;

  line_1 = (unsigned short *)adr_out;
  line_1 += (height / 2 - 1) * width;
  line_n = (unsigned short *)adr_out;
  line_n += (height - 1) * width;
  line_in = (unsigned short *)adr_in;

  for (int y = 0; y < height / 2; y++) {
    memcpy(line_1, line_in, width * sizeof(unsigned short));
    line_in += width;
    memcpy(line_n, line_in, width * sizeof(unsigned short));
    line_in += width;

    line_1 -= width;
    line_n -= width;
  }
}

void pcoEdgePlugin::sort_lines_D(int width, int height, void *adr_out,
                                 void *adr_in) {
  unsigned short *line_1;
  unsigned short *line_n;
  unsigned short *line_in;

  line_1 = (unsigned short *)adr_out;
  line_n = (unsigned short *)adr_out;
  line_n += (height / 2) * width;
  line_in = (unsigned short *)adr_in;

  for (int y = 0; y < height / 2; y++) {
    memcpy(line_1, line_in, width * sizeof(unsigned short));
    line_in += width;
    memcpy(line_n, line_in, width * sizeof(unsigned short));
    line_in += width;

    line_1 += width;
    line_n += width;
  }
}

/*
 * drvNDFileHDF5XML.h
 *
 * Asyn driver for callbacks to write data to HDF5XML files for area detectors.
 *
 * Author: Tim Madden 
 *
 * Created Nov 4, 2009
 */

#ifndef DRV_NDFileHDF5XML_H
#define DRV_NDFileHDF5XML_H

#ifdef __cplusplus
extern "C" {
#endif

int drvNDFileHDF5XMLConfigure(const char *portName, int queueSize, int blockingCallbacks,
                             const char *NDArrayPort, int NDArrayAddr,
                             int priority, int stackSize);

#ifdef __cplusplus
}
#endif
#endif

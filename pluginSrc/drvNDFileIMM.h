/*
 * drvNDFileIMM.h
 *
 * Asyn driver for callbacks to write data to IMM files for area detectors.
 *
 * Author: Tim Madden (thanks to J. Hammonds, B. Tieman)
 *
 * Created Nov 4, 2009
 */

#ifndef DRV_NDFileIMM_H
#define DRV_NDFileIMM_H

#ifdef __cplusplus
extern "C" {
#endif

int drvNDFileIMMConfigure(const char *portName, int queueSize, int blockingCallbacks,
                             const char *NDArrayPort, int NDArrayAddr,
                             int priority, int stackSize);

#ifdef __cplusplus
}
#endif
#endif

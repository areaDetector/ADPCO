/* drvPCOEpics.c
 *
 * This is the EPICS dependent code for the driver for a simulated area detector.
 * By making this separate file for the EPICS dependent code the driver itself
 * only needs libCom from EPICS for OS-independence.
 *
 * Author: Mark Rivers
 *         University of Chicago
 *
 * Created:  March 20, 2008
 *
 */

#include <string.h>
#include <iocsh.h>
#include <drvSup.h>
#include <epicsExport.h>

#include "drvPco.h"


/* Code for iocsh registration */


/*
 *
 */


/* PCOConfig */
static const iocshArg PCOConfigArg0 = {"Port name", iocshArgString};
static const iocshArg PCOConfigArg1 = {"SERVNAME", iocshArgString};
static const iocshArg PCOConfigArg2 = {"maxBuffers", iocshArgInt};
static const iocshArg PCOConfigArg3 = {"maxMemory", iocshArgInt};
static const iocshArg PCOConfigArg4 = {"priority", iocshArgInt};
static const iocshArg PCOConfigArg5 = {"stackSize", iocshArgInt};

static const iocshArg * const PCOConfigArgs[] =  {&PCOConfigArg0,
                                                          &PCOConfigArg1,
                                                          &PCOConfigArg2,
                                                          &PCOConfigArg3,
                                                          &PCOConfigArg4,
                                                          &PCOConfigArg5};



static const iocshFuncDef configPCO = {"PCOConfig", 6, PCOConfigArgs};
static void configPCOCallFunc(const iocshArgBuf *args)
{
    PCOConfig(args[0].sval, args[1].sval, args[2].ival, args[3].ival,
                      args[4].ival, args[5].ival);
}






/*
 *
 */


/* PCOConfig */
static const iocshArg PCOBlockArg0 = {"cmd", iocshArgString};
static const iocshArg PCOBlockArg1 = {"blocknum", iocshArgInt};

static const iocshArg * const PCOBlockArgs[] =  {&PCOBlockArg0, &PCOBlockArg1};


static const iocshFuncDef blockPCO = {"PCOBlock", 2, PCOBlockArgs};
static void blockPCOCallFunc(const iocshArgBuf *args)
{
   if ( strcmp(args[0].sval,"blockN")==0)
        pco_blocking = args[1].ival;

   if ( strcmp(args[0].sval,"prMutex")==0)
       pco_pr_mutex=args[1].ival;

   if ( strcmp(args[0].sval,"prMutex2")==0)
       pco_pr_mutex2=args[1].ival;


}


/*
*
*/





static void PCORegister(void)
{

    iocshRegister(&configPCO, configPCOCallFunc);
    iocshRegister(&blockPCO, blockPCOCallFunc);


}

epicsExportRegistrar(PCORegister);



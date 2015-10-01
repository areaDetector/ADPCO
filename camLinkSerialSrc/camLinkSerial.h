#ifndef camLinkSerial_H
#define camLinkSerial_H

#include "asynPortDriver.h"

#include "comportInterface.h"
#include "siswSerialPort.h"

#ifdef _WIN32
#include "cl_com_port2.h"
#endif

#include <string.h>
#include <map>

/*
 * Tim Madden- 2013AD
 */
 
 
 /*
 

typedef enum {
    asynParamUndefined,    
    asynParamInt32,
    asynParamUInt32Digital,
    asynParamFloat64,
    asynParamOctet,
    asynParamInt8Array,
    asynParamInt16Array,
    asynParamInt32Array,
    asynParamFloat32Array,
    asynParamFloat64Array,
    asynParamGenericPointer
} asynParamType;

 
 */
 
    
 
class epicsShareFunc camLinkSerial : public asynPortDriver {
public:
    camLinkSerial(
        const char *portName, 
        const char *comportname,
        int maxAddr,
        int interfaceMask, 
        int interruptMask,
        int asynFlags, 
        int autoConnect, 
        int priority, 
        int stackSize);

    /* These are the methods that we override from asynPortDriver */
   virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars,
                                        size_t *nActual, int *eomReason);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars,
                                        size_t *nActual);
    virtual asynStatus flushOctet(asynUser *pasynUser);
    
    virtual void report(FILE *fp, int details);
    void optionsToSerial(void);

    
      virtual asynStatus readOption(asynUser *pasynUser, const char *key, char *value, int maxChars);
    virtual asynStatus writeOption(asynUser *pasynUser, const char *key, const char *value);
  

 
protected:

//map for the option key val pairs
std::map <std::string, std::string> option_pairs;

comportInterface *serial_port;
    //
    // params
    //
    int clser_first;
    #define FIRST_CLS_PARAM clser_first
    
    int clser_portname;
    
    
    int clser_last;
    #define LAST_CLS_PARAM clser_last

    //
    // End params
    //
    
    
   
};

#define NUM_CLS_PARAMS (&LAST_CLS_PARAM - &FIRST_CLS_PARAM + 1)
#endif

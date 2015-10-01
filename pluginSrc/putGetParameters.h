/*******************************************************************************
 *
 *
 *    Author: Tim Madden
 *    Date:    7/16/03
 *    Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/


/*
 * Include files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>

/*
 * Double incluson protection.
 */
#ifndef _PUT_GET_PARAMETERS_H
    #define _PUT_GET_PARAMETERS_H


#ifdef BUILD_DLL
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP __declspec(dllimport)
#endif



/*******************************************************************************
 *
 * class to contain arrays of volatile bools, floats, ints, void pointers.
 * this is for thread safe communication between threads, by setting flags etc.
 *
 ******************************************************************************/

class DLLIMPEXP putGetParameters 
{
    public:
    
        putGetParameters(const bool is_share_params);
        virtual ~putGetParameters();

        // Download serial stream to camera.
        virtual int putIntParam(const int index, const int value);
        virtual int getIntParam(const int index, int &value);
        virtual int getIntParam(const int index);

        virtual int putBoolParam(const int index, const bool value);
        virtual int getBoolParam(const int index, bool &value);
        virtual bool getBoolParam(const int index);

        virtual int putFloatParam(const int index, const float value);
        virtual int getFloatParam(const int index, float &value);
        virtual float getFloatParam(const int index);

        virtual int putDoubleParam(const int index, const double value);
        virtual int getDoubleParam(const int index, double &value);
        virtual double getDoubleParam(const int index);

        virtual int putPtrParam(const int index, const void* value);
        virtual int getPtrParam(const int index, void* &value);
        virtual void* getPtrParam(const int index);

        virtual int putStringParam(const int index, const char* value);
        virtual int getStringParam(const int index, const char* value);
        virtual char* getStringParam(const int index);
        
        virtual int getParamType(const int index);
        
        virtual int getNumParameters(void) {return num_parameters;};
    
        /*
         * The idea here is that if we set a parameter, say, detector_gain, then some software
         * must know that it must iupdate the gain on the detector. So it looks in the flags for
         * any parameters that were put. After iuopdateding the detector,  the software calls
         * clearChanges.
         */
        // if we put a param, it flags that that param is changed.
        virtual void putIsChanged(const int index);

        // return true of param X is changed.
        virtual bool getIsChanged(const int index);
        // clears all flags that  params  changed.
        virtual int clearChanges(void);
        virtual int clearChanges(const int index);
        // if any params changed, return true
        virtual bool getIsAnyChanges(void);

        // These datatypes shamelessly copued and modified from asynPortDriver.h from the EPICS Gurus.
        enum paramType
        { 
            paramUndef, 
            paramInt, 
            paramDouble, 
            paramString,
            paramFloat,
            paramPtr,
            paramBool
        };
    // this subclass based on asynPortDriver.h paramlist class.
    class paramVal
    {
        public:
        paramType type;
        bool is_changed;
        void *callback_ptr;
        union
        {
            double dval;
            int    ival;
            char  *sval;
            float fval;
            void *pval;
            bool bval;
        } data;
    };
    enum {
        def_num_param = 2048,
        param_str_len = 512
    };

         // if all params are to be shared, we declare some static memory.
 static paramVal stat_vals[def_num_param];     
        // set that param X is changed.
        //void putIsChanged(const int index);

static bool is_init_params;
protected:



 int num_parameters;
 // in most cases we do NOT share params between objs. so we have independant pointer...
 paramVal* vals;
    
};

#endif

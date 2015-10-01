/*
 * Include files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>

//#include "serial_stream.h"
#include "exe.h"
//#include "thread.h"
//#include "mutex.h"
#include "putGetParameters.h"
#include "genCamControllerEnums.h"
/*
 * Double incluson protection.
 */
#ifndef _GEN_CAM_CONTROLLER_H
    #define _GEN_CAM_CONTROLLER_H



#ifdef BUILD_DLL
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP __declspec(dllimport)
#endif


//using namespace genCamControllerEnum;



/*******************************************************************************
 *
 * this is abstract,
 * a real class will have nigroup and other objects in it. and the interface
 * will wrap it. the idea is to make the interface like the camera, and not
 * the NI card.inside the real class, a modeport object will be there. when
 * we change camera modes, we create and destroy nigroup, serialport objects.
 * the ni card is setup in the constructors and destructors of these objects.
 ******************************************************************************/

class DLLIMPEXP genCamController :
    public exe,
    //public thread,
    //public mutex,
    public putGetParameters
{
    public:



        genCamController();
        //genCamController(bool is_share_params);

        virtual ~genCamController();
        virtual int_string* getParamIntStringArray();
        virtual int getNumParams();
        virtual int getThisID();
        virtual int getTotalNumParams();
        virtual int_string* getCompleteParamIntStrArray();

        // Threading and mutexes etc.. wrappers for the callback function.
        virtual void sleep(int ms);
        virtual void suspend(void);
        virtual void suspend(int ms);
        virtual void wakeUp(void);
        virtual void wakeUpEveryone(void);
        virtual void imageCallback(void *ptr);

        virtual void preWait(int param);
        virtual void waitDone(int timeout_ms);
        virtual void notifyWaiters(int param);
        virtual void addAttribute(
            const char *pName,
            const char *pDescription,
            genCamControllerEnum::NDAttrDataType_t dataType,
            void *pValue);


        static int defaultCallback(genCamControllerEnum::callbackReasons reason, void* ptr) { return 0;}

        // when we callback to sleep, this tells how long...
        int sleepy_time;
// when we add attribute to NDArray type image, we store vals here.

         char *attribute_name;
         char *attribute_description;
         genCamControllerEnum::NDAttrDataType_t attribute_datatype;
         void *attribute_pvalue;


        enum {max_inherits=32};
        // we keep track of how many classes inherit from this class.
        // this counts GENERATATIONs, not how many calsses are consturcted.
        // for example, a serial download obj inherit from this. then something else
        //. inherit from serial download, that gives num_inherits = 2.
        int num_inherits;
        // each inherited class has its own set of parameters. To allow
        // inherited calsss to simply add to the list of parameters we
        // keep track of all the parameter lists in an array. each derived
        // class gets a slot in the array.
        int_string *paramIntStrArray[max_inherits];
        int num_params[max_inherits];

protected:

        static int id_counter;
        int this_id;

        int (*callbackFunction)(genCamControllerEnum::callbackReasons reason, void* ptr);
        volatile int wait_counter;
        volatile int pre_wait_counter;
        volatile int wait_param;


};

extern "C" DLLIMPEXP void makeControllerObjects(
    int *num_objects,
    genCamController *objs[],
    void *callback_func,
    void **img_data_ptr);


#endif

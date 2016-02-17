// AncDemoRotationExpDlg.h : header file
//

#ifndef _grabberInterface_H
#define _grabberInterface_H


class grabberInterface
{
// Construction
public:
 enum 
 {
 	gDALSA,
	gSISW,
	gSOFTWARE
	};
      
      virtual int getGrabberType()=0;

   virtual void GetSignalStatus()=0;

	virtual bool CreateObjects()=0;
	virtual bool DestroyObjects()=0;
	virtual bool DestroyObjectsNoDelete()=0;

  	virtual bool initialize(int size_x, int size_y)=0;
	// use to override the image size in the ccf file.
	virtual bool initialize(int size_x, int size_y,bool is_force_size)=0;
	virtual void setConfigFileName(char *name)=0;
	// set CC pins on the card.
	virtual void setPin(char* pinstr, int val)=0;
	virtual void snap()=0;
	 virtual void grab()=0;
	virtual void freeze()=0;
	virtual void abort()=0;
	virtual bool isFrameAvailable(void)=0;
	virtual bool getFrame(void *mem_ptr)=0;
	virtual bool getFrame(void *mem_ptr, unsigned int *grabberInterface_timestamp)=0;
	virtual bool getFrame(void *copy_memory,unsigned int *coreco_timestamp,int nbytes)=0;

	virtual bool isMissedFrame(void)=0;
	virtual void clearMissedFrames(void)=0;
	virtual long getTotalMissedFrames(void)=0;
	virtual long getRecentMissedFrames(void)=0;
	virtual void setNumBuffers(int b)=0;
	virtual void makeView(void)=0;
	virtual void setCamController(void* cc)=0;
	virtual int getWidth(void) =0;
	virtual int getHeight(void) =0;

	virtual void setDoubleWidth(int isdw)=0;

	// clears any images left in buffer memory by resetting all counters to 0
	virtual void resetBufferCount(void)=0;

   virtual int getNumFreeBuffers(void)=0;
   virtual int getNumBuffers(void)=0;
   //inc missed frames counter
   virtual void incMissedFrames(void)=0;


};


#endif

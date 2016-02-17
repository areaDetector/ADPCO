// AncDemoRotationExpDlg.h : header file
//

#ifndef _softwareGrabber_H
#define _softwareGrabber_H

#include "grabberInterface.h"

class softwareGrabber : public grabberInterface {
  // Construction
 public:
  softwareGrabber();

  int getGrabberType();

  void GetSignalStatus();

  bool CreateObjects();
  bool DestroyObjects();
  bool DestroyObjectsNoDelete();

  bool initialize(int size_x, int size_y);
  // use to override the image size in the ccf file.
  bool initialize(int size_x, int size_y, bool is_force_size);
  void setConfigFileName(char *name);
  // set CC pins on the card.
  void setPin(char *pinstr, int val);
  void snap();
  void grab();
  void freeze();
  void abort();
  bool isFrameAvailable(void);
  bool getFrame(void *mem_ptr);
  bool getFrame(void *mem_ptr, unsigned int *softwareGrabber_timestamp);
  bool getFrame(void *copy_memory, unsigned int *coreco_timestamp, int nbytes);

  bool isMissedFrame(void);
  void clearMissedFrames(void);
  long getTotalMissedFrames(void);
  long getRecentMissedFrames(void);
  void setNumBuffers(int b);
  void makeView(void);
  void setCamController(void *cc);
  int getWidth(void);
  int getHeight(void);

  void setDoubleWidth(int isdw);

  // clears any images left in buffer memory by resetting all counters to 0
  void resetBufferCount(void);

  int getNumFreeBuffers(void);
  int getNumBuffers(void);
  // inc missed frames counter
  void incMissedFrames(void);

 private:
  int m_size_x, m_size_y;
  int m_grab;
  int m_timestamp;
  enum { imgpixels = 6000000, nimages = 5 };

  unsigned short *images[nimages];

  int imgcnt;
};

#endif

// AncDemoRotationExpDlg.h : header file
//
#ifdef USE_SISW

#ifndef _siSoftware_H
#define _siSoftware_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time.h"

//#include "board_and_dll_chooser.h"
#include "grabberInterface.h"
#include "fgrab_struct.h"
#include "fgrab_prototyp.h"
#include "fgrab_define.h"
//#include "SapClassGui.h"

#include "logfile.h"

//#include <AncDlgLogo.h>

#ifndef WIN32

#define TRUE true
#define FALSE false

#endif

/////////////////////////////////////////////////////////////////////////////
// CAncDemoRotationExpDlg dialog

class siSoftware : public grabberInterface {
  // Construction
 public:
  siSoftware(bool is_use_fpga);  // standard constructor

  int getGrabberType();

  bool CreateObjects();
  bool DestroyObjects();
  bool DestroyObjectsNoDelete();
  void GetSignalStatus();
  void setCamController(void *cc);

  virtual bool initialize(int size_x, int size_y);
  // use to override the image size in the ccf file.
  virtual bool initialize(int size_x, int size_y, bool is_force_size);
  void setConfigFileName(char *name);
  // set CC pins on the card.
  void setPin(char *pinstr, int val);
  void snap();
  void grab();
  void freeze();
  void abort();
  bool isFrameAvailable(void);
  bool getFrame(void *mem_ptr);
  bool getFrame(void *mem_ptr, unsigned int *siSoftware_timestamp);
  bool getFrame(void *copy_memory, unsigned int *siSoftware_timestamp,
                int nbytes);

  bool isMissedFrame(void);
  void clearMissedFrames(void);
  long getTotalMissedFrames(void);
  long getRecentMissedFrames(void);
  void setNumBuffers(int b);
  void makeView(void);
  void setDoubleWidth(int isdw);

  int getWidth(void);
  int getHeight(void);

  // clears any images left in buffer memory by resetting all counters to 0
  void resetBufferCount(void){};

  int getNumFreeBuffers(void);
  int getNumBuffers(void);

  // inc missed frames counter
  void incMissedFrames(void);

  bool is_force_size;

  bool is_destroyed;

  static volatile long frames_to_cpu;
  static volatile long frame_count;
  static volatile long missed_frames;
  static volatile long recent_missed_frames;
  static volatile bool is_missed_frame;

  static volatile int sap_buffer_count;
  volatile int sensor_width;
  volatile int sensor_height;
  volatile int num_buffers;

  bool is_blocking;

  bool is_rst_server;

  bool m_IsSignalDetected;  // TRUE if camera signal is detected

  int acq_device_number;

  char camera_format_file[256];
  //    char fpga_file[256];
  char acq_server_name[256];
  char device_name[256];
  // SapView        *m_View;
  static siSoftware *mycard;

  int dispId0;
  int boardNr;
  Fg_Struct *fg;
  void *ptrMem;
  size_t totalBufferSize;
  int camPort;
  bool is_display;

  // if we grab 16 bit pix in 8 bit mode, as in edge, we have to doub width.
  // othersize we don't worry about it..
  double is_doub_width;

  int dbg_last_imgnum;

  log_file lf;

  // true of we have special siso fpga fw (dll or hapfile) for edge global shut
  bool is_edge_globalshut;
  int act_sccmos_version;

  enum { HAP_VERSION_NOISE = 12 };
};

#endif  // !defined(AFX_ANCROTATIONDEMODLG_H__82BFE149_F01E_11D1_AF74_00A0C91AC0FB__INCLUDED_)

#endif

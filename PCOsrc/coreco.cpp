// AncDemoRotationExpDlg.cpp : implementation file
//

#ifdef USE_SAP

//#include "stdafx.h"
#include <iostream>

#include "coreco.h"

#include "ccd_exception.h"

static char *g_pDesignStr = EXPANSION_RTPRO_ROTATION_STR;

volatile long coreco::frames_to_cpu = 0;
volatile long coreco::frame_count = 0;

volatile long coreco::missed_frames = 0;
volatile bool coreco::is_missed_frame = false;
volatile int coreco::sap_buffer_count = 0;
volatile long coreco::recent_missed_frames = 0;

// genCamController* coreco::cam_control=0;

coreco *coreco::mycard = 0;

/////////////////////////////////////////////////////////////////////////////
// corecoFPGA dialog

coreco::coreco(bool is_use_fpga) {
  //    m_bEnableRtPro = is_use_fpga;
  m_Xfer = NULL;
  m_Acq = NULL;
  m_Buffers = NULL;
  // m_View            = NULL;

  m_View = NULL;
  m_View2 = NULL;

  m_IsSignalDetected = TRUE;
  is_force_size = false;

  is_rst_server = false;
  // m_RtproDesign = NULL;
  // setFpgaFileName("null");
  setConfigFileName("D:/corecofiles/c1k1k16bit.ccf");

  is_double_width = false;

  coreco::frames_to_cpu = 0;
  coreco::frame_count = 0;

  coreco::missed_frames = 0;
  coreco::is_missed_frame = false;
  coreco::sap_buffer_count = 0;
  coreco::recent_missed_frames = 0;

  num_buffers = 2;
  is_destroyed = true;

  coreco::mycard = this;
}

int coreco::getGrabberType() { return ((int)gDALSA); }

void coreco::makeView(void) {
  m_View = new SapView(m_Buffers);
  m_View->Create();
}
void coreco::setNumBuffers(int b) { num_buffers = b; }

int coreco::getNumFreeBuffers(void) {
  return (sap_buffer_count - (frame_count - frames_to_cpu));
}
int coreco::getNumBuffers(void) { return (sap_buffer_count); }

bool coreco::isFrameAvailable(void) {
  if (frame_count > frames_to_cpu) return true;

  return false;
}

void coreco::setDoubleWidth(int isdw) { is_double_width = isdw; }
bool coreco::isMissedFrame(void) { return (is_missed_frame); }

void coreco::clearMissedFrames(void) {
  frames_to_cpu = frame_count;
  is_missed_frame = false;
  recent_missed_frames = 0;
}
long coreco::getTotalMissedFrames(void) { return (missed_frames); }

long coreco::getRecentMissedFrames(void) { return (recent_missed_frames); }
//
bool coreco::getFrame(void *copy_memory, unsigned int *coreco_timestamp) {
  // bool sap_result;
  // int grab_index;
  // unsigned short  *image_address;

  if (isFrameAvailable()) {
    // getIndex returns last buffer camera dumped to. not the next one to read
    // from...
    // grab_index = m_Buffers->GetIndex();
    grab_index = (grab_index + 1) % sap_buffer_count;

    sap_result = m_Buffers->GetAddress(grab_index, (void **)&image_address);
    m_Buffers->GetCounterStamp(grab_index, (int *)coreco_timestamp);

    memcpy(copy_memory, image_address,
           (sensor_height * sensor_width * sizeof(unsigned short)));
    frames_to_cpu++;
    sap_result = m_Buffers->ReleaseAddress(grab_index, (void *)image_address);
    if (sap_result == false) throw ccd_exception("release Address failed");

    if (sap_result == false) throw ccd_exception("SAP getAddress failed");
  }

  return false;
}

void coreco::resetBufferCount(void) {
  clearMissedFrames();
  m_Buffers->ResetIndex();
  grab_index = m_Buffers->GetIndex();
}

//
bool coreco::getFrame(void *copy_memory, unsigned int *coreco_timestamp,
                      int nbytes) {
  //    bool sap_result;
  //    int grab_index;
  //    unsigned short  *image_address;

  if (isFrameAvailable()) {
    // grab_index = m_Buffers->GetIndex();
    grab_index = (grab_index + 1) % sap_buffer_count;

    sap_result = m_Buffers->GetAddress(grab_index, (void **)&image_address);
    m_Buffers->GetCounterStamp(grab_index, (int *)coreco_timestamp);

    memcpy(copy_memory, image_address, nbytes);
    frames_to_cpu++;
    sap_result = m_Buffers->ReleaseAddress(grab_index, (void *)image_address);
    if (sap_result == false) throw ccd_exception("release Address failed");

    if (sap_result == false) throw ccd_exception("SAP getAddress failed");
  }

  return false;
}

bool coreco::getFrame(void *copy_memory) {
  ///    bool sap_result;
  //    int grab_index;
  //    unsigned short  *image_address;

  if (isFrameAvailable()) {
    // grab_index = m_Buffers->GetIndex();
    grab_index = (grab_index + 1) % sap_buffer_count;

    sap_result = m_Buffers->GetAddress(grab_index, (void **)&image_address);

    if (copy_memory != 0)
      memcpy(copy_memory, image_address,
             (sensor_height * sensor_width * sizeof(unsigned short)));

    frames_to_cpu++;
    sap_result = m_Buffers->ReleaseAddress(grab_index, (void *)image_address);
    if (sap_result == false) throw ccd_exception("release Address failed");

    if (sap_result == false) throw ccd_exception("SAP getAddress failed");
  }

  return false;
}
/////////////////////////////////////////////////////////////////////////////
// corecoFPGA message handlers

void coreco::XferCallback(SapXferCallbackInfo *pInfo) {
  //!!    corecoFPGA *pDlg= (corecoFPGA *) pInfo->GetContext();
  int i;

  i = 1;
  frame_count++;
  if (frame_count - frames_to_cpu > sap_buffer_count) {
    is_missed_frame = true;
    missed_frames += 1;
    recent_missed_frames += 1;
  }
  if (mycard->m_View != NULL) mycard->m_View->Show();

  // if (cam_control!= 0)
  //    cam_control->imageCallback(NULL);
}

void coreco::SignalCallback(SapAcqCallbackInfo *pInfo) {
  //    corecoFPGA *pDlg = (corecoFPGA *) pInfo->GetContext();
  //  pDlg->GetSignalStatus(pInfo->GetSignalStatus());
  // pInfo->GetSignalStatus()
  int i;

  i = 1;
}

void coreco::setCamController(void *cc) {
  // cam_control=(genCamController*)cc;
}

int coreco::getWidth(void) { return (sensor_width); }

int coreco::getHeight(void) { return (sensor_height); }

//***********************************************************************************
// Initialize Demo Dialog based application
//***********************************************************************************
bool coreco::initialize(int size_x, int size_y) {
  // if (dlg.DoModal() == IDOK)
  //{
  // Define on-line objects
  m_Acq = makeAcquision();
  //        m_Buffers    = new SapBufferWithTrash(2, m_Acq);
  m_Buffers = new SapBuffer(num_buffers, m_Acq);
  m_Buffers->SetFormat(SapFormatMono16);
  m_Buffers->SetHeight(size_y);
  m_Buffers->SetWidth(size_x);
  m_Buffers->SetType(SapBuffer::TypeContiguous);

  this->sensor_height = size_y;
  this->sensor_width = size_x;

  m_Xfer = new SapTransfer(XferCallback, this);
  //        if (m_bEnableRtPro)
  //            m_RtproDesign = NewRtproDesign( m_Acq->GetLocation());

  // Create all objects
  if (!CreateObjects()) {
    return false;
  }

  grab_index = m_Buffers->GetIndex();
  // Get current input signal connection status
  GetSignalStatus();
  m_Acq->SaveParameters("D:/corecofiles/current_params.ccf");

  return TRUE;  // return TRUE  unless you set the focus to a control
}

//***********************************************************************************
// Initialize Demo Dialog based application
//***********************************************************************************
bool coreco::initialize(int size_x, int size_y, bool is_force_size) {
  // if (dlg.DoModal() == IDOK)
  //{
  // Define on-line objects
  m_Acq = makeAcquision();

  this->is_force_size = is_force_size;

  //        m_Buffers    = new SapBufferWithTrash(2, m_Acq);
  m_Buffers = new SapBuffer(num_buffers, m_Acq);
  m_Buffers->SetFormat(SapFormatMono16);
  m_Buffers->SetHeight(size_y);
  m_Buffers->SetWidth(size_x);
  m_Buffers->SetType(SapBuffer::TypeContiguous);

  this->sensor_height = size_y;
  this->sensor_width = size_x;

  m_Xfer = new SapTransfer(XferCallback, this);
  //        if (m_bEnableRtPro)
  //            m_RtproDesign = NewRtproDesign( m_Acq->GetLocation());

  // Create all objects
  if (!CreateObjects()) {
    return false;
  }
  grab_index = m_Buffers->GetIndex();

  // Get current input signal connection status
  GetSignalStatus();

  m_Acq->SaveParameters("D:/corecofiles/current_params.ccf");

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void coreco::setConfigFileName(char *name) { strcpy(camera_format_file, name); }

SapAcquisition *coreco::makeAcquision() {
  bool sap_result;
  SapAcquisition *sap_acquisition;

  acq_device_number = 0;

  SapManager::GetServerName(1, acq_server_name, sizeof(acq_server_name));
  if (is_rst_server)
    sap_result = SapManager::ResetServer(acq_server_name, NULL, NULL);

  SapManager::GetServerName(1, acq_server_name, sizeof(acq_server_name));
  SapManager::GetResourceName(acq_server_name, SapManager::ResourceAcq,
                              acq_device_number, device_name,
                              sizeof(device_name));

  sap_location = new SapLocation(acq_server_name, acq_device_number);

  // if (sap_acquisition != NULL)
  //    sap_acquisition->Destroy ();

  sap_acquisition = new SapAcquisition(*sap_location);

  sap_result = sap_acquisition->SetConfigFile(camera_format_file);
  // sap_result = sap_acquisition->Create ();

  return (sap_acquisition);
}

bool coreco::CreateObjects() {
  is_destroyed = false;

  // add transfer paths here
  if (m_Xfer) {
    // connect the acq directly to the host.
    SapXferPair acqToHostPair(m_Acq, m_Buffers, TRUE);
    acqToHostPair.SetCallbackInfo(XferCallback, this);
    m_Xfer->AddPair(acqToHostPair);
  }

  // Create acquisition object
  if (m_Acq && !*m_Acq && !m_Acq->Create()) {
    DestroyObjects();
    // throw ccd_exception("coreco::CreateObjects could not create Acq");
    return FALSE;
  }

  if (is_force_size) {
    int w2;
    int ntaps;

    w2 = sensor_width;

    m_Acq->GetParameter(CORACQ_PRM_TAPS, (void *)(&ntaps));

    // we mult by 2 because we have 8bit depth instead of 16bit...
    // real pixels are 16bit but grabber assumes 8bit.
    if (ntaps == 10) w2 = w2 * 2;

    m_Acq->SetParameter(CORACQ_PRM_CROP_HEIGHT, sensor_height, false);
    m_Acq->SetParameter(CORACQ_PRM_CROP_WIDTH, w2, false);

    m_Acq->SetParameter(CORACQ_PRM_SCALE_VERT, sensor_height, false);
    m_Acq->SetParameter(CORACQ_PRM_SCALE_HORZ, w2, false);

    m_Acq->SetParameter(CORACQ_PRM_VACTIVE, sensor_height, false);

    m_Acq->SetParameter(CORACQ_PRM_HACTIVE, w2 / ntaps, true);
  }

  // Create buffer object
  if (m_Buffers && !*m_Buffers && !m_Buffers->Create()) {
    char mm[128];
    DestroyObjects();
    sprintf(mm, "coreco::CreateObjects could not create SapBuffers %i %i ",
            sensor_height, sensor_width);
    ///  throw ccd_exception(mm);
    return FALSE;
  }

  // Create view object

  // Create transfer object
  if (m_Xfer != NULL && !*m_Xfer && !m_Xfer->Create()) {
    DestroyObjects();
    // throw ccd_exception("coreco::CreateObjects could not create Xfer");

    return FALSE;
  }

  if (m_Xfer && *m_Xfer) {
    m_Xfer->SetAutoEmpty(FALSE);
  }

  sap_buffer_count = m_Buffers->GetCount();
  sensor_width = m_Buffers->GetWidth();
  sensor_height = m_Buffers->GetHeight();

  return TRUE;
}

bool coreco::DestroyObjects() {
  if (is_destroyed) return (true);

  if (m_Xfer) {
    if (*m_Xfer) {
      m_Xfer->Destroy();
    }
    m_Xfer->RemoveAllPairs();
  }

  // Destroy view object
  // if (m_View && *m_View) m_View->Destroy();

  // Destroy buffer object
  if (m_Buffers && *m_Buffers) m_Buffers->Destroy();

  // Destroy acquisition object
  if (m_Acq && *m_Acq) m_Acq->Destroy();

  // Delete all objects
  if (m_Xfer) delete m_Xfer;
  // if (m_ImageWnd)     delete m_ImageWnd;
  //    if (m_View)             delete m_View;
  if (m_Buffers) delete m_Buffers;
  if (m_Acq) delete m_Acq;
  is_destroyed = true;
  return TRUE;
}

bool coreco::DestroyObjectsNoDelete() {
  if (m_Xfer) {
    if (*m_Xfer) {
      m_Xfer->Destroy();
    }
    m_Xfer->RemoveAllPairs();
  }

  // destroy rtpro design

  // Destroy view object
  // if (m_View && *m_View) m_View->Destroy();

  // Destroy buffer object
  if (m_Buffers && *m_Buffers) m_Buffers->Destroy();

  // Destroy acquisition object
  if (m_Acq && *m_Acq) m_Acq->Destroy();

  // Delete all objects

  return TRUE;
}

//*****************************************************************************************
//
//                    Acquisition Control
//
//*****************************************************************************************

void coreco::abort() {
  if (m_Xfer) {
    m_Xfer->Abort();
  }

  // UpdateMenu();
}

void coreco::freeze() {
  if (m_Xfer) {
    m_Xfer->Freeze();
  }

  //    UpdateMenu();
}

void coreco::grab() {
  if (m_Xfer) {
    //    m_statusWnd.SetWindowText("");
    m_Xfer->Grab();
  }

  // UpdateMenu();
}

void coreco::snap() {
  // m_statusWnd.SetWindowText("");
  if (m_Xfer) {
    m_Xfer->Snap();

    //     if (CAbortDlg(this, m_Xfer).DoModal() != IDOK)
    //       m_Xfer->Abort();
  }

  // UpdateMenu();
}

// inc missed frames counter
void coreco::incMissedFrames(void) {
  missed_frames++;
  recent_missed_frames++;
}

//*****************************************************************************************
//
//                    Acquisition Options
//
//*****************************************************************************************

//*****************************************************************************************
//
//                    File Options
//
//*****************************************************************************************

//**************************************************************************************
//
//            Processing Options
//
//**************************************************************************************

void coreco::GetSignalStatus() {
  SapAcquisition::SignalStatus signalStatus;

  if (m_Acq && m_Acq->IsSignalStatusAvailable()) {
    if (m_Acq->GetSignalStatus(&signalStatus, SignalCallback, this))
      GetSignalStatus(signalStatus);
  }
}

void coreco::GetSignalStatus(SapAcquisition::SignalStatus signalStatus) {
  m_IsSignalDetected = (signalStatus != SapAcquisition::SignalNone);
}

void coreco::setPin(char *pinstr, int val) {
  int status, status2;
// char msg[255];
// char errbuff[1024];
// char infobuff[1024];

//    coreco_log.log("Doing Reset in Coreco Card");

#if USE_SAP

  hAcq = m_Acq->GetHandle(0);
  // sprintf(msg,"sap_acquisition->GetHandle %p",hAcq);
  // coreco_log.log(msg);

  //;instr  = "CC4" val is 0 or 1
  status = CorAcqSetCamIoControl(hAcq, pinstr, val);
// CorManGetStatusText(status,errbuff,1023,infobuff,1023);
// sprintf(msg,"CorAcqSetCamIoControl %i\d",status);
// coreco_log.log(msg);
// coreco_log.log(errbuff);
// coreco_log.log(infobuff);
#endif
}

#endif

// softwareCam.cpp : Defines the entry point for the DLL application.
//

//#include "stdafx.h"

#include "pco.h"
#include <stdio.h>

#ifdef USE_SISW
#include "siswSerialPort.h"

#endif

// using namespace genCamControllerEnum;

#define _LIMITIMGSIZE
//!!#define _LIMITIMGSIZE600

/** This thread controls acquisition, reads TIFF files to get the image data,
 * and
 * does the callbacks to send it to higher layers */
void pco::pcoTask() {
  char mesx[256];

  lf.log("PCO Serial Thread started up");

  while (true) {
    epicsThreadSleep(0.1);
    if (getIntParam(pco_is_running) > 0) {
      // release serial mutex in case some exception caused us not to release
      // properly...
      /// if we never owned it, then it just errors.. who cares.
      // releaseSerialMutex();
      // releaseGrabberMutex();

      setIntegerParam(pco_run_counter, getIntParam(pco_run_counter) + 1);
      callParamCallbacks();
      if ((getIntParam(is_com_open) == 1) && (getIntParam(ADAcquire) == 1) &&
          (time1.isElapsed((double)getIntParam(pco_check_time) / 1000.0))) {
        grabSerialMutex();
        getPcoStatusParams();
        releaseSerialMutex();

        // dump memory stats on the driver... nd array use:
        sprintf(mesx, "cor_nd_datasize %d", getIntParam(cor_nd_datasize));
        lf.log(mesx);
        sprintf(mesx, "cor_max_ndbuffers %d", getIntParam(cor_max_ndbuffers));
        lf.log(mesx);
        sprintf(mesx, "cor_num_ndbuffers %d", getIntParam(cor_num_ndbuffers));
        lf.log(mesx);
        sprintf(mesx, "cor_max_ndmemory %d", getIntParam(cor_max_ndmemory));
        lf.log(mesx);
        sprintf(mesx, "cor_alloc_ndmemory %d", getIntParam(cor_alloc_ndmemory));
        lf.log(mesx);
        sprintf(mesx, "cor_free_ndbuffers %d", getIntParam(cor_free_ndbuffers));
        lf.log(mesx);
        sprintf(mesx, "cor_est_buffers_left %d",
                getIntParam(cor_est_buffers_left));
        lf.log(mesx);

        sprintf(mesx, "total_missed_frames %d",
                getIntParam(total_missed_frames));
        lf.log(mesx);
        sprintf(mesx, "recent_missed_frames %d",
                getIntParam(recent_missed_frames));
        lf.log(mesx);

        sprintf(mesx, "cor_cant_get_ndarray %d",
                getIntParam(cor_cant_get_ndarray));
        lf.log(mesx);

        time1.tic();
        callParamCallbacks();
      }

      if ((getIntParam(is_com_open) == 1) && (getIntParam(ADAcquire) == 0) &&
          time2.isElapsed((double)getIntParam(pco_check_time2) / 1000.0))

      {
        // lf.log("Checking all Params on Camera");
        // we assume we have recursive mutex..
        // epicmmutex is recursive according to docs
        grabSerialMutex();
        // lf.log("pcotasx get mutex");
        // epicsMutexShowAll(1,1);
        try {
          setIntegerParam(pco_force_check, 0);
          getPcoStatusParams();
          getPcoGeneralParams();
          getPcoSensorParams();
          getPcoTimingParams();
          getPcoStorageParams();
          getPcoRecordingParams();
          getPcoImageReadParams();
          getPcoCameraLinkParams();
        } catch (...) {
          lf.log("pco task threw exception...");
        }
        releaseSerialMutex();
        // lf.log("pcotasx get mutex");
        // epicsMutexShowAll(1,1);
        time2.tic();
        callParamCallbacks();
      }

      if ((getIntParam(pco_dump_camera_memory) == 1) &&
          (getIntParam(ADAcquire) == 0)) {
        grabSerialMutex();
        dumpCameraMemory();
        // setIntegerParam(ADAcquire,0);
        // doHighLevelParams(ADAcquire);
        releaseSerialMutex();

      }  // if dump mem

    }  // if is rnning
  }  // while true
}

/** This thread controls acquisition, reads TIFF files to get the image data,
 * and
 * does the callbacks to send it to higher layers */
void pco::pcoTask2() {}

void pco::clearPipe(FILE *fp) {
  int ret;
  int counter = 0;
  ret = fgetc(fp);
  while (ret != EOF) {
    counter++;
    if (counter > 100) break;
  }
}

int pco::updateParameters(asynUser *pasynUser, epicsInt32 ivalue,
                          epicsFloat64 dvalue, int paramtype) {
  int err;

  int function = pasynUser->reason;

  // lf.log("PCO entering main function");

  doSerialTransactions(function, ivalue, dvalue, paramtype);

  // if we got some excaption it is possuible we still own the serial mutex...
  // so we release.
  // if we never oened it, then it just errors... who cares.
  // releaseSerialMutex();
  // releaseGrabberMutex();

  setIntegerParam(pco_force_check, 1);

  callParamCallbacks();

  // lf.log("PCOSerCam leaving main funtionc");

  return (0);
}

int pco::doSerialTransactions(int function, int ivalue, double dvalue,
                              int paramtype) {
  char mesg[128];
  int err;

  try {
    lf.enableLog(getIntParam(pco_is_log));

    // setIntegerParam(pco_run_counter, getIntParam(pco_run_counter) + 1);

    // call back to client so it can do somehing like update params in EPICS
    // etc.
    // my_addriver->callback(general, this);

    if (function == open_com && ivalue == 1) {
      setIntegerParam(open_com, 0);

      lf.log("Attempt to open PCO Camera");
      grabSerialMutex();
      OpenCamera();
      releaseSerialMutex();
      setIntegerParam(pco_force_check, 1);
    }

    if (function == pco_comport_number) {
      char cpstr[32];

      sprintf(cpstr, "COM%d", getIntParam(pco_comport_number));

      setStringParam(com_port_name, cpstr);
    }

    if (function == close_com && getIntParam(close_com) == 1) {
      setIntegerParam(is_com_open, 0);
      setIntegerParam(close_com, 0);
      grabSerialMutex();

#ifndef USEASYNSERIAL
      serial_port->close();
#else
      asynSetOption(myServerPort, 0, "open", "close");
#endif

      releaseSerialMutex();
    }

    if (function == pco_dbg_serwrite || function == pco_dbg_serread) {
      dbgSerial();
    }
    if (function == pco_reconfig_grabber) {
      //!! need to actuallty reconfig here!!!
      setIntegerParam(pco_reconfig_grabber, 0);

      grabSerialMutex();
      reconfigGrabber();
      releaseSerialMutex();
    }

    //			setIntegerParam(size_x,getIntParam(ADSizeX));
    //			setIntegerParam(size_y,getIntParam(ADSizeY));
    //			setIntegerParam(frame_delay,getIntParam(ADAcquirePeriod));
    if (getIntParam(is_com_open)) {
      int stacode;

      // function = function;

      // stacode=checkCameraMessages();
      // while(stacode==0)
      //	 stacode=checkCameraMessages();
      grabSerialMutex();
      // lf.log("doSerialTransactions got mutex");
      // epicsMutexShowAll(1,1);

      try {
        setPcoBaudrate(function);
        doHighLevelParams(function);
        setPcoGeneralParams(function);
        setPcoSensorParams(function);
        setPcoTimingParams(function);
        // setPcoStorageParams();  deletes memory--- let us do this when we acq.
        setPcoRecordingParams(function);
        setPcoImageReadParams(function);
        setPcoCameraLinkParams(function);
      } catch (...) {
        lf.log("doSeriaTrans threw exception");
      }
      releaseSerialMutex();
      // lf.log("doSerialTransactions release mutex");
      // epicsMutexShowAll(1,1);
      //!! put this into a task
      // setPcoBaudrate();

      //					 if (
      //(getIntParam(ADAcquire)==0) &&
      //					 ((getIntParam(pco_force_check)==1)
      //||
      //					 (time2.isElapsed((double)getIntParam(pco_check_time2)/1000.0))))
      if (((getIntParam(ADAcquire) == 0) &&
           (getIntParam(pco_force_check) == 1)) ||
          function == pco_force_check) {
        // lf.log("Checking all Params on Camera");

        setIntegerParam(pco_force_check, 0);

        grabSerialMutex();
        // lf.log("doSerialTransactions get mutex");
        // epicsMutexShowAll(1,1);
        try {
          getPcoGeneralParams();
          getPcoSensorParams();
          getPcoTimingParams();
          getPcoStorageParams();
          getPcoRecordingParams();
          getPcoImageReadParams();
          getPcoCameraLinkParams();
          time2.tic();

        } catch (...) {
          lf.log("doSeriaTrans threw exception");
        }
        releaseSerialMutex();

        // lf.log("doSerialTransactions rel mutex");
        // epicsMutexShowAll(1,1);
      }

    } else
      setIntegerParam(ADStatus, ADStatusError);

  }  // try
  catch (ccd_exception err) {
    lf.log(err.err_mess());

    setIntegerParam(ADStatus, ADStatusError);

  }  // catch
  return (0);
}  // main

//
//
// We assume we have Serial Mutex when we enter this function
// we must release serial mutex to mess w./ grabber so we only have one mutex at
// a time
// then we must regrab serial mutex
//
void pco::reconfigGrabber(void) {
  setIntegerParam(pco_reconfig_grabber, 0);

  // make sure acquire is OFF.

  setIntegerParam(ADAcquire, 0);

  doHighLevelParams(ADAcquire);

  setIntegerParam(ADStatus, ADStatusWaiting);
  // my_addriver->callback(general, this);
  lf.log("Setting up Grabber");
// grabSerialMutex();
#ifndef USEASYNSERIAL
  serial_port->close();
#else
  asynSetOption(myServerPort, 0, "open", "close");
#endif
  // releaseSerialMutex();
  setIntegerParam(is_com_open, 0);

  if (getIntParam(pco_which_camera) == pco_edge) {
    // setStringParam(ccf_filename,"D:/corecofiles/P_Edge_5120_2160_.ccf");
    setIntegerParam(is_force_img_size, 1);
    setIntegerParam(is_mult_width2, 1);
    if (getIntParam(pco_edge_fastscan) == 2) setIntegerParam(is_mult_width2, 2);

  } else {
    // setStringParam(ccf_filename,"D:/corecofiles/dimax2k2k.ccf");
    setIntegerParam(is_force_img_size, 1);
    setIntegerParam(is_mult_width2, 0);
  }

  //!! need to load ccf here

  setIntegerParam(is_loadccf, 1);
  // we will grab BOTH serial and grabber mutex

  grabberSetup(is_loadccf);

  setIntegerParam(is_grab, 1);

  grabberSetup(is_grab);

  setIntegerParam(ADStatus, ADStatusIdle);
  // my_addriver->callback(general, this);
  // setIntegerParam(open_com,1);

  lf.log("Attempt to open PCO Camera");
  //	grabSerialMutex();
  OpenCamera();

  //!! must open here
  setIntegerParam(pco_reset_memory, 1);
  //!! must do here
  setIntegerParam(pco_force_check, 1);
  //!! must do here

  setIntegerParam(ADStatus, ADStatusIdle);
}

void pco::dbgSerial(void) {
  char serstr[256];
  char textstr[512];
  char *msg;
  int k, m;
  char bytestr[16];
  int len;

  grabSerialMutex();

  if (getIntParam(pco_dbg_serwrite) > 0) {
    for (k = 0; k < 512; k++) textstr[k] = 0;

    msg = getStringParam(pco_dbg_serstr);
    len = strlen(msg);
    // format is 00ffef23a2, all hex nibbles- take byte at a time and comvert to
    // real bivnary codes
    m = 0;
    for (k = 0; k < len; k = k + 2) {
      bytestr[0] = msg[k];
      bytestr[1] = msg[k + 1];
      bytestr[2] = 0;
      sscanf(bytestr, "%x", &serstr[m]);
      m++;
    }
    for (k = 0; k < getIntParam(pco_dbg_serwrite); k++) {
      printf("%02x", serstr[k]);
    }
    printf("\n");

#ifndef USEASYNSERIAL
    serial_port->write((unsigned char *)serstr, getIntParam(pco_dbg_serwrite));
#else
    pasynOctetSyncIO->write(pasynUserSerial, (const char *)serstr,
                            getIntParam(pco_dbg_serwrite), 0, &mynwrite);
#endif
  } else if (getIntParam(pco_dbg_serread) > 0) {
    for (k = 0; k < 256; k++) serstr[k] = 0;

    for (k = 0; k < 512; k++) textstr[k] = 0;

#ifndef USEASYNSERIAL
// serial_port->read((unsigned char*)serstr,getIntParam(pco_dbg_serread));
#else
    pasynOctetSyncIO->read(pasynUserSerial, (char *)serstr,
                           getIntParam(pco_dbg_serread), 0, &mynread,
                           &myeomreason);
#endif

    m = 0;
    for (k = 0; k < getIntParam(pco_dbg_serread); k++) {
      sprintf(bytestr, "%02x", serstr[k]);
      textstr[m] = bytestr[0];
      textstr[m + 1] = bytestr[1];
      m = m + 2;
    }
    printf("%s\n", textstr);
    // setStringParam(pco_dbg_serstr,textstr);
  }

  setIntegerParam(pco_dbg_serwrite, 0);
  setIntegerParam(pco_dbg_serread, 0);

  releaseSerialMutex();
}

int pco::doSerialCommand(pco_command &cmd, pco_response &rsp,
                         unsigned char *obj, int len) {
  unsigned short code;
  unsigned short length;
  unsigned char code_hi;
  unsigned char code_lo;
  char mesgx[128];
  bool is_print = false;
  unsigned char *xptr;
  int k;

  // grabSerialMutex();
  try {
    // Sleep(250);
    code = cmd.getCode();
    //	code_lo = (unsigned char)(code&255);
    //	code_hi = (unsigned char)((code&(255*256)) / 256);
    length = cmd.getLen();

    if (getIntParam(pco_dbg_serprint) == code) is_print = true;

// send the command
#ifndef USEASYNSERIAL
    serial_port->write((unsigned char *)&code, sizeof(unsigned short));
#else
    pasynOctetSyncIO->write(pasynUserSerial, (const char *)&code,
                            sizeof(unsigned short), 0, &mynwrite);

#endif

    if (is_print) printf("Write: %04x", code);

    if (getIntParam(pco_ser_waitms) > 0)
      epicsThreadSleep((double)(getIntParam(pco_ser_waitms)) / 1000.0);

// send all the data in the command...
// need length -2 because getData has no codehi and codelo, but length includes
// it...
#ifndef USEASYNSERIAL
    serial_port->write(cmd.getData(), length - 2);
#else
    pasynOctetSyncIO->write(pasynUserSerial, (const char *)cmd.getData(),
                            length - 2, 0, &mynwrite);
#endif

    if (is_print) {
      xptr = cmd.getData();
      for (k = 0; k < length - 2; k++) {
        printf("%02x", *(xptr + k));
      }

      printf("\n\n");
    }
#ifndef USEASYNSERIAL
    serial_port->flush();
#else
    pasynOctetSyncIO->flush(pasynUserSerial);
#endif

// wait for the camera to do something...
// Sleep(250);

#ifndef USEASYNSERIAL
    serial_port->read((unsigned char *)&code, sizeof(unsigned short));
#else

    pasynOctetSyncIO->read(pasynUserSerial, (char *)&code,
                           sizeof(unsigned short), 0, &mynread, &myeomreason);
#endif

    if (is_print) printf("read: %04x", code);
    //	code=((unsigned short)code_hi)*256 + (unsigned short)code_lo;
    rsp.setCode(code);
#ifndef USEASYNSERIAL
    serial_port->read((unsigned char *)&length, sizeof(unsigned short));
#else
    pasynOctetSyncIO->read(pasynUserSerial, (char *)&length,
                           sizeof(unsigned short), 0, &mynread, &myeomreason);
#endif

    rsp.setLength(length);
    // first word of data is the length, which is already read in...
    // lenth-4 to not count code and length word.
    // getData2 to give pointer AFTER length word.
    if (is_print) printf("%04x", length);

    if (length > 256) {
      sprintf(mesgx,
              "Length Too Long:\n Command: %x Response: %x Rsp Length: %d\n",
              cmd.getCode(), rsp.getCode(), length);
      lf.log(mesgx);

      lf.log("Close/OPen port");

#ifndef USEASYNSERIAL
      serial_port->close();
#else
      asynSetOption(myServerPort, 0, "open", "close");
#endif

      // because openCamera grabs serial mutex, we release it here so we dont
      // grab twite
      // releaseSerialMutex();
      OpenCamera();
      // grabSerialMutex();
      com_error_counter++;

      if (com_error_counter > getIntParam(pco_com_err_max)) {
#ifndef USEASYNSERIAL
        serial_port->close();
#else
        asynSetOption(myServerPort, 0, "open", "close");
#endif
        setIntegerParam(is_com_open, 0);
        throw ccd_exception("Lost Link to Camera");
      }
      // releaseSerialMutex();

      return (1);
    } else {
#ifndef USEASYNSERIAL
      serial_port->read(rsp.getData2(), length - 4);
#else
      pasynOctetSyncIO->read(pasynUserSerial, (char *)rsp.getData2(),
                             length - 4, 0, &mynread, &myeomreason);

#endif

      if (is_print) {
        xptr = rsp.getData2();
        for (k = 0; k < length - 4; k++) {
          printf("%02x", *(xptr + k));
        }

        printf("\n\n");
      }
    }

    if (rsp.verifyCheckSum()) {
      ReportError(cmd.getCode(), "CheckSum/Com Error, Clear pipe");

#ifndef USEASYNSERIAL
      serial_port->clearPipe();
#else
      asynSetOption(myServerPort, 0, "clearPipe", "yes");
#endif
      // lf.log("Wait 1s Timeout");
      // Sleep(1000);
      com_error_counter++;

      if (com_error_counter > getIntParam(pco_com_err_max)) {
#ifndef USEASYNSERIAL
        serial_port->close();
#else
        asynSetOption(myServerPort, 0, "open", "close");
#endif

        setIntegerParam(is_com_open, 0);
        throw ccd_exception("Lost Link to Camera");
      }

      // releaseSerialMutex();
      return (1);
    }

    if (rsp.getCode() == rsp.getErrCode()) {
      ReportError(rsp.getCode(), "PCO Returned Error Code ");
      sprintf(mesgx, "Command: %x Response: %x Error: %x\n", cmd.getCode(),
              rsp.getCode(), rsp.getULong(2));
      lf.log(mesgx);
      com_error_counter = 0;
      // releaseSerialMutex();
      return (2);

    } else if (rsp.getCode() != rsp.getExpCode()) {
      ReportError(rsp.getCode(), "PCO Returned Unknown Error Code ");

      lf.log("Close Open port");

#ifndef USEASYNSERIAL
      serial_port->close();
#else
      asynSetOption(myServerPort, 0, "open", "close");
#endif
      OpenCamera();

      com_error_counter++;

      if (com_error_counter > getIntParam(pco_com_err_max)) {
#ifndef USEASYNSERIAL
        serial_port->close();
#else
        asynSetOption(myServerPort, 0, "open", "close");
#endif
        setIntegerParam(is_com_open, 0);
        throw ccd_exception("Lost Link to Camera");
      }
      // releaseSerialMutex();
      return (3);
    } else {
      com_error_counter = 0;
      if (obj > 0) {
        rsp.copy2Obj(obj, len);
      }
    }
    com_error_counter = 0;
    // releaseSerialMutex();
    return (0);

  } catch (ccd_exception err) {
    lf.log(err.err_mess());
  } catch (...) {
    lf.log("Serial Port threw exceptiuon");
  }

  // releaseSerialMutex();
  return (8);
}

/*****************************************************************************
 *
 *
 *****************************************************************************/

int pco::checkCameraMessages(void) {
  pco_response rsp;
  unsigned short code;
  unsigned short length;
  unsigned char code_hi;
  unsigned char code_lo;
  char mesgx[128];

  try {
#ifndef USEASYNSERIAL
    serial_port->read((unsigned char *)&code, sizeof(unsigned short));
#else
    pasynOctetSyncIO->read(pasynUserSerial, (char *)&code,
                           sizeof(unsigned short), 0, &mynread, &myeomreason);

#endif

  } catch (...) {
    return (6);
  }
  //	code=((unsigned short)code_hi)*256 + (unsigned short)code_lo;
  rsp.setCode(code);
#ifndef USEASYNSERIAL
  serial_port->read((unsigned char *)&length, sizeof(unsigned short));
#else
  pasynOctetSyncIO->read(pasynUserSerial, (char *)&length,
                         sizeof(unsigned short), 0, &mynread, &myeomreason);

#endif

  rsp.setLength(length);
  // first word of data is the length, which is already read in...
  // lenth-4 to not count code and length word.
  // getData2 to give pointer AFTER length word.

  if (length > 256) {
    sprintf(mesgx,
            "checkCameraMessages: Length Too Long:\n  Response: %x Rsp Length: "
            "%d\n",
            rsp.getCode(), length);
    lf.log(mesgx);
#ifndef USEASYNSERIAL
    serial_port->clearPipe();
#else

    asynSetOption(myServerPort, 0, "clearpipe", "yes");
#endif

    return (1);
  } else {
#ifndef USEASYNSERIAL
    serial_port->read(rsp.getData2(), length - 4);
#else
    pasynOctetSyncIO->read(pasynUserSerial, (char *)rsp.getData2(), length - 4,
                           0, &mynread, &myeomreason);
#endif
  }

  lf.log("Got Cam Stat message");
  rsp.sprintHeader(mesgx);
  lf.log(mesgx);

  if (rsp.verifyCheckSum()) {
    ReportError(rsp.getCode(), "checkCameraMessages: CheckSum/Com Error");
    return (1);
  }

  return (0);
}

/*****************************************************************************
 *
 * Load DLL for PCO Cam. copied from PCO example...
 *
 *****************************************************************************/

int pco::getlib(void) { return FALSE; }

/*****************************************************************************
 *
 * Load DLL for PCO Cam. copied from PCO example...
 *
 *****************************************************************************/

int pco::ReportError(int code, char *usrmsg) {
  char mesgx[256];
  char errx[128];
  // PCO_GetErrorText(code, errx, 127);
  // sprintf(mesgx,"Disconnected: %s %d  %s\n",usrmsg,code,errx);
  sprintf(mesgx, "ReportError: %s %d \n", usrmsg, code);
  lf.log(mesgx);

  return (0);
}

/*****************************************************************************
 *
 * Load DLL for PCO Cam. copied from PCO example...
 *
 *****************************************************************************/

int pco::Disconnected(int code, char *usrmsg) {
  ReportError(code, usrmsg);
  lf.log("Closing Serial Port");

#ifndef USEASYNSERIAL
  serial_port->close();
#else
  asynSetOption(myServerPort, 0, "open", "close");
#endif

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::OpenCamera(void) {
  pco_command cmd;
  pco_response rsp;

// grabSerialMutex();

#ifndef USEASYNSERIAL
  serial_port->setPortName(getStringParam(com_port_name));
  serial_port->open(getIntParam(pco_baudrate), 0, 8, 0);
#else
  asynSetOption(myServerPort, 0, "name", getStringParam(com_port_name));

  char msg[64];
  sprintf(msg, "%d", getIntParam(pco_baudrate));
  asynSetOption(myServerPort, 0, "baud", msg);
  asynSetOption(myServerPort, 0, "parity", "none");
  asynSetOption(myServerPort, 0, "bits", "8");
  asynSetOption(myServerPort, 0, "stop", "0");
  asynSetOption(myServerPort, 0, "open", "open");
#endif

  setIntegerParam(is_com_open, 1);

  //  releaseSerialMutex();
  return 0;
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoImageReadParams(int function) {
  pco_command cmd;
  pco_response rsp;

  if (function == pco_play_slow || function == pco_play_stop ||
      function == pco_setallparams) {
    // setIntegerParam(pco_play_slow,0);

    lf.log("Play images");
    // Read Images from Segment (Recorder Mode only)
    cmd.setCode(0x0B15);
    cmd.addUShort(getIntParam(pco_read_which_seg));
    cmd.addUShort(1);
    if (getIntParam(pco_play_slow) == 1)
      cmd.addUShort(3);  // mode
    else if (getIntParam(pco_play_stop) == 1)
      cmd.addUShort(0);  // mode
    else {
      cmd.addUShort(0);  // mode
      lf.log("ERROR");
    }

    cmd.addUShort(1);  // speed

    cmd.addULong(getIntParam(pco_read_st_img));
    cmd.addULong(getIntParam(pco_read_end_img));
    cmd.addULong(getIntParam(pco_read_st_img));

    rsp.setExpCode(0x0B95);
    rsp.setErrCode(0x0BD5);
    setIntegerParam(pco_play_stop, 0);
    setIntegerParam(pco_play_slow, 0);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      lf.log("Play Started");

    } else {
      lf.log("Could not start play");
    }
  }

  if (function == pco_do_read_imgs || function == pco_setallparams) {
    setIntegerParam(pco_do_read_imgs, 0);

    // Read Images from Segment (Recorder Mode only)
    cmd.setCode(0x0515);
    cmd.addUShort(getIntParam(pco_read_which_seg));
    cmd.addULong(getIntParam(pco_read_st_img));
    cmd.addULong(getIntParam(pco_read_end_img));
    rsp.setExpCode(0x0595);
    rsp.setErrCode(0x05D5);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_read_which_seg, rsp.getUShort(2));
      setIntegerParam(pco_read_st_img, rsp.getULong(4));
      setIntegerParam(pco_read_end_img, rsp.getULong(8));
      // notify IOC to update the PVs to reflect in epics DB
      // my_addriver->callback(general, this);
    }
  }

  // Request Image
  if (function == pco_req_img || function == pco_setallparams) {
    setIntegerParam(pco_req_img, 0);

    cmd.setCode(0x0615);
    rsp.setExpCode(0x0695);
    rsp.setErrCode(0x06D5);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  // Reeat Image
  if (function == pco_rpt_img || function == pco_setallparams) {
    setIntegerParam(pco_rpt_img, 0);

    cmd.setCode(0x0815);
    cmd.addShort(0);
    cmd.addShort(0);
    cmd.addShort(0);
    rsp.setExpCode(0x0895);
    rsp.setErrCode(0x08D5);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  //!! image dump must be on a task so we can cancel it

  // Cancel Image Transfer
  if (function == pco_cancel_img || function == pco_setallparams) {
    setIntegerParam(pco_cancel_img, 0);

    cmd.setCode(0x0715);
    cmd.addShort(0);
    rsp.setExpCode(0x0795);
    rsp.setErrCode(0x07D5);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  // Set Bit Alignment
  if (function == pco_bit_alignment || function == pco_setallparams) {
    cmd.setCode(0x0A15);
    cmd.addShort(getIntParam(pco_bit_alignment));
    rsp.setExpCode(0x0A95);
    rsp.setErrCode(0x0AD5);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_bit_alignment, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/
int pco::getPcoStatusParams(void) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;
  double prd_sec;

  // Get Number Of Images in Segment 0
  if (getIntParam(pco_which_camera) == pco_dimax) {
    cmd.setCode(0x0215);
    cmd.addUShort(1);
    rsp.setExpCode(0x0295);
    rsp.setErrCode(0x02D5);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_num_imgs_seg0, rsp.getULong(4));
      setIntegerParam(pco_max_imgs_seg0, rsp.getULong(8));
    }
  }

  // Get Recording Status
  cmd.setCode(0x0514);
  rsp.setExpCode(0x0594);
  rsp.setErrCode(0x05D4);
  if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
    setIntegerParam(pco_rec_status, rsp.getUShort(2));

    if (getIntParam(pco_rec_status) == 0) {
      // if is_allow_kill_acquire is true, then recording was started, and now
      // its done.
      // if is_allow_kill_acquire is false, then there was no recording, so
      // leave ADAcquire alone in case
      // someone just set it to 1, requestig a new recording. this aviods race
      // condition.
      if (is_allow_kill_acquire && getIntParam(pco_which_camera) == pco_dimax) {
        setIntegerParam(ADAcquire, 0);
        doHighLevelParams(ADAcquire);
        is_allow_kill_acquire = false;
        lf.log("sensed Recording OFF, so shut Acquire");
      }

    } else {
      setIntegerParam(ADStatus, ADStatusAcquire);
      // case where ACquire is 0 but cam is recording
      if (getIntParam(ADAcquire) == 0) {
        lf.log("Acqiure OFF, but Recording ON, so stop Camera");
        doHighLevelParams(ADAcquire);
        is_allow_kill_acquire = false;
      }
    }
  }

  // Get Camera Busy status
  cmd.setCode(0x0612);
  rsp.setErrCode(0x06D2);
  rsp.setExpCode(0x0692);
  if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
    setIntegerParam(pco_busy, rsp.getUShort(2));
  }

  getPcoTimingParams();

  // Get Camera Health Status
  cmd.setCode(0x0210);
  rsp.setErrCode(0x02D0);
  rsp.setExpCode(0x0290);

  if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
    lval1 = rsp.getULong(2);
    setIntegerParam(pco_health_warning, lval1);

    if (lval1 != 0) {
      sprintf(msgx, "Warning from PCO: %d\n", lval1);
      lf.log(msgx);
      setIntegerParam(ADStatus, ADStatusError);
    }

    lval1 = rsp.getULong(6);
    setIntegerParam(pco_health_error, lval1);

    if (lval1 != 0) {
      sprintf(msgx, "Error from PCO: %d\n", lval1);
      lf.log(msgx);
      setIntegerParam(ADStatus, ADStatusError);
    }

    lval1 = rsp.getULong(10);
    setIntegerParam(pco_health_status, lval1);
  }

#if 0
//!! this is completely messed up!!!


//get frame fate from camera
	if (getIntParam(pco_rec_status)==1)
	{
  cmd.setCode(0x1012);
   cmd.addUShort(1);
  rsp.setExpCode(0x1092);
  rsp.setErrCode(0x10D2);
  if (doSerialCommand(cmd,rsp,0,0)==0)
  {
	prd_sec = (double)(rsp.getULong(2));
	prd_sec += (double)(rsp.getULong(6)) * 1e-9;
	if (prd_sec>0.0)
		setDoubleParam(pco_frame_rate,1.0/prd_sec);

  }
	}
	else
	{
		setDoubleParam(pco_frame_rate,0.0);
	}
#endif

  // notify IOC to update the PVs to reflect in epics DB
  //	my_addriver->callback(general, this);
  // clear changes to prevent constant update

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::getPcoImageReadParams(void) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;
  int k;

  if (getIntParam(pco_which_camera) != pco_dimax) return (0);

  for (k = 1; k <= 4; k++) {
    // Get Segment Image Settings
    cmd.setCode(0x0115);
    cmd.addUShort(k);  // number of segment 0 to 3?
    rsp.setExpCode(0x0195);
    rsp.setErrCode(0x01D5);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      sprintf(msgx,
              "Seg 0x%x ResHV %d,%d BinXY (%d , %d) ROIXY0 (%d,%d) ROI1XY "
              "(%d,%d) \n",
              rsp.getUShort(2), rsp.getUShort(4), rsp.getUShort(6),
              rsp.getUShort(8), rsp.getUShort(10), rsp.getUShort(12),
              rsp.getUShort(14), rsp.getUShort(16), rsp.getUShort(18));
      //  lf.log(msgx);

    } else {
      lf.log("Seg Settings Read Error");
    }
  }

  // Get Number Of Images in Segment 0
  cmd.setCode(0x0215);
  cmd.addUShort(1);
  rsp.setExpCode(0x0295);
  rsp.setErrCode(0x02D5);
  doSerialCommand(cmd, rsp, 0, 0);
  setIntegerParam(pco_num_imgs_seg0, rsp.getULong(4));
  setIntegerParam(pco_max_imgs_seg0, rsp.getULong(8));

  // Get Number Of Images in Segment 1
  cmd.setCode(0x0215);
  cmd.addUShort(2);
  rsp.setExpCode(0x0295);
  rsp.setErrCode(0x02D5);
  doSerialCommand(cmd, rsp, 0, 0);
  setIntegerParam(pco_num_imgs_seg1, rsp.getULong(4));
  setIntegerParam(pco_max_imgs_seg1, rsp.getULong(8));

  // Get Number Of Images in Segment 2
  cmd.setCode(0x0215);
  cmd.addUShort(3);
  rsp.setExpCode(0x0295);
  rsp.setErrCode(0x02D5);
  doSerialCommand(cmd, rsp, 0, 0);
  setIntegerParam(pco_num_imgs_seg2, rsp.getULong(4));
  setIntegerParam(pco_max_imgs_seg2, rsp.getULong(8));

  // Get Number Of Images in Segment 3
  cmd.setCode(0x0215);
  cmd.addUShort(4);
  rsp.setExpCode(0x0295);
  rsp.setErrCode(0x02D5);
  doSerialCommand(cmd, rsp, 0, 0);
  setIntegerParam(pco_num_imgs_seg3, rsp.getULong(4));
  setIntegerParam(pco_max_imgs_seg3, rsp.getULong(8));

  // which seg? ret num valid images and num max images

  // Get Bit Alignment
  cmd.setCode(0x0915);
  rsp.setExpCode(0x0995);
  rsp.setErrCode(0x09D5);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_bit_alignment, rsp.getUShort(2));

  // notify IOC to update the PVs to reflect in epics DB
  // my_addriver->callback(general, this);
  // clear changes to prevent constant update

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::getPcoCameraLinkParams(void) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;

  // Get Number Of Images in Segment 0
  cmd.setCode(0x3416);

  rsp.setExpCode(0x3496);
  rsp.setErrCode(0x34D6);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_camlink_pixfreq, rsp.getULong(2));
  setIntegerParam(pco_cclines, rsp.getUChar(6));
  setIntegerParam(pco_camlink_pixperclk, rsp.getUChar(7));
  setIntegerParam(pco_camlink_cont, rsp.getUChar(8));

  // notify IOC to update the PVs to reflect in epics DB
  //	my_addriver->callback(general, this);

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoCameraLinkParams(int function) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;

  // Get Number Of Images in Segment 0

  if (function == pco_camlink_pixfreq || function == pco_cclines ||
      function == pco_camlink_pixperclk || function == pco_camlink_cont ||
      function == pco_setallparams) {
    cmd.setCode(0x3516);
    cmd.addULong((unsigned long)getIntParam(pco_camlink_pixfreq));
    cmd.addUChar((unsigned char)getIntParam(pco_cclines));
    cmd.addUChar((unsigned char)getIntParam(pco_camlink_pixperclk));
    cmd.addUChar((unsigned char)getIntParam(pco_camlink_cont));

    rsp.setExpCode(0x3596);
    rsp.setErrCode(0x35D6);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_camlink_pixfreq, rsp.getULong(2));
      setIntegerParam(pco_cclines, rsp.getUChar(6));
      setIntegerParam(pco_camlink_pixperclk, rsp.getUChar(7));
      setIntegerParam(pco_camlink_cont, rsp.getUChar(8));
      // my_addriver->callback(general, this);
    }
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoBaudrate(int function) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;

  if (getIntParam(is_com_open) == 0) {
    return (1);
  }

  // Get Number Of Images in Segment 0

  if (function == pco_baudrate || function == pco_setallparams) {
    lf.log("Testing baud at 9600");

#ifndef USEASYNSERIAL
    serial_port->close();
    serial_port->open(9600, 0, 8, 0);
#else
    asynSetOption(myServerPort, 0, "open", "close");

    asynSetOption(myServerPort, 0, "baud", "9600");
    asynSetOption(myServerPort, 0, "parity", "none");
    asynSetOption(myServerPort, 0, "bits", "8");
    asynSetOption(myServerPort, 0, "stop", "0");
    asynSetOption(myServerPort, 0, "open", "open");
#endif
    //

    lf.log("getting baudrate to see of we are on comm");
    cmd.setCode(0x3216);
    rsp.setExpCode(0x3296);
    rsp.setErrCode(0x32D6);
    com_error_counter = 0;
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      lf.log("Baudrate Successful at 9600");
    } else {
      lf.log("No commat 9600, try 115200");
#ifndef USEASYNSERIAL
      serial_port->close();
      serial_port->open(115200, 0, 8, 0);
#else
      asynSetOption(myServerPort, 0, "open", "close");

      asynSetOption(myServerPort, 0, "baud", "115200");
      asynSetOption(myServerPort, 0, "parity", "none");
      asynSetOption(myServerPort, 0, "bits", "8");
      asynSetOption(myServerPort, 0, "stop", "0");
      asynSetOption(myServerPort, 0, "open", "open");
#endif
      com_error_counter = 0;

      lf.log("getting baudrate to see of we are on comm");
      cmd.setCode(0x3216);
      rsp.setExpCode(0x3296);
      rsp.setErrCode(0x32D6);
      if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
        lf.log("Can comm at 115200. ");

      } else {
        lf.log("Something wrong- cannot comm at 115200 either. restart camera");
      }
    }

    cmd.setCode(0x3316);
    cmd.addULong((unsigned long)getIntParam(pco_baudrate));

    rsp.setExpCode(0x3396);
    rsp.setErrCode(0x33D6);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      lf.log("Baudrate change OK");
      // setIntegerParam(pco_baudrate,rsp.getULong(2));

      // my_addriver->callback(general, this);

      //

      lf.log("close Ser port");
#ifndef USEASYNSERIAL
      serial_port->close();
#else
      asynSetOption(myServerPort, 0, "open", "close");
#endif
      setIntegerParam(is_com_open, 0);

      epicsThreadSleep(1.0);

      lf.log("Open Serial port, new baudrate");

#ifndef USEASYNSERIAL
      serial_port->open(getIntParam(pco_baudrate), 0, 8, 0);

#else
      char msg[64];
      sprintf(msg, "%d", getIntParam(pco_baudrate));
      asynSetOption(myServerPort, 0, "baud", msg);
      asynSetOption(myServerPort, 0, "parity", "none");
      asynSetOption(myServerPort, 0, "bits", "8");
      asynSetOption(myServerPort, 0, "stop", "0");
      asynSetOption(myServerPort, 0, "open", "open");
#endif

      setIntegerParam(is_com_open, 1);
      lf.log("Get baudrate");

      epicsThreadSleep(0.500);
      cmd.setCode(0x3216);
      rsp.setExpCode(0x3296);
      rsp.setErrCode(0x32D6);
      if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
        lf.log("Baudrate Successful");
        setIntegerParam(pco_baudrate, rsp.getULong(2));
        // my_addriver->callback(general, this);

      } else {
        lf.log("Baudrate Failed");
        setIntegerParam(pco_baudrate, 9600);
        // my_addriver->callback(general, this);

        lf.log("close Ser port");
#ifndef USEASYNSERIAL
        serial_port->close();
#else
        asynSetOption(myServerPort, 0, "open", "close");
#endif
        setIntegerParam(is_com_open, 0);
        throw ccd_exception("Baudrate error");
      }

    } else {
      lf.log("Baudrate change error.");
      setIntegerParam(pco_baudrate, 9600);
      //	my_addriver->callback(general, this);

      lf.log("close Ser port");
#ifndef USEASYNSERIAL
      serial_port->close();
#else
      asynSetOption(myServerPort, 0, "open", "close");
#endif

      setIntegerParam(is_com_open, 0);
      throw ccd_exception("Baudrate error");
    }
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoRecordingParams(int function) {
  pco_command cmd;
  pco_response rsp;

  // Set pco_arm_camera
  if (function == pco_arm_camera || function == pco_setallparams) {
    lf.log("Arm Camera");

    cmd.setCode(0x0A14);
    rsp.setExpCode(0x0A94);
    rsp.setErrCode(0x0AD4);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      lf.log("Camera Armed");
      setIntegerParam(ADStatus, ADStatusIdle);
    } else {
      lf.log("ERROR: Could not Arm Camera");
      setIntegerParam(ADStatus, ADStatusError);
    }
  }

  // Set Recording State
  if (function == pco_rec_status || function == pco_setallparams) {
    cmd.setCode(0x0614);
    cmd.addUShort(getIntParam(pco_rec_status));
    rsp.setExpCode(0x0694);
    rsp.setErrCode(0x06D4);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_rec_status, rsp.getUShort(2));
      // my_addriver->callback(general, this);

      if (getIntParam(pco_rec_status) == 1) {
        lf.log("Started Recording");
        setIntegerParam(ADStatus, ADStatusAcquire);
      } else {
        lf.log("Stopped Recording");
        setIntegerParam(ADStatus, ADStatusIdle);
      }

    } else {
      lf.log("ERROR: Could not set recording");
      setIntegerParam(ADStatus, ADStatusError);
    }

    //  my_addriver->callback(general, this);
  }

  //
  // All commands below are for dimax only
  //

  if (getIntParam(pco_which_camera) != pco_dimax) {
    return (0);
  }

  // Set Storage Mode (Recorder / FIFO buffer)
  if (function == pco_storage_mode || function == pco_setallparams) {
    cmd.setCode(0x0214);
    cmd.addUShort(getIntParam(pco_storage_mode));
    rsp.setExpCode(0x0294);
    rsp.setErrCode(0x02D4);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_storage_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set Recorder Submode (Sequence / Ring buffer)
  if (function == pco_rec_submode || function == pco_setallparams) {
    cmd.setCode(0x0414);
    cmd.addUShort(getIntParam(pco_rec_submode));
    rsp.setExpCode(0x0494);
    rsp.setErrCode(0x04D4);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_rec_submode, rsp.getUShort(2));
      //	 my_addriver->callback(general, this);
    }
  }

  if (getIntParam(pco_rec_status))
    setIntegerParam(ADStatus, ADStatusAcquire);
  else {
    setIntegerParam(ADStatus, ADStatusIdle);
  }

  // Set Acquire mode (Auto / External)
  if (function == pco_acq_mode || function == pco_setallparams) {
    cmd.setCode(0x0814);
    cmd.addUShort(getIntParam(pco_acq_mode));
    rsp.setExpCode(0x0894);
    rsp.setErrCode(0x08D4);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_acq_mode, rsp.getUShort(2));
      //	 my_addriver->callback(general, this);
    }
  }

  //!! set date time...

  // Set timestamp mode
  if (function == pco_timestamp_mode || function == pco_setallparams) {
    cmd.setCode(0x0D14);
    cmd.addUShort(getIntParam(pco_timestamp_mode));
    rsp.setExpCode(0x0D94);
    rsp.setErrCode(0x0DD4);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_timestamp_mode, rsp.getUShort(2));
      //	 my_addriver->callback(general, this);
    }
  }

  // Set Record Stop Event
  if (function == (pco_rec_stop_event_mode) ||
      function == (pco_rec_stop_event_nimgs) || function == pco_setallparams) {
    cmd.setCode(0x0F14);
    cmd.addUShort(getIntParam(pco_rec_stop_event_mode));
    cmd.addULong(getIntParam(pco_rec_stop_event_nimgs));
    rsp.setExpCode(0x0F94);
    rsp.setErrCode(0x0FD4);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_rec_stop_event_mode, rsp.getUShort(2));
      setIntegerParam(pco_rec_stop_event_nimgs, rsp.getULong(4));
      //	 my_addriver->callback(general, this);
    }
  }

  // Stop Record
  if (function == (pco_stop_record) || function == pco_setallparams) {
    cmd.setCode(0x1014);
    cmd.addUShort(0);
    cmd.addULong(0);
    rsp.setExpCode(0x1094);
    rsp.setErrCode(0x10D4);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::getPcoRecordingParams(void) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;

  // Get Storage Mode (Recorder / FIFO buffer)
  cmd.setCode(0x0114);
  rsp.setExpCode(0x0194);
  rsp.setErrCode(0x01D4);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_storage_mode, rsp.getUShort(2));

  // Get Recorder Submode (Sequence / Ring buffer)
  cmd.setCode(0x0314);
  rsp.setExpCode(0x0394);
  rsp.setErrCode(0x03D4);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_rec_submode, rsp.getUShort(2));

  // Get Recording Status
  cmd.setCode(0x0514);
  rsp.setExpCode(0x0594);
  rsp.setErrCode(0x05D4);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_rec_status, rsp.getUShort(2));

  // GetAcquire mode (Auto / External)
  cmd.setCode(0x0714);
  rsp.setExpCode(0x0794);
  rsp.setErrCode(0x07D4);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_acq_mode, rsp.getUShort(2));

  // Get <acq enbl> Signal Status
  cmd.setCode(0x0914);
  rsp.setExpCode(0x0994);
  rsp.setErrCode(0x09D4);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_acqen_sig_stat, rsp.getUShort(2));

  // Get Timestamp Mode
  cmd.setCode(0x0C14);
  rsp.setExpCode(0x0C94);
  rsp.setErrCode(0x0CD4);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_timestamp_mode, rsp.getUShort(2));

  if (getIntParam(pco_which_camera) != pco_edge) {
    // Get Record Stop Event
    cmd.setCode(0x0E14);
    rsp.setExpCode(0x0E94);
    rsp.setErrCode(0x0ED4);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  //!!

  // notify IOC to update the PVs to reflect in epics DB
  //	my_addriver->callback(general, this);
  // clear changes to prevent constant update

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoStorageParams(int function) {
  pco_command cmd;
  pco_response rsp;

  if (getIntParam(pco_which_camera) != pco_dimax) {
    return (0);
  }
  // Set Camera RAM Segment Size

  if (function == (pco_camera_seg_size0) ||
      function == (pco_camera_seg_size1) ||
      function == (pco_camera_seg_size2) ||
      function == (pco_camera_seg_size3) || function == pco_setallparams) {
    cmd.setCode(0x0313);
    cmd.addULong(getIntParam(pco_camera_seg_size0));
    cmd.addULong(getIntParam(pco_camera_seg_size1));
    cmd.addULong(getIntParam(pco_camera_seg_size2));
    cmd.addULong(getIntParam(pco_camera_seg_size3));

    rsp.setExpCode(0x0393);
    rsp.setErrCode(0x03D3);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      lf.log("Successfully set seg size");
      setIntegerParam(pco_camera_seg_size0, rsp.getULong(2));
      setIntegerParam(pco_camera_seg_size1, rsp.getULong(6));
      setIntegerParam(pco_camera_seg_size2, rsp.getULong(10));
      setIntegerParam(pco_camera_seg_size3, rsp.getULong(14));
      //	 my_addriver->callback(general, this);

    } else {
      lf.log("Error setting seg mem size- serial port- try again");
      if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
        lf.log("Successfully set seg size");
        setIntegerParam(pco_camera_seg_size0, rsp.getULong(2));
        setIntegerParam(pco_camera_seg_size1, rsp.getULong(6));
        setIntegerParam(pco_camera_seg_size2, rsp.getULong(10));
        setIntegerParam(pco_camera_seg_size3, rsp.getULong(14));
        //	 my_addriver->callback(general, this);

      } else {
        lf.log("Failed 2nd time- setting seg size");
      }
    }
  }

  // Clear RAM Segment
  if (function == (pco_clear_ram_seg) || function == pco_setallparams) {
    cmd.setCode(0x0413);
    rsp.setExpCode(0x0493);
    rsp.setErrCode(0x04D3);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  // Set Active RAM Segment
  if (function == (pco_active_seg) || function == pco_setallparams) {
    cmd.setCode(0x0613);
    cmd.addUShort(getIntParam(pco_active_seg));
    rsp.setExpCode(0x0693);
    rsp.setErrCode(0x06D3);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_active_seg, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::getPcoStorageParams(void) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;

  if (getIntParam(pco_which_camera) != pco_dimax) {
    return (0);
  }

  // Get Camera RAM size
  cmd.setCode(0x0113);
  rsp.setErrCode(0x01D3);
  rsp.setExpCode(0x0193);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_camera_ram_page_size, rsp.getUShort(6));
  setIntegerParam(pco_camera_ram_npages, rsp.getULong(2));
  // setIntegerParam(
  //	  pco_camera_tot_ram_size,
  //	  getIntParam(pco_camera_ram_page_size) *
  //getIntParam(pco_camera_ram_npages));

  // Get Camera RAM Segment Size
  cmd.setCode(0x0213);
  rsp.setErrCode(0x02D3);
  rsp.setExpCode(0x0293);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_camera_seg_size0, rsp.getULong(2));
  setIntegerParam(pco_camera_seg_size1, rsp.getULong(6));
  setIntegerParam(pco_camera_seg_size2, rsp.getULong(10));
  setIntegerParam(pco_camera_seg_size3, rsp.getULong(14));

  // Clear RAM Segment
  // cmd.setCode(0x0413);
  // rsp.setErrCode(0x0493);
  // rsp.setExpCode(0x04D3);
  // doSerialCommand(cmd,rsp,0,0);

  // Get Active RAM Segment
  cmd.setCode(0x0513);
  rsp.setErrCode(0x05D3);
  rsp.setExpCode(0x0593);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_active_seg, rsp.getUShort(2));

  // notify IOC to update the PVs to reflect in epics DB
  // my_addriver->callback(general, this);
  // clear changes to prevent constant update

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoTimingParams(int function) {
  pco_command cmd;
  pco_response rsp;

  // Set Timebase

  if (function == (pco_exp_timebase) || function == (pco_dly_timebase) ||
      function == pco_setallparams) {
    unsigned short id, ie;

    cmd.setCode(0x0D12);
    cmd.addUShort(getIntParam(pco_dly_timebase));
    cmd.addUShort(getIntParam(pco_exp_timebase));
    rsp.setExpCode(0x0D92);
    rsp.setErrCode(0x0DD2);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_dly_timebase, rsp.getUShort(2));
      setIntegerParam(pco_exp_timebase, rsp.getUShort(4));
      // my_addriver->callback(general, this);
    }
  }
  //

  // set exptime delay time
  if ((getIntParam(pco_is_frame_rate_mode) == 0) &&

      (function == (pco_delay_time) || function == (ADAcquireTime) ||
       function == pco_setallparams)

          ) {
    unsigned long ex, dl;
    double exb, dlb;
    char mxxx[80];

    sprintf(mxxx, "EXPDLYMODE Set DlyTime %f ExpTime%f \n",
            getDoubleParam(pco_delay_time), getDoubleParam(ADAcquireTime));
    lf.log(mxxx);

    sprintf(mxxx, "Min DlyTime %d ns MaxDkytime %d ms\n",
            pco_desc.dwMinDelayDESC, pco_desc.dwMaxDelayDESC);
    lf.log(mxxx);

    sprintf(mxxx, "Min Exptime %d ns Exptime %d ms\n",
            pco_desc.dwMinExposureDESC, pco_desc.dwMaxExposureDESC);
    lf.log(mxxx);

    if (getIntParam(pco_exp_timebase) == 2)
      exb = 1000.0;
    else if (getIntParam(pco_exp_timebase) == 1)
      exb = 1000000.0;
    else
      exb = 1000000000.0;

    if (getIntParam(pco_dly_timebase) == 2)
      dlb = 1000.0;
    else if (getIntParam(pco_dly_timebase) == 1)
      dlb = 1000000.0;
    else
      dlb = 1000000000.0;

    ex = (unsigned long)floor(getDoubleParam(ADAcquireTime) * exb);

    dl = (unsigned long)floor(getDoubleParam(pco_delay_time) * dlb);

    cmd.setCode(0x0212);
    cmd.addULong(dl);
    cmd.addULong(ex);
    rsp.setExpCode(0x0292);
    rsp.setErrCode(0x02D2);

    printf("SND ex %d dl %d dlb %f exb %f", ex, dl, dlb, exb);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      dl = rsp.getULong(2);
      ex = rsp.getULong(6);
      printf("RCV ex %d dl %d dlb %f exb %f", ex, dl, dlb, exb);

      setDoubleParam(ADAcquireTime, ((double)ex) / exb);
      setDoubleParam(pco_delay_time, ((double)dl) / dlb);
      // my_addriver->callback(general, this);
    }

    setIntegerParam(pco_arm_camera, 1);
    setPcoRecordingParams(function);

    getPcoStatusParams();
    sprintf(mxxx, "Actual DlyTime %f ExpTime%f \n",
            getDoubleParam(pco_delay_time), getDoubleParam(ADAcquireTime));
    lf.log(mxxx);
  }

  // set exptime delay time
  if ((getIntParam(pco_is_frame_rate_mode) == 1) &&
      (function == (pco_set_frame_rate) || function == (ADAcquirePeriod) ||
       function == (ADAcquireTime) || function == pco_setallparams)) {
    unsigned long ex, dl;
    double exb, dlb;

    char mxxx[80];

    if (function == (ADAcquirePeriod)) {
      sprintf(mxxx, "FRMODE Set AcPrd %f ExpTime%f \n",
              getDoubleParam(ADAcquirePeriod), getDoubleParam(ADAcquireTime));
      lf.log(mxxx);

      if (getDoubleParam(ADAcquirePeriod) > 0.0)
        setDoubleParam(pco_set_frame_rate,
                       1.0 / getDoubleParam(ADAcquirePeriod));

    } else {
      sprintf(mxxx, "FRMODE Set FrRate %f ExpTime%f \n",
              getDoubleParam(pco_set_frame_rate),
              getDoubleParam(ADAcquireTime));
      lf.log(mxxx);
    }

    sprintf(mxxx, "Min DlyTime %d ns MaxDkytime %d ms\n",
            pco_desc.dwMinDelayDESC, pco_desc.dwMaxDelayDESC);
    lf.log(mxxx);

    sprintf(mxxx, "Min Exptime %d ns Exptime %d ms\n",
            pco_desc.dwMinExposureDESC, pco_desc.dwMaxExposureDESC);
    lf.log(mxxx);

    // convert to ns
    ex = (unsigned long)floor(getDoubleParam(ADAcquireTime) * 1e9);
    // covert to mHz
    dl = (unsigned long)floor(getDoubleParam(pco_set_frame_rate) * 1e3);

    // if (getIntParam(pco_cdi_mode)==1)
    //{
    //	dl=dl*2;
    // }
    cmd.setCode(0x1812);
    cmd.addUShort(1);  // trim exp time
    cmd.addULong(dl);  // frame rate
    cmd.addULong(ex);  // ex time
    rsp.setExpCode(0x1892);
    rsp.setErrCode(0x18D2);

    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      dl = rsp.getULong(4);
      ex = rsp.getULong(8);

      setDoubleParam(ADAcquireTime, ((double)ex) / 1e9);
      setDoubleParam(pco_set_frame_rate, ((double)dl) / 1000.0);

      if (getDoubleParam(pco_set_frame_rate) > 0.0) {
        setDoubleParam(ADAcquirePeriod,
                       1.0 / getDoubleParam(pco_set_frame_rate));
      }

      // my_addriver->callback(general, this);
    }

    // arm the camera
    setIntegerParam(pco_arm_camera, 1);
    setPcoRecordingParams(pco_arm_camera);

    getPcoStatusParams();
    sprintf(mxxx, "Actual Frate %f ExpTime%f \n",
            getDoubleParam(pco_set_frame_rate), getDoubleParam(ADAcquireTime));
    lf.log(mxxx);
  }

  // Set FPS Exposure Mode

  if (function == (pco_fps_mode) || function == pco_setallparams) {
    cmd.setCode(0x1412);
    cmd.addUShort(getIntParam(pco_fps_mode));
    rsp.setExpCode(0x1492);
    rsp.setErrCode(0x14D2);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_fps_mode, rsp.getUShort(2));

      //	 my_addriver->callback(general, this);
    }
  }

  if (function == (pco_trigger_mode) || function == pco_setallparams) {
    cmd.setCode(0x0412);
    cmd.addUShort(getIntParam(pco_trigger_mode));

    rsp.setExpCode(0x0492);
    rsp.setErrCode(0x04D2);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_trigger_mode, rsp.getUShort(2));

      // my_addriver->callback(general, this);
    }
  }

  if (function == (pco_force_trigger) || function == pco_setallparams) {
    lf.log("Force Trigger");
    cmd.setCode(0x0512);
    rsp.setExpCode(0x0592);
    rsp.setErrCode(0x05D2);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::doHighLevelParams(int function) {
  int edx, k;
  stopWatch timex;
  double tx;
  char stxx[128];

  pco_response rsp;
  pco_command cmd;

  if (function == (pco_edge_fastscan) || function == pco_setallparams) {
    if (getIntParam(pco_edge_fastscan) == 2) {
      // setIntegerParam(pco_sensor_format,1);
      // set pixclock to 286000000Hz
      setIntegerParam(pco_pixelrate, 286000000);
      // turn on lookup table
      if (getIntParam(pco_global_shutter) == 0)
        setIntegerParam(pco_1612_lookup, 5650);
      else
        setIntegerParam(pco_1612_lookup, 0);

      // set clink to 12but lut
      setIntegerParam(pco_camlink_pixperclk, 7);
      setIntegerParam(pco_camlink_cont, 3);
      setDoubleParam(pco_set_frame_rate, 20.0);

      // tells image grab to conv 12but format to 16bit if rolling shut
      if (getIntParam(pco_global_shutter) == 0)
        setIntegerParam(pco_conv_12to16, 1);
      else
        setIntegerParam(pco_conv_12to16, 0);

      // to set up grabber for 12 bit compressed mode... mut xsize by 1.5
      setIntegerParam(is_mult_width2, 2);

    } else if (getIntParam(pco_edge_fastscan) == 1) {
// setIntegerParam(pco_sensor_format,0);
#ifdef _LIMITIMGSIZE
      setIntegerParam(ADSizeX, 1800);
      setIntegerParam(ADSizeY, 1800);
#endif
      // set pixclock to 286000000Hz
      setIntegerParam(pco_pixelrate, 286000000);
      // turn on lookup table
      setIntegerParam(pco_1612_lookup, 0);
      // set clink to 12but lut

      if (getIntParam(pco_global_shutter) == 0)
        setIntegerParam(pco_camlink_pixperclk, 5);
      else
        setIntegerParam(pco_camlink_pixperclk, 7);

      setIntegerParam(pco_conv_12to16, 0);

      setIntegerParam(pco_camlink_cont, 3);

      // to set up grabber for 12 bit compressed mode... mut xsize by 1.5
      setIntegerParam(is_mult_width2, 1);

      //!! setDoubleParam(pco_set_frame_rate,20.0);

    } else {
      setIntegerParam(ADSizeX, 2560);
      setIntegerParam(ADSizeY, 2160);
      // setIntegerParam(pco_sensor_format,0);
      // set pixclock to 95333333Hz
      setIntegerParam(pco_pixelrate, 95333333);
      // turn off looup table.
      setIntegerParam(pco_1612_lookup, 0);
      // set clink to 16bit
      // setIntegerParam(pco_camlink_pixperclk,5);
      if (getIntParam(pco_global_shutter) == 0)
        setIntegerParam(pco_camlink_pixperclk, 5);
      else
        setIntegerParam(pco_camlink_pixperclk, 7);

      setIntegerParam(pco_camlink_cont, 3);

      // to set up grabber for 12 bit compressed mode... mut xsize by 1.5
      setIntegerParam(is_mult_width2, 1);
      setIntegerParam(pco_conv_12to16, 0);

      setDoubleParam(pco_set_frame_rate, 1.0);
    }

    setPcoSensorParams(pco_setallparams);
    setPcoTimingParams(pco_setallparams);
    setPcoCameraLinkParams(pco_setallparams);
    reconfigGrabber();
  }

  if ((getIntParam(pco_which_camera) == pco_dimax) &&
      (function == (pco_live_view) || function == pco_setallparams)) {
    setIntegerParam(pco_camlink_cont, getIntParam(pco_live_view));

    if (getIntParam(pco_live_view)) {
      setIntegerParam(pco_rec_submode, 1);
      // setIntegerParam(ADNumImages,100000);

    } else {
      setIntegerParam(pco_rec_submode, 0);
    }

    setPcoCameraLinkParams(pco_setallparams);
    setPcoRecordingParams(pco_setallparams);
  }

  // arm and record/run
  if (function == (ADAcquire)) {
    if (getIntParam(ADAcquire) == 1) {
      // set up memory before new acq
      if (is_reset_memory) {
        resetDimaxMemory();
        is_reset_memory = false;
      }

      setIntegerParam(is_grab, 1);

      grabberSetup(is_grab);

      setIntegerParam(ADNumImagesCounter, 0);

      lf.log("Arm and Run");
      setIntegerParam(pco_arm_camera, 1);
      setPcoRecordingParams(pco_arm_camera);
      // Sleep(100);

      setIntegerParam(pco_rec_status, 1);

      // here we are getting 2 mutexes--- we want to make sure grabber loop is
      // not doing somtehing important..
      // sio wegrab is nutex

      setPcoRecordingParams(pco_rec_status);

      // getPcoRecordingParams();
      // Sleep(100);
      // once we start recording- allow driver to turn off ADAcquire when
      // recording is done.
      // this prevents threading race condition problem where getstatus params
      // from camera always tirns off ADACquire
      // once every vew seconds, that may happen same time as user hits
      // ADAcquire.

      // for dimax- we acquire into RAM. then the camera stops recording.
      // we wnat to turn off ADACquire when camera stops recording, when mem is
      // full
      // for dimax only
      is_allow_kill_acquire = true;

      setIntegerParam(pco_ready2acquire, 1);
    }

    if (getIntParam(ADAcquire) == 0) {
      lf.log("Stop Run");
      setIntegerParam(pco_rec_status, 0);

      // setIntegerParam(is_abort,1);

      // grabberSetup(is_abort);
      setIntegerParam(is_grab, 0);

      grabberSetup(is_grab);

      // we get grabber mutex to make sure it is not doing anything in grabber
      // loop.

      setPcoRecordingParams(pco_rec_status);

      if (getIntParam(pco_dump_counter) < 1)
        setIntegerParam(pco_dump_counter, 1);
      else if (getIntParam(pco_dump_counter) > getIntParam(pco_num_imgs_seg0))
        setIntegerParam(pco_dump_counter, getIntParam(pco_num_imgs_seg0));

      if (getIntParam(pco_imgs2dump) > getIntParam(pco_num_imgs_seg0))
        setIntegerParam(pco_imgs2dump, getIntParam(pco_num_imgs_seg0));
      else if (getIntParam(pco_imgs2dump) < 1)
        setIntegerParam(pco_imgs2dump, getIntParam(pco_num_imgs_seg0));
    }
  }

  if (function == (pco_reset_memory) || function == (ADNumImages) ||
      function == pco_setallparams) {
    lf.log("req reset Memory- will reset on ACQ");
    // resetDimaxMemory();
    is_reset_memory = true;
    is_reset_dump_counters = true;
    setIntegerParam(pco_imgs2dump, getIntParam(ADNumImages));
    setIntegerParam(pco_dump_counter, 0);
  }

  if (function == (pco_dump_maxdatarate) || function == pco_setallparams) {
    setIntegerParam(pco_favor_dlytime, 0);
  }

  if (function == (pco_dump_waitms) || function == pco_setallparams) {
    setIntegerParam(pco_favor_dlytime, 1);
  }

  //	if ((getIntParam(pco_dump_camera_memory)==1) &&
  //(getIntParam(ADAcquire)==0))
  //	{
  //		dumpCameraMemory();
  //	}//if dump mem
  setIntegerParam(ADStatus, ADStatusIdle);
  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

void pco::dumpCameraMemory(void) {
  int edx, k;
  stopWatch timex;
  double tx;
  char stxx[128];

  pco_response rsp;
  pco_command cmd;

  int waitms;
  int last_val;
  int max_data_rate;
  float max_fps;
  float min_frame_period;
  int waitcounter;
  int start_cnt;

  int dumpcntsave;

  setIntegerParam(pco_cancel_dump, 0);

  // in bytes/sec.param is in kB/sec
  // max_data_rate = 1024*getIntParam(pco_dump_maxdatarate);
  // sprintf(stxx,"MaxDataRate in Bytes %d\n",max_data_rate);
  // lf.log(stxx);
  // max_fps=((float)max_data_rate)
  // /((float)(getIntParam(ADSizeX)*getIntParam(ADSizeY)*2));
  // if (max_fps>0.0)
  //	min_frame_period=1.0/max_fps;

  // make sure wait time is as long as min frame poeriod. or else we lose frames
  // 1000 to concert sec to ms.
  // setIntegerParam(pco_dump_waitms, (int)(min_frame_period*1000.0));
  // sprintf(stxx,"Max FPS %f Min Frame Period %f\n",max_fps,min_frame_period);
  // lf.log(stxx);

  setIntegerParam(is_grab, 1);
  grabberSetup(is_grab);

  //		waitms = getIntParam(pco_dump_waitms);

  setIntegerParam(ADStatus, ADStatusReadout);
  setIntegerParam(pco_camlink_cont, 1);

  setPcoCameraLinkParams(pco_setallparams);

  lf.log("To dump Memory");
  lf.log("Getting Read info from camera");
  getPcoImageReadParams();

  // Read Images from Segment (Recorder Mode only)

  setIntegerParam(total_missed_frames, 0);
  setIntegerParam(pco_double_image_error, 0);

  if (getIntParam(pco_dump_counter) < 1)
    setIntegerParam(pco_dump_counter, 1);
  else if (getIntParam(pco_dump_counter) > getIntParam(pco_num_imgs_seg0))
    setIntegerParam(pco_dump_counter, getIntParam(pco_num_imgs_seg0));

  if (getIntParam(pco_imgs2dump) > getIntParam(pco_num_imgs_seg0))
    setIntegerParam(pco_imgs2dump, getIntParam(pco_num_imgs_seg0));
  else if (getIntParam(pco_imgs2dump) < 1)
    setIntegerParam(pco_imgs2dump, getIntParam(pco_num_imgs_seg0));

  edx = getIntParam(pco_imgs2dump);

  callParamCallbacks();

  setIntegerParam(pco_array_counter, getIntParam(pco_dump_counter));

  if (getIntParam(pco_imgs2dump) > getIntParam(pco_num_imgs_seg0))
    setIntegerParam(pco_imgs2dump, getIntParam(pco_num_imgs_seg0));
  else if (getIntParam(pco_imgs2dump) < 1)
    setIntegerParam(pco_imgs2dump, getIntParam(pco_num_imgs_seg0));

  edx = getIntParam(pco_imgs2dump);

  callParamCallbacks();

  setIntegerParam(pco_array_counter, getIntParam(pco_dump_counter));

  // timex.tic();

  dumpcntsave = getIntParam(pco_dump_counter);

  waitcounter = 0;

  // we will grab images on THIS tgread. The grabber thread will block and do
  // nothing.
  grabGrabberMutex();

  // read one image at a time.
  for (k = getIntParam(pco_dump_counter);
       (k <= edx) && (is_cancel_dump == false); k++) {
    // this is current counter that says how many images from beginning of time
    // have vbeen grabbed. we use it to see if a frame came into the grabber.
    last_val = getIntParam(pco_array_counter);

    // tell camera to send frame k.
    // checks to see if pcoarray counter increments... it wont, because we
    // killed the
    // grabber thread...
    waitcounter = 0;
    while (dumpOneFrame(k) != 0 && waitcounter < 10 &&
           is_cancel_dump == false) {
      // if we got in here we had serial port error, so try again.
      // wait for 1ms... just for fun... we DUID have an error...
      epicsThreadSleep(0.001);
      waitcounter++;
    }

    if (waitcounter > 8) {
      lf.log("Too many serial port errors...cannot dump memory");
      break;  // exit for loop
    }
    waitcounter = 0;

    // this has to run ONCE- bacause pro array counter will not increment until
    // grabber gets a frame
    // because we stopped the grabber thread, no frame is ther.
    // so we call oneLoopImage at least once.
    while (last_val == getIntParam(pco_array_counter) && waitcounter < 200 &&
           is_cancel_dump == false) {
      // run the grabber code once to get the frame
      // it MAY not get frame if it did not arrive from camera yet. so we have
      // to check.
      oneLoopImage();

      // if the image never came, we sleep and try again.
      if (last_val == getIntParam(pco_array_counter))
        epicsThreadSleep(getIntParam(cor_sleep_ms) / 1000.0);

      waitcounter++;

      // we weill tend to cancel if we get stuck in this loop...
      // check for cancellation.
    }

    if (waitcounter > 198) {
      lf.log("Could not dump memory- ");
      setIntegerParam(total_missed_frames, 1000);
      k = 100000000;  // forces break from the for...
    }  // if

    // oupdate epics dump counter pv every 16 shots

    setIntegerParam(pco_dump_counter, k);

    callParamCallbacks();

  }  // for

  releaseGrabberMutex();

  lf.log("Finished Dump");

  setIntegerParam(pco_cancel_dump, 0);
  is_cancel_dump = false;

  setIntegerParam(ADStatus, ADStatusIdle);
  setIntegerParam(pco_camlink_cont, 0);
  setIntegerParam(pco_dump_counter, dumpcntsave);
  setPcoCameraLinkParams(pco_setallparams);

  setIntegerParam(pco_dump_camera_memory, 0);

  setIntegerParam(is_grab, 0);
  grabberSetup(is_grab);

  callParamCallbacks();

}  // if dump mem

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::dumpOneFrame(int k) {
  pco_response rsp;
  pco_command cmd;

  cmd.setCode(0x0515);
  cmd.addUShort(1);
  cmd.addULong(k);
  cmd.addULong(k);
  rsp.setExpCode(0x0595);
  rsp.setErrCode(0x05D5);

  if (doSerialCommand(cmd, rsp, 0, 0) != 0) {
    // com error
    // see if frame made it
    // wait and timeout at 1s
    //!! need to do eqv of waitDone(1000);

    return (-1);
  }  // if do serial

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

void pco::resetDimaxMemory(void) {
  double npixels;
  double pagepixels;
  double npages_img;
  int req_pages;

  lf.log("Resetting memory- deleting images on camera");
  setIntegerParam(pco_reset_memory, 0);

  getPcoSensorParams();
  getPcoStorageParams();

  // calc needed memory for one image.
  // num pixels
  npixels = (double)(getIntParam(ADSizeX) * getIntParam(ADSizeY));
  pagepixels = (double)(getIntParam(pco_camera_ram_page_size));
  npages_img = npixels / pagepixels;
  // in manual- we need room for 3 images if we want to store 1. so I add 3.
  double nimgsfudge = ((double)getIntParam(pco_dimax_nimgs_fudge) +
                       (double)getIntParam(ADNumImages));
  if (nimgsfudge < 3.0) nimgsfudge = 3.0;

  req_pages = (int)ceil(npages_img * nimgsfudge);

  if (getIntParam(pco_cdi_mode)) req_pages = req_pages * 2;

  if (req_pages > getIntParam(pco_camera_ram_npages)) {
    lf.log("Req too many images for RAM, use Max Ram");
    req_pages = getIntParam(pco_camera_ram_npages);
  }

  lf.log("Set Seg0 to RAm based on #Images PV");
  // setIntegerParam(pco_camera_seg_size0,getIntParam(pco_camera_ram_npages));
  setIntegerParam(pco_camera_seg_size0, req_pages);
  setIntegerParam(pco_camera_seg_size1, 0);
  setIntegerParam(pco_camera_seg_size2, 0);
  setIntegerParam(pco_camera_seg_size3, 0);

  // run 5 times bacause it keeps not working... it pass thru once it works.
  setPcoStorageParams(pco_setallparams);
  // setPcoStorageParams();
  // setPcoStorageParams();
  // setPcoStorageParams();
  // setPcoStorageParams();

  lf.log("Set seg0 active");

  setIntegerParam(pco_active_seg, 1);
  setPcoStorageParams(pco_active_seg);
  lf.log("Clear Seg memory");
  setIntegerParam(pco_clear_ram_seg, 1);
  setPcoStorageParams(pco_clear_ram_seg);

  if (is_reset_dump_counters) {
    //			getPcoStatusParams();
    setIntegerParam(pco_dump_counter, 1);
    setIntegerParam(pco_imgs2dump, getIntParam(ADNumImages));
    is_reset_dump_counters = false;
  }
}

int pco::getPcoTimingParams(void) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  double etb, dtb;

  pco_command cmd;
  pco_response rsp;

  etb = 1e-3;
  dtb = 1e-3;

  // Get Timebase
  cmd.setCode(0x0C12);
  rsp.setErrCode(0x0CD2);
  rsp.setExpCode(0x0C92);
  if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
    // 0 1 2= ns, us. ms
    setIntegerParam(pco_exp_timebase, rsp.getUShort(4));
    setIntegerParam(pco_dly_timebase, rsp.getUShort(2));

    etb = 1e-9;

    if (getIntParam(pco_exp_timebase) == 1) etb = 1e-6;

    if (getIntParam(pco_exp_timebase) == 2) etb = 1e-3;

    dtb = 1e-9;

    if (getIntParam(pco_dly_timebase) == 1) dtb = 1e-6;

    if (getIntParam(pco_dly_timebase) == 2) dtb = 1e-3;
  }

  if (getIntParam(pco_is_frame_rate_mode) == 0) {
    // Get Delay / Exposure Time
    cmd.setCode(0x0112);
    rsp.setErrCode(0x01D2);
    rsp.setExpCode(0x0192);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setDoubleParam(ADAcquireTime, etb * (double)(rsp.getULong(6)));
      setDoubleParam(pco_delay_time, dtb * (double)(rsp.getULong(2)));
    }
  } else {
    // Get fraerate / Exposure Time
    cmd.setCode(0x1712);
    rsp.setErrCode(0x17D2);
    rsp.setExpCode(0x1792);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setDoubleParam(ADAcquireTime, (1e-9) * (double)(rsp.getULong(8)));
      setDoubleParam(pco_set_frame_rate, (1e-3) * (double)(rsp.getULong(4)));

      if (getDoubleParam(pco_set_frame_rate) > 0.0) {
        setDoubleParam(ADAcquirePeriod,
                       1.0 / getDoubleParam(pco_set_frame_rate));
      }
    }
  }
  // Get delay / exposure time table
  // cmd.setCode(0x0A12);
  // rsp.setErrCode(0x0AD2);
  // rsp.setExpCode(0x0A92);
  // doSerialCommand(cmd,rsp,0,0);

  if (getIntParam(pco_which_camera) == pco_other) {
    // Get FPS Exposure Mode
    cmd.setCode(0x1312);
    rsp.setErrCode(0x13D2);
    rsp.setExpCode(0x1392);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_fps_mode, rsp.getUShort(2));
      if (getIntParam(pco_fps_mode) == 1) {
        setDoubleParam(ADAcquireTime, 10e-9 * (double)(rsp.getULong(4)));
      }
    }
  }
  // Get Trigger Mode
  cmd.setCode(0x0312);
  rsp.setErrCode(0x03D2);
  rsp.setExpCode(0x0392);
  if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
    switch (rsp.getUShort(2)) {
      case 0:
        setIntegerParam(ADTriggerMode, 2);  // auto
        break;
      case 1:
        setIntegerParam(ADTriggerMode, ADTriggerInternal);  // int
        break;
      case 2:
        setIntegerParam(ADTriggerMode, ADTriggerExternal);  // ext
        break;
      case 3:
        setIntegerParam(ADTriggerMode, 3);  // gate
        break;
      default:
        setIntegerParam(ADTriggerMode, ADTriggerInternal);  // auto
    }
  }
  // Get Camera Busy status
  cmd.setCode(0x0612);
  rsp.setErrCode(0x06D2);
  rsp.setExpCode(0x0692);
  if (doSerialCommand(cmd, rsp, 0, 0) == 0)
    setIntegerParam(pco_busy, rsp.getUShort(2));

  if (getIntParam(pco_which_camera) == pco_other) {
    // Get Power Down Mode
    cmd.setCode(0x0E12);
    rsp.setErrCode(0x0ED2);
    rsp.setExpCode(0x0E92);
    doSerialCommand(cmd, rsp, 0, 0);

    // Get User Power Down Time
    cmd.setCode(0x0712);
    rsp.setErrCode(0x07D2);
    rsp.setExpCode(0x0792);
    doSerialCommand(cmd, rsp, 0, 0);
  }

  // Get Get <exp trig> Signal Status
  cmd.setCode(0x0912);
  rsp.setErrCode(0x09D2);
  rsp.setExpCode(0x0992);
  if (doSerialCommand(cmd, rsp, 0, 0) == 0)
    setIntegerParam(pco_exp_trig_stat, rsp.getUShort(2));

  // notify IOC to update the PVs to reflect in epics DB

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::getPcoGeneralParams(void) {
  unsigned short val1;
  char msgx[256];
  unsigned long lval1;

  pco_command cmd;
  pco_response rsp;
  // Get Camera Type
  cmd.setCode(0x0110);
  rsp.setErrCode(0x01D0);
  rsp.setExpCode(0x0190);

  setIntegerParam(pco_which_camera, pco_other);

  doSerialCommand(cmd, rsp, 0, 0);

  setStringParam(ADManufacturer, "PCO-Cooke");
  setStringParam(ADManufacturer, "PCO-Cooke");

  val1 = rsp.getUShort(2);
  if ((val1 & 0xff00) == 0x0100) setStringParam(ADModel, "PCO 1200 Camera");

  if ((val1 & 0xff00) == 0x0200) setStringParam(ADModel, "PCO 13k-4k Camera");

  if ((val1 & 0xff00) == 0x0800) setStringParam(ADModel, "PCO 13k14k Camera");

  if ((val1 & 0xff00) == 0x0900) setStringParam(ADModel, "PCO Pixelfly Camera");

  if ((val1 & 0xff00) == 0x1000) {
    setStringParam(ADModel, "PCO Dimax Camera");
    setIntegerParam(pco_which_camera, pco_dimax);
    setIntegerParam(cor_check_timestamps, 0);
    setIntegerParam(pco_descramble, 0);
    setIntegerParam(is_mult_width2, 0);
    setIntegerParam(is_force_img_size, 1);
    setIntegerParam(cor_use_image_mode, 0);
  }

  if ((val1 & 0xff00) == 0x1200) setStringParam(ADModel, "PCO Sensicam Camera");

  if ((val1 & 0xff00) == 0x1300) {
    setStringParam(ADModel, "PCO Edge Camera");
    setIntegerParam(pco_which_camera, pco_edge);
    setIntegerParam(cor_check_timestamps, 1);
    setIntegerParam(pco_descramble, 1);
    setIntegerParam(is_mult_width2, 1);

    if (getIntParam(pco_edge_fastscan) == 2) setIntegerParam(is_mult_width2, 2);

    setIntegerParam(is_force_img_size, 1);
    setIntegerParam(cor_use_image_mode, 1);
  }

  // Get Camera Health Status
  cmd.setCode(0x0210);
  rsp.setErrCode(0x02D0);
  rsp.setExpCode(0x0290);

  doSerialCommand(cmd, rsp, 0, 0);

  lval1 = rsp.getULong(2);
  setIntegerParam(pco_health_warning, lval1);

  if (lval1 != 0) {
    sprintf(msgx, "Warning from PCO: %d\n", lval1);
    lf.log(msgx);
    setIntegerParam(ADStatus, ADStatusError);
  }

  lval1 = rsp.getULong(6);
  setIntegerParam(pco_health_error, lval1);

  if (lval1 != 0) {
    sprintf(msgx, "Error from PCO: %d\n", lval1);
    lf.log(msgx);
    setIntegerParam(ADStatus, ADStatusError);
  }

  lval1 = rsp.getULong(10);
  setIntegerParam(pco_health_status, lval1);

  // Get Camera FW Ver
  // cmd.setCode(0x0810);
  // rsp.setErrCode(0x08D0);
  // rsp.setExpCode(0x0890);
  //	doSerialCommand(cmd,rsp,0,0);

  // Get Camera HW Ver
  // cmd.setCode(0x0710);
  // rsp.setErrCode(0x07D0);
  // rsp.setExpCode(0x0790);
  //	doSerialCommand(cmd,rsp,0,0);

  // Get Camera Temp
  cmd.setCode(0x0610);
  rsp.setErrCode(0x06D0);
  rsp.setExpCode(0x0690);
  doSerialCommand(cmd, rsp, 0, 0);
  short sval1 = (short)rsp.getUShort(2);

  setDoubleParam(ADTemperature, (double)sval1);

  // notify IOC to update the PVs to reflect in epics DB
  // my_addriver->callback(general, this);
  // clear changes to prevent constant update

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoGeneralParams(int function) {
  // Reset Settings to Default

  if (function == (pco_reset_default_settings) ||
      function == pco_setallparams) {
    setIntegerParam(pco_reset_default_settings, 0);

    pco_command cmd;
    pco_response rsp;

    cmd.setCode(0x0310);
    rsp.setExpCode(0x0390);
    rsp.setErrCode(0x03D0);
    doSerialCommand(cmd, rsp, 0, 0);
    lf.log("Reset Camera to Default Settings");
  }

  // Initiate Selftest Procedure

  if (function == pco_init_selftest || function == pco_setallparams) {
    setIntegerParam(pco_init_selftest, 0);

    pco_command cmd;
    pco_response rsp;

    cmd.setCode(0x0510);
    rsp.setExpCode(0x0590);
    rsp.setErrCode(0x05D0);
    doSerialCommand(cmd, rsp, 0, 0);
    lf.log("Init Self Test");
  }

  if (function == pco_global_shutter &&
      getIntParam(pco_which_camera) == pco_edge) {
    lf.log("pco_global_shutter- getting cam setup");
    char mesgx[200];

    // the scan speed on camera will be reset to slow scan
    setIntegerParam(pco_edge_fastscan, 0);

    setIntegerParam(ADStatus, ADStatusWaiting);
    callParamCallbacks();
    pco_command cmd;
    pco_response rsp;

    // Get Camera Setup
    cmd.setCode(0x1110);
    // wtype- what is type?
    cmd.addUShort(0);
    // add 4 flags
    cmd.addULong(0);
    cmd.addULong(0);
    cmd.addULong(0);
    cmd.addULong(0);

    rsp.setExpCode(0x1190);
    rsp.setErrCode(0x11D0);
    doSerialCommand(cmd, rsp, 0, 0);

    unsigned short wtype = rsp.getUShort(2);
    unsigned int p0new = 0;
    unsigned int p0 = rsp.getULong(4);
    unsigned int p1 = rsp.getULong(6);
    unsigned int p2 = rsp.getULong(8);
    unsigned int p3 = rsp.getULong(10);

    int PCO_EDGE_SETUP_GLOBAL_SHUTTER = 2;
    int PCO_EDGE_SETUP_ROLLING_SHUTTER = 1;

    sprintf(mesgx, "wtype= %d p's %d %d %d %d\n", wtype, p0, p1, p2, p3);

    lf.log(mesgx);

    // Set timeouts

    if (getIntParam(pco_global_shutter) == 1) {
      // reboot w/ global shutter ON
      p0new = PCO_EDGE_SETUP_GLOBAL_SHUTTER;
      setIntegerParam(ADSizeX, 2560);
      setIntegerParam(ADSizeY, 2160);
      setStringParam(cor_ccf_filename, getStringParam(pco_globshut_mcfname));
    }

    if (getIntParam(pco_global_shutter) == 0) {
      // reboot w/ global shutter OFF
      p0new = PCO_EDGE_SETUP_ROLLING_SHUTTER;
      setIntegerParam(ADSizeX, 2560);
      setIntegerParam(ADSizeY, 2160);
      setStringParam(cor_ccf_filename, getStringParam(pco_rollshut_mcfname));
    }

    //
    // if camera is already set to correct settings we need not reboot.
    // figure oyt of we must reboot
    //
    if (p0 != p0new) {
      sprintf(mesgx, "pco::setPcoGeneralParams - mcf/ccf file name %s",
              getStringParam(cor_ccf_filename));
      lf.log(mesgx);
      lf.log("SetCameraSetuyp");
      // Set Camera Setup
      cmd.setCode(0x1210);
      // wtype- what is type?
      cmd.addUShort(wtype);
      // add 4 flags
      cmd.addULong(p0new);  // p0 determine global or rolling shutter setup
      cmd.addULong(p1);
      cmd.addULong(p2);
      cmd.addULong(p3);

      rsp.setExpCode(0x1290);
      rsp.setErrCode(0x12D0);
      doSerialCommand(cmd, rsp, 0, 0);

      lf.log("Reboot Camera");

      cmd.setCode(0x1704);
      rsp.setExpCode(0x1794);
      rsp.setErrCode(0x17D4);
      doSerialCommand(cmd, rsp, 0, 0);

      lf.log("Close Ser port");

#ifndef USEASYNSERIAL
      serial_port->close();
#else
      asynSetOption(myServerPort, 0, "open", "close");
#endif

      lf.log("Waiting 10s for reboot Camera");
      epicsThreadSleep(10.0);

      lf.log("Open Ser port");

#ifndef USEASYNSERIAL
      serial_port->open();
#else
      asynSetOption(myServerPort, 0, "open", "open");
#endif
    }
    reconfigGrabber();
    setIntegerParam(ADStatus, ADStatusIdle);
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::setPcoSensorParams(int function) {
  char mesgxx[256];
  pco_command cmd;
  pco_response rsp;

  // Set Sensor Format

  if ((function == pco_sensor_format || function == pco_setallparams) &&
      getIntParam(pco_which_camera) == pco_dimax) {
    cmd.setCode(0x1511);
    cmd.addUShort(getIntParam(pco_sensor_format));
    rsp.setExpCode(0x1591);
    rsp.setErrCode(0x15D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_sensor_format, rsp.getUShort(2));

      // my_addriver->callback(general, this);
    }
  }

  // Set ROI

  if (function == ADSizeY || function == ADSizeX ||
      function == pco_setallparams) {
    double x0, y0, x1, y1;
    unsigned short xs0, ys0, xs1, ys1;
    int sizex, sizey;

    // turn off acquisition if we change image size
    setIntegerParam(ADAcquire, 0);
    doHighLevelParams(ADAcquire);

    setIntegerParam(ADMinX, 1);
    setIntegerParam(ADMinY, 1);

    if (getIntParam(pco_which_camera) == pco_edge) {
#ifdef _LIMITIMGSIZE600
      if (getIntParam(ADSizeY) < 600) setIntegerParam(ADSizeY, 600);
#endif

#ifdef _LIMITIMGSIZE
      if (getIntParam(pco_edge_fastscan) == 1) {
        if (getIntParam(ADSizeX) > 1800) setIntegerParam(ADSizeX, 1800);

        if (getIntParam(ADSizeY) > 1800) setIntegerParam(ADSizeY, 1800);
      }
#endif
    }

    cmd.setCode(0x0311);

    x0 = (double)getIntParam(ADMinX);
    y0 = (double)getIntParam(ADMinY);
    x1 = getIntParam(ADSizeX) + getIntParam(ADMinX) - 1;
    y1 = getIntParam(ADSizeY) + getIntParam(ADMinY) - 1;
    // in dimax pco_desc.wRoiHorStepsDESC is 24. We use 48 , so we have
    // divisable by 16 too, for grabber.
    //!! need better way to do this.

    int magic_number = pco_desc.wRoiHorStepsDESC;
    if (magic_number > 2560) {
      magic_number = 16;
      printf("Problem with magic number\n");
    }
    while ((magic_number < 16) || ((magic_number % 16) != 0)) {
      magic_number = magic_number * 2;
    }
    printf("magic number %d\n", magic_number);
    double magicd = (double)magic_number;
    // make size x by divisible by twice mag number
    //!!sizex=getIntParam(ADSizeX);
    //!!sizex = sizex - (sizex % (2*magic_number));
    //!!int maxsizex=getIntParam(ADMaxSizeX);
    //!!x0 = 1;
    //!!x1 =  sizex;

    x0 = 1.0 + (floor(((x0 - 1.0) / magicd) + 0.5) * magicd);
    x1 = (floor(((x1) / magicd) + 0.5) * magicd);

    // sizey=getIntParam(ADSizeY);
    // sizey = sizey - (sizey % (2*pco_desc.wRoiVertStepsDESC));
    // int maxsizey=getIntParam(ADMaxSizeY);
    // y0 = 1;
    // y1 = sizey;

    int vsteps = 2 * pco_desc.wRoiVertStepsDESC;
    y0 = 1.0 + (floor(((y0 - 1.0) / vsteps) + 0.5) * vsteps);
    y1 = (floor(((y1) / vsteps) + 0.5) * vsteps);

    xs0 = (unsigned short)x0;
    xs1 = (unsigned short)x1;
    ys0 = (unsigned short)y0;
    ys1 = (unsigned short)y1;

    cmd.addUShort(xs0);
    cmd.addUShort(ys0);
    cmd.addUShort(xs1);
    cmd.addUShort(ys1);

    sprintf(mesgxx, "x0=%d x1=%d  y0=%d  y1=%d  \n", xs0, xs1, ys0, ys1);
    lf.log(mesgxx);
    rsp.setExpCode(0x0391);
    rsp.setErrCode(0x03D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(ADMinX, rsp.getUShort(2));
      setIntegerParam(ADMinY, rsp.getUShort(4));
      setIntegerParam(ADSizeX, rsp.getUShort(6) - getIntParam(ADMinX) + 1);
      setIntegerParam(ADSizeY, rsp.getUShort(8) - getIntParam(ADMinY) + 1);

      // my_addriver->callback(general, this);

      // setIntegerParam(pco_reconfig_grabber,1);

      reconfigGrabber();

      lf.log("reconfig'ed grabber");
      sprintf(mesgxx, "sizex =%d sizey=%d  \n", getIntParam(ADSizeX),
              getIntParam(ADSizeY));
    }
  }

  // Set Binn

  if ((function == ADBinX || function == ADBinY ||
       function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x0311);
    cmd.addUShort(getIntParam(ADBinX));
    cmd.addUShort(getIntParam(ADBinY));

    rsp.setExpCode(0x0391);
    rsp.setErrCode(0x03D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(ADBinX, rsp.getUShort(2));
      setIntegerParam(ADBinY, rsp.getUShort(4));

      // my_addriver->callback(general, this);
    }
  }

  // Set pixelrate

  if (function == pco_pixelrate || function == pco_setallparams) {
    cmd.setCode(0x0711);
    cmd.addULong(getIntParam(pco_pixelrate));
    rsp.setExpCode(0x0791);
    rsp.setErrCode(0x07D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_pixelrate, rsp.getULong(2));

      // my_addriver->callback(general, this);
    }
  }

  // Set Gain- Conv Factor

  if ((function == ADGain || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    unsigned short igain;
    igain = (unsigned short)floor(getDoubleParam(ADGain) * 100.0);

    cmd.setCode(0x0911);
    cmd.addUShort(igain);
    rsp.setExpCode(0x0991);
    rsp.setErrCode(0x09D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setDoubleParam(ADGain, ((double)rsp.getUShort(2)) / 100.0);

      // my_addriver->callback(general, this);
    }
  }

  // Set doub image mode

  if ((function == pco_doub_img_mode || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x0B11);
    cmd.addUShort(getIntParam(pco_doub_img_mode));
    rsp.setExpCode(0x0B91);
    rsp.setErrCode(0x0BD1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_doub_img_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set ADC Operation

  if ((function == pco_adc_mode || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x0D11);
    cmd.addUShort(getIntParam(pco_adc_mode));
    rsp.setExpCode(0x0D91);
    rsp.setErrCode(0x0DD1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_adc_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set Cooling Setpoint Temperature

  if (function == (pco_temp_setpt) || function == pco_setallparams) {
    cmd.setCode(0x1111);
    cmd.addShort(getIntParam(pco_temp_setpt));
    rsp.setExpCode(0x1191);
    rsp.setErrCode(0x11D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_temp_setpt, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // set Hot Pix Correction Mode

  if ((function == pco_hotpix_corr || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_edge)) {
    cmd.setCode(0x1F11);
    cmd.addUShort(getIntParam(pco_hotpix_corr));
    rsp.setExpCode(0x1F91);
    rsp.setErrCode(0x1FD1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_hotpix_corr, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set Offset Mode

  if ((function == pco_offset_mode || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x1311);
    cmd.addUShort(getIntParam(pco_offset_mode));
    rsp.setExpCode(0x1391);
    rsp.setErrCode(0x13D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_offset_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set Noise Filter Mode

  if ((function == pco_noise_filt_mode || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x1A11);
    cmd.addUShort(getIntParam(pco_noise_filt_mode));
    rsp.setExpCode(0x1A91);
    rsp.setErrCode(0x1AD1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_noise_filt_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set pco_cdi_mode

  if ((function == pco_cdi_mode || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x3011);
    cmd.addUShort(getIntParam(pco_cdi_mode));
    cmd.addUShort(0);
    rsp.setExpCode(0x3091);
    rsp.setErrCode(0x30D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_cdi_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set pco_dnsu_mode

  if ((function == pco_dnsu_mode || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x2D11);
    cmd.addUShort(getIntParam(pco_dnsu_mode));
    cmd.addUShort(0);
    rsp.setExpCode(0x2D91);
    rsp.setErrCode(0x2DD1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_dnsu_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  // Set pco_dnsu_mode

  if ((function == pco_dnsu_init_mode || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_dimax)) {
    cmd.setCode(0x2E11);
    cmd.addUShort(getIntParam(pco_dnsu_init_mode));
    cmd.addUShort(0);
    rsp.setExpCode(0x2E91);
    rsp.setErrCode(0x2ED1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_dnsu_init_mode, rsp.getUShort(2));
      // my_addriver->callback(general, this);

      lf.log("Init DNSU Mode");
    }
  }

  // set 16bit 2 12 bit lookup table

  if ((function == pco_1612_lookup || function == pco_setallparams) &&
      (getIntParam(pco_which_camera) == pco_edge)) {
    cmd.setCode(0x3311);
    cmd.addUShort(getIntParam(pco_1612_lookup));
    cmd.addUShort(0);
    rsp.setExpCode(0x3391);
    rsp.setErrCode(0x33D1);
    if (doSerialCommand(cmd, rsp, 0, 0) == 0) {
      setIntegerParam(pco_1612_lookup, rsp.getUShort(2));
      // my_addriver->callback(general, this);
    }
  }

  return (0);
}

/*********************************************************************************************
 *
 *
 *
 *********************************************************************************************/

int pco::getPcoSensorParams(void) {
  pco_command cmd;
  pco_response rsp;

  // Get Camera Description
  cmd.setCode(0x0111);
  rsp.setExpCode(0x0191);
  rsp.setErrCode(0x01D1);
  doSerialCommand(cmd, rsp, (unsigned char *)&pco_desc, sizeof(pco_desc));

  setIntegerParam(ADMaxSizeX, rsp.getUShort(6));
  setIntegerParam(ADMaxSizeY, rsp.getUShort(8));

  // Get Sensor Format
  cmd.setCode(0x1411);
  rsp.setExpCode(0x1491);
  rsp.setErrCode(0x14D1);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_sensor_format, rsp.getUShort(2));

  // Get ROI
  cmd.setCode(0x0211);
  rsp.setExpCode(0x0291);
  rsp.setErrCode(0x02D1);
  doSerialCommand(cmd, rsp, 0, 0);
  setIntegerParam(ADMinX, rsp.getUShort(2));
  setIntegerParam(ADMinY, rsp.getUShort(4));

  setIntegerParam(ADSizeX, 1 + rsp.getUShort(6) - rsp.getUShort(2));
  setIntegerParam(ADSizeY, 1 + rsp.getUShort(8) - rsp.getUShort(4));

  // Get Binning
  cmd.setCode(0x0411);
  rsp.setExpCode(0x0491);
  rsp.setErrCode(0x04D1);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(ADBinX, rsp.getUShort(2));
  setIntegerParam(ADBinY, rsp.getUShort(4));

  // Get Pixelrate
  cmd.setCode(0x0611);
  rsp.setExpCode(0x0691);
  rsp.setErrCode(0x06D1);
  doSerialCommand(cmd, rsp, 0, 0);

  setIntegerParam(pco_pixelrate, rsp.getULong(2));

  if ((getIntParam(pco_which_camera) == pco_dimax)) {
    // Get Conversion Factor
    cmd.setCode(0x0811);
    rsp.setExpCode(0x0891);
    rsp.setErrCode(0x08D1);
    doSerialCommand(cmd, rsp, 0, 0);

    // in elec/count. need 100 in there...

    setDoubleParam(ADGain, ((double)(rsp.getUShort(2))) / 100.0);

    // Get doub img mode
    cmd.setCode(0x0a11);
    rsp.setExpCode(0x0a91);
    rsp.setErrCode(0x0aD1);
    doSerialCommand(cmd, rsp, 0, 0);

    setIntegerParam(pco_doub_img_mode, rsp.getUShort(2));  // 1 =on 0=off
  }
  if ((getIntParam(pco_which_camera) == pco_other)) {
    // Get cam_adc_op
    cmd.setCode(0x0c11);
    rsp.setExpCode(0x0c91);
    rsp.setErrCode(0x0cD1);
    doSerialCommand(cmd, rsp, 0, 0);

    // 1 for 1ADC, 2 for 2ADCs
    setIntegerParam(pco_adc_mode, rsp.getUShort(2));
  }
  // Get cam_ir_sense
  // cmd.setCode(0x0e11);
  // rsp.setExpCode(0x0e91);
  // rsp.setErrCode(0x0eD1);
  // doSerialCommand(cmd,rsp,0,0);

  if ((getIntParam(pco_which_camera) == pco_edge)) {
    // Get cam_stpt_temp
    cmd.setCode(0x1011);
    rsp.setExpCode(0x1091);
    rsp.setErrCode(0x10D1);
    doSerialCommand(cmd, rsp, 0, 0);

    // signed temp in C
    setIntegerParam(pco_temp_setpt, rsp.getShort(2));
  }

  if (getIntParam(pco_which_camera) == pco_other) {
    // Get cam_offsmode
    cmd.setCode(0x1211);
    rsp.setExpCode(0x1291);
    rsp.setErrCode(0x12D1);
    doSerialCommand(cmd, rsp, 0, 0);
    // 0 auto,1 off
    setIntegerParam(pco_offset_mode, rsp.getShort(2));
  }
  if (getIntParam(pco_which_camera) == pco_dimax) {
    // Get cam_noise_filt_mode
    cmd.setCode(0x1911);
    cmd.addShort(1);  // mode 0=auto, 1 - OFF
    rsp.setExpCode(0x1991);
    rsp.setErrCode(0x19D1);
    doSerialCommand(cmd, rsp, 0, 0);
    // 0,1,2 = off, on, onHotPix
    setIntegerParam(pco_noise_filt_mode, rsp.getShort(2));
  }
  if (getIntParam(pco_which_camera) == pco_edge) {
    // Get pco_hotpix_corr
    cmd.setCode(0x1E11);
    rsp.setExpCode(0x1E91);
    rsp.setErrCode(0x1ED1);
    doSerialCommand(cmd, rsp, 0, 0);
    // 0,1,2 = off, on, onHotPix
    setIntegerParam(pco_hotpix_corr, rsp.getShort(2));

    // Get pco_1612_lookup
    cmd.setCode(0x3211);
    rsp.setExpCode(0x3291);
    rsp.setErrCode(0x32D1);
    doSerialCommand(cmd, rsp, 0, 0);
    // 0,1,2 = off, on, onHotPix
    setIntegerParam(pco_1612_lookup, rsp.getShort(2));
  }

  if (getIntParam(pco_which_camera) != pco_edge) {
    // Get pco_cdi_mode
    cmd.setCode(0x2F11);
    cmd.addShort(1);  // mode 0=auto, 1 - OFF
    rsp.setExpCode(0x2F91);
    rsp.setErrCode(0x2FD1);
    doSerialCommand(cmd, rsp, 0, 0);
    // 0,1,2 = off, on, onHotPix
    setIntegerParam(pco_cdi_mode, rsp.getShort(2));
  }
  if (getIntParam(pco_which_camera) != pco_edge) {
    // Get pco_dnsu_mode
    cmd.setCode(0x2C11);
    cmd.addShort(1);  // mode 0=auto, 1 - OFF
    rsp.setExpCode(0x2C91);
    rsp.setErrCode(0x2CD1);
    doSerialCommand(cmd, rsp, 0, 0);
    // 0,1,2 = off, on, onHotPix
    setIntegerParam(pco_dnsu_mode, rsp.getShort(2));
  }

  return (0);
}

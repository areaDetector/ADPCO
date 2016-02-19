/*******************************************************************************
 *
 *    siswSerialPort.h
 *
 *    Author: Tim Madden
 *    Date:    8/01/06
 *    Project: CCD Detector DAQ.
 *
 *
 ******************************************************************************/

/*
 * Include files.
 */
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "time.h"
#include "comportInterface.h"
#include "ccd_exception.h"
// si software serial port calls

#ifdef USE_SISW

//#include "clser.h"
/*
 * Double incluson protection.
 */
#ifndef _siswSerialPort_H
#define _siswSerialPort_H

/*
         * Class for
         */

class siswSerialPort : public comportInterface {
 public:
  siswSerialPort(char *name);
  ~siswSerialPort();
  /*
   * ports we can write to
   */

  virtual void open(void);
  virtual void open(int baud, int parity, int nbits, int nstop);

  virtual void open(int baud, int parity, int nbits, int nstop, int rdtimeout);

  virtual void write(unsigned char *buffer, int length);
  virtual void read(unsigned char *buffer, int length);
  virtual void write(unsigned char c);
  virtual unsigned char read(void);
  virtual void close(void);
  virtual void flush(void);
  virtual void clearPipe();

  virtual void setPortName(char *n){};

  virtual void wait(int us);

  // for timing things. call tic. toc returns time in s since last tic.
  virtual void tic();
  virtual double toc();

 protected:
  void readDll(void);

  bool is_open;

  double currenttime, elapsedtime;

  void *serial_ref;
  char mesgx[512];
};

#endif
#endif

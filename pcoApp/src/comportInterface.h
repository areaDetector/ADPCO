/**
 * Interface to a com port, on a grabber.\
 * We inherite this with any class to represent the serial port ona grabber. 
 * Pure virtual class.
 *
 *@author Timothy Madden
 *@date 2005
 *
 */

/*
 * Include files.
 */
/*
 * Double incluson protection.
 */
#ifndef _comportInterface_H
#define _comportInterface_H

/*
                 * Class for
                 */

class comportInterface {
 public:
  virtual void open(void) = 0;
  virtual void open(int baud, int parity, int nbits, int nstop) = 0;

  virtual void open(int baud, int parity, int nbits, int nstop,
                    int rdtimeout) = 0;

  virtual void write(unsigned char *buffer, int length) = 0;
  virtual void read(unsigned char *buffer, int length) = 0;
  virtual void write(unsigned char c) = 0;
  virtual unsigned char read(void) = 0;
  virtual void close(void) = 0;
  virtual void flush(void) = 0;
  virtual void clearPipe() = 0;
  virtual void setPortName(char *n) = 0;

  virtual void wait(int us) = 0;

  // for timing things. call tic. toc returns time in s since last tic.
  virtual void tic() = 0;
  virtual double toc() = 0;
};

#endif

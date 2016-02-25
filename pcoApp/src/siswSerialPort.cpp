/**
 * Class derived from interface comportInterface to hide all vendor api calls
 * to the serial port on grabber. 
 * 
 *@author Tim Madden
 *@date 2012
 */

#include "siswSerialPort.h"
//#include <windows.h>

#ifdef USE_SISW

#define CL_OK 0
#define CL_ERR_NO_ERR 0
#define CL_ERR_BUFFER_TOO_SMALL -10001
#define CL_ERR_MANU_DOES_NOT_EXIST -10002
#define CL_ERR_PORT_IN_USE -10003
#define CL_ERR_TIMEOUT -10004
#define CL_ERR_INVALID_INDEX -10005
#define CL_ERR_INVALID_REFERENCE -10006
#define CL_ERR_ERROR_NOT_FOUND -10007
#define CL_ERR_BAUD_RATE_NOT_SUPPORTED -10008
#define CL_ERR_FUNCTION_NOT_FOUND -10099

#define CL_BAUDRATE_9600 1
#define CL_BAUDRATE_19200 2
#define CL_BAUDRATE_38400 4
#define CL_BAUDRATE_57600 8
#define CL_BAUDRATE_115200 16
#define CL_BAUDRATE_230400 32
#define CL_BAUDRATE_460800 64
#define CL_BAUDRATE_921600 128

static int (*clSerialInit)(unsigned long serialIndex, void** serialRefPtr);
static int (*clSerialRead)(void* serialRef, char* buffer,
                           unsigned long* bufferSize,
                           unsigned long serialTimeout);
static int (*clSerialWrite)(void* serialRef, char* buffer,
                            unsigned long* bufferSize,
                            unsigned long SerialTimeout);
static void (*clSerialClose)(void* serialRef);

static int (*clSetParity)(void* serialRef, unsigned int parityOn);
static int (*clGetManufacturerInfo)(char* ManufacturerName,
                                    unsigned int* bufferSize,
                                    unsigned int* version);
static int (*clGetNumSerialPorts)(unsigned int* numSerialPorts);
static int (*clGetSerialPortIdentifier)(unsigned int serialIndex, char* portID,
                                        unsigned int* bufferSize);
static int (*clGetNumBytesAvail)(void* serialRef, unsigned int* bufferSize);
static int (*clFlushPort)(void* serialRef);
static int (*clGetSupportedBaudRates)(void* serialRef, unsigned int* baudRates);
static int (*clSetBaudRate)(void* serialRef, unsigned int baudRate);
static int (*clGetErrorText)(int errorCode, char* errorText,
                             unsigned int* errorTextSize);

 /**
 * Read vendor dll for serial port functions. In linux it is an so file. 
 */

void siswSerialPort::readDll(void) {
#ifndef _WIN32
#define HMODULE void *
#define LoadLibrary dlopen
#define GetProcAddress dlsym
#define __cdecl
#define DWORD unsigned long
#endif

  HMODULE hMod;

  printf("Loading C:/CameraLink/Serial/clsersis.dll\n");

#ifdef _WIN32
  hMod = LoadLibrary("C:/CameraLink/Serial/clsersis.dll");
#else
  hMod = LoadLibrary("libclserme4.so", RTLD_NOW);
#endif

  if (hMod == NULL) {
    printf("Could not find camlink serial lib\n");
    exit(-1);
  }

  clSerialInit =
      (int(__cdecl*)(unsigned long serialIndex,
                     void** serialRefPtr))GetProcAddress(hMod, "clSerialInit");
  clSerialRead = (int(
      __cdecl*)(void* serialRef, char* buffer, unsigned long* bufferSize,
                unsigned long serialTimeout))GetProcAddress(hMod,
                                                            "clSerialRead");
  clSerialWrite = (int(
      __cdecl*)(void* serialRef, char* buffer, unsigned long* bufferSize,
                unsigned long SerialTimeout))GetProcAddress(hMod,
                                                            "clSerialWrite");
  clSerialClose =
      (void(__cdecl*)(void* serialRef))GetProcAddress(hMod, "clSerialClose");

  clSetParity =
      (int(__cdecl*)(void* serialRef, unsigned int parityOn))GetProcAddress(
          hMod, "clSetParity");
  clGetManufacturerInfo = (int(
      __cdecl*)(char* ManufacturerName, unsigned int* bufferSize,
                unsigned int* version))GetProcAddress(hMod,
                                                      "clGetManufacturerInfo");
  clGetNumSerialPorts =
      (int(__cdecl*)(unsigned int* numSerialPorts))GetProcAddress(
          hMod, "clGetNumSerialPorts");
  clGetSerialPortIdentifier =
      (int(__cdecl*)(unsigned int serialIndex, char* portID,
                     unsigned int* bufferSize))
          GetProcAddress(hMod, "clGetSerialPortIdentifier");
  clGetNumBytesAvail =
      (int(__cdecl*)(void* serialRef, unsigned int* bufferSize))GetProcAddress(
          hMod, "clGetNumBytesAvail");
  clFlushPort =
      (int(__cdecl*)(void* serialRef))GetProcAddress(hMod, "clFlushPort");
  clGetSupportedBaudRates =
      (int(__cdecl*)(void* serialRef, unsigned int* baudRates))GetProcAddress(
          hMod, "clGetSupportedBaudRates");
  clSetBaudRate =
      (int(__cdecl*)(void* serialRef, unsigned int baudRate))GetProcAddress(
          hMod, "clSetBaudRate");
  clGetErrorText = (int(__cdecl*)(int errorCode, char* errorText,
                                  unsigned int* errorTextSize))
      GetProcAddress(hMod, "clGetErrorText");

  if (clSerialInit == NULL) {
    printf("Cound not find clSerialInit in dll\n");
    exit(-1);
  }
}

 /**
 * Make serial port obhect. name is like "COM1" but not used.  
 */

 siswSerialPort::siswSerialPort(char* name) {
  is_open = false;
  readDll();
}

 /**
 *  close serial port and destrpy objhect. 
 */

 siswSerialPort::~siswSerialPort() {
  if (is_open) close();
}

 /**
 *  open serial port
 * baud- baud rate.
 * parity - 1/0
 * nbuts -7/8
 * nstop - 0/1
 * rdtimeout- millisec for port to time out on reading. 
 */

 void siswSerialPort::open(int baud, int parity, int nbits, int nstop,
                          int rdtimeout) {
  this->open(baud, parity, nbits, nstop);
}

void siswSerialPort::open(int baud, int parity, int nbits, int nstop) {
  char errmess[128];
  DWORD errcode;
  int baud_index;
  int cardnum = 0;

  if (is_open) {
#ifdef THROWS
    throw ccd_exception("siswSerialPort::open Already Open");
#endif
  }

  switch (baud) {
    case 9600:
      baud_index = CL_BAUDRATE_9600;
      break;
    case 19200:
      baud_index = CL_BAUDRATE_19200;
      break;
    case 115200:
      baud_index = CL_BAUDRATE_115200;
      break;
    default:
      baud_index = CL_BAUDRATE_9600;
      break;
  }

  int stat = -1;
  while (stat < 0) {
    stat = clSerialInit(cardnum, &serial_ref);
    printf("SiSW Card Num %d,  OPen Serial Status %i SerialRefPtr %d \n",
           cardnum, stat, serial_ref);

    if (stat != 0) {
      unsigned int xx = 512;
      clGetErrorText(stat, mesgx, &xx);
      printf("ERROR: %s\n", mesgx);
    }
    cardnum++;

    if (cardnum > 50) {
#ifdef THROWS
      throw ccd_exception("cannot open SISW card");
#else
      printf("cannot open SISW card\n");
#endif
    }
  }

  stat = clSetBaudRate(serial_ref, baud_index);

  if (stat != 0) {
    unsigned int xx = 512;
    clGetErrorText(stat, mesgx, &xx);
    printf("ERROR: %s\n", mesgx);
  }

  stat = clSetParity(serial_ref, parity);
  if (stat != 0) {
    unsigned int xx = 512;
    clGetErrorText(stat, mesgx, &xx);
    printf("ERROR: %s\n", mesgx);
  }

  is_open = true;
}

 /**
 *  open serial port to default settings
 */
void siswSerialPort::open(void) { open(115200, 0, 8, 1); }

 /**
 * Write one byte to ser port. 
 */
void siswSerialPort::write(unsigned char c) {
  char buffer[10];

  buffer[0] = c;
  write((unsigned char*)buffer, 1);
}

 /**
 * Write buffer of chars of length to serial port.  
 */

void siswSerialPort::write(unsigned char* buffer, int length) {
  int l;

  l = length;

  if (is_open) {
    int stat;

    stat = clSerialWrite(serial_ref, (char*)buffer, (unsigned long*)&l, 200);
    if (stat != 0) {
      unsigned int xx = 512;
      clGetErrorText(stat, mesgx, &xx);
      printf("ERROR: %s\n", mesgx);
    }

  } else {
#ifdef THROWS
    throw ccd_exception("siswSerialPort::write  NOT OPEN");
#else
    printf("siswSerialPort::write  NOT OPEN\n");
#endif
  }
}

 /**
 * read one byte from ser port and ret. 
 */
 
unsigned char siswSerialPort::read(void) {
  char buffer[10];
  if (is_open) {
    read((unsigned char*)buffer, 1);
  } else {
#ifdef THROWS
    throw ccd_exception("siswSerialPort::read Port is closed");
#else
    printf("siswSerialPort::read Port is closed\n");
#endif
  }
  return (buffer[0]);
}

 /**
 * read buffer of length from serial port and ret to supplied pointer.  
 */

void siswSerialPort::read(unsigned char* buffer, int length) {
  char errmess[128];
  DWORD errcode;
  char xx = 'R';
  int readcnt;
  int lensave = length;
  // recordfile.puts(&xx,1);
  if (is_open) {
    int stat;

    stat =
        clSerialRead(serial_ref, (char*)buffer, (unsigned long*)&length, 200);
    readcnt = 0;

    while (stat == -10004 && readcnt < 5) {
      // on timeout try again.
      length = lensave;
      stat =
          clSerialRead(serial_ref, (char*)buffer, (unsigned long*)&length, 200);
      readcnt++;
    }
    if (stat != 0) {
      unsigned int xx = 512;
      clGetErrorText(stat, mesgx, &xx);
      printf("Read Error: stat= %d req len=%d ret length = %d \n", stat,
             lensave, length);
    }

  } else {
#ifdef THROWS
    throw ccd_exception("siswSerialPort::read Port is closed");
#else
    printf("siswSerialPort::read Port is closed\n");
#endif
  }
}
void siswSerialPort::flush(void) {
  int stat;

  //!!disable this function
  return;

  if (is_open) {
    stat = clFlushPort(serial_ref);
    if (stat != 0) {
      unsigned int xx = 512;
      clGetErrorText(stat, mesgx, &xx);
      printf("ERROR: %s\n", mesgx);
    }

  } else {
#ifdef THROWS
    throw ccd_exception("siswSerialPort::flush Port is closed");
#else
    printf("siswSerialPort::flush Port is closed\n");
#endif
  }
}

void siswSerialPort::close() {
  if (is_open) {
    clSerialClose(serial_ref);
    is_open = false;
  } else {
#ifdef THROWS
    throw ccd_exception("siswSerialPort::close Port is not open");
#else
    printf("siswSerialPort::close Port is not open\n");
#endif
  }
}

 /**
 *  read ser port until no chars left. for clearing errors. 
 */

void siswSerialPort::clearPipe(void) {
  char ret;
  int counter = 0;
  ret = this->read();
  while (ret != EOF) {
    ret = this->read();
    counter++;
    if (counter > 10) break;
  }
}

 /**
 *  Wait so many microsec, don in for loop and not sleep.
 */
void siswSerialPort::wait(int us) {
  int t0, t1;

  t0 = clock();

  t1 = us * CLOCKS_PER_SEC / 1000000;

  while ((clock() - t0) < t1) {
  }
}

 /**
 * Start a stop watch, like matlab tic.
 */
void siswSerialPort::tic() {
  currenttime = (double)clock() / (double)CLOCKS_PER_SEC;
}

 /**
 *  Read back stop watch in sec. Like matlab toc.
 */
 
double siswSerialPort::toc() {
  elapsedtime = ((double)clock() / (double)CLOCKS_PER_SEC) - currenttime;
  return (elapsedtime);
}
#endif

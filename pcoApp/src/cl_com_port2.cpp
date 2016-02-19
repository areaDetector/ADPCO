
#ifdef _WIN32

#include "cl_com_port2.h"

cl_com_port::cl_com_port(char *name) {
  strcpy(port_name, name);
  is_open = false;
}
cl_com_port::~cl_com_port() {
  if (is_open) close();
}

void cl_com_port::setPortName(char *n) { strcpy(port_name, n); }

void cl_com_port::open(int baud, int parity, int nbits, int nstop) {
  char errmess[128];
  DWORD errcode;

  if (is_open) {
    return;
  }

  if ((comport =
           CreateFile(port_name,                     // open com5:
                      GENERIC_READ | GENERIC_WRITE,  // for reading and writing
                      0,                             // exclusive access
                      NULL,                          // no security attributes
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,  // was 0
                      NULL)) == INVALID_HANDLE_VALUE)

  {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open CreateFile ERROR code  %i\n",
            (int)errcode);
#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }

  is_open = true;

  // Set Port parameters.
  // Make a call to GetCommState() first in order to fill
  // the comSettings structure with all the necessary values.
  // Then change the ones you want and call SetCommState().
  SecureZeroMemory(&comSettings, sizeof(DCB));
  comSettings.DCBlength = sizeof(DCB);
  bStatus = GetCommState(comport, &comSettings);
  if (bStatus == 0) {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open secureZeroMemory ERROR code  %i\n",
            (int)errcode);
#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }

  comSettings.BaudRate = baud;
  comSettings.StopBits = nstop;
  comSettings.ByteSize = nbits;
  comSettings.Parity = parity;
  // comSettings.fParity  = FALSE;
  bStatus = SetCommState(comport, &comSettings);

  if (bStatus == 0) {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open setcommstate ERROR code  %i\n",
            (int)errcode);

#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }
  // Set timeouts in milliseconds

  CommTimeouts.ReadIntervalTimeout = 500;
  CommTimeouts.ReadTotalTimeoutMultiplier = 10;
  CommTimeouts.ReadTotalTimeoutConstant = 500;
  CommTimeouts.WriteTotalTimeoutMultiplier = 10;
  CommTimeouts.WriteTotalTimeoutConstant = 500;
  bStatus = SetCommTimeouts(comport, &CommTimeouts);

  if (bStatus == 0) {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open setCommtimeouts ERROR code  %i\n",
            (int)errcode);
#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }
}

void cl_com_port::open(int baud, int parity, int nbits, int nstop,
                       int rdtimeout) {
  char errmess[128];
  DWORD errcode;

  if (is_open) {
    return;
  }

  if ((comport =
           CreateFile(port_name,                     // open com5:
                      GENERIC_READ | GENERIC_WRITE,  // for reading and writing
                      0,                             // exclusive access
                      NULL,                          // no security attributes
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,  // was 0
                      NULL)) == INVALID_HANDLE_VALUE)

  {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open CreateFile ERROR code  %i\n",
            (int)errcode);
#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }

  is_open = true;

  // Set Port parameters.
  // Make a call to GetCommState() first in order to fill
  // the comSettings structure with all the necessary values.
  // Then change the ones you want and call SetCommState().
  SecureZeroMemory(&comSettings, sizeof(DCB));
  comSettings.DCBlength = sizeof(DCB);
  bStatus = GetCommState(comport, &comSettings);
  if (bStatus == 0) {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open secureZeroMemory ERROR code  %i\n",
            (int)errcode);
#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }

  comSettings.BaudRate = baud;
  comSettings.StopBits = nstop;
  comSettings.ByteSize = nbits;
  comSettings.Parity = parity;
  // comSettings.fParity  = FALSE;
  bStatus = SetCommState(comport, &comSettings);

  if (bStatus == 0) {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open setcommstate ERROR code  %i\n",
            (int)errcode);

#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }
  // Set timeouts in milliseconds

  CommTimeouts.ReadIntervalTimeout = rdtimeout;
  CommTimeouts.ReadTotalTimeoutMultiplier = 1;
  CommTimeouts.ReadTotalTimeoutConstant = 500;
  CommTimeouts.WriteTotalTimeoutMultiplier = 10;
  CommTimeouts.WriteTotalTimeoutConstant = 500;
  bStatus = SetCommTimeouts(comport, &CommTimeouts);

  if (bStatus == 0) {
    errcode = GetLastError();
    sprintf(errmess, "cl_com_port::open setCommtimeouts ERROR code  %i\n",
            (int)errcode);
#ifdef THROWS
    throw ccd_exception(errmess);
#else
    printf(errmess);
#endif
  }
}

void cl_com_port::open(void) {
  if (is_open) {
    return;
  }

  if ((comport =
           CreateFile(port_name,                     // open com5:
                      GENERIC_READ | GENERIC_WRITE,  // for reading and writing
                      0,                             // exclusive access
                      NULL,                          // no security attributes
                      OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL,  // was 0
                      NULL)) == INVALID_HANDLE_VALUE)

  {
#ifdef THROWS
    throw ccd_exception("cl_com_port::open cannot open for some reason");
#else
    printf("cl_com_port::open cannot open for some reason\n");
#endif
  }

  is_open = true;

  // Set Port parameters.
  // Make a call to GetCommState() first in order to fill
  // the comSettings structure with all the necessary values.
  // Then change the ones you want and call SetCommState().
  SecureZeroMemory(&comSettings, sizeof(DCB));
  comSettings.DCBlength = sizeof(DCB);
  bStatus = GetCommState(comport, &comSettings);
  if (bStatus == 0) {
#ifdef THROWS
    throw ccd_exception("cl_com_port::open GetCommState failed.");
#else
    printf("cl_com_port::open GetCommState failed.\n");
#endif
  }

  comSettings.BaudRate = CBR_115200;
  comSettings.StopBits = ONESTOPBIT;
  comSettings.ByteSize = 8;
  comSettings.Parity = NOPARITY;
  // comSettings.fParity  = FALSE;
  bStatus = SetCommState(comport, &comSettings);

  if (bStatus == 0) {
#ifdef THROWS
    throw ccd_exception("cl_com_port::open cannot set baud rate etc.");
#else
    printf("cl_com_port::open cannot set baud rate etc.\n");
#endif
  }
  // Set timeouts in milliseconds

  CommTimeouts.ReadIntervalTimeout = 15;
  CommTimeouts.ReadTotalTimeoutMultiplier = 1;
  CommTimeouts.ReadTotalTimeoutConstant = 250;
  CommTimeouts.WriteTotalTimeoutMultiplier = 1;
  CommTimeouts.WriteTotalTimeoutConstant = 250;
  bStatus = SetCommTimeouts(comport, &CommTimeouts);

  if (bStatus == 0) {
#ifdef THROWS
    throw ccd_exception("cl_com_port::open cannot set timeouts");
#else
    printf("cl_com_port::open cannot set timeouts\n");
#endif
  }
}

void cl_com_port::write(unsigned char c) {
  this->write(&c, 1);
  flush();
}

void cl_com_port::write(unsigned char *buffer, int length) {
  char errmess[128];
  DWORD errcode;
  // recordfile.puts(buffer,length);
  if (is_open) {
    bStatus = WriteFile(comport,         // Handle
                        buffer,          // Outgoing data
                        length,          // Number of bytes to write
                        &bytes_written,  // Number of bytes written
                        NULL);

    if (bStatus == 0) {
      errcode = GetLastError();
      sprintf(errmess, "cl_com_port::write cannot send data:  %i\n",
              (int)errcode);
#ifdef THROWS
      throw ccd_exception(errmess);
#else
      printf(errmess);
#endif
    }

    if (bytes_written < length) {
#ifdef THROWS
      throw ccd_exception("cl_com_port::write Did not send all bytes.");
#else
      printf("cl_com_port::write Did not send all bytes.\n");
#endif
    }

  } else {
#ifdef THROWS
    throw ccd_exception("cl_com_port::write Port is closed.");
#else
    printf("cl_com_port::write Port is closed\n");
#endif
  }
}

unsigned char cl_com_port::read(void) {
  unsigned char buffer[10];
  read(buffer, 1);
  flush();
  return (buffer[0]);
}
void cl_com_port::read(unsigned char *buffer, int length) {
  char errmess[128];
  DWORD errcode;
  char xx = 'R';
  // recordfile.puts(&xx,1);
  if (is_open) {
    bStatus = ReadFile(comport,      // Handle
                       buffer,       // Incoming data
                       length,       // Number of bytes to read
                       &bytes_read,  // Number of bytes read
                       NULL);

    if (bStatus == 0) {
      errcode = GetLastError();
      sprintf(errmess, "cl_com_port::read cannot read data:  %i\n",
              (int)errcode);
#ifdef THROWS
      throw ccd_exception(errmess);
#else
      printf(errmess);
#endif
    }
    // we assume EOF here...
    if (length == 1 && bytes_read == 0) {
      buffer[0] = EOF;
    }
  } else {
#ifdef THROWS
    throw ccd_exception("cl_com_port::read Port is closed\n");
#else
    printf("cl_com_port::read Port is closed\n");
#endif
  }
}
void cl_com_port::flush(void) {
  if (is_open)
    FlushFileBuffers(comport);
  else {
#ifdef THROWS
    throw ccd_exception("cl_com_port::flush Port is closed");
#else
    printf("cl_com_port::flush Port is closed\n");
#endif
  }
}

void cl_com_port::close() {
  if (is_open) {
    CloseHandle(comport);
    is_open = false;
  }
}
/*
*
*/

void cl_com_port::clearPipe(void) {
  char ret;
  int counter = 0;
  ret = this->read();
  while (ret != EOF) {
    ret = this->read();
    counter++;
    if (counter > 10) break;
  }
}

/**************************************************************************
 *
 * wait x us
 *
 **************************************************************************/
void cl_com_port::wait(int us) {
  int t0, t1;

  t0 = clock();

  t1 = us * CLOCKS_PER_SEC / 1000000;

  while ((clock() - t0) < t1) {
  }
}

/**************************************************************************
 *
 * stat stop watch
 *
 **************************************************************************/
void cl_com_port::tic() {
  currenttime = (double)clock() / (double)CLOCKS_PER_SEC;
}

/**************************************************************************
 *
 * read stop watch
 *
 **************************************************************************/
double cl_com_port::toc() {
  elapsedtime = ((double)clock() / (double)CLOCKS_PER_SEC) - currenttime;
  return (elapsedtime);
}

#endif

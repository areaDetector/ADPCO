/*******************************************************************************
 *
 *	cl_com_port.h
 *
 *	Author: Tim Madden
 *	Date:	8/01/06
 *	Project: CCD Detector DAQ.
 *
 *
 ******************************************************************************/


/*
 * Include files.
 */
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "time.h"
#include "comportInterface.h"
#include "CCD_exception.h"
/*
 * Double incluson protection.
 */
#ifndef _CL_COM_PORT_H
	#define _CL_COM_PORT_H

/*
		 * Class for 
		 */

class cl_com_port : public comportInterface
		{
		public:	
			cl_com_port(char *name);
			~cl_com_port();
			/*
			 * ports we can write to
			 */

		
			

			virtual void open(void);
			virtual void open(
				int baud, 
				int parity, 
				int nbits, 
				int nstop);

			
			virtual void open(
				int baud, 
				int parity, 
				int nbits, 
				int nstop,
				int rdtimeout);

			virtual void write(unsigned char *buffer, int length);
			virtual void read(unsigned char *buffer, int length);
			virtual void write(unsigned char c);
			virtual unsigned char read(void);
			virtual void close(void);
			virtual void flush(void);
			virtual void clearPipe();

			virtual void setPortName(char *n);


			virtual void wait(int us);

			// for timing things. call tic. toc returns time in s since last tic.
			virtual void tic();
			virtual double toc();

		protected:

bool is_open;


  double currenttime,elapsedtime;

    char INBUFFER[500];

    char OUTBUFFER[20];

    DWORD        bytes_read;    // Number of bytes read from port

    DWORD        bytes_written;    // Number of bytes written to the port

    HANDLE       comport;  // Handle COM port

 int   bStatus;

    DCB          comSettings;          // Contains various port settings

    COMMTIMEOUTS CommTimeouts;
  
	char port_name[64];


		};

	

#endif

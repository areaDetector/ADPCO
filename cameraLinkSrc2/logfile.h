/*******************************************************************************
 *
 *	log_file.h
 *
 *	Author: Tim Madden
 *	Date:	7/27/03
 *	Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/


/*
 * Include files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Double incluson protection.
 */
#ifndef _LOG_FILE_H
	#define _LOG_FILE_H




/*******************************************************************************
 *
 *	Class LOG_FILE
 *
 *
 ******************************************************************************/

class log_file 
{
	public:
		// make file object.
		log_file(char* filename);

		// destroy file object.
		~log_file();

		void enableLog(bool is_en);
		// output to log ile.
		void log(char* message);
		void logNoDate(char* message);
		void puts(char* message,int len);
		void enablePrintf(bool is_pr);

	protected:

		enum {num_saved_files=5};
		
		// file pointer
		FILE *fp;
		char log_file_name[255];
		bool is_enabled;
		bool is_printf;

};

#endif


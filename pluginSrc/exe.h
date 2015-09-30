/*******************************************************************************
 *
 *	exe.h
 *
 *	Author: Tim Madden
 *	Date:	7/01/03
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
#include <string>



/*
 * Double incluson protection.
 */
#ifndef _EXE_H
	#define _EXE_H



#ifdef BUILD_DLL
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP __declspec(dllimport)
#endif


/*******************************************************************************
 *
 *	Class exe
 *
 *	Pass to thread object for execution of a specific task. This is an
 *  abstract class. We overload the main function. exe::main is executed.
 ******************************************************************************/

class DLLIMPEXP exe
{
	public:
		// make exe object.
		exe();

		// run this function.
		virtual int main(int arg);

		// another function that runs on parent thread and returns
		virtual int processCallback(int arg);

	protected:

};

#endif

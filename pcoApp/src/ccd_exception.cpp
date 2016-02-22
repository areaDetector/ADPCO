/**
 *  Class for storing errors from detector. Originally written for ccd cameras, but works for anything...
 *    
 *
 *@author Tim Madden
 *@date  June 2003
 *
 *
 */

/*
 * Include files.
 */

#include "ccd_exception.h"

/**
 * Constructor. default, puts "error" as a message. code is enum unmknown. 
 */
 
ccd_exception::ccd_exception() {
  strcpy(err, "error");
  code = unknown;
}

/**
 * Constructor to spec. error string and code as int. 
 */
 
ccd_exception::ccd_exception(error_code er, const char *mess) {
  strcpy(err, mess);
  code = er;
}

/**
 * construcor for speced error code. 
 */
 
ccd_exception::ccd_exception(error_code er) { code = er; }

/**
 * Constructor to spec error string. . 
 */
 
ccd_exception::ccd_exception(const char *x) {
  strcpy(err, x);
  code = unknown;
}

/**
 * Return error message. 
 */
 
char *ccd_exception::err_mess(void) { return err; }

/**
 * return error code.
 */
 
ccd_exception::error_code ccd_exception::getErrCode(void) { return code; }

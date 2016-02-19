/*******************************************************************************
 *
 *    ccd_exception.cpp
 *
 *    Author: Tim Madden
 *    Date:    6/17/03
 *    Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/

/*
 * Include files.
 */

#include "ccd_exception.h"

ccd_exception::ccd_exception() {
  strcpy(err, "error");
  code = unknown;
}
// make default err message
ccd_exception::ccd_exception(error_code er, const char *mess) {
  strcpy(err, mess);
  code = er;
}
// make default err message
ccd_exception::ccd_exception(error_code er) { code = er; }
// make default err message

ccd_exception::ccd_exception(const char *x) {
  strcpy(err, x);
  code = unknown;
}

char *ccd_exception::err_mess(void) { return err; }

// Return error message.
ccd_exception::error_code ccd_exception::getErrCode(void) { return code; }

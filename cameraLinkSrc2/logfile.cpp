/*******************************************************************************
 *
 *    log_file.cpp
 *
 *    Author: Tim Madden
 *    Date:    7/27/03
 *    Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/

/*
 * Include files.
 */
#include <time.h>
#include <stdio.h>

#include "logfile.h"

#ifdef _WIN32
#include <io.h>
#define access _access
#define F_OK 0

#else

#include <unistd.h>
#endif

/*******************************************************************************
 *
 *    Class LOG_FILE
 *
 *
 ******************************************************************************/
log_file::log_file(char* filename) {
  time_t tp;
  char fnameto[256];
  char fnamefrom[256];

  int k;

  // we save old log files by renaming them... file->file1, ...file3->file4
  // etc...
  // rename only if they exist.
  // for 5 files,, k=4,3,2,1
  // we look for fromfile, 3,2,1,_  and rename to 4,3,2,1
  for (k = num_saved_files - 1; k >= 1; k = k - 1) {
    // make new filename
    sprintf(fnameto, "save%d_%s", k, filename);
    if (k > 1)
      sprintf(fnamefrom, "save%d_%s", k - 1, filename);
    else
      sprintf(fnamefrom, "%s", filename);

    // check if file exists, if so rename it
    if (access(fnamefrom, F_OK) != -1) {
      // if name to exist, then delete it
      if (access(fnameto, F_OK) != -1) remove(fnameto);

      rename(fnamefrom, fnameto);
      printf("logfile::logfile cp %s to %s\n", fnamefrom, fnameto);
    }
  }

  strcpy(log_file_name, filename);

  fp = fopen(filename, "w");
  if (fp != 0) {
    time(&tp);
    fprintf(fp, "%s  -CCD Camera Log file opened. \n", ctime(&tp));

    fflush(fp);
    // fclose(fp);
    // fp=0;
  }
  is_enabled = true;
  is_printf = false;
}

/*******************************************************************************
 *
 *    Class LOG_FILE
 *
 *
 ******************************************************************************/
// destroy file object.
log_file::~log_file() {
  if (fp != 0) fclose(fp);
}

/*******************************************************************************
 *
 *    Class LOG_FILE
 *
 *
 ******************************************************************************/
// output to log ile.
void log_file::log(char* message) {
  time_t tp;

  if (is_enabled) {
    // fp = fopen(log_file_name,"a");
    if (fp != 0) {
      time(&tp);
      fprintf(fp, "%s  -%s \n", ctime(&tp), message);

      if (is_printf) printf("%s  -%s \n", ctime(&tp), message);

      fflush(fp);
      // fclose(fp);
      // fp=0;
    }
  }
}

/*******************************************************************************
 *
 *    Class LOG_FILE
 *
 *
 ******************************************************************************/
// output to log ile.
void log_file::logNoDate(char* message) {
  time_t tp;

  if (is_enabled) {
    // fp = fopen(log_file_name,"a");
    if (fp != 0) {
      time(&tp);
      fprintf(fp, "%s", message);
      if (is_printf) printf("%s", message);

      fflush(fp);
      // fclose(fp);
      // fp=0;
    }
  }
}

void log_file::puts(char* message, int len) {
  unsigned int ixx;
  unsigned char cxx;

  if (is_enabled) {
    // fp = fopen(log_file_name,"a");
    if (fp != 0) {
      for (int i = 0; i < len; i++) {
        cxx = (unsigned char)message[i];
        ixx = (unsigned short)cxx;
        fprintf(fp, "%x \n", ixx);
        if (is_printf) printf("%x \n", ixx);
      }
      fflush(fp);
      // fclose(fp);
      // fp=0;
    }
  }
}

void log_file::enableLog(bool is_en) { is_enabled = is_en; }

void log_file::enablePrintf(bool is_en) { is_printf = is_en; }

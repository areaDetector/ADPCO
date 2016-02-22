/**
 *
 *    Provide a log file to screen and files. writes log messages to fie with date and times. 
 *
 *@author  Tim Madden
 *@date 2003
 *
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

/**
 *
 *  constructor, give filename to write to. Like "C:/a/b/cc/dd.txt"
 * or linux name like "/hello/meme/logs/me.txt"
 *
 */
 
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


/**
 * destroy file object.
 */
 
log_file::~log_file() {
  if (fp != 0) fclose(fp);
}


/**
 * write message or string to log file w/ current date time. also writes to screen.
 */
 
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


/**
 * write string to logfile w/ no timestamp.
 */

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

/**
 * low level write to log file. string and lengt of string.
 */

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

/**
 * True.False to enable or disable logs to the file.
 */

void log_file::enableLog(bool is_en) { is_enabled = is_en; }

/**
 * true.false to enable print logs to screen as well as file. 
 */
 
void log_file::enablePrintf(bool is_en) { is_printf = is_en; }

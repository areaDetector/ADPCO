/* mcaMain */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "epicsExit.h"

#include "iocsh.h"
#include <epicsThread.h>
// defined in legacystubs.c
// neded for VS2015
void init_legacy(void);

int main(int argc, char *argv[]) {
  init_legacy();

  if (argc >= 2) {
    iocsh(argv[1]);
    epicsThreadSleep(.2);
  }
  iocsh(NULL);
  epicsExit(0);
  return (0);
}

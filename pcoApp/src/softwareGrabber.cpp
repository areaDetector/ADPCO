 /**
 * Virtual grabber. If no hardware grabber in system, build with this grabber SW.
 * Software pretends there is a grabber and sends fake images. 
 *@author Tim Madden 
 * @date 2010
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "softwareGrabber.h"
#include <string.h>

#include <epicsTime.h>
#include <epicsThread.h>
softwareGrabber::softwareGrabber() {
  m_grab = 0;

  m_size_x = 1024;
  m_size_y = 1024;

  for (int k = 0; k < nimages; k++) images[k] = new unsigned short[imgpixels];

  imgcnt = 0;
}

int softwareGrabber::getGrabberType() { return (gSOFTWARE); }

void softwareGrabber::GetSignalStatus() {}

bool softwareGrabber::CreateObjects() { return (true); }

bool softwareGrabber::DestroyObjects() { return (true); }

bool softwareGrabber::DestroyObjectsNoDelete() { return (true); }

bool softwareGrabber::initialize(int size_x, int size_y) {
  this->initialize(size_x, size_y, true);

  return (true);
}

// use to override the image size in the ccf file.
bool softwareGrabber::initialize(int size_x, int size_y, bool is_force_size) {
  m_size_x = size_x;
  m_size_y = size_y;

  int k;

  for (k = 0; k < nimages; k++) {
    int row = 1;

    for (int p = 0; p < m_size_x * m_size_y; p++) {
      images[k][p] = rand() & 100 + row;

      row = k / m_size_x;
    }
  }

  return (true);
}

void softwareGrabber::setConfigFileName(char *name) {}

// set CC pins on the card.
void softwareGrabber::setPin(char *pinstr, int val) {}

void softwareGrabber::snap() {}

void softwareGrabber::grab() { m_grab = 1; }

void softwareGrabber::freeze() {}

void softwareGrabber::abort() { m_grab = 0; }

bool softwareGrabber::isFrameAvailable(void) { return (true); }

bool softwareGrabber::getFrame(void *mem_ptr) {
  unsigned int timestamp;

  this->getFrame(mem_ptr, &timestamp);
  return (true);
}

bool softwareGrabber::getFrame(void *mem_ptr, unsigned int *timestamp) {
  int nbytes = m_size_x * m_size_y * 2;

  this->getFrame(mem_ptr, timestamp, nbytes);
  return (true);
}

bool softwareGrabber::getFrame(void *copy_memory,
                               unsigned int *coreco_timestamp, int nbytes) {
  if (m_grab == 0) return (false);

  *coreco_timestamp = m_timestamp;
  m_timestamp++;

  // unsigned short *smem=(unsigned short*)copy_memory;

  memcpy(copy_memory, images[imgcnt], nbytes);

  imgcnt++;
  imgcnt = imgcnt % nimages;

  epicsThreadSleep(0.01);

  return (true);
}

bool softwareGrabber::isMissedFrame(void) { return (false); }

void softwareGrabber::clearMissedFrames(void) {}

long softwareGrabber::getTotalMissedFrames(void) { return (0); }

long softwareGrabber::getRecentMissedFrames(void) { return (0); }

void softwareGrabber::setNumBuffers(int b) {}

void softwareGrabber::makeView(void) {}

void softwareGrabber::setCamController(void *cc) {}

int softwareGrabber::getWidth(void) { return (m_size_x); }

int softwareGrabber::getHeight(void) { return (m_size_y); }

void softwareGrabber::setDoubleWidth(int isdw) {}

// clears any images left in buffer memory by resetting all counters to 0
void softwareGrabber::resetBufferCount(void) {}

int softwareGrabber::getNumFreeBuffers(void) { return (16); }

int softwareGrabber::getNumBuffers(void) { return (16); }

// inc missed frames counter
void softwareGrabber::incMissedFrames(void) {}

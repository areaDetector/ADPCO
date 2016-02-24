/**
 * Code for creating serial binary commands and parsing binary responses from 
 * PCO Cameras. Also has defines and a struct for long responses from the cameras.
 *
 *@author Timothy Madden
 *@date 2010
 */
 

// ------------------------------------------------------------------------ //
// -- Defines for Get Camera Type Command: -------------------------------- //
// ------------------------------------------------------------------------ //

// pco.camera types
#define CAMERATYPE_PCO1200HS 0x0100
#define CAMERATYPE_PCO1300 0x0200
#define CAMERATYPE_PCO1600 0x0220
#define CAMERATYPE_PCO2000 0x0240
#define CAMERATYPE_PCO4000 0x0260

// pco.1300 types

/**
 * Inline class  to create a binary command to be sent to pco cameras over serial port.
 *
 *
 *
 */

class pco_command {
 public:
 
 /*
  * Constructor for a new single binary command to camera.
  */
  
  pco_command() { setCode(0); };

 /*
  * Set command code, as defined in PCO documentation. this is main command word.
  */
  
  void setCode(unsigned short cx) {
    addr = 0;
    code = cx;
    // space for length.
    length = (unsigned short *)data;
    addUShort(0);
    calcCheckSum();
  }

 /*
  * commands are created by adding data one word at a time. Add unsigned short to command.
  */
  
  void addUShort(unsigned short msg) {
    unsigned short *ptr;
    ptr = (unsigned short *)(data + addr);
    *ptr = msg;
    addr += 2;
    calcCheckSum();
  };

 /*
  * add unsigned char to command
  */
  
  void addUChar(unsigned char msg) {
    unsigned char *ptr;
    ptr = (unsigned char *)(data + addr);
    *ptr = msg;
    addr += 1;
    calcCheckSum();
  };

 /*
  * Add signed short to command.
  */
  
  void addShort(short msg) {
    short *ptr;
    ptr = (short *)(data + addr);
    *ptr = msg;
    addr += 2;
    calcCheckSum();
  };

 /*
  * Add 32 but signed int to command.
  */
  
  void addLong(long msg) {
    long *ptr;
    ptr = (long *)(data + addr);
    *ptr = msg;
    addr += 4;
    calcCheckSum();
  };

 /*
  * Add unsigned int to the command.
  */
  
  void addULong(unsigned long msg) {
    unsigned long *ptr;
    ptr = (unsigned long *)(data + addr);
    *ptr = msg;
    addr += 4;
    calcCheckSum();
  };


 /*
  * Get raw data string into provided memory.
  */
  
  unsigned char *getData(void) { return ((unsigned char *)data); };

 /*
  * Get length of command in bytes. Return in provided memory.
  */
  
  unsigned short getLen(void) { return (*length); };

 /*
  * Get main command code from the binary command.
  */
  
  unsigned short getCode(void) { return (code); };

 protected:
  // code senyt to detector
  unsigned short code;

  unsigned short *length;
  unsigned char *checksum;

  char data[256];



 /*
  * Calc checksum to command to make it valid.
  */
  
  void calcCheckSum(void) {
    int i;
    int codelo;
    int codehi;
    // addr incl lenggm but not checksum or code
    *length = addr + 2 + 1;
    // addr where checksum is stored
    checksum = (unsigned char *)data + addr;
    totalsum = 0;
    codelo = code & 255;
    codehi = code & (255 * 256);
    codehi = codehi / 256;
    totalsum += codelo;
    totalsum += codehi;

    for (i = 0; i < addr; i++) totalsum += data[i];

    *checksum = (unsigned char)(totalsum & 255);
  };
  int addr;
  int totalsum;
};

/**
 * Class to define a single binary response from pco cameras, received over serial port.
 *
 */

class pco_response {
 public:

 /*
  * Create a new empty response message to be filled by camera serial port data.
  */
  
  pco_response() {
    addr = 0;
    code = 0;
    // space for length.
    length = (unsigned short *)data;
    addUShort(5);
  };

 /*
  * Set main command code in the response.
  */
  
  void setCode(unsigned short cx) { code = cx; };

 /*
  * Set expected return resp. code from camera. camera should send this back.
  */
  
  void setExpCode(unsigned short cx) { exp_code = cx; };

 /*
  * Set error code in this response. camera will send back this code on err condition. 
  */
  
  void setErrCode(unsigned short cx) { error_code = cx; };

 /*
  * Set length of response.
  */
  
  void setLength(unsigned short lx) {
    *length = lx;
    addr = *length - 2;
    checksum = (unsigned char *)data + addr - 1;
  };

 /*
  * Take binary string from serial port, and copy into this response obj for parsing.
  */
  
  void copy2Obj(unsigned char *obj, int len) {
    int k;
    for (k = 0; k < len; k++) {
      obj[k] = data[k];
    }
  }

 /*
  * add unsigned char to message.
  */
  
  void addUChar(unsigned char msg) {
    unsigned char *ptr;
    ptr = (unsigned char *)(data + addr);
    *ptr = msg;
    addr += 1;
  };

 /*
  * add string of uchars of len to this message.
  */
  
  void addUChar(unsigned char *msg, int len) {
    int i;
    for (i = 0; i < len; i++) addUChar(*(msg + i));
  };

 /*
  * add single ushort to this message.
  */
  
  void addUShort(unsigned short msg) {
    unsigned short *ptr;
    ptr = (unsigned short *)(data + addr);
    *ptr = msg;
    addr += 2;
  };

 /*
  * Add signed short to this message.
  */
  
  void addShort(short msg) {
    short *ptr;
    ptr = (short *)(data + addr);
    *ptr = msg;
    addr += 2;
  };

 /*
  * Add signed 32 bit int to this message. 
  */
  
  void addLong(long msg) {
    long *ptr;
    ptr = (long *)(data + addr);
    *ptr = msg;
    addr += 4;
  };

 /*
  * Add unsigned int 32 bit to this message. 
  */
  
  void addULong(unsigned long msg) {
    unsigned long *ptr;
    ptr = (unsigned long *)(data + addr);
    *ptr = msg;
    addr += 4;
  };

 /*
  * Get a ushort out of message, and inc. internal counter, so we can get each part of message
  * in sequence. 
  */
  
  unsigned short getUShort(int adx) {
    unsigned short *ptr;

    ptr = (unsigned short *)(&data[adx]);
    return (*ptr);
  };


 /*
  * get short out of message, inc. internal counter.
  */
  
  short getShort(int adx) {
    short *ptr;

    ptr = (short *)(&data[adx]);
    return (*ptr);
  };


 /*
  * get unsigned char from message.
  */
  
  unsigned char getUChar(int adx) {
    unsigned char *ptr;

    ptr = (unsigned char *)(&data[adx]);
    return (*ptr);
  };

 /*
  * Get unsigned long from message.
  */
  
  unsigned long getULong(int adx) {
    unsigned long *ptr;

    ptr = (unsigned long *)(&data[adx]);
    return (*ptr);
  };

 /*
  * Print the contants of the message to screen. 
  */
  
  void sprintHeader(char *strg) {
    verifyCheckSum();
    sprintf(strg, "code: 0x%x  length: %d cks: 0x%x verified cks: 0x%x \n",
            getCode(), getLen(), *checksum, verified_chksm);
  };

 /*
  * Get raw data string from message.
  */
  
  unsigned char *getData(void) { return ((unsigned char *)data); };
  // data excluding length word
 /*
  *Get data string from message, starting 2 bytes from beginning. 1st 2 bytes are not useful.
  */
  
  unsigned char *getData2(void) { return (2 + (unsigned char *)data); };
 /*
  * get len of messave.
  */
  
  unsigned short getLen(void) { return (*length); };
 /*
  * Get comand code from message.
  */
  
  unsigned short getCode(void) { return (code); };
 /*
  * Get expected return code from message.
  */
  
  unsigned short getExpCode(void) { return (exp_code); };
 /*
  * Get any error code message. 
  */
  
  unsigned short getErrCode(void) { return (error_code); };

 /*
  * Make sure checksum from camera is correct. 
  */
  
  int verifyCheckSum(void) {
    int i, codelo, codehi;
    // checksum and length is included in address. not code.
    verified_length = addr + 2;
    // address where detector checksum is stored
    checksum = (unsigned char *)data + addr - 1;
    totalsum = 0;
    codelo = code & 255;
    codehi = code & (255 * 256);
    codehi = codehi / 256;
    totalsum += codelo;
    totalsum += codehi;

    // addr-1 because addr is the checksum, and it is not counted in checksum
    for (i = 0; i < (addr - 1); i++) totalsum += data[i];

    verified_chksm = (unsigned char)(totalsum & 255);

    if (*length != verified_length) return 1;

    if (*checksum != verified_chksm) return 2;

    return 0;
  };

 protected:
  int addr;
  // code returne from the det.
  unsigned short code;

  // expected good code from det.
  unsigned short exp_code;

  // expected error code from det.
  unsigned short error_code;

  // length from detector
  unsigned short *length;
  // checksum from detector
  unsigned char *checksum;
  // pc figures out length and checksum we actaully got
  // if different from what det sent we have errlr.
  unsigned char verified_chksm;
  unsigned short verified_length;

  char data[256];

  int totalsum;
};

#ifndef _WIN32
#define WORD unsigned short
#define DWORD unsigned long
#define SHORT short

#endif

/**
 *aps designed struct based on PCO SDK. Holds camera info structure.
 */
struct xPCO_Description {
  WORD wSize;                 // Sizeof this struct
  WORD wSensorTypeDESC;       // Sensor type
  WORD wSensorSubTypeDESC;    // Sensor subtype
  WORD wMaxHorzResStdDESC;    // Maxmimum horz. resolution in std.mode
  WORD wMaxVertResStdDESC;    // Maxmimum vert. resolution in std.mode // 10
  WORD wMaxHorzResExtDESC;    // Maxmimum horz. resolution in ext.mode
  WORD wMaxVertResExtDESC;    // Maxmimum vert. resolution in ext.mode
  WORD wDynResDESC;           // Dynamic resolution of ADC in bit
  WORD wMaxBinHorzDESC;       // Maxmimum horz. binning
  WORD wBinHorzSteppingDESC;  // Horz. bin. stepping (0:bin, 1:lin)    // 20
  WORD wMaxBinVertDESC;       // Maxmimum vert. binning
  WORD wBinVertSteppingDESC;  // Vert. bin. stepping (0:bin, 1:lin)
  WORD wRoiHorStepsDESC;      // Minimum granularity of ROI in pixels
  WORD wRoiVertStepsDESC;     // Minimum granularity of ROI in pixels
  WORD wNumADCsDESC;          // Number of ADCs in system              // 30

  DWORD dwPixelRateDESC[4];  // Possible pixelrate in Hz              // 48
                             // 128
  WORD wConvFactDESC[4];     // Possible conversion factor in e/cnt   // 136
                             // 176
  WORD wIRDESC;              // IR enhancment possibility

  DWORD dwMinDelayDESC;         // Minimum delay time in ns
  DWORD dwMaxDelayDESC;         // Maximum delay time in ms
  DWORD dwMinDelayStepDESC;     // Minimum stepping of delay time in ns  // 192
  DWORD dwMinExposureDESC;      // Minimum exposure time in ns
  DWORD dwMaxExposureDESC;      // Maximum exposure time in ms           // 200
  DWORD dwMinExposureStepDESC;  // Minimum stepping of exposure time in ns
  DWORD dwMinDelayIRDESC;       // Minimum delay time in ns
  DWORD dwMaxDelayIRDESC;       // Maximum delay time in ms              // 212
  DWORD dwMinExposureIRDESC;    // Minimum exposure time in ns
  DWORD dwMaxExposureIRDESC;    // Maximum exposure time in ms           // 220
  WORD wTimeTableDESC;          // Timetable for exp/del possibility
  WORD wDoubleImageDESC;        // Double image mode possibility
  SHORT sMinCoolSetDESC;        // Minimum value for cooling
  SHORT sMaxCoolSetDESC;        // Maximum value for cooling
  SHORT sDefaultCoolSetDESC;    // Default value for cooling             // 230
  WORD wPowerDownModeDESC;      // Power down mode possibility
  WORD wOffsetRegulationDESC;   // Offset regulation possibility
  WORD wColorPatternDESC;       // Color pattern of color chip
                                // four nibbles (0,1,2,3) in word
                                //  -----------------
                                //  | 3 | 2 | 1 | 0 |
                                //  -----------------
                                //
                                // describe row,column  2,2 2,1 1,2 1,1
                                //
                                //   column1 column2
                                //  -----------------
                                //  |       |       |
                                //  |   0   |   1   |   row1
                                //  |       |       |
                                //  -----------------
                                //  |       |       |
                                //  |   2   |   3   |   row2
                                //  |       |       |
                                //  -----------------
                                //
  WORD wPatternTypeDESC;        // Pattern type of color chip
                                // 0: Bayer pattern RGB
                                // 1: Bayer pattern CMY

  DWORD reserved[9];
};

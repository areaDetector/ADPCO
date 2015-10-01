


// ------------------------------------------------------------------------ //
// -- Defines for Get Camera Type Command: -------------------------------- //
// ------------------------------------------------------------------------ //

// pco.camera types
#define CAMERATYPE_PCO1200HS     0x0100
#define CAMERATYPE_PCO1300       0x0200
#define CAMERATYPE_PCO1600       0x0220
#define CAMERATYPE_PCO2000       0x0240
#define CAMERATYPE_PCO4000       0x0260

// pco.1300 types


/**********************************************************************
 *
 *
 *
 *
 ************************************************************************/


    class pco_command
    {
        public:
            pco_command()
                { 
                    setCode(0);
                };

    
        void setCode(unsigned short cx)
        {

                    addr=0;
                    code = cx;
                    // space for length.
                    length=(unsigned short*)data;
                    addUShort(0);
                    calcCheckSum();
        }

        void addUShort(unsigned short msg)
        {
            unsigned short *ptr;
            ptr = (unsigned short*)(data+addr);
            *ptr=msg;
            addr+=2;
            calcCheckSum();
        };


        void addUChar(unsigned char msg)
        {
            unsigned char *ptr;
            ptr = (unsigned char*)(data+addr);
            *ptr=msg;
            addr+=1;
            calcCheckSum();
        };

        void addShort(short msg)
        {
            short *ptr;
            ptr = (short*)(data+addr);
            *ptr=msg;
            addr+=2;
            calcCheckSum();
        };

        void addLong(long msg)
        {
            long *ptr;
            ptr = (long*)(data+addr);
            *ptr=msg;
            addr+=4;
            calcCheckSum();
        };
        void addULong(unsigned long msg)
        {
            unsigned long *ptr;
            ptr = (unsigned long*)(data+addr);
            *ptr=msg;
            addr+=4;
            calcCheckSum();
        };

        unsigned char* getData(void) {return( (unsigned char*)data);};
        unsigned short getLen(void) {return(*length);};
        unsigned short getCode(void) {return(code);};

        protected:
        // code senyt to detector
                unsigned short code;


        unsigned short *length;
        unsigned char *checksum;

        char data[256];
            void calcCheckSum(void)
                {    int i;
                    int codelo;
                    int codehi;
                    //addr incl lenggm but not checksum or code
                    *length = addr + 2 + 1;
                    //addr where checksum is stored
                    checksum=(unsigned char*)data+addr;
                    totalsum=0;
                    codelo=code&255;
                    codehi=code&(255*256);
                    codehi = codehi/256;
                    totalsum+=codelo;
                    totalsum+=codehi;
                    
                    for (i=0;i<addr;i++)
                        totalsum+=data[i];

                    *checksum=(unsigned char)(totalsum&255);
                };
         int addr;
         int totalsum;
    };
/*************************************************************************************
*
*
***************************************************************************************/

    class pco_response
    {
        public:
            pco_response()
                { 
                    addr=0;
                    code = 0;
                    // space for length.
                    length=(unsigned short*)data;
                    addUShort(5);
                    
                };

            void setCode(unsigned short cx) {code = cx;};

            void setExpCode(unsigned short cx) {exp_code = cx;};
            void setErrCode(unsigned short cx) {error_code = cx;};

            void setLength(unsigned short lx) {
                *length = lx;
                addr=*length-2;
                checksum=(unsigned char*)data+addr-1;

            };

        void copy2Obj(unsigned char *obj,int len)
        {
            int k;
            for (k=0;k<len;k++)
            {
                obj[k]=data[k];
                
            }

        }
        void addUChar(unsigned char msg)
        {
            unsigned char *ptr;
            ptr = (unsigned char*)(data+addr);
            *ptr=msg;
            addr+=1;
            
        };

        void addUChar(unsigned char *msg,int len)
        {
            int i;
            for (i=0;i<len;i++)
                addUChar(*(msg+i));
            
        };

        void addUShort(unsigned short msg)
        {
            unsigned short *ptr;
            ptr = (unsigned short*)(data+addr);
            *ptr=msg;
            addr+=2;
            
        };
        void addShort(short msg)
        {
            short *ptr;
            ptr = (short*)(data+addr);
            *ptr=msg;
            addr+=2;
            
        };

        void addLong(long msg)
        {
            long *ptr;
            ptr = (long*)(data+addr);
            *ptr=msg;
            addr+=4;
            
        };
        void addULong(unsigned long msg)
        {
            unsigned long *ptr;
            ptr = (unsigned long*)(data+addr);
            *ptr=msg;
            addr+=4;
            
        };

        unsigned short getUShort(int adx)
        {
            unsigned short *ptr;

            ptr = (unsigned short*)(&data[adx]);
            return(*ptr);

        };
        
         short getShort(int adx)
        {
             short *ptr;

            ptr = ( short*)(&data[adx]);
            return(*ptr);

        };

        unsigned char getUChar(int adx)
        {
            unsigned char *ptr;

            ptr = (unsigned char*)(&data[adx]);
            return(*ptr);

        };

        unsigned long getULong(int adx)
        {
            unsigned long *ptr;

            ptr = (unsigned long*)(&data[adx]);
            return(*ptr);

        };


        void sprintHeader(char* strg) {

            verifyCheckSum();
            sprintf(strg,
                "code: 0x%x  length: %d cks: 0x%x verified cks: 0x%x \n",
                getCode(),getLen(),*checksum,verified_chksm);

        };

        unsigned char* getData(void) {return( (unsigned char*)data);};
        // data excluding length word
        unsigned char* getData2(void) {return( 2+(unsigned char*)data);};
        unsigned short getLen(void) {return(*length);};
        unsigned short getCode(void) {return(code);};
        unsigned short getExpCode(void) {return(exp_code);};
        unsigned short getErrCode(void) {return(error_code);};

        int verifyCheckSum(void)
                {    int i,codelo,codehi;
                    // checksum and length is included in address. not code.
                    verified_length = addr + 2;
                    //address where detector checksum is stored
                    checksum=(unsigned char*)data+addr-1;
                    totalsum=0;
                    codelo=code&255;
                    codehi=code&(255*256);
                    codehi = codehi/256;
                    totalsum+=codelo;
                    totalsum+=codehi;
                    
                    //addr-1 because addr is the checksum, and it is not counted in checksum
                    for (i=0;i<(addr-1);i++)
                        totalsum+=data[i];

                    verified_chksm=(unsigned char)(totalsum&255);

                    if (*length != verified_length)
                        return 1;

                    if (*checksum != verified_chksm)
                        return 2;

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
        //checksum from detector
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


// aps designed struct based on PCO SDK
struct xPCO_Description
{
  WORD        wSize;                   // Sizeof this struct
  WORD        wSensorTypeDESC;         // Sensor type
  WORD        wSensorSubTypeDESC;      // Sensor subtype
  WORD        wMaxHorzResStdDESC;      // Maxmimum horz. resolution in std.mode
  WORD        wMaxVertResStdDESC;      // Maxmimum vert. resolution in std.mode // 10
  WORD        wMaxHorzResExtDESC;      // Maxmimum horz. resolution in ext.mode
  WORD        wMaxVertResExtDESC;      // Maxmimum vert. resolution in ext.mode
  WORD        wDynResDESC;             // Dynamic resolution of ADC in bit
  WORD        wMaxBinHorzDESC;         // Maxmimum horz. binning
  WORD        wBinHorzSteppingDESC;    // Horz. bin. stepping (0:bin, 1:lin)    // 20
  WORD        wMaxBinVertDESC;         // Maxmimum vert. binning
  WORD        wBinVertSteppingDESC;    // Vert. bin. stepping (0:bin, 1:lin)
  WORD        wRoiHorStepsDESC;        // Minimum granularity of ROI in pixels
  WORD        wRoiVertStepsDESC;       // Minimum granularity of ROI in pixels
  WORD        wNumADCsDESC;            // Number of ADCs in system              // 30
  
  DWORD       dwPixelRateDESC[4];      // Possible pixelrate in Hz              // 48
                                                 // 128
  WORD        wConvFactDESC[4];        // Possible conversion factor in e/cnt   // 136
                                                 // 176
  WORD        wIRDESC;                 // IR enhancment possibility
  
  DWORD       dwMinDelayDESC;          // Minimum delay time in ns
  DWORD       dwMaxDelayDESC;          // Maximum delay time in ms
  DWORD       dwMinDelayStepDESC;      // Minimum stepping of delay time in ns  // 192
  DWORD       dwMinExposureDESC;       // Minimum exposure time in ns
  DWORD       dwMaxExposureDESC;       // Maximum exposure time in ms           // 200
  DWORD       dwMinExposureStepDESC;   // Minimum stepping of exposure time in ns
  DWORD       dwMinDelayIRDESC;        // Minimum delay time in ns
  DWORD       dwMaxDelayIRDESC;        // Maximum delay time in ms              // 212
  DWORD       dwMinExposureIRDESC;     // Minimum exposure time in ns
  DWORD       dwMaxExposureIRDESC;     // Maximum exposure time in ms           // 220
  WORD        wTimeTableDESC;          // Timetable for exp/del possibility
  WORD        wDoubleImageDESC;        // Double image mode possibility
  SHORT       sMinCoolSetDESC;         // Minimum value for cooling
  SHORT       sMaxCoolSetDESC;         // Maximum value for cooling
  SHORT       sDefaultCoolSetDESC;     // Default value for cooling             // 230
  WORD        wPowerDownModeDESC;      // Power down mode possibility
  WORD        wOffsetRegulationDESC;   // Offset regulation possibility
  WORD        wColorPatternDESC;       // Color pattern of color chip
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
  WORD        wPatternTypeDESC;        // Pattern type of color chip
                                       // 0: Bayer pattern RGB
                                        // 1: Bayer pattern CMY

  DWORD reserved[9];
  
};

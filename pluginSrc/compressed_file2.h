
#ifndef _COMPRESSED_FILE_H
#define _COMPRESSED_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>

#ifndef _WIN32

#include <stdint.h>
#endif


#include "image_file2.h"



#ifdef _WIN32
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;


#endif


/*************************************************************************************
 *
 *
 *
 *
 *
 *
 *************************************************************************************/

namespace nd_imm_plugin {

class compressed_header
{
public:

/*
header(1,:)  = {'mode',          fread(fid,1 ,'int')}                      ; % starting byte   0 : length  4 bytes
header(2,:)  = {'compression',   fread(fid,1 ,'int')}                      ; % starting byte   4 : length  4 bytes
header(3,:)  = {'date',          fread(fid,32,'*char')'}                   ; % starting byte   8 : length 32 bytes
header(4,:)  = {'prefix',        fread(fid,16,'*char')'}                   ; % starting byte  40 : length 16 bytes
header(5,:)  = {'number',        fread(fid,1 ,'int')}                      ; % starting byte  56 : length  4 bytes
header(6,:)  = {'suffix',        fread(fid,16,'*char')'}                   ; % starting byte  60 : length 16 bytes
header(7,:)  = {'monitor',       fread(fid,1 ,'int')}                      ; % starting byte  76 : length  4 bytes
header(8,:)  = {'shutter',       fread(fid,1 ,'int')}                      ; % starting byte  80 : length  4 bytes
header(9,:)  = {'row_beg',       fread(fid,1 ,'int')}                      ; % starting byte  84 : length  4 bytes
header(10,:) = {'row_end',       fread(fid,1 ,'int')-1}                    ; % starting byte  88 : length  4 bytes
header(11,:) = {'col_beg',       fread(fid,1 ,'int')}                      ; % starting byte  92 : length  4 bytes
header(12,:) = {'col_end',       fread(fid,1 ,'int')-1}                    ; % starting byte  96 : length  4 bytes
header(13,:) = {'row_bin',       fread(fid,1 ,'int')}                      ; % starting byte 100 : length  4 bytes
header(14,:) = {'col_bin',       fread(fid,1 ,'int')}                      ; % starting byte 104 : length  4 bytes
header(15,:) = {'rows',          fread(fid,1 ,'int')}                      ; % starting byte 108 : length  4 bytes
header(16,:) = {'cols',          fread(fid,1 ,'int')}                      ; % starting byte 112 : length  4 bytes
header(17,:) = {'bytes',         fread(fid,1 ,'int')}                      ; % starting byte 116 : length  4 bytes
header(18,:) = {'kinetics',      fread(fid,1 ,'int')}                      ; % starting byte 120 : length  4 bytes
header(19,:) = {'kinwinsize',    fread(fid,1 ,'int')}                      ; % starting byte 124 : length  4 bytes
header(20,:) = {'elapsed',       fread(fid,1 ,'double')}                   ; % starting byte 128 : length  8 bytes
header(21,:) = {'preset',        fread(fid,1 ,'double')}                   ; % starting byte 136 : length  8 bytes
header(22,:) = {'topup',         fread(fid,1 ,'int')}                      ; % starting byte 144 : length  4 bytes
header(23,:) = {'inject',        fread(fid,1 ,'int')}                      ; % starting byte 148 : length  4 bytes
header(24,:) = {'dlen',          fread(fid,1 ,'int')}                      ; % starting byte 152 : length  4 bytes
header(25,:) = {'roi_number',    fread(fid,1 ,'int')}                      ; % starting byte 156 : length  4 bytes
% --- modified position as of 20060306
header(26,:) = {'buffer_number', fread(fid,1 ,'uint32')}                   ; % starting byte 160 : length  4 bytes
header(27,:) = {'systick',       fread(fid,1 ,'uint32')}                   ; % starting byte 164 : length  4 bytes
% --- shifted header positions as of 20060306
header(28,:) = {'pv1',           fread(fid,40,'*char')}                    ; % starting byte 168 : length 40 bytes
header(29,:) = {'pv1VAL',        fread(fid,1 ,'float')}                    ; % starting byte 208 : length  4 bytes
header(30,:) = {'pv2',           fread(fid,40,'*char')}                    ; % starting byte 212 : length 40 bytes
header(31,:) = {'pv2VAL',        fread(fid,1 ,'float')}                    ; % starting byte 252 : length  4 bytes
header(32,:) = {'pv3',           fread(fid,40,'*char')}                    ; % starting byte 256 : length 40 bytes
header(33,:) = {'pv3VAL',        fread(fid,1 ,'float')}                    ; % starting byte 296 : length  4 bytes
header(34,:) = {'pv4',           fread(fid,40,'*char')}                    ; % starting byte 300 : length 40 bytes
header(35,:) = {'pv4VAL',        fread(fid,1 ,'float')}                    ; % starting byte 340 : length  4 bytes
header(36,:) = {'pv5',           fread(fid,40,'*char')}                    ; % starting byte 344 : length 40 bytes
header(37,:) = {'pv5VAL',        fread(fid,1 ,'float')}                    ; % starting byte 384 : length  4 bytes
header(38,:) = {'pv6',           fread(fid,40,'*char')}                    ; % starting byte 388 : length 40 bytes
header(39,:) = {'pv6VAL',        fread(fid,1 ,'float')}                    ; % starting byte 428 : length  4 bytes
header(40,:) = {'pv7',           fread(fid,40,'*char')}                    ; % starting byte 432 : length 40 bytes
header(41,:) = {'pv7VAL',        fread(fid,1 ,'float')}                    ; % starting byte 472 : length  4 bytes
header(42,:) = {'pv8',           fread(fid,40,'*char')}                    ; % starting byte 476 : length 40 bytes
header(43,:) = {'pv8VAL',        fread(fid,1 ,'float')}                    ; % starting byte 516 : length  4 bytes
header(44,:) = {'pv9',           fread(fid,40,'*char')}                    ; % starting byte 520 : length 40 bytes
header(45,:) = {'pv9VAL',        fread(fid,1 ,'float')}                    ; % starting byte 560 : length  4 bytes
header(46,:) = {'pv10',          fread(fid,40,'*char')}                    ; % starting byte 564 : length 40 bytes
header(47,:) = {'pv10VAL',       fread(fid,1 ,'float')}                    ; % starting byte 604 : length  4 bytes
% --- new fields (added 10/2006)
header(48,:) = {'imageserver',   fread(fid,1 ,'float')}                    ; % starting byte 608 : length  4 bytes
header(49,:) = {'CPUspeed',      fread(fid,1 ,'float')}                    ; % starting byte 612 : length  4 bytes
header(50,:) = {'immversion',    fread(fid,1 ,'int')}                      ; % starting byte 616 : length  4 bytes
header(51,:) = {'corecotick',    fread(fid,1 ,'uint32')}                   ; % starting byte 620 : length  4 bytes
header(52,:) = {'cameratype',    fread(fid,1 ,'int')}                      ; % starting byte 624 : length  4 bytes
header(53,:) = {'threshhold',    fread(fid,1 ,'float')}                    ; % starting byte 628 : length  4 bytes

*/
int32_t                 mode;                 //comp mode
int32_t                 compression;  //comp?
unsigned char              date[32];        //date today
unsigned char              prefix[16];       //?
int32_t                 number;     // filenum
unsigned char              suffix[16];
int32_t                 monitor;    //0
int32_t                 shutter;    //0
int32_t                 row_beg;    //
int32_t               row_end;      // whatever they are
int32_t                 col_beg;
int32_t               col_end;
int32_t                 row_bin;      //1
int32_t                 col_bin;      //1
int32_t                 rows;         //
int32_t                 cols;
int32_t                 bytes;        //2
int32_t                 kinetics;     //0 part of ccd
int32_t                 kinwinsize;   //0
double              elapsed;      //timestamp
double              preset;       //exp time
int32_t                 topup;     //0
int32_t                 inject;       //0
 uint32_t                 dlen;
int32_t                 roi_number;   //1

int          buffer_number;    //0
unsigned int          systick;//0

unsigned char               pv1[40];
float               pv1VAL;
unsigned char               pv2[40];
float               pv2VAL;
unsigned char               pv3[40];
float               pv3VAL;
unsigned char               pv4[40];
float               pv4VAL;
unsigned char               pv5[40];
float               pv5VAL;
unsigned char               pv6[40];
float               pv6VAL;
unsigned char               pv7[40];
float               pv7VAL;
unsigned char               pv8[40];
float               pv8VAL;
unsigned char               pv9[40];
float               pv9VAL;
unsigned char               pv10[40];
float               pv10VAL;

float               imageserver;  //make up
float               CPUspeed;     //0

enum {immver=12};

int32_t                 immversion;   //immver
int          corecotick;   //
int                 cameratype;//160
float               threshhold;   //my val

//here is 632 bytes- or byte 0 to 631

char byte632;//the is byte 632 counting from 0 to 632

// 1024- (633+84 + 12)
char empty_space[295];

enum {z_len=84};
enum {f_len = 12};

//fill with 00's
unsigned char ZZZZ[z_len];

//fill with FF's
unsigned char FFFF[f_len];

enum {header_size=1024};
};

class fpga_pix_rec
{
public:

    unsigned short pval0;
    unsigned short loclsb;
    unsigned char locmsb;
    unsigned char offset;
    unsigned short pval1;

};

class comp_img_header
{
public:
    unsigned short num_pixels;
    unsigned short num_pixels_msb;
    unsigned short ave_coef0;
    unsigned short blocksize;
    unsigned short timestamp0;
    unsigned short timestamp1;
    unsigned short timestamp2;
    unsigned char numframes;
    unsigned char flags;

    unsigned short mem_st_b_lsb;
    unsigned short mem_st_b_msb;

    unsigned short mem_st_e_lsb;
    unsigned short mem_st_e_msb;

};


class compressed_file
{
public:

    compressed_file();
    ~compressed_file();

    void open_w(
        char* name,
        int start_num,
        int end_number);

    void open_w(
        char* name,
        char* path,
        int start_num,
        int end_number);

    void open_r(
        char* name,
        int start_num,
        int end_number);
    void open_r(
        char* name,
        char* path,
        int start_num,
        int end_number);


    void close();

    void appendImage(
        compressed_header *head,
        void *img_ptr);

    // make new empty header. resercves header_siuze of memory
    compressed_header* newHeader(void);

    // fill in 00's amd ZZ's intop header
    void fillHeaderZZFF(compressed_header *h);


    void readTopHeader(compressed_header* head);
    void readImage(
        int index,
        compressed_header *head,
        void *img_data);

    image_file *myfile;

    bool is_open;

void fpgaToIMM(
    unsigned char* fpga_image,
    int fpga_bytes,
    int img_index,
    unsigned char** IMM_image,
    int *IMM_bytes);

void rawToIMM(
    unsigned char* raw_image,
    int raw_bytes,// tottal bytes
    int raw_precision,//num bytes per pixel
    int raw_x_pixels,
    int raw_y_pixels,
    int threshold,
    int raw_timestamp,
    int img_index,
    unsigned char** IMM_image,
    int *IMM_bytes);


void rawToCompIMM(
    unsigned char* raw_image,
    int raw_bytes,// tottal bytes
    int raw_precision,//num bytes per pixel
    int raw_x_pixels,
    int raw_y_pixels,
    int threshold,
    int raw_timestamp,
    int img_index,
    unsigned char** IMM_image,
    int *IMM_bytes);


    int getFpgaNumFrames(void* img);

    comp_img_header* getFpgaHeadAddr(unsigned char *img, int img_index);
     uint32_t ushortAddrPix0(fpga_pix_rec* pr);
     uint32_t ushortAddrPix1(fpga_pix_rec* pr);

    int numFpgaPixRec(comp_img_header *h);


    int pixDataBytes(comp_img_header *h);
 uint32_t getFpgaTimeStamp32(comp_img_header *h);
        // 48 bit raw time stamp at 133MHz
        double getFpgaTimeStamp48(comp_img_header *h);
        // 32 bit time stamp in us. mask out to get lower bit timestamp
unsigned int getFpgaUsTimeStamp32(comp_img_header *h,unsigned int mask);

     uint32_t pixDataByteAdr(comp_img_header *h);
bool fpgaIsValidHeader(unsigned char *fpga_image);


void saveFileIMMRaw(
    unsigned int time_stamp,
    int comp_threshold,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period
    );



void saveFileIMMRaw(
    unsigned int coreco_ts,
    double elapsed_ts,
    unsigned int systick_ts,
        int comp_threshold,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period
    );

void saveFileIMMComp(
    unsigned int time_stamp,
    int comp_threshold,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period,
    int *num_comp_pixels
    );

void saveFileIMMComp(
    unsigned int coreco_ts,
    double elapsed_ts,
    unsigned int systick_ts,
    int comp_threshold,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period,
    int *num_comp_pixels
    );



void saveFileFPGACompIMM(
    int comp_threshold,
    unsigned int corecoticksg,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period
    );

void saveFileFPGACompIMM(
    int comp_threshold,
    unsigned int corecoticksg,
    double xtime_stamp,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period
    );



bool saveFrameFPGACompIMM(
    int comp_threshold,
    unsigned int corecoticksg,
    double xtime_stamp,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period,
    int which_frame
    );

    void setFPGAImgSize(
        unsigned int xs,
        unsigned int ys,
        unsigned int bytes);


    void setBufferNumber(int b);



    int buf_num;

    // memory in which we can put IMM compressed data, IMM as in Alec IMM at APS
    unsigned char *scratch_memory;
    unsigned char *scratch_memory2;
    enum {scratch_mem_bytes = 10000000};
    enum {fpga_header_bytes = 8192};
    enum {fpga_max_comp_frames=16};
    //measured fpga clk freq in Hz. Problem: we may have freq differences between grabber cards
    // this was measured on Mica
    enum {fpga_clk_freq_hz=132906500};


    unsigned int fpga_img_num_bytes;
    unsigned int fpga_x_size;
    unsigned int fpga_y_size;


     uint32_t last_nbytes;
    void *last_data;
};

};

#endif

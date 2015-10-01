/*******************************************************************************
 *
 * tifflib.h
 *
 *    Author: Tim Madden
 *    Date:    8/10/03
 *    Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/


/*
 * Include files.
 */

#ifdef _WINDOWS
#include <windows.h>
#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HANDLE FILE*
#endif
/*
 * Double incluson protection.
 */
#ifndef _TIFF_FILE_H
    #define _TIFF_FILE_H




/*******************************************************************************
 *
 *    Class tifffile
 *
 *
 ******************************************************************************/

class tiff_file 
{
    public:
        // make file object.
        tiff_file();

        // returns true if a TIFF file
        bool tifCheck(char* filename);
        bool tifCheck(HANDLE fp);
        // return true of big endian- onlu little endian supported
        bool tifEndian(void);

        void tifIFD(char* filename,unsigned long offset);

        void tifIFD(HANDLE fp,unsigned long offset);

        unsigned long tifFindIFD(void);

        void tifRd(char* filename,unsigned short* img);
        void tifRd(HANDLE fp,unsigned short* img);


        // reads info on tiff file- fills in structures
        void tifRdInfo(char* filename);
        void tifRdInfo(HANDLE fp);

        void tifWr(
            char* filename,
            unsigned short *img,
            int sizex,
            int sizey);

        void tifWr(
            HANDLE fp,
            unsigned short *img,
            int sizex,
            int sizey);

    


        void tifWriteIFD(void);

        void tifMakeIFD(void);
        void tifImgSpecs(void);


        char* getHeader(void);
        void putStrHeader(char * strg);



        class img_specs 
        {
        public:
            long width;
            long length;
            long bit_depth;
            long compression;
            long strip_offset;
            long strip_bytecnt;
        };

        class ifd_field 
        {
        public:
            unsigned short tag;
            unsigned short type;
            unsigned long count;
            long valoffs;

        };

        class tiff_header
        {
        public:
            unsigned short endian;
            unsigned short fortytwo;
            unsigned long first_ifd;
        };
    enum 
        {
            header_size = 4096
        };

        char head_data[header_size];

        // numbers to put into tiff ifd .
        long scalars[16];
        img_specs specs;

    protected:

        // file pointer
    //    FILE *fp;
        HANDLE fp;
        char file_name[255];

    

        // header data not tiff header- bessrc data header
    //    char head_data[header_size];
        tiff_header header;

        char *header_ptr;




        // raw IFD data buffer
        ifd_field ifd[64];
        ifd_field *ext_specs;
        unsigned short num_ifd_fields;
        unsigned long next_ifd;


};

#endif


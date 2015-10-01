/*******************************************************************************
 *
 *    image_file.h
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

#endif


#include "image_ram2.h"
#include "tifflib.h"

/*
 * Double incluson protection.
 */
#ifndef _IMAGE_FILE_H
    #define _IMAGE_FILE_H




/*******************************************************************************
 *
 *    Class image_file
 *
 *
 ******************************************************************************/

class image_file 
{
    public:
        // make file object.
        image_file(char* name);
        // make file object.
        image_file(
            char* name,
            int count);

        image_file(
            char* name,
            int count,
            bool is_tiff);

        image_file(
            char* name,
            char* path,
            int count,
            bool is_tiff);

        // set current dir.
        bool cd(char* path);

        //oopen reading
        bool open_r(void);
        // open for writing.
        bool open_w(void);
        // open for writing at end of file.
        bool open_a(void);
        bool close(void);
        // write image
        bool write(image_ram& ram);
        // write data.
        bool write(
            char *ptr,
            int size);
        // write data- leaving room for header
        bool write(
            char *ptr,
            int size,
            int skipbytes);

        // write data.- take large img memory, write 1 quqdrant of it.UNSTICH img
        bool write(
            char *ptr,
            int size,
            int quadrant,
            int linesize);


        // set file to beginning.
        bool rewind(void);
        // read file data.
        bool read(
            char *ptr,
            unsigned long size);
        // read file data. with header skip
        bool read(
            char *ptr,
            int size,
            int skip);


        // read file data. with header skip- reads small img as one quadrant into larger imagemem.
        //STICH IMG
        bool read(
            char *ptr,
            int size,
            int skip,
            int quadrant,
            int linesize);

        // make dtdosplay header wih image size.
        char* dtHeader(
            int x,
            int y);

        void setTiff(bool usetiff);

        char* getHeader(void);
        unsigned long getHeaderSize(void);
        void putStrHeader(char * strg);
        void putFileHeader(char * filename);

        bool is_use_tiff;
        tiff_file tiff;

#ifdef _WINDOWS
        HANDLE fp;
#else
        FILE *fp;
#endif
        // file pointer
        char file_name[512];
        char path_name[512];

    protected:


        enum 
        {
            header_size = 4096
        };

        // header data
        char head_data[header_size];
        char *header_ptr;

        
};

#endif


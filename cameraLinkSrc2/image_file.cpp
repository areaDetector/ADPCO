/*******************************************************************************
 *
 *    image_file.cpp
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

#include "image_file.h"
#include <math.h>


/*******************************************************************************
 *
 *
 ******************************************************************************/
image_file::image_file(char* name) : tiff()
{
    path_name[0] = 0;

    strcpy(file_name,name);
    
    is_use_tiff = false;
    for (int i = 0; i<header_size; i++)
        head_data[i] = 0;

    header_ptr=head_data;

}

/*******************************************************************************
 *
 *
 ******************************************************************************/
// make file object.
image_file::image_file(
    char* name,
    int count) : tiff()
{
        path_name[0] = 0;


    sprintf(file_name,"%s%0#3i",name,count);
    is_use_tiff = false;

    for (int i = 0; i<header_size; i++)
        head_data[i] = 0;

    header_ptr=head_data;


}


/*******************************************************************************
 *
 *
 ******************************************************************************/
// make file object.
#if 1
image_file::image_file(
    char* name,
    int count,
    bool is_tiff) : tiff()
{
        path_name[0] = 0;

    is_use_tiff = is_tiff;
    if (count>=0)
    {
        if (is_use_tiff)
            sprintf(file_name,"%s_%0#5i.tif",name,count);
        else
            sprintf(file_name,"%s_%0#5i.raw",name,count);
    }
    else
    {
        if (is_use_tiff)
            sprintf(file_name,"%s.tif",name);
        else
            sprintf(file_name,"%s",name);
    }
    
    for (int i = 0; i<header_size; i++)
        head_data[i] = 0;

    header_ptr=head_data;


}

#endif

/*******************************************************************************
 *
 *
 ******************************************************************************/
// make file object.
#if 1
image_file::image_file(
    char* name,
    char* path,
    int count,
    bool is_tiff) : tiff()
{
        path_name[0] = 0;


    is_use_tiff = is_tiff;

    strcpy(path_name,path);

    // put / at end of path
    if (path_name[0] != 0 && (path_name[strlen(path_name)-1] != '/'))
        strcat(path_name,"/");

    if (count>=0)
    {
        if (is_use_tiff)
            sprintf(file_name,"%s%s_%0#5i.tif",path_name,name,count);
        else
            sprintf(file_name,"%s%s_%0#5i.raw",path_name,name,count);
    }
    else
    {
        if (is_use_tiff)
            sprintf(file_name,"%s%s.tif",path_name,name);
        else
            sprintf(file_name,"%s%s",path_name,name);
    }
    
    for (int i = 0; i<header_size; i++)
        head_data[i] = 0;

    header_ptr=head_data;


}

#endif

/*******************************************************************************
 *
 *
 ******************************************************************************/
// set current dir.
bool image_file::cd(char* path)
{//!!

#if 0
    int er;

    if (path[0] != 0)
    {
        er = SetCurrentDirectory(path);
        if (er==0)
            return false;

        return true;

    }

#endif
    return false;
}

/*******************************************************************************
 *
 *
 ******************************************************************************/
//oopen reading
bool image_file::open_r(void)
{//!!

#ifdef _WINDOWS
    fp = CreateFile(file_name,           // file to create 
             GENERIC_READ,                // open for writing 
             0,                            // do not share 
             NULL,                         // default security 
             OPEN_EXISTING,                // overwrite existing 
             FILE_ATTRIBUTE_NORMAL,       // normal file 
             NULL);                        // no attr. template 

    if (fp == INVALID_HANDLE_VALUE)
        return false;

#else

    fp = fopen(file_name,"r");
    if (fp==0)
        return false;

#endif

    return true;
}


/*******************************************************************************
 *
 *
 ******************************************************************************/
// open for writing.
bool image_file::open_w(void)
{//!!

    #ifdef _WINDOWS

    fp = CreateFile(file_name,           // file to create 
             GENERIC_WRITE,                // open for writing 
             0,                            // do not share 
             NULL,                         // default security 
             CREATE_ALWAYS,                // overwrite existing 
             FILE_ATTRIBUTE_NORMAL,       // normal file 
             NULL);                        // no attr. template 

        if (fp == INVALID_HANDLE_VALUE)
        return false;

#else

    fp = fopen(file_name,"w");
    if (fp==0)
        return false;
#endif

    return true;
}

/*******************************************************************************
 *
 *
 ******************************************************************************/
// open for writing at end of file.
bool image_file::open_a(void)
{
//    fp = fopen(file_name,"a");
    if (fp == 0)
        return false;

    return true;
}


/*******************************************************************************
 *
 *
 ******************************************************************************/

bool image_file::close(void)
{

#ifdef _WINDOWS
    CloseHandle(fp);
#else
    fclose(fp);

#endif


    return true;
}


/*******************************************************************************
 *
 *
 ******************************************************************************/
// write image
bool image_file::write(image_ram& ram)
{
    unsigned char* ptr = ram.getMem();
    bool err;

    int    s=ram.getImgSize();
    unsigned long ww;

    if (is_use_tiff)
    {
    tiff.tifWr(fp,(unsigned short*)ram.getMem(),ram.getImgX(),ram.getImgY());
    return true;
    }

//    ww=fwrite(ptr,s,sizeof(short),fp);
#ifdef _WINDOWS

    err =WriteFile(
    fp,
        ptr,
        s*sizeof(short),
        &ww,
        NULL
        );


#else
ww=fwrite(ptr,s,sizeof(short),fp);
#endif



    return true;
    //!!
}


/*******************************************************************************
 *
 *
 ******************************************************************************/
// write data.
bool image_file::write(
            char *ptr,
            int size)
{
    unsigned long ww;
    bool err;

    if (is_use_tiff)
    {
        unsigned long sizeq=(unsigned long)sqrt((double)(size/sizeof(short)));
    tiff.tifWr(fp,(unsigned short*)ptr,sizeq,sizeq);
    return true;
    }

#ifdef _WINDOWS
    err =WriteFile(
    fp,
        ptr,
        size,
        &ww,
        NULL
        );

#else
ww=fwrite(ptr,size,sizeof(char),fp);
#endif


    return true;
    //!!
}
/*******************************************************************************
 *
 *
 ******************************************************************************/
// write data.
bool image_file::write(
            char *ptr,
            int size,
            int skipbytes)
{
    unsigned long ww;
    bool err;
    // write skipbutes from the image ram. copy header info here later

    
    if (is_use_tiff)
    {
        //size is num of bytes- convert to x and y pixels for square images.
        unsigned long sizeq=(unsigned long)sqrt((double)(size/sizeof(short)));
    tiff.tifWr(fp,(unsigned short*)ptr,sizeq,sizeq);
    return true;
    }

    if (skipbytes>header_size) skipbytes = header_size;

    #ifdef _WINDOWS

    err =WriteFile(
    fp,
        head_data,
        skipbytes,
        &ww,
        NULL
        );

#else
ww=fwrite(head_data,skipbytes,sizeof(char),fp);
#endif


    //now we write the whole image.


    #ifdef _WINDOWS
    err =WriteFile(
    fp,
        ptr,
        size,
        &ww,
        NULL
        );

#else
ww=fwrite(ptr,size,sizeof(char),fp);
#endif


    return true;
    //!!
}

/*******************************************************************************
 *
 *            char *ptr, - pointer image sdata
*            int size, total size of image in bytes
*            int quadrant, 0-3, which quadrant to write
*            int linesize -nium of pixels in one line
 ******************************************************************************/
// write data.
bool image_file::write(
            char *ptr,
            int size,
            int quadrant,
            int linesize)
{
    bool is_ok;
    unsigned long got;
    char *startptr;
    int linecnt;

// now we switch based on quadrant.
    if (quadrant==0) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr;
    }
    if (quadrant==1) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr+sizeof(unsigned short)*linesize;
    }    
    if (quadrant==2) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr+sizeof(unsigned short)*2*linesize*linesize;
    }    
    if (quadrant==3) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr+sizeof(unsigned short)*(2*linesize*linesize+linesize);
    }

    image_ram *temp_ram = new image_ram(linesize,linesize);
    unsigned char* temp_ptr = temp_ram->getMem();

    for (linecnt = 0; linecnt<linesize;linecnt++)
    {
    //copy 1 line at a time

    memcpy(temp_ptr,startptr,linesize*sizeof(unsigned short));
    startptr+= sizeof(unsigned short)*2*linesize;
    temp_ptr+=sizeof(unsigned short)*linesize;
    }

    is_ok = this->write(*temp_ram);
    delete(temp_ram);

    return(is_ok);

    //!!
}


/*******************************************************************************
 *
 *
 ******************************************************************************/
        // set file to beginning.
bool image_file::rewind(void)
{//!!
    return false;
}


/*******************************************************************************
 *
 *
 ******************************************************************************/
        // read file data.
bool image_file::read(
            char *ptr,
            unsigned long size)
{
    bool is_ok;
    unsigned long got;


    if (tiff.tifCheck(fp))
    {
        setTiff(true);
        tiff.tifRd(fp,(unsigned short*) ptr);

        return true;
    }
//!!
    if (size==0) //we then read 4096 header_size or until eof
    {

#ifdef _WINDOWS
        size = GetFileSize(fp,0);
#else
        fseek(fp,0,SEEK_END);
        size=ftell(fp);
        fseek(fp,0,SEEK_SET);
#endif
        if (size>header_size) size=header_size;
    }

#ifdef _WINDOWS
    is_ok= ReadFile( 
        fp, 
        ptr, 
         size, 
        &got, 
        0
        ); 

#else
    fread(ptr,1,size,fp);

#endif


    return(is_ok);
}
/*******************************************************************************
 *
 *
 ******************************************************************************/
        // read file data.
bool image_file::read(
            char *ptr,
            int size,
            int skip)
{
    bool is_ok;
    unsigned long got;
    
    if (tiff.tifCheck(fp))
    {
                setTiff(true);
        tiff.tifRd(fp,(unsigned short*) ptr);
        return true;
    }

//!!
//    fread(ptr,size,size,fp);
// read in header into image mem-- we overrite it below
#ifdef _WINDOWS

    is_ok= ReadFile( 
        fp, 
        this->head_data, 
         skip, 
        &got, 
        0
        ); 

//read in data
    is_ok= ReadFile( 
        fp, 
        ptr, 
         size, 
        &got, 
        0
        ); 
#else
fread(ptr,1,skip,fp);
fread(ptr,1,size,fp);

#endif

    return(is_ok);
}


/*******************************************************************************
 *this version assumes a small image file is read into a large memory. That is
 * we store one quadrant of data on the disk, and then we read it into fill 2x2 image
 * memory space. For stiching images.
 *
 *ptr is image mrmory wiere to write img
 *size is the total img size in the file
 *skip is leading header size of data to skip
 *quadrant is 0-3 integer. which qiuadrant of img
 *linesize is num of pixels in the line of one quadrant- or 1/2 of total 4 quad img.
 * qiadrants are stored as follows
 * 0   1
 * 2   3
 * wiere 0 is qiad 0, at start address of file.
 *
 ******************************************************************************/
        // read file data.
bool image_file::read(
            char *ptr,
            int size,
            int skip,
            int quadrant,
            int linesize)
{
    bool is_ok = true;
    unsigned long got;
    char *startptr;
    int linecnt;



    image_ram *temp_ram = new image_ram(linesize,linesize);

    is_ok=this->read((char *)temp_ram->getMem(),sizeof(short)*linesize*linesize);

// now we switch based on quadrant.
    if (quadrant==0) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr;
    }
    if (quadrant==1) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr+sizeof(unsigned short)*linesize;
    }    
    if (quadrant==2) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr+sizeof(unsigned short)*2*linesize*linesize;
    }    
    if (quadrant==3) //we are in quad 2 or 3, so skip qiad 0 and 1.
    {
        startptr = ptr+sizeof(unsigned short)*(2*linesize*linesize+linesize);
    }

    
   char *sourceptr = (char *)temp_ram->getMem();

    for (linecnt = 0; linecnt<linesize;linecnt++)
    {

    memcpy(startptr,sourceptr,sizeof(unsigned short)*linesize);
    sourceptr+=sizeof(unsigned short)*linesize;
    startptr+= sizeof(unsigned short)*2*linesize;
    }
    delete(temp_ram);

    return(is_ok);
}

/*******************************************************************************
 *
 *    Class image_ram
 *
 *
 ******************************************************************************/
// make dtdosplay header wih image size.
char* image_file::dtHeader(
    int x,
    int y)
{
    // pinter to header data.
    char *ptr;

    // start at the beginning.
    ptr = head_data;

    // clear
    for (int i = 0; i<header_size; i++)
        head_data[i] = 0;

    strcpy(ptr,"{\n");
    // get to end of str.
    while (*ptr != 0) ptr++;

    strcpy(ptr,"HEADER_BYTES= 1024;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"TYPE=mad;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"BYTE_ORDER=big_endian;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"DIM=2;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"SIZE1=1024;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"SIZE2=1024;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"DTDISPLAY_ORIENTATION=+X+Y;\n");
    while (*ptr != 0) ptr++;

    // put in imae size here.
    sprintf(ptr,"X_BEAM=   %i;\n",x);
    while (*ptr != 0) ptr++;
    sprintf(ptr,"Y_BEAM=   %i;\n",y);
    while (*ptr != 0) ptr++;
    sprintf(ptr,"X_SIZE=   %i;\n",x);
    while (*ptr != 0) ptr++;
    sprintf(ptr,"Y_SIZE=   %i;\n",y);
    while (*ptr != 0) ptr++;

    // rest is i dont know...
    strcpy(ptr,"X_SCALE=     14.6285768;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"Y_SCALE=     14.6285768;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"SPACING=      1.0000000;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"PIXEL_SIZE=      0.0683594;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"XINT_START=           1;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"XINT_STEP=           4;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"YINT_START=           1;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"YINT_STEP=           4;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"XINV_START=           1;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"XINV_STEP=           4;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"YINV_START=           1;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"YINV_STEP=           4;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"BAD_FLAG=     1000000;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"PSCALE=      0.0100000;\n");
    while (*ptr != 0) ptr++;
    strcpy(ptr,"}\n");


    return head_data;


}

/*******************************************************************************
 *
 *    Class image_ram
 *
 *
 ******************************************************************************/
// make dtdosplay header wih image size.
void image_file::setTiff(bool usetiff)
{

    is_use_tiff = usetiff;
}


char* image_file::getHeader(void)
{
    if (is_use_tiff)
        return(tiff.head_data);

    return head_data;
}

unsigned long image_file::getHeaderSize(void)
{

    if (is_use_tiff)
    { 
        return tiff.header_size;
    }
    return header_size;
}

void image_file::putStrHeader(char * strg)
{
        
    if (is_use_tiff)
    {
        tiff.putStrHeader(strg);
        return;
    }


    strcpy(header_ptr,strg);
    while (*header_ptr != 0) header_ptr++;

}
void image_file::putFileHeader(char * filename)
{
char *ptr;

image_file* header_file;
header_file = new image_file(filename);

ptr = this->getHeader();

header_file->open_r();
header_file->read(ptr,0);
header_file->close();

delete(header_file);



}



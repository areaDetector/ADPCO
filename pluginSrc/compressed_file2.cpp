
#include "compressed_file2.h"
#include "time.h"
#include "math.h"


namespace nd_imm_plugin {

compressed_file::compressed_file()
{

    last_nbytes =0;
    last_data=0;
    is_open=false;
    myfile=0;
    setFPGAImgSize(1024,1024,1024*1024*2);
    scratch_memory=new unsigned char[scratch_mem_bytes];
    scratch_memory2=new unsigned char[scratch_mem_bytes];

    buf_num=0;
}

compressed_file::~compressed_file()
{
    delete scratch_memory;
    delete scratch_memory2;
}

void compressed_file::open_w(
        char* name,
        int start_num,
        int end_number)
{

    char fullname[512];

    if (start_num!= -1)
        sprintf(fullname,"%s_%05i-%05i.bin",name,start_num,end_number);
    else
        sprintf(fullname,"%s",name);


    myfile = new image_file(
            fullname,
            -1,
            false);


    is_open=myfile->open_w();
    //is_open=true;

}

void compressed_file::open_w(
        char* name,
        char* path,
        int start_num,
        int end_number)
{

    char fullname[512];

    if (start_num!= -1)
        sprintf(fullname,"%s_%05i-%05i.imm",name,start_num,end_number);
    else
        sprintf(fullname,"%s",name);

    myfile = new image_file(
            fullname,
            path,
            -1,
            false);


    is_open=myfile->open_w();
    //is_open=true;

}



void compressed_file::open_r(
        char* name,
        int start_num,
        int end_number)
{

    char fullname[512];

    sprintf(fullname,"%s_%05i-%05i.bin",name,start_num,end_number);

    myfile = new image_file(
            fullname,
            -1,
            false);


    is_open=myfile->open_r();
    //is_open=true;

}

void compressed_file::open_r(
        char* name,
        char* path,
        int start_num,
        int end_number)
{

    char fullname[512];

    sprintf(fullname,"%s_%05i-%05i.bin",name,start_num,end_number);

    myfile = new image_file(
            fullname,
            path,
            -1,
            false);


    is_open=myfile->open_r();
    //is_open=true;

}


void compressed_file::close(void)
{
    if (myfile!=0)
    {
        myfile->close();
        is_open=false;
        delete(myfile);
        myfile=0;
    }
}

void compressed_file::appendImage(
        compressed_header *head,
        void *img_ptr)
{
    int num_bytes;
    myfile->write((char*)head, compressed_header::header_size);

    num_bytes = head->rows * head->bytes * head->cols;
    myfile->write((char*)img_ptr, num_bytes);

}

compressed_header* compressed_file::newHeader(void)
{
    char *mem;

    mem= new char[compressed_header::header_size];
    fillHeaderZZFF((compressed_header*)mem);

    return((compressed_header*)mem );

}


    // fill in 00's amd ZZ's intop header
void compressed_file::fillHeaderZZFF(compressed_header *h)
{
    int k;

    for(k=0;k<compressed_header::z_len;k++)
    {
        h->ZZZZ[k] = 0;
    }

    for(k=0;k<compressed_header::f_len;k++)
    {
        h->FFFF[k] = 255;
    }

}


void compressed_file::readTopHeader(compressed_header* head)
{
    myfile->rewind();
    myfile->read((char*)head, compressed_header::header_size);

}

void compressed_file::readImage(
        int index,
        compressed_header *head,
        void *img_data)
{
    int num_bytes, skip_bytes;

    readTopHeader(head);

    //index from 0 to ...
    skip_bytes = index * (compressed_header::header_size +
            head->rows * head->bytes * head->cols);

    num_bytes = head->rows * head->bytes * head->cols;

    myfile->read(
            (char*) head,
            compressed_header::header_size,
            skip_bytes);


    myfile->read(
            (char*) img_data,
            num_bytes);


}
/******************************************************************************
 *
 *
 *****************************************************************************/

void compressed_file::fpgaToIMM(
    unsigned char* fpga_image,
    int fpga_bytes,
    int img_index,
    unsigned char** IMM_image,
    int *IMM_bytes)
{

    int k;
    int num_pix_rec;
    int num_pixels;
    int pix_rec_bytes;
    uint32_t *IMM_data_int32_t;
    comp_img_header *fpga_header;
    compressed_header *IMM_header;
    fpga_pix_rec *current_pix_rec;
    unsigned short* IMM_data_short;
unsigned char *IMMptr;
    fpga_header=getFpgaHeadAddr(fpga_image,img_index);

    num_pix_rec=numFpgaPixRec(fpga_header);


    pix_rec_bytes = pixDataBytes(fpga_header);


    // get memory for new compressed image
    *IMM_image = this->scratch_memory;

    // point header to top of new memory
    IMM_header=(compressed_header*)*IMM_image;

    IMMptr = *IMM_image;
    for (k=0;k<compressed_header::header_size;k++)
        IMMptr[k] = 0;


    fillHeaderZZFF(IMM_header);
    //
    // Put some magic numbers in the header
    //
    IMM_header->cols=fpga_x_size;//!! FPGA only has 1k size as of now...
    IMM_header->rows=fpga_y_size;

            // bytes per pixel
    IMM_header->bytes = 2;
        // magic numbers
        IMM_header->immversion=compressed_header::immver;
        IMM_header->mode = 2;
        IMM_header->compression=6;
        // data length- actually num of exposed pixels.
        // real len is  numpix*(4+ bytes);
        // fpga stores 2 pixels in each record.
        //IMM_header->dlen = num_pixels;

        IMM_header->col_beg=0;
        IMM_header->col_end=fpga_x_size-1;
        //timestamp in us, 31 bit
        IMM_header->corecotick=getFpgaUsTimeStamp32(fpga_header,2147483647);

//        IMM_header->corecotick=getFpgaTimeStamp32(fpga_header);

     IMM_data_int32_t = (uint32_t*)(*IMM_image+compressed_header::header_size);

    current_pix_rec=(fpga_pix_rec*)(fpga_image + pixDataByteAdr(fpga_header));

    //
    // Fill the pixel location section
    //
    num_pixels=0;


    for (k=0;k<num_pix_rec;k++)
    {

        if (current_pix_rec->pval0 > 0)
        {
            *IMM_data_int32_t=ushortAddrPix0(current_pix_rec);
            num_pixels++;
            IMM_data_int32_t+=1;
        }

        if (current_pix_rec->pval1 > 0)
        {
            *IMM_data_int32_t=ushortAddrPix1(current_pix_rec);
            IMM_data_int32_t+=1;
            num_pixels++;
        }

        current_pix_rec+=1;

    }

    //
    // Fill the pix value data
    //
    IMM_data_short=(unsigned short*)IMM_data_int32_t;

    current_pix_rec=(fpga_pix_rec*)(fpga_image + pixDataByteAdr(fpga_header));
    for (k=0;k<num_pix_rec;k++)
    {


        if (current_pix_rec->pval0>0)
        {
            *IMM_data_short=current_pix_rec->pval0;
            IMM_data_short+=1;
        }

        if (current_pix_rec->pval1>0)
        {
            *IMM_data_short=current_pix_rec->pval1;
            IMM_data_short+=1;
        }
        current_pix_rec+=1;

    }


    IMM_header->dlen = num_pixels;
    // calculate the size of the new compressde image in bytes
    *IMM_bytes = compressed_header::header_size;
    *IMM_bytes = *IMM_bytes + sizeof(uint32_t)*num_pixels;
    *IMM_bytes = *IMM_bytes + sizeof(unsigned short)*num_pixels;

}





/******************************************************************************
 *
 *
 *****************************************************************************/


void compressed_file::rawToIMM(
    unsigned char* raw_image,
    int raw_bytes,// tottal bytes
    int raw_precision,//num bytes per pixel
    int raw_x_pixels,
    int raw_y_pixels,
    int threshold,
    int raw_timestamp,
    int img_index,
    unsigned char** IMM_image,
    int *IMM_bytes)
{

    int k;
    int num_pixels;
    int pix_rec_bytes;
    int num_raw_pixels;
    unsigned short* raw_image_us;
    unsigned char* raw_image_uc;

    uint32_t *IMM_data_int32_t;
    compressed_header *IMM_header;
    unsigned short* IMM_data_short;
    unsigned char *IMMptr;



    // get memory for new compressed image
    *IMM_image = this->scratch_memory;

    // point header to top of new memory
    IMM_header=(compressed_header*)*IMM_image;

    IMMptr = *IMM_image;
    for (k=0;k<compressed_header::header_size;k++)
        IMMptr[k] = 0;


    fillHeaderZZFF(IMM_header);

    //
    // Put some magic numbers in the header
    //
    IMM_header->cols=raw_x_pixels;
    IMM_header->rows=raw_y_pixels;

            // bytes per pixel
    IMM_header->bytes = raw_precision;
        // magic numbers
        IMM_header->immversion=compressed_header::immver;
        IMM_header->mode = 2;
        IMM_header->compression=0;


        IMM_header->col_beg=0;
        IMM_header->col_end=raw_x_pixels-1;
        IMM_header->corecotick=raw_timestamp;

     IMM_data_short = (unsigned short*)(*IMM_image+compressed_header::header_size);


    //
    // Fill the pixel location section
    //


    raw_image_us = (unsigned short*)raw_image;
    raw_image_uc = raw_image;
    //raw_image_ul = (uint32_t*)raw_image;

    num_raw_pixels = raw_bytes/raw_precision;
    num_pixels=num_raw_pixels;

    memcpy((void*)IMM_data_short,(void*)raw_image_us,raw_bytes);




    IMM_header->dlen = num_pixels;
    // calculate the size of the new compressde image in bytes
    *IMM_bytes = compressed_header::header_size;
    *IMM_bytes = *IMM_bytes + raw_bytes;

}



/******************************************************************************
 *
 *
 *****************************************************************************/


void compressed_file::rawToCompIMM(
    unsigned char* raw_image,
    int raw_bytes,// tottal bytes
    int raw_precision,//num bytes per pixel
    int raw_x_pixels,
    int raw_y_pixels,
    int threshold,
    int raw_timestamp,
    int img_index,
    unsigned char** IMM_image,
    int *IMM_bytes)
{

    int k;
    int num_pixels;
    int pix_rec_bytes;
    int num_raw_pixels;
    unsigned short* raw_image_us;
    unsigned char* raw_image_uc;

    unsigned short* pix_val_s;
    unsigned int *IMM_data_int;
    compressed_header *IMM_header;
    unsigned short* IMM_data_short;
    unsigned char *IMMptr;
    int location;


    // get memory for new compressed image
    *IMM_image = this->scratch_memory;

    // point header to top of new memory
    IMM_header=(compressed_header*)*IMM_image;

    IMMptr = *IMM_image;
    for (k=0;k<compressed_header::header_size;k++)
        IMMptr[k] = 0;

    fillHeaderZZFF(IMM_header);


    //
    // Put some magic numbers in the header
    //
    IMM_header->cols=raw_x_pixels;
    IMM_header->rows=raw_y_pixels;

            // bytes per pixel
    IMM_header->bytes = raw_precision;
        // magic numbers
        IMM_header->immversion=compressed_header::immver;
        IMM_header->mode = 2;
        IMM_header->compression=6;


        IMM_header->col_beg=0;
        IMM_header->col_end=raw_x_pixels-1;
        IMM_header->corecotick=raw_timestamp;

    // IMM_data_short = (unsigned short*)(*IMM_image+compressed_header::header_size);
     IMM_data_int = (unsigned int*)(*IMM_image+compressed_header::header_size);


    //
    // Fill the pixel location section
    //


    raw_image_us = (unsigned short*)raw_image;
    raw_image_uc = raw_image;
    //raw_image_ul = (uint32_t*)raw_image;

    num_raw_pixels = raw_bytes/raw_precision;

    num_pixels=0;
    location = 0;

    pix_val_s=(unsigned short*)this->scratch_memory2;


    if (raw_precision==2)
    {
        for (k=0;k<num_raw_pixels; k++)
        {
            if (*raw_image_us > threshold)
            {
                *IMM_data_int = location;
                *pix_val_s=*raw_image_us;
                IMM_data_int++;
                pix_val_s++;
                num_pixels++;
            }
            raw_image_us++;
            location ++;

        }
    }
    else//for char data
    {
        for (k=0;k<num_raw_pixels; k++)
        {
            if (*raw_image_uc > threshold)
            {
                *IMM_data_int = location;
                *pix_val_s=(unsigned short)*raw_image_uc;
                IMM_data_int++;
                pix_val_s++;
                num_pixels++;
            }
            raw_image_uc++;

            location ++;
        }
    }



    // fill in the pix value section of the IMM image. just copy...
    memcpy((void*)IMM_data_int,(void*)this->scratch_memory2,num_pixels*sizeof(short));




    IMM_header->dlen = num_pixels;


    // calculate the size of the new compressde image in bytes
    *IMM_bytes = compressed_header::header_size;
    *IMM_bytes = *IMM_bytes + sizeof(uint32_t)*num_pixels;
    *IMM_bytes = *IMM_bytes + sizeof(unsigned short)*num_pixels;


}



/******************************************************************************
 *
 *
 *****************************************************************************/



    int compressed_file::getFpgaNumFrames(void* img)
    {
        comp_img_header *h;

        h=(comp_img_header*)img;
        return (1+(int)(h->numframes));
    }



/******************************************************************************
 *
 *
 *****************************************************************************/

    comp_img_header* compressed_file::getFpgaHeadAddr(unsigned char *img, int img_index)
    {
        unsigned char* ptr;
        comp_img_header *h;
        int bsize;

        h=(comp_img_header*)img;
        bsize=h->blocksize;
        ptr = img + img_index*bsize;

        return( (comp_img_header*)ptr);
    }

/******************************************************************************
 *
 *
 *****************************************************************************/

    uint32_t compressed_file::ushortAddrPix0(fpga_pix_rec* pr)
    {
        uint32_t octobyte_ind;
        uint32_t pix_ind;
        octobyte_ind = 65536* (uint32_t)pr->locmsb + (uint32_t)pr->loclsb;
        pix_ind = 4 * (octobyte_ind-1) + (uint32_t)pr->offset;
        return(pix_ind);
    }

/******************************************************************************
 *
 *
 *****************************************************************************/

    uint32_t compressed_file::ushortAddrPix1(fpga_pix_rec* pr)
    {
        uint32_t pix_ind;
        pix_ind = ushortAddrPix0(pr) + 1;
        return(pix_ind);
    }
/******************************************************************************
 *
 *
 *****************************************************************************/


    int compressed_file::numFpgaPixRec(comp_img_header *h)
    {
        return(
            (int)(h->num_pixels) + 65536*(int)(h->num_pixels_msb)
            );
    }


/******************************************************************************
 *
 *
 *****************************************************************************/


    uint32_t compressed_file::getFpgaTimeStamp32(comp_img_header *h)
    {
        uint32_t ts,tsl,tsb;
        tsl = (uint32_t)h->timestamp0;
        tsb=(uint32_t)h->timestamp1;
        ts = tsl + 65536*tsb;


        return (ts);
    }


    double compressed_file::getFpgaTimeStamp48(comp_img_header *h)
    {
        double ts,tsl,tsb,tsbb;
        double mult;
        mult = 65536.0;
        tsl = (double)h->timestamp0;
        tsb=(double)h->timestamp1;
        tsbb=(double)h->timestamp2;

        ts = tsl + mult*tsb + mult*mult*tsbb;


        return (ts);
    }
    unsigned int compressed_file::getFpgaUsTimeStamp32(comp_img_header *h,unsigned int mask)
    {
        double ts,tsl,tsb,tsbb;
        double tssec;
        double tsus;
        double mult;
        double million;
        unsigned int tsusb;

        million=1000000.0;
        mult = 65536.0;
        tsl = (double)h->timestamp0;
        tsb=(double)h->timestamp1;
        tsbb=(double)h->timestamp2;

        // 48 bit time stamp
        ts = tsl + mult*tsb + mult*mult*tsbb;
        // time stamp in sec
        tssec = ts / (double)fpga_clk_freq_hz;
        // time stamp in us
        tsus=million*tssec;
        // binary timestamp, 32 bits, round to us
        tsusb=(unsigned int)floor(tsus+0.5);
        // mask out bits we want...
        tsusb = tsusb&mask;

        return (tsusb);
    }



/******************************************************************************
 *
 *
 *****************************************************************************/

    int compressed_file::pixDataBytes(comp_img_header *h)
    {
        return( 8*numFpgaPixRec(h));
    }
/******************************************************************************
 *
 *
 *****************************************************************************/

    uint32_t compressed_file::pixDataByteAdr(comp_img_header *h)
    {

        uint32_t base;
        uint32_t nom_size;
        nom_size = fpga_img_num_bytes;

        base = (uint32_t)(h->mem_st_b_lsb) +
            65536*(h->mem_st_b_msb);

        while(base>nom_size)
            base = base-nom_size;

        return(base);

    }
/******************************************************************************
 *
 *
 *****************************************************************************/
    void compressed_file::setFPGAImgSize(
        unsigned int xs,
        unsigned int ys,
        unsigned int bytes)
    {
        fpga_img_num_bytes = bytes;
        fpga_x_size = xs;
        fpga_y_size = ys;
    }

/******************************************************************************
 *
 *
 *****************************************************************************/

bool compressed_file::fpgaIsValidHeader(unsigned char *fpga_image)
{

    int num_frames;
    int num_rec;
    int k;


    comp_img_header *h;

    if ( (num_frames=getFpgaNumFrames(fpga_image)) >fpga_max_comp_frames)
        return false;

    if (num_frames<1)
        return false;

    for (k=0; k<num_frames; k++)
    {
        h=getFpgaHeadAddr(fpga_image,k);

        if ( (num_rec = numFpgaPixRec(h)) > ((fpga_img_num_bytes/8)/num_frames))
            return false;

        if (num_rec<0)
            return false;

        if (pixDataByteAdr(h) > (fpga_img_num_bytes))
            return false;


        if (pixDataByteAdr(h) < fpga_header_bytes)
            return false;

        if ( (pixDataByteAdr(h) + pixDataBytes(h)) > (fpga_img_num_bytes))
            return false;

    }


    return true;
}



/******************************************************************************
 *
 *
 *****************************************************************************/

void compressed_file::saveFileIMMRaw(
    unsigned int time_stamp,
    int comp_threshold,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period
    )
{
    compressed_header *header;

    unsigned char* IMM_image;
    int IMM_bytes;

     time_t rawtime;
  struct tm * timeinfo;
  char today[32];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
   sprintf(today,"%s",asctime(timeinfo));



    rawToIMM(
        (unsigned char*)imgdata,
        bytes_per_pix*sizex*sizey,
        2,
        sizex,
        sizey,
        comp_threshold,
        0,//timestamp
        0,//imgindex
        &IMM_image,
        &IMM_bytes);

    header=(compressed_header*)IMM_image;
        header->threshhold=comp_threshold;
        header->elapsed=time_stamp;
        header->corecotick=time_stamp;
        header->systick=time_stamp;
        header->col_beg=0;
        header->col_beg=0;
        header->col_end=sizex;
        header->row_beg=0;
        header->row_end=sizey;
        header->number=filenumber;
        strcpy((char*)header->date,today);

        header->rows=sizey;
        header->cols=sizex;
        header->cameratype=camtype;// dalsa
        header->preset=acq_period;
        header->row_bin=1;
        header->col_bin=1;
        header->roi_number=1;

        header->buffer_number = buf_num;




            this->myfile->write((char*)IMM_image,IMM_bytes);




    last_nbytes=IMM_bytes;
    last_data = IMM_image;


}



/******************************************************************************
 *
 *
 *****************************************************************************/

void compressed_file::saveFileIMMRaw(
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
    )
{
    compressed_header *header;

    unsigned char* IMM_image;
    int IMM_bytes;

     time_t rawtime;
  struct tm * timeinfo;
  char today[32];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
   sprintf(today,"%s",asctime(timeinfo));



    rawToIMM(
        (unsigned char*)imgdata,
        bytes_per_pix*sizex*sizey,
        2,
        sizex,
        sizey,
        comp_threshold,
        0,//timestamp
        0,//imgindex
        &IMM_image,
        &IMM_bytes);

    header=(compressed_header*)IMM_image;
        header->threshhold=comp_threshold;
        header->elapsed=elapsed_ts;
        header->corecotick=coreco_ts;
        header->systick=systick_ts;
        header->col_beg=0;
        header->col_beg=0;
        header->col_end=sizex;
        header->row_beg=0;
        header->row_end=sizey;
        header->number=filenumber;
        strcpy((char*)header->date,today);

        header->rows=sizey;
        header->cols=sizex;
        header->cameratype=camtype;// dalsa
        header->preset=acq_period;
        header->row_bin=1;
        header->col_bin=1;
        header->roi_number=1;

        header->buffer_number = buf_num;



            this->myfile->write((char*)IMM_image,IMM_bytes);



        last_nbytes=IMM_bytes;
    last_data = IMM_image;




}



/******************************************************************************
 *
 *
 *****************************************************************************/

void compressed_file::saveFileIMMComp(
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
    )
{
    compressed_header *header;

    unsigned char* IMM_image;
    int IMM_bytes;

     time_t rawtime;
  struct tm * timeinfo;
  char today[32];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
   sprintf(today,"%s",asctime(timeinfo));



    rawToCompIMM(
        (unsigned char*)imgdata,
        bytes_per_pix*sizex*sizey,
        2,
        sizex,
        sizey,
        comp_threshold,
        0,//timestamp
        0,//imgindex
        &IMM_image,
        &IMM_bytes);

    header=(compressed_header*)IMM_image;
        header->threshhold=comp_threshold;
        header->elapsed=time_stamp;
        header->corecotick=time_stamp;
        header->systick=time_stamp;

        header->col_beg=0;
        header->col_beg=0;
        header->col_end=sizex;
        header->row_beg=0;
        header->row_end=sizey;
        header->number=filenumber;
        strcpy((char*)header->date,today);

        header->rows=sizey;
        header->cols=sizex;
        header->cameratype=camtype;// dalsa
        header->preset=acq_period;
        header->row_bin=1;
        header->col_bin=1;
        header->roi_number=1;

        header->buffer_number = buf_num;

        *num_comp_pixels=header->dlen;



            this->myfile->write((char*)IMM_image,IMM_bytes);

        last_nbytes=IMM_bytes;
    last_data = IMM_image;




}

/******************************************************************************
 *
 *
 *****************************************************************************/

void compressed_file::saveFileIMMComp(
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
    )
{
    compressed_header *header;

    unsigned char* IMM_image;
    int IMM_bytes;

     time_t rawtime;
  struct tm * timeinfo;
  char today[32];

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
   sprintf(today,"%s",asctime(timeinfo));



    rawToCompIMM(
        (unsigned char*)imgdata,
        bytes_per_pix*sizex*sizey,
        2,
        sizex,
        sizey,
        comp_threshold,
        0,//timestamp
        0,//imgindex
        &IMM_image,
        &IMM_bytes);

    header=(compressed_header*)IMM_image;
        header->threshhold=comp_threshold;
        header->elapsed=elapsed_ts;
        header->corecotick=coreco_ts;
        header->systick=systick_ts;

        header->col_beg=0;
        header->col_beg=0;
        header->col_end=sizex;
        header->row_beg=0;
        header->row_end=sizey;
        header->number=filenumber;
        strcpy((char*)header->date,today);

        header->rows=sizey;
        header->cols=sizex;
        header->cameratype=camtype;// dalsa
        header->preset=acq_period;
        header->row_bin=1;
        header->col_bin=1;
        header->roi_number=1;

        header->buffer_number = buf_num;

        *num_comp_pixels=header->dlen;



            this->myfile->write((char*)IMM_image,IMM_bytes);


        last_nbytes=IMM_bytes;
            last_data = IMM_image;



}


/******************************************************************************
 *
 * save imm from data compressed by FPGA - do not pass raw tiff like data
 * saves all 8 or whatever images in the
 *****************************************************************************/

void compressed_file::saveFileFPGACompIMM(
    int comp_threshold,
    unsigned int corecoticksg,
    int sizex,
    int sizey,
    int bytes_per_pix,
    void *imgdata,
    int filenumber,
    int camtype,
    double acq_period
    )
{

    int k;
    int num_frames;
    unsigned char* fpga_image;

    fpga_image=(unsigned char*)imgdata;
    if (fpgaIsValidHeader(fpga_image) == false)
    {
        return;
    }

    num_frames = getFpgaNumFrames((void*)fpga_image);

    for (k=0; k<num_frames; k++)
    {

        saveFrameFPGACompIMM(
             comp_threshold,
            corecoticksg,
            (double)corecoticksg,
            sizex,
            sizey,
            bytes_per_pix,
            imgdata,
            filenumber,
            camtype,
            acq_period,
            k
            );
    }


}



/******************************************************************************
 *
 * save imm from data compressed by FPGA - do not pass raw tiff like data
 * saves only ONE of the frames in he image.
 *****************************************************************************/

bool compressed_file::saveFrameFPGACompIMM(
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
    )
{
    compressed_header *head;

    int k;
    unsigned char* fpga_image;
    unsigned char* IMM_image;
    int IMM_bytes;
    int num_frames;
    compressed_header *header;

    int time_stamp;
     time_t rawtime;
  struct tm * timeinfo;
  char today[32];

    setFPGAImgSize(sizex,sizey,sizex*sizey*bytes_per_pix);

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
   sprintf(today,"%s",asctime(timeinfo));

    //time_stamp=(unsigned int)clock();



    fpga_image=(unsigned char*)imgdata;

    if (fpgaIsValidHeader(fpga_image) == false)
    {
        return(false);
    }

    num_frames = getFpgaNumFrames((void*)fpga_image);


    //for (k=0; k<num_frames; k++)
    //{

    if (which_frame>=num_frames)
        return(false);

    if (which_frame<0)
        return(false);

    k=which_frame;

        fpgaToIMM(
            fpga_image,
            sizex*sizey*bytes_per_pix,
            k,
            &IMM_image,
            &IMM_bytes);

        header=(compressed_header*)IMM_image;
        header->threshhold=comp_threshold;


        header->systick=corecoticksg;
        header->elapsed=xtime_stamp;
        header->col_beg=0;
        header->col_beg=0;
        header->col_end=sizex;
        header->row_beg=0;
        header->row_end=sizey;
        header->number=filenumber;
    strcpy((char*)header->date,today);



        header->rows=sizey;
        header->cols=sizex;
        header->cameratype=camtype;// dalsa
        header->preset=acq_period;
        header->row_bin=1;
        header->col_bin=1;
        header->roi_number=1;

            header->buffer_number = buf_num;


        // need a function to convert FPGA dat to alec data.
        if (myfile != 0)
        {
            myfile->write((char*)IMM_image,IMM_bytes);

            last_nbytes=IMM_bytes;
            last_data = IMM_image;
            return(true);
        }


        return(false);


}




/******************************************************************************
 *
 * save imm from data compressed by FPGA - do not pass raw tiff like data
 *****************************************************************************/

void compressed_file::saveFileFPGACompIMM(
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
    )
{


    int k;
    unsigned char* fpga_image;

    int num_frames;



    fpga_image=(unsigned char*)imgdata;

    if (fpgaIsValidHeader(fpga_image) == false)
    {


        return;
    }

    num_frames = getFpgaNumFrames((void*)fpga_image);


    for (k=0; k<num_frames; k++)
    {

        saveFrameFPGACompIMM(
            comp_threshold,
            corecoticksg,
            xtime_stamp,
            sizex,
            sizey,
            bytes_per_pix,
            imgdata,
            filenumber,
            camtype,
            acq_period,
            k
            );

    }


}

void compressed_file::setBufferNumber(int b)
{
    buf_num = b;
}


};

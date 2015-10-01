
#include "compressed_file.h"
#include "math.h"
compressed_file::compressed_file()
{
    is_open=false;
    myfile=0;
    scratch_memory=new unsigned char[scratch_mem_bytes];
}

compressed_file::~compressed_file()
{
    delete scratch_memory;
}

void compressed_file::open_w(
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


    myfile->open_w();
    is_open=true;

}

void compressed_file::open_w(
        char* name, 
        char* path,
        int start_num, 
        int end_number)
{

    char fullname[512];

    sprintf(fullname,"%s_%05i-%05i.imm",name,start_num,end_number);

    myfile = new image_file(
            fullname,
            path,
            -1,
            false);


    myfile->open_w();
    is_open=true;

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


    myfile->open_r();
    is_open=true;

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


    myfile->open_r();
    is_open=true;

}


void compressed_file::close(void)
{
    if (myfile!=0)
    {
        myfile->close();
        is_open=true;
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
    return((compressed_header*)mem );

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
    unsigned long *IMM_data_long;
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


    //
    // Put some magic numbers in the header
    //
    IMM_header->cols=1024;//!! FPGA only has 1k size as of now...
    IMM_header->rows=1024;

            // bytes per pixel
    IMM_header->bytes = 2;
        // magic numbers
        IMM_header->immversion=11;
        IMM_header->mode = 2;
        IMM_header->compression=6;
        // data length- actually num of exposed pixels. 
        // real len is  numpix*(4+ bytes);
        // fpga stores 2 pixels in each record.
        //IMM_header->dlen = num_pixels;

        IMM_header->col_beg=0;
        IMM_header->col_end=1023;
        //timestamp in us, 31 bit
        IMM_header->corecotick=getFpgaUsTimeStamp32(fpga_header,2147483647);
        //IMM_header->corecotick=getFpgaTimeStamp32(fpga_header);

     IMM_data_long = (unsigned long*)(*IMM_image+compressed_header::header_size);

    current_pix_rec=(fpga_pix_rec*)(fpga_image + pixDataByteAdr(fpga_header));

    // 
    // Fill the pixel location section
    //
    num_pixels=0;
    

    for (k=0;k<num_pix_rec;k++)
    {
        
        if (current_pix_rec->pval0 > 0)
        {
            *IMM_data_long=ushortAddrPix0(current_pix_rec);
            num_pixels++;
            IMM_data_long+=1;
        }

        if (current_pix_rec->pval1 > 0)
        {
            *IMM_data_long=ushortAddrPix1(current_pix_rec);
            IMM_data_long+=1;
            num_pixels++;
        }

        current_pix_rec+=1;

    }
    
    //
    // Fill the pix value data
    //
    IMM_data_short=(unsigned short*)IMM_data_long;

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
    *IMM_bytes = *IMM_bytes + sizeof(unsigned long)*num_pixels;
    *IMM_bytes = *IMM_bytes + sizeof(unsigned short)*num_pixels;

}




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
    
    unsigned long *IMM_data_long;
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

    //
    // Put some magic numbers in the header
    //
    IMM_header->cols=raw_x_pixels;
    IMM_header->rows=raw_y_pixels;

            // bytes per pixel
    IMM_header->bytes = raw_precision;
        // magic numbers
        IMM_header->immversion=11;
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
    //raw_image_ul = (unsigned long*)raw_image;

    num_raw_pixels = raw_bytes/raw_precision;
    num_pixels=num_raw_pixels;

    memcpy((void*)IMM_data_short,(void*)raw_image_us,raw_bytes);
    

    

    IMM_header->dlen = num_pixels;
    // calculate the size of the new compressde image in bytes
    *IMM_bytes = compressed_header::header_size;
    *IMM_bytes = *IMM_bytes + raw_bytes;

}





    int compressed_file::getFpgaNumFrames(void* img)
    {
        comp_img_header *h;

        h=(comp_img_header*)img;
        return (1+(int)(h->numframes));
    }
    
    
    comp_img_header* compressed_file::getFpgaHeadAddr(unsigned char *img, int img_index)
    {
        unsigned char* ptr;
        ptr = img + img_index*512;

        return( (comp_img_header*)ptr);
    }
    unsigned long compressed_file::ushortAddrPix0(fpga_pix_rec* pr)
    {
        unsigned long octobyte_ind;
        unsigned long pix_ind;
        octobyte_ind = 65536* (unsigned long)pr->locmsb + (unsigned long)pr->loclsb;
        pix_ind = 4 * (octobyte_ind-1) + (unsigned long)pr->offset;
        return(pix_ind);
    }
    unsigned long compressed_file::ushortAddrPix1(fpga_pix_rec* pr)
    {
        unsigned long pix_ind;
        pix_ind = ushortAddrPix0(pr) + 1;
        return(pix_ind);
    }
    int compressed_file::numFpgaPixRec(comp_img_header *h)
    {
        return( 
            (int)(h->num_pixels) + 65536*(int)(h->num_pixels_msb)
            );
    }


    unsigned long compressed_file::getFpgaTimeStamp32(comp_img_header *h)
    {
        unsigned long ts,tsl,tsb;
        tsl = (unsigned long)h->timestamp0;
        tsb=(unsigned long)h->timestamp1;
        
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



    int compressed_file::pixDataBytes(comp_img_header *h)
    {
        return( 8*numFpgaPixRec(h));
    }

    unsigned long compressed_file::pixDataByteAdr(comp_img_header *h)
    {
        
        unsigned long base;
        unsigned long nom_size;
        nom_size = 1024*1024*2;

        base = (unsigned long)(h->mem_st_b_lsb) + 
            65536*(h->mem_st_b_msb);

        while(base>nom_size)
            base = base-nom_size;

        return(base);
            
    }

    
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

        if ( (num_rec = numFpgaPixRec(h)) > (262144/num_frames))
            return false;

        if (num_rec<0)
            return false;

        if (pixDataByteAdr(h) > (1024*1024*2))
            return false;


        if (pixDataByteAdr(h) < fpga_header_bytes)
            return false;

        if ( (pixDataByteAdr(h) + pixDataBytes(h)) > (1024*1024*2))
            return false;

    }


    return true;
}


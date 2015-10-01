/********************************************************
 *
 *
 *
 *********************************************************/

#include "tifflib.h"
#include "ccd_exception.h"

tiff_file::tiff_file()
{

    for (int i=0;i<header_size;i++)
        head_data[i] = 0;

    header_ptr=head_data;

}




/////////////////////////////////////////////////////
//
//ifd ifdlen header specs extspecs nextifd all in class space
///////////////////////////////////////////////////

bool tiff_file::tifCheck(char* filename)
{ 
     
    unsigned long got;
    bool is_tiff =false;

 // fp = fopen(filename,'r','n');
#ifdef _WINDOWS
    fp = CreateFile(filename,           // file to create 
             GENERIC_READ,                // open for writing 
             0,                            // do not share 
             NULL,                         // default security 
             OPEN_EXISTING,                // overwrite existing 
             FILE_ATTRIBUTE_NORMAL,       // normal file 
             NULL);  // no attr. template 


    if (fp == INVALID_HANDLE_VALUE)
        throw ccd_exception("tiff_file::tifCheck bad file handle");


    ReadFile( 
        fp, 
        (unsigned char*) &header, 
         sizeof(tiff_header), 
        &got, 
        0
        ); 
#else
    fp = fopen(filename,"r");
    if (fp == 0)
        throw ccd_exception("tiff_file::tifCheck bad file handle");


    fread((void *)&header,sizeof(tiff_header),1,fp);

#endif



#ifdef _WINDOWS
    CloseHandle(fp);
#else
        fclose(fp);
#endif

        try 
        {
            tifEndian();
        }
        catch (ccd_exception error)
        {
            is_tiff = 0;
            return(is_tiff);
        }

    if (header.fortytwo == 42)
    {
        is_tiff = 1;
    }
    return(is_tiff);

}
/////////////////////////////////////////////////////
//
//ifd ifdlen header specs extspecs nextifd all in class space
///////////////////////////////////////////////////

bool tiff_file::tifCheck(HANDLE fp)
{ 
     
    unsigned long got;
    bool is_tiff =false;

    this->fp = fp;
 // fp = fopen(filename,'r','n');
#ifdef _WINDOWS

        SetFilePointer(
            fp,
            0,
            0,
            FILE_BEGIN);

    ReadFile( 
        fp, 
        (unsigned char*) &header, 
         sizeof(tiff_header), 
        &got, 
        0
        ); 

        SetFilePointer(
            fp,
            0,
            0,
            FILE_BEGIN);

#else
        fseek(fp,0,SEEK_SET);


    fread((unsigned char*) &header,sizeof(tiff_header), 1,fp);

        fseek(fp,0,SEEK_SET);


#endif
        try 
        {
            tifEndian();
        }
        catch (ccd_exception error)
        {
            is_tiff = 0;
            return(is_tiff);
        }
    

    if (header.fortytwo == 42)
    {
        is_tiff = 1;
    }
    return(is_tiff);

}

/////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////


bool tiff_file::tifEndian(void)
{
    bool is_big;
 if (header.endian == 0x4949)//0x4949=18761
 { 
  is_big = false;
  }
 else if (header.endian == 0x4d4d)
 {
     is_big=true;
 }
 else 
     throw ccd_exception("tiff_file::tifEndian  doesn't seem to be TiFF"); 

     
     return is_big;

}

/////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////



///////////////////////////////////////////////////
//
//define these args in the clss- ifd, nextifd, ifdlen. make a void
//
//////////////////////////////////////////////////////
void tiff_file::tifIFD(char* filename,unsigned long offset)
{
  unsigned long ifdlen;
  bool is_tiff;
  unsigned long got;
  unsigned long dir_start;

    is_tiff = tifCheck(filename);

    if (is_tiff)
    {
        if (offset==0)
        {
            dir_start = tifFindIFD();
        }
        else
        {
            dir_start = offset;
        }

#ifdef _WINDOWS

        fp = CreateFile(filename,           // file to create 
             GENERIC_READ,                // open for writing 
             0,                            // do not share 
             NULL,                         // default security 
             OPEN_EXISTING,                // overwrite existing 
             FILE_ATTRIBUTE_NORMAL,       // normal file 
             NULL);  // no attr. template 


        if (fp == INVALID_HANDLE_VALUE)
            throw ccd_exception("tiff_file::tifIFD bad file handle");






//        fseek(fp,dir_start,'bof');
        SetFilePointer(
            fp,
            (long)dir_start,
            0,
            FILE_BEGIN);




        //fields = fread(fp,1,'ushort');

        ReadFile( 
            fp, 
            (unsigned char*)&num_ifd_fields, 
            sizeof(unsigned short), 
            &got, 
            0
            ); 


        ifdlen=num_ifd_fields*12;
        
//        ifd = fread(fp,fields*12,'uchar');
        ReadFile( 
            fp, 
            (unsigned char*)ifd, 
            ifdlen, 
            &got, 
            0
            ); 

//        nextifd = fread(fp,1,'ul');
        ReadFile( 
            fp, 
            (unsigned char*)&next_ifd, 
            sizeof(unsigned long), 
            &got, 
            0
            ); 
        
        
        

    CloseHandle(fp);
#else
        fopen(filename,"r");
        if (fp == 0)
            throw ccd_exception("tiff_file::tifIFD bad file handle");

        fseek(fp,dir_start,SEEK_SET);
        fread(&num_ifd_fields,sizeof(short),1,fp);
        ifdlen=num_ifd_fields*12;
        fread(ifd,1,ifdlen,fp);
            fread(&next_ifd,sizeof(unsigned long),1,fp);

    fclose(fp);


#endif


    for (int k=0;k<16;k++)
        scalars[k] = ifd[k+11].valoffs;


    }
    else
    {
        throw ccd_exception("tiff_file::tifIFD Not a Tiff File");
    } 
    
 
}

void tiff_file::tifIFD(HANDLE fp,unsigned long offset)
{
  unsigned long ifdlen;
  bool is_tiff;
  unsigned long got;
  unsigned long dir_start;

  this->fp = fp;

    is_tiff = tifCheck(fp);

    if (is_tiff)
    {
        if (offset==0)
        {
            dir_start = tifFindIFD();
        }
        else
        {
            dir_start = offset;
        }

        

#ifdef _WINDOWS
        SetFilePointer(
            fp,
            (long)dir_start,
            0,
            FILE_BEGIN);

        ReadFile( 
            fp, 
            (unsigned char*)&num_ifd_fields, 
            sizeof(unsigned short), 
            &got, 
            0
            ); 
        ifdlen=num_ifd_fields*12;
        
        ReadFile( 
            fp, 
            (unsigned char*)ifd, 
            ifdlen, 
            &got, 
            0
            ); 

        ReadFile( 
            fp, 
            (unsigned char*)&next_ifd, 
            sizeof(unsigned long), 
            &got, 
            0
            ); 
        
      
        #else

        fseek(fp,dir_start,SEEK_SET);
        fread(&num_ifd_fields,sizeof(short),1,fp);
        ifdlen=num_ifd_fields*12;
        fread(ifd,1,ifdlen,fp);
            fread(&next_ifd,sizeof(unsigned long),1,fp);



#endif

        for (int k=0;k<16;k++)
            scalars[k] = ifd[k+11].valoffs;


    }
    else
    {
        throw ccd_exception("tiff_file::tifIFD Not a Tiff File");
    } 
    
 
}



///////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////


unsigned long tiff_file::tifFindIFD(void)
{
  return(header.first_ifd);
}
///////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////

void tiff_file::tifRd(char* filename,unsigned short* img)
{
// single img tif only
//unsigned char ifd[1024]; define in class 
//unsigned long nextifd; define in class 
unsigned long offset = 0;
long strt;
long sz;
unsigned long got;

//unsigned long ifdlen; define in class 
//unsigned long specs[6]; define in header in class scope
// make emptuy IFD
        specs.width = 0;
    specs.length= 0;
    specs.bit_depth=16;
    specs.compression = 1;
    specs.strip_offset = sizeof(tiff_header);
    specs.strip_bytecnt = 0*0*sizeof(unsigned short);
// make default empty ifd
    tifMakeIFD();
//read file and overrite the ifd

    tifIFD(filename,0);

    tifImgSpecs();
    
    strt = specs.strip_offset;
    sz = specs.strip_bytecnt;
#ifdef _WINDOWS

    fp = CreateFile(filename,           // file to create 
        GENERIC_READ,                // open for writing 
        0,                            // do not share 
        NULL,                         // default security 
        OPEN_EXISTING,                // overwrite existing 
        FILE_ATTRIBUTE_NORMAL,       // normal file 
        NULL);  // no attr. template 

        SetFilePointer(
            fp,
            strt,
            0,
            FILE_BEGIN);

        ReadFile( 
            fp, 
            (unsigned char*)img, 
            sz, 
            &got, 
            0
            ); 
    
    CloseHandle(fp);
#else
    fopen(filename,"r");

    fseek(fp,strt,SEEK_SET);

    fread(img,1,sz,fp);
    
fclose(fp);


#endif


    //read in the header etc.
    tifRdInfo(filename);

 

}
///////////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////////////

void tiff_file::tifRd(HANDLE fp,unsigned short* img)
{
// single img tif only
//unsigned char ifd[1024]; define in class 
//unsigned long nextifd; define in class 
unsigned long offset = 0;
long strt;
long sz;
unsigned long got;

//unsigned long ifdlen; define in class 
//unsigned long specs[6]; define in header in class scope
    this->fp = fp;

        specs.width = 0;
    specs.length= 0;
    specs.bit_depth=16;
    specs.compression = 1;
    specs.strip_offset = sizeof(tiff_header);
    specs.strip_bytecnt = 0*0*sizeof(unsigned short);

    tifMakeIFD();

    tifIFD(fp,0);

    tifImgSpecs();
    
    strt = specs.strip_offset;
    sz = specs.strip_bytecnt;

#ifdef _WINDOWS

        SetFilePointer(
            fp,
            strt,
            0,
            FILE_BEGIN);

        ReadFile( 
            fp, 
            (unsigned char*)img, 
            sz, 
            &got, 
            0
            ); 
#else
    fseek(fp,strt,SEEK_SET);

    fread(img,1,sz,fp);
    
#endif

    //read in the header etc.
    tifRdInfo(fp);
}

/////////////////////////////////////////////////////
//
//
//
/////////////////////////////////////////////////////

void tiff_file::tifRdInfo(char* filename)
{
// single img tif only
unsigned long got;
unsigned long strt;
unsigned long sz;

        specs.width = 0;
    specs.length= 0;
    specs.bit_depth=16;
    specs.compression = 1;
    specs.strip_offset = sizeof(tiff_header);
    specs.strip_bytecnt = 0*0*sizeof(unsigned short);

    tifMakeIFD();


 tifIFD(filename,0);
 tifImgSpecs();
    if (ifd[11].tag==6000)
    {
    
        strt =ifd[11].valoffs;
         sz = ifd[11].count;

#ifdef _WINDOWS

     fp = CreateFile(filename,           // file to create 
        GENERIC_READ,                // open for writing 
        0,                            // do not share 
        NULL,                         // default security 
        OPEN_EXISTING,                // overwrite existing 
        FILE_ATTRIBUTE_NORMAL,       // normal file 
        NULL);  // no attr. template 

        SetFilePointer(
            fp,
            strt,
            0,
            FILE_BEGIN);

        ReadFile( 
            fp, 
            (unsigned char*)head_data, 
            sz, 
            &got, 
            0
            ); 
    
    CloseHandle(fp);
#else

         fopen(filename,"r");

        fseek(fp,strt,SEEK_SET);

        fread(head_data,1,sz,fp);

        fclose(fp);

#endif




    }
  
}


void tiff_file::tifRdInfo(HANDLE fp)
{
// single img tif only
unsigned long got;
unsigned long strt;
unsigned long sz;

this->fp = fp;

        specs.width = 0;
    specs.length= 0;
    specs.bit_depth=16;
    specs.compression = 1;
    specs.strip_offset = sizeof(tiff_header);
    specs.strip_bytecnt = 0*0*sizeof(unsigned short);

    tifMakeIFD();

 tifIFD(fp,0);
 tifImgSpecs();
    
    if (ifd[11].tag==6000)
    {
        strt =(unsigned long)ifd[11].valoffs;
        sz = ifd[11].count;

#ifdef _WINDOWS

        SetFilePointer(
            fp,
            strt,
            0,
            FILE_BEGIN);

        ReadFile( 
            fp, 
            (unsigned char*)head_data, 
            sz, 
            &got, 
            0
            ); 
#else

        fseek(fp,strt,SEEK_SET);

        fread(head_data,1,sz,fp);

#endif

    
    }  
}


///////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////
void tiff_file::tifWr(
    char* filename,
    unsigned short *img,
    int sizex,
    int sizey)
{

    unsigned long got;


    specs.width = sizex;
    specs.length= sizey;
    specs.bit_depth=16;
    specs.compression = 1;
    specs.strip_offset = sizeof(tiff_header);
    specs.strip_bytecnt = sizex*sizey*sizeof(unsigned short);

    tifMakeIFD();
    header.endian = 18761;
    header.fortytwo  = 42;
    header.first_ifd = specs.strip_bytecnt + sizeof(tiff_header)+header_size;

#ifdef _WINDOWS

    fp = CreateFile(
            filename,           // file to create 
             GENERIC_WRITE,                // open for writing 
             0,                            // do not share 
             NULL,                         // default security 
             CREATE_ALWAYS,                // overwrite existing 
             FILE_ATTRIBUTE_NORMAL,       // normal file 
             NULL);                        // no attr. template 

//write TIFF header



    WriteFile(
    fp,
        &header,
        sizeof(tiff_header),
        &got,
        NULL
        );

    //write image
    WriteFile(
    fp,
        (unsigned char*) img,
        specs.strip_bytecnt,
        &got,
        NULL
        );
    //write header
    WriteFile(
    fp,
        (unsigned char*) head_data,
        header_size,
        &got,
        NULL
        );

    ifd[11].tag = 6000;
    ifd[11].type = 2;
    ifd[11].count = 4096;
    ifd[11].valoffs = sizeof(tiff_header) + specs.strip_bytecnt;


tifWriteIFD();


    WriteFile(
    fp,
        (unsigned char*) &next_ifd,
        sizeof(long),
        &got,
        NULL
        );


CloseHandle(fp);

#else
        fopen(filename,"w");

//write TIFF header

    fwrite(&header,sizeof(tiff_header),1,fp);

    //write image
    fwrite(img,1,specs.strip_bytecnt,fp);
    //write header
    fwrite(head_data,1,header_size,fp);

    ifd[11].tag = 6000;
    ifd[11].type = 2;
    ifd[11].count = 4096;
    ifd[11].valoffs = sizeof(tiff_header) + specs.strip_bytecnt;


tifWriteIFD();

    fwrite(&next_ifd,sizeof(long),1,fp);

fclose(fp);

#endif

}

///////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////
void tiff_file::tifWr(
    HANDLE fp,
    unsigned short *img,
    int sizex,
    int sizey)
{

    unsigned long got;

    this->fp = fp;

    specs.width = sizex;
    specs.length= sizey;
    specs.bit_depth=16;
    specs.compression = 1;
    specs.strip_offset = sizeof(tiff_header);
    specs.strip_bytecnt = sizex*sizey*sizeof(unsigned short);

    tifMakeIFD();
    header.endian = 18761;
    header.fortytwo  = 42;
    header.first_ifd = specs.strip_bytecnt + sizeof(tiff_header)+header_size;

    

//write TIFF header

#ifdef _WINDOWS

    WriteFile(
    fp,
        &header,
        sizeof(tiff_header),
        &got,
        NULL
        );

    //write image
    WriteFile(
    fp,
        (unsigned char*) img,
        specs.strip_bytecnt,
        &got,
        NULL
        );
    //write header
    WriteFile(
    fp,
        (unsigned char*) head_data,
        4096,
        &got,
        NULL
        );

    ifd[11].tag = 6000;
    ifd[11].type = 2;
    ifd[11].count = 4096;
    ifd[11].valoffs = sizeof(tiff_header) + specs.strip_bytecnt;


tifWriteIFD();


    WriteFile(
    fp,
        (unsigned char*) &next_ifd,
        sizeof(long),
        &got,
        NULL
        );


#else
//write TIFF header

    fwrite(&header,sizeof(tiff_header),1,fp);

    //write image
    fwrite(img,1,specs.strip_bytecnt,fp);
    //write header
    fwrite(head_data,1,header_size,fp);

    ifd[11].tag = 6000;
    ifd[11].type = 2;
    ifd[11].count = 4096;
    ifd[11].valoffs = sizeof(tiff_header) + specs.strip_bytecnt;


tifWriteIFD();

    fwrite(&next_ifd,sizeof(long),1,fp);


#endif


}



/////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////
void tiff_file::tifWriteIFD(void)
{
    bool is_ok;
    unsigned long got;

    

#ifdef _WINDOWS

// write num of foelds
    is_ok =WriteFile(
    fp,
        (unsigned char*) &num_ifd_fields,
        sizeof(unsigned short),
        &got,
        NULL
        );

//write ifd

        is_ok =WriteFile(
            fp,
        (unsigned char*) ifd,
        num_ifd_fields*sizeof(ifd_field),
        &got,
        NULL
        );
#else
// write num of foelds

    fwrite(&num_ifd_fields,sizeof(unsigned short),1,fp);

//write ifd

    fwrite(ifd,sizeof(ifd_field),num_ifd_fields,fp);

#endif

}

/////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////

void tiff_file::tifMakeIFD(void)
{
 int field;
    num_ifd_fields = 27;
    next_ifd = 0;

    ifd[0].tag = 256; 
    ifd[0].type = 3;
    ifd[0].count = 1;
    ifd[0].valoffs = specs.width;


    ifd[1].tag = 257; 
    ifd[1].type = 3;
    ifd[1].count = 1;
    ifd[1].valoffs = specs.length;


    ifd[2].tag = 258; 
    ifd[2].type = 3;
    ifd[2].count = 1;
    ifd[2].valoffs = specs.bit_depth;


    ifd[3].tag = 259; 
    ifd[3].type = 3;
    ifd[3].count = 1;
    ifd[3].valoffs = specs.compression;


    ifd[4].tag = 262; 
    ifd[4].type = 3;
    ifd[4].count = 1;
    ifd[4].valoffs = 1;


    ifd[5].tag = 270; 
    ifd[5].type = 2;
    ifd[5].count = 0;
    ifd[5].valoffs = 0;


    ifd[6].tag = 273; 
    ifd[6].type = 3;
    ifd[6].count = 1;
    ifd[6].valoffs = specs.strip_offset;


    ifd[7].tag = 277; 
    ifd[7].type = 3;
    ifd[7].count = 1;
    ifd[7].valoffs = 1;


    ifd[8].tag = 278; 
    ifd[8].type = 3;
    ifd[8].count = 1;
    ifd[8].valoffs = specs.width;


    ifd[9].tag = 279; 
    ifd[9].type = 3;
    ifd[9].count = 1;
    ifd[9].valoffs = specs.strip_bytecnt;

    ifd[10].tag = 284; 
    ifd[10].type = 3;
    ifd[10].count = 1;
    ifd[10].valoffs = 1;

    ifd[11].tag = 6000; 
    ifd[11].type = 3;
    ifd[11].count = 0;
    ifd[11].valoffs = 0;

     for (field=12;field<num_ifd_fields;field++)
     {
        ifd[field].tag = 6000 + field - 11; 
        ifd[field].type = 4;
        ifd[field].count = 1;
        ifd[field].valoffs = scalars[field-11];
     }

}

/////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////


void tiff_file::tifImgSpecs(void)
{
int field;

 for (field=0;field<num_ifd_fields;field++)
 {
//  [tag, typ, numval, valoff] = tifParseIFD(field,ifd);
 
  if (ifd[field].tag == 256) //img width
    specs.width = ifd[field].valoffs;
 
  if (ifd[field].tag == 257) //img len
    specs.length = ifd[field].valoffs;
  
  


  if (ifd[field].tag == 258) //bits per samp
     specs.bit_depth = ifd[field].valoffs;
 
  
  
  if (ifd[field].tag == 259) //compression
     specs.compression = ifd[field].valoffs;
 
  
  
  if (ifd[field].tag == 273) //strip offsets - where img is
    specs.strip_offset = ifd[field].valoffs;
  
  
  if (ifd[field].tag == 279) //img dtat length
     specs.strip_bytecnt = ifd[field].valoffs;
  
  

 }

}



char* tiff_file::getHeader(void)
{

    return head_data;
}

void tiff_file::putStrHeader(char * strg)
{
        
    strcpy(header_ptr,strg);
    while (*header_ptr != 0) header_ptr++;

}




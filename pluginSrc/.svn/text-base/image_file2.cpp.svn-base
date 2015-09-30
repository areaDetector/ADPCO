/*******************************************************************************
 *
 *	image_file.cpp
 *
 *	Author: Tim Madden
 *	Date:	8/10/03
 *	Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/


/*
 * Include files.
 */

#include "image_file2.h"
#include <math.h>
#include <stdio.h>


namespace nd_imm_plugin {
/*******************************************************************************
 *
 *
 ******************************************************************************/
image_file::image_file(char* name)
{

	path_name[0] = 0;

	strcpy(file_name,name);

	is_use_tiff = false;
	for (int i = 0; i<header_size; i++)
		head_data[i] = 0;

	header_ptr=head_data;

	if (strstr(name,"NULL")==0)
		is_null_file = false;
	else
		is_null_file = true;

}

/*******************************************************************************
 *
 *
 ******************************************************************************/
// make file object.
image_file::image_file(
	char* name,
	int count)
{
		path_name[0] = 0;


	sprintf(file_name,"%s%0#3i",name,count);
	is_use_tiff = false;

	for (int i = 0; i<header_size; i++)
		head_data[i] = 0;

	header_ptr=head_data;

	if (strstr(name,"NULL")==0)
		is_null_file = false;
	else
		is_null_file = true;

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
	bool is_tiff)
{
		path_name[0] = 0;

	is_use_tiff = is_tiff;

	if (strstr(name,"NULL")==0)
		is_null_file = false;
	else
		is_null_file = true;



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
	bool is_tiff)
{
		path_name[0] = 0;

	if (strstr(name,"NULL")==0)
		is_null_file = false;
	else
		is_null_file = true;




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

	if (is_null_file)
		return(true);

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

	fp = fopen(file_name,"rb");
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
	if (is_null_file)
		return(true);

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

	fp = fopen(file_name,"wb");
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
	if (is_null_file)
		return(true);

//	fp = fopen(file_name,"a");
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
	if (is_null_file)
		return(true);

#ifdef _WINDOWS

	if (fp != INVALID_HANDLE_VALUE)
 	    CloseHandle(fp);
#else

	if (fp!=0)
		fclose(fp);

#endif


	return true;
}


/*******************************************************************************
 *
 *
 ******************************************************************************/

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

	if (is_null_file)
		return(true);


	if (is_use_tiff)
	{
		unsigned long sizeq=(unsigned long)sqrt((double)(size/sizeof(short)));
	//tiff.tifWr(fp,(unsigned short*)ptr,sizeq,sizeq);
	return false;
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
	if (is_null_file)
		return(true);


	if (is_use_tiff)
	{
		//size is num of bytes- convert to x and y pixels for square images.
		unsigned long sizeq=(unsigned long)sqrt((double)(size/sizeof(short)));
	//tiff.tifWr(fp,(unsigned short*)ptr,sizeq,sizeq);
	return false;
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

	if (is_null_file)
		return(true);



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

	if (is_null_file)
		return(true);

//!!
//	fread(ptr,size,size,fp);
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

};


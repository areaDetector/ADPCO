/*******************************************************************************
 *
 *	image_ram.h
 *
 *	Author: Tim Madden
 *	Date:	6/30/03
 *	Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/


/*
 * Include files.
 */
#include "ccd_exception.h"
#include "image_ram2.h"
#include <math.h>
#include <memory.h>

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/

image_ram::image_ram()
{
	image_x = def_size;
	image_y = def_size;
	ram_x = image_x;
	ram_y = image_y;
	// int data
	is_float=false;

	ram_size = def_size*def_size;
	// 64 is a dumb fidge factor to get a little extra mem.
	// hw bug in camera sends 2 FFFF pixels- i just ignore them!
	memory = (unsigned char*)(new unsigned short[ram_size+64]);
	memorysave = memory;

	sigma_delta_error = 0;
	random_numbers=0;

	ram_bytes = ram_size*sizeof(unsigned short);

	is_binning = false;
	image_size = ram_size/2;

	is_img_rot=false;
	is_y_inv=false;
	is_x_inv=false;

	numccds_x = 1;
	numccds_y = 1;

	makeRandTable();

	if (image_size*2 != ram_size)
		throw ccd_exception("image_ram::image_ram - need even size memory");
}



/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/

image_ram::image_ram(int x, int y)
{
	image_x = x;
	image_y = y;
	ram_size = x*y;
	ram_x = image_x;
	ram_y = image_y;
	// 64 is a dumb fidge factor to get a little extra mem.
	// hw bug in camera sends 2 FFFF pixels- i just ignore them!

	memory = (unsigned char*)(new unsigned short[ram_size+64]);
	memorysave = memory;
	ram_bytes = ram_size*sizeof(unsigned short);

	is_float=false;

	is_binning = false;
	image_size = ram_size;


	is_img_rot=false;
	is_y_inv=false;
	is_x_inv=false;

		sigma_delta_error = 0;
	random_numbers=0;

	makeRandTable();

	if (ram_size&1)
		throw ccd_exception("image_ram::image_ram - need even size memory");
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/

image_ram::image_ram(int x, int y,bool fl)
{
	if (fl==true)//float data
		is_float=true;
	else
		is_float=false;


	image_x = x;
	image_y = y;
	ram_size = x*y;
	ram_x = image_x;
	ram_y = image_y;
	// 64 is a dumb fidge factor to get a little extra mem.
	// hw bug in camera sends 2 FFFF pixels- i just ignore them!


	is_img_rot=false;
	is_y_inv=false;
	is_x_inv=false;


	if (is_float==false)
	{
		memory = (unsigned char*)(new unsigned short[ram_size+64]);
		ram_bytes = ram_size*sizeof(unsigned short);

	}
	else
	{
		memory = (unsigned char*)(new float[ram_size+64]);
		ram_bytes = ram_size*sizeof(float);

	}

	is_binning = false;
	image_size = ram_size;
	memorysave = memory;

		sigma_delta_error = 0;
	random_numbers=0;
	makeRandTable();

//	if (ram_size&1)
//		throw ccd_exception("image_ram::image_ram - need even size memory");
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/

image_ram::image_ram(image_ram &img)
{
	is_float=img.getIsFloat();
	ram_size = img.ram_size;

	if (is_float==false)
	{
		memory = (unsigned char*)(new unsigned short[ram_size+64]);
		ram_bytes = ram_size*sizeof(unsigned short);

	}
	else
	{
		memory = (unsigned char*)(new float[ram_size+64]);
		ram_bytes = ram_size*sizeof(float);
	}
	memorysave = memory;

		sigma_delta_error = 0;
	random_numbers=0;
	makeRandTable();


	this->copy(img);
}


/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		// destroy mutex object.
image_ram::~image_ram()
{
		delete memorysave;
		delete random_numbers;


}

void image_ram::makeRandTable(void)
{
	rand_index = 0;
	random_numbers = new double[rand_tab_size];
	for (int k=0; k<rand_tab_size; k++)
	{
		random_numbers[k] = ((double)(rand()) / (double)RAND_MAX);
	}
}


double image_ram::getRand(void)
{
	if (rand_index< (rand_tab_size-1) )
		rand_index++;
	else
		rand_index=0;

	return(random_numbers[rand_index]);
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/

void image_ram::copy(image_ram &img)
{
	unsigned short *mcnts0;
	unsigned short *mcnts1;
	float *mcntf0;
	float *mcntf1;


	is_float=img.getIsFloat();


	image_x = img.image_x;
	image_y = img.image_y;
	ram_size = img.ram_size;
	ram_x = img.ram_x;
	ram_y = img.ram_y;
	is_binning = img.is_binning;
	image_size = img.image_size;

	// 64 is a dumb fidge factor to get a little extra mem.
	// hw bug in camera sends 2 FFFF pixels- i just ignore them!


	is_img_rot=img.is_img_rot;
	is_y_inv=img.is_y_inv;
	is_x_inv=img.is_x_inv;


	if (is_float==false)
	{
		int k;

		mcnts1=(unsigned short*)img.getMem();
		mcnts0=(unsigned short*)memory;

		for (k=0;k<image_size;k++)
		{
			mcnts0[k]=mcnts1[k];
		}
	}
	else
	{
		int k;

		mcntf1=(float*)img.getMem();
		mcntf0=(float*)memory;

		for (k=0;k<image_size;k++)
		{
			mcntf0[k]=mcntf1[k];
		}
	}




//	if (ram_size&1)
//		throw ccd_exception("image_ram::image_ram - need even size memory");

}





void image_ram::copyQuad(image_ram &img, int quadrant)
{
	unsigned short *mcnts0;
	unsigned short *mcnts1;
	float *mcntf0;
	float *mcntf1;


	is_float=img.getIsFloat();


	image_x = img.image_x / 2;
	image_y = img.image_y / 2;
	ram_size = img.ram_size;
	ram_x = img.ram_x;
	ram_y = img.ram_y;
	is_binning = img.is_binning;
	image_size = img.image_size / 4;

	// 64 is a dumb fidge factor to get a little extra mem.
	// hw bug in camera sends 2 FFFF pixels- i just ignore them!


	is_img_rot=img.is_img_rot;
	is_y_inv=img.is_y_inv;
	is_x_inv=img.is_x_inv;

	if (is_float==true)
		throw ccd_exception("image_ram::copyQuad only works for unsigned short images");


	mcnts1=(unsigned short*)img.getMem();
	// lins size in puixels
	int linesize = this->image_x;
	// pointer to source iamge
	unsigned short *startptr;

	// now we switch based on quadrant.
	if (quadrant==0) //we are in quad 2 or 3, so skip qiad 0 and 1.
	{
		startptr = mcnts1;
	}
	if (quadrant==1) //we are in quad 2 or 3, so skip qiad 0 and 1.
	{
		startptr = mcnts1+sizeof(unsigned short)*linesize;
	}
	if (quadrant==2) //we are in quad 2 or 3, so skip qiad 0 and 1.
	{
		startptr = mcnts1+sizeof(unsigned short)*2*linesize*linesize;
	}
	if (quadrant==3) //we are in quad 2 or 3, so skip qiad 0 and 1.
	{
		startptr = mcnts1+sizeof(unsigned short)*(2*linesize*linesize+linesize);
	}


		int k;


		mcnts0=(unsigned short*)memory;

	for (k = 0; k<linesize;k++)
	{
	//copy 1 line at a time

	memcpy(mcnts0,startptr,linesize*sizeof(unsigned short));
	startptr+= sizeof(unsigned short)*2*linesize;
	mcnts0+=sizeof(unsigned short)*linesize;
	}


}



/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		// mem_ptr
unsigned char* image_ram::getMem2(void)
{
	// 2 is a damn fudge factor-- bug in camera hw sends 2 pixels of shit.
	// we ignire them here. tjm
		return memory+2;

}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		// mem_ptr
unsigned char* image_ram::getMem(void)
{
		return memory;
}


void image_ram::putMem(void *ptr)
{

//	delete memory;
	memory=(unsigned char*)ptr;
}


/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		// clearimg
void image_ram::clrRam(void)
{
	int i;
	unsigned short *memcnt;

	if (this->getIsFloat())
		throw ccd_exception("image_ram::clrRam only works for int images");

	memcnt = (unsigned short *)memory;

		for (i = 0;i<image_size; i++)
		{
			memcnt[i] = 0;
		}

}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		//get size
int image_ram::getRamSize(void)
{
	return ram_size;
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		//get size
int image_ram::getImgX(void)
{
	return image_x;
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		//get size
int image_ram::getImgY(void)
{
	return image_y;
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		//get size
int image_ram::getNumCcdX(void)
{
	return numccds_x;
}
/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		//get size
int image_ram::getNumCcdY(void)
{
	return numccds_y;
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
void image_ram::setNumCcdX(int x, int y)
{
	numccds_x = x;
	numccds_y = y;
}



/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
		//get size
int image_ram::getImgSize(void)
{
	return image_size;
}




/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/

void image_ram::setImageSize(
	int sizex,
	int sizey)
{
	image_x=sizex;
	image_y=sizey;
	image_size=image_x*image_y;
	if (image_size>ram_size)
	{
		image_x=def_size;
		image_y=def_size;
		image_size=image_x*image_y;
		throw ccd_exception("image_ram::setImageSize - request image too big");
	}


}


/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
			// set binning mode
void image_ram::setBinning(bool mode)
{
	is_binning = mode;
	if (is_binning)
	{
		image_size = ram_size/2;
		image_x = ram_x/2;
		image_y = ram_y/2;
	}
	else
	{
		image_size = ram_size;
		image_x = ram_x;
		image_y = ram_y;
	}


}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
			// check binning
bool image_ram::getBinning(void)
{
	return is_binning;
}


/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
// set image id
void image_ram::setImageID(const unsigned long id)
{
	image_id = id;
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 *
 ******************************************************************************/
// get image id
unsigned long image_ram::getImageID(void)
{
	return image_id;
}


/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/

#if 0
bool image_ram::checkTags(void)
{
	unsigned short* idata=(unsigned short*)getMem2();

	int x,y;
	unsigned short pixel;

	// check right side.top
	x=2047;

	for (y=0; y<1024; y++)
	{
		pixel=idata[y*getImgY() + x];
		if (pixel!=y)
			return(false);

	}
// rhs bottom
	for (y=1024; y<2048; y++)
	{
		pixel=idata[y*getImgY() + x];
		if (pixel!= (2047-y))
			return(false);

	}

	//dont checl left side for now... there is aminor hw bug.

	return(true);
}

#endif
// BESSRC check tags
bool image_ram::checkTags(void)
{
 int x,y,ystep,count,val;

	if (this->getNumCcdX()==1) // single ccd
	{
		x = 0;
		ystep = 2048/this->getImgY();
		count = 60439;

		for ( y = 0; y<this->getImgY(); y+=ystep)
		{
			val = this->getPixi(x,y);
			if ((val) != count)
				return(false);

			count = (count+2048) % 65536;
		}

	}
	else //quad ccd
	{
		x = 0;
		ystep = 4096/this->getImgY();
		count = 60439;

		for ( y = 0; y<this->getImgY(); y+=ystep)
		{
			val = this->getPixi(x,y);
			if ((val) != count)
				return(false);

			count = (count+2048) % 65536;
		}


		x = this->getImgX()/2;
		 ystep = 4096/this->getImgY();
		 count = 60439;
		for ( y = 0; y<this->getImgY(); y+=ystep)
		{
			val = this->getPixi(x,y);
			if ((val) != count)
				return(false);

			count = (count+2048) % 65536;
		}


	}

	return(true);
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		// is it floating point?
bool image_ram::getIsFloat(void)
{
	return is_float;
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		//get pixel vlue as float
float image_ram::getPix(int x, int y)
{
	unsigned long addr;
	int temp;


	if (is_x_inv)
		x=getImgX()-x;

	if (is_y_inv)
		y=getImgY()-y;

	if (is_img_rot)
	{
		temp=x;
		x=y;
		y=temp;
	}
	if (is_float)
	{
		addr=(sizeof(float)*(this->getImgY()*y+x));
		return *((float*)(memory+addr));
	}

		addr=(sizeof(unsigned short) * (this->getImgY()*y+x));
		return *((unsigned short*)(memory+addr));


}

/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		//get pixel vlue as float
unsigned short image_ram::getPixi(int x, int y)
{
	unsigned long addr;
	int temp;

		if (x<0 || x>=image_x || y <0 || y>=image_y)
			return((unsigned short)0);


		addr=(sizeof(unsigned short) * (image_y*y+x));
		return *((unsigned short*)(memory+addr));


}


/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
void image_ram::setPix(int x, int y, float val)
{
	unsigned long addr;
	int temp;


	if (is_x_inv)
		x=getImgX()-x;

	if (is_y_inv)
		y=getImgY()-y;

	if (is_img_rot)
	{
		temp=x;
		x=y;
		y=temp;
	}

	if (is_float)
	{
		addr=(sizeof(float)*(this->getImgY()*y+x));
		*((float*)(memory+addr))=val;
	}
	else
	{
		addr=(sizeof(unsigned short) * (this->getImgY()*y+x));
		*((unsigned short*)(memory+addr))=(unsigned short)val;
	}

}

/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
void image_ram::setPixi(int x, int y, unsigned short val)
{
	unsigned long addr;
	int temp;


		if (x<0 || x>=image_x || y <0 || y>=image_y)
			return;



		addr=(sizeof(unsigned short) * (image_y*y+x));
		*((unsigned short*)(memory+addr))=(unsigned short)val;


}




/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		// swap x,y coords on getPix, setPix
void image_ram::setIsRot(bool is_rot)
{
	is_img_rot=is_rot;
}
/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		// take x=2048-x to reverse x coord
void image_ram::setInvX(bool is_inv)
{
	is_y_inv=is_inv;
}
/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
void image_ram::setInvY(bool is_inv)
{
	is_x_inv=is_inv;
}



/*******************************************************************************
 *
 *	Class image_ram
 *
 ******************************************************************************/
		// Add a const. of sign to the image. sign=true for +
void image_ram::addConst(float bias)
{

	float *memf;
	unsigned short *memi;
	unsigned short biasi;
	int k;
	unsigned short intersum;

	if (is_float)
	{
		memf=(float*)memory;

		for (k=0;k<image_size;k++)
		{
			memf[k] = memf[k] + (float)bias;
		}
	}
	else
	{
		memi=(unsigned short*)memory;
		biasi=(unsigned short)abs(bias);
		if (bias>=0)
		{
			for (k=0;k<image_size;k++)
			{
				intersum = memi[k] + biasi;
				if (intersum >= memi[k])
					memi[k] = intersum;
				else
					memi[k] = 65535;
			}
		}
		else
		{
			for (k=0;k<image_size;k++)
			{
				intersum = memi[k] - biasi;
				if (intersum <= memi[k])
					memi[k] = intersum;
				else
					memi[k] = 0;
			}
		}
	}

}





/*******************************************************************************
 *
 *	Class image_ram
 *
 ******************************************************************************/
		// Add a const. of sign to the image. sign=true for +
void image_ram::multConst(float bias)
{

	float *memf;
	unsigned short *memi;
	unsigned short biasi;
	int k;
	unsigned short intersum;
	float fnum0,fnum1,fnum2,fnum3;

	if (is_float)
	{
		memf=(float*)memory;

		for (k=0;k<image_size;k++)
		{
			memf[k] = memf[k] * (float)bias;
		}
	}
	else
	{
		memi=(unsigned short*)memory;
		//biasi=(unsigned short)abs(bias);

			for (k=0;k<image_size;k++)
			{
				fnum0 = (float)(memi[k]);
				fnum1=floor(fnum0*bias + 0.5);
				if (fnum1<0.0)
					fnum1=0.0;

				if (fnum1>65535.0)
					fnum1=65535.0;

					memi[k] = (unsigned short)fnum1;

			}

	}

}






/*******************************************************************************
 *
 *	Class image_ram
 *
 ******************************************************************************/
		// Add a const. of sign to the image. sign=true for +
void image_ram::setFloor(float minv)
{

	float *memf;
	unsigned short *memi;
	unsigned short biasi;
	int k;
	unsigned short intersum;
	float fnum0,fnum1,fnum2,fnum3;
	unsigned short minvs;

	if (is_float)
	{
		memf=(float*)memory;

		for (k=0;k<image_size;k++)
		{
			if (memf[k]<minv)
				memf[k] = minv;
		}
	}
	else
	{
		memi=(unsigned short*)memory;
		//biasi=(unsigned short)abs(bias);
		minvs=(unsigned short)floor(minv+0.5);

			for (k=0;k<image_size;k++)
			{
				if (memi[k]<minvs)
					memi[k]=minvs;

			}

	}

}






void image_ram::float2Short(float multval)
{
	unsigned char *memoryi, *oldmem;
	float *memf;
    unsigned short *memi;
    int k;
	float pixvalf;
	float noise;

	if (!is_float)
		throw ccd_exception("image_ram::float2Short img not float");

//	oldmem=memory;

//		memoryi= (unsigned char*)(new unsigned short[ram_size+64]);
    memi=(unsigned short*)memory;
	memf=(float* )memory;
	for (k=0;k<image_size;k++)
	{
		pixvalf = multval*memf[k];
		noise = ((float)rand()) / ((float)RAND_MAX);

		pixvalf = floor(pixvalf + noise);
		memi[k] = (unsigned short)pixvalf;
	}

	is_float=false;

//	memory=memoryi;
//	delete(oldmem);
}


void image_ram::short2Float(float multval)
{
	unsigned char *memoryf, *oldmem;
	float *memf;
    unsigned short *memi;
    int k;
	bool is_del=false;

	if (is_float)
		throw ccd_exception("image_ram::short2Float img not short");
	if (ram_bytes < sizeof(float)*ram_size)
	{
		oldmem=memory;
		memoryf= (unsigned char*)(new float[ram_size+64]);
		ram_bytes = ram_size*sizeof(float);
	    memi=(unsigned short*)memory;
		memf=(float* )memoryf;
		is_del = true;
	}
	else
	{
	    memi=(unsigned short*)memory;
		memf=(float* )memory;
	}

	for (k=image_size-1;k>=0;k=k-1)
	{
		memf[k] = multval*((float)memi[k]);
	}

	is_float=true;

	if (is_del)
	{
		memory=memoryf;
		delete(oldmem);

	}
}

/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		// Add an image (or subratt image with sign = 0)
void image_ram::addImage(
	image_ram& img,
	bool sign)
		{

	float *memf;
	unsigned short *memi;
	float *memf2;
	unsigned short *memi2;
	int k;
	unsigned short intersum;





	if (is_float)
	{
		memf=(float*)memory;
		memf2=(float*)img.getMem();

		if (sign)
		{
			for (k=0;k<image_size;k++)
			{
				memf[k] = memf[k] + memf2[k];
			}
		}
		else
		{
			for (k=0;k<image_size;k++)
			{
				memf[k] = memf[k] - memf2[k];
			}
		}
	}
	else
	{
		memi=(unsigned short*)memory;
		memi2=(unsigned short*)img.getMem();
		if (sign)
		{
			for (k=0;k<image_size;k++)
			{
				intersum = memi[k] + memi2[k];
				if (intersum >= memi[k])
					memi[k] = intersum;
				else
					memi[k] = 65535;
			}
		}
		else
		{
			for (k=0;k<image_size;k++)
			{
				intersum = memi[k] - memi2[k];
				if (intersum <= memi[k])
					memi[k] = intersum;
				else
					memi[k] = 0;
			}
		}
	}

}


/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		// Add an image (or subratt image with sign = 0)
void image_ram::multImage(
	image_ram& img,
	int bias)
		{

	float *memf;
	unsigned short *memi;
	float *memf2;
	int k;
	float pixvalf;

	if (is_float)
	{
		memf=(float*)memory;
		memf2=(float*)img.getMem();

			for (k=0;k<image_size;k++)
			{
				pixvalf=memf[k];
				pixvalf=pixvalf-(float)bias;
				pixvalf=pixvalf*memf2[k];
				if (memf2[k] != 0.0) // to retain mask of zeros
				{
					 pixvalf=pixvalf+(float)bias;
				}
				memf[k] = pixvalf;
			}
	}
	else
	{
		memi=(unsigned short*)memory;
		memf2=(float*)img.getMem();
			for (k=0;k<image_size;k++)
			{
				if (memf2[k] != 0.0) // to retain mask of zeros
				{
					pixvalf=(float)memi[k];
					pixvalf=pixvalf-(float)bias;
					pixvalf=pixvalf*memf2[k];

					pixvalf=pixvalf+(float)bias;

					if (pixvalf<0.0)
						pixvalf = 0.0;
					if (pixvalf>65535.0)
						pixvalf = 65535.0;
					// add dither before converting to short...
					memi[k] = (unsigned short)floor( pixvalf + (((double)rand())/((double)RAND_MAX)  ));;
				}
				else
				memi[k]=0;
			}
	}

}


/*******************************************************************************
 *
 *	Class image_ram
 *
 * the camera shoud put pixels on left and right edges of image. count from
  * 0 to 1023, then back to 0. retuen true of tags OK
 ******************************************************************************/
		// Add an image (or subratt image with sign = 0)
void image_ram::multImage(
	image_ram& img,
	int bias,
	int bits)
		{

	unsigned short *memi, *memi2;
	signed long pixval;

	int k;
	int randsize;
	float randsizef;
	signed long rnum;
		randsize = 2<<bits;
		randsizef = (float)randsize;

		memi=(unsigned short*)memory;
		memi2=(unsigned short*)img.getMem();
			for (k=0;k<image_size;k++)
			{
				if (memi2[k] != 0) // to retain mask of zeros
				{
					pixval=(signed long)memi[k];
					pixval=pixval-bias;

					pixval=pixval*memi2[k];
					// add dither here...
					rnum=(signed long)(getRand() * randsizef);
					pixval = (pixval + rnum)>>bits;

					pixval=pixval+bias;
					if (pixval>65535)
						pixval=65535;

					if (pixval<0)
						pixval=0;

					memi[k] = (unsigned short)pixval;
				}
				else
					memi[k] = 0;
			}


}


/*******************************************************************************
 *
 		// note that we can only shift left right up down. Not at an angle.
		// if whole_pixels_y is nonzero, then whole_pizels_x must be 0, or else
		// the copy is messed up. It is actually a digital filter with zero order
		// interpolation. It only works in one dimension.



		Left right top bottom is determined by

		y
		|		top
		|
		|left			right
		|
		|		bottom
		|
		(0,0)-----------------------x


 ******************************************************************************/
		// Shift entire image or 1/2 of image, or 1/4 of image.
void image_ram::shiftImage(
	direction image_portion,
	direction shift_direction,
	double num_pixels)
{
	// Make a copy of this iamge on the heap.
	image_ram *img_copy;
	int whole_pixels_x;
	int whole_pixels_y;
	int whole_pixels_x1;
	int whole_pixels_y1;
	// portion of pixel we shift.
	double percent= 1.0-(num_pixels - floor(num_pixels));

	int whole_pixels = (int)floor(num_pixels);

	if (getIsFloat())
		throw ccd_exception("image_ram::shiftImage only works for unsigned short images");

	img_copy = new image_ram(*this);


	if (shift_direction==left)
	{
		whole_pixels_y=0;
		whole_pixels_x=whole_pixels;
		whole_pixels_y1=0;
		whole_pixels_x1=1+whole_pixels_x;
	}
	else if (shift_direction == right)
	{
		whole_pixels_y=0;
		whole_pixels_x=-1*whole_pixels;
		whole_pixels_y1=0;
		whole_pixels_x1=whole_pixels_x-1;
	}
	else if (shift_direction == down)
	{
		whole_pixels_y=whole_pixels;
		whole_pixels_x=0;
		whole_pixels_y1=whole_pixels_y+1;
		whole_pixels_x1=0;
	}
	else if (shift_direction == up)
	{
		whole_pixels_y=-1*whole_pixels;
		whole_pixels_x=0;
		whole_pixels_y1=whole_pixels_y-1;
		whole_pixels_x1=0;
	}

	// Clear the image ram.
	clrRam();

	/*
	 * Here we shift whole image.
	 */

	if (image_portion == whole_image)
	{



		// copy the data from *img_copy to *this


			pixelShift(0,
					0,
					getImgX(),
					getImgY(),
					percent,
					whole_pixels_x,
					whole_pixels_x1,
					whole_pixels_y,
					whole_pixels_y1,
					img_copy);



	}
	else if (image_portion == top)
	{


			// pixel position for 1/2 image size
			int half_img=getImgY()/2;


			pixelCopy(0, 0, getImgX(), half_img, img_copy);

		//
		// For top, we copy shifted image.
		//
		pixelShift(0,
					half_img,
					getImgX(),
					getImgY(),
					percent,
					whole_pixels_x,
					whole_pixels_x1,
					whole_pixels_y,
					whole_pixels_y1,
					img_copy);
	}
	else if (image_portion == bottom)
	{


			// pixel position for 1/2 image size
			int half_img=getImgY()/2;

		//
		// Copy top half unchanged.
			pixelCopy(0, half_img, getImgX(), getImgY(), img_copy);

		//
		// For top, we copy shifted image.
		//
		//
		// For bottom, we copy shifted image.
		//
		pixelShift(0,
					0,
					getImgX(),
					half_img,
					percent,
					whole_pixels_x,
					whole_pixels_x1,
					whole_pixels_y,
					whole_pixels_y1,
					img_copy);

	}
	else if (image_portion == right)
	{

			// pixel position for 1/2 image size
			int half_img=getImgX()/2;

		//
		// Copy left half unchanged.
			pixelCopy(0, 0, half_img, getImgY(), img_copy);

			//right side is shifted...
			pixelShift(half_img,
					0,
					getImgX(),
					getImgY(),
					percent,
					whole_pixels_x,
					whole_pixels_x1,
					whole_pixels_y,
					whole_pixels_y1,
					img_copy);


	}
	else if (image_portion == left)
	{
			// pixel position for 1/2 image size
			int half_img=getImgX()/2;

		//
		// Copy eight half unchanged.
			pixelCopy(half_img, 0, getImgX(), getImgY(), img_copy);

			//left side is shifted...
			pixelShift(0,
					0,
					half_img,
					getImgY(),
					percent,
					whole_pixels_x,
					whole_pixels_x1,
					whole_pixels_y,
					whole_pixels_y1,
					img_copy);


	}
	else if (image_portion == top_R)
	{
		delete(img_copy);
		throw ccd_exception("image_ram::shiftImage not implemented");
	}
	else if (image_portion == top_L)
	{
		delete(img_copy);
		throw ccd_exception("image_ram::shiftImage not implemented");
	}
	else if (image_portion == bottom_R)
	{
		delete(img_copy);
		throw ccd_exception("image_ram::shiftImage not implemented");
	}
	else if (image_portion == bottom_L)
	{
		delete(img_copy);
		throw ccd_exception("image_ram::shiftImage not implemented");
	}

	// delete copy of the image.
	delete(img_copy);
}



/***************************************************************
 *

	copy pixles from one image to another.
	only works for integer images.

 **************************************************************/

void image_ram::pixelCopy(
	int xs,
	int ys,
	int	xe,
	int	ye,
	image_ram *img_copy)
{
	unsigned short val;

	if (getIsFloat())
		throw ccd_exception("image_ram::pixelCopy only works for unsigned short images");

		//
		// Copy top half unchanged.
		for (int xx = xs; xx<xe;xx++)
		{

			for (int yy = ys; yy<ye;yy++)
			{
				val =img_copy->getPixi(
						xx,
						yy);

				setPixi(xx,yy,val);

			}
		}

}



/***************************************************************
 *

	copy pixles from one image to another.
	only works for integer images.

 **************************************************************/

void image_ram::pixelShift(
	int xs,
	int ys,
	int	xe,
	int	ye,
	double percent,
	int whole_pixels_x,
	int whole_pixels_x1,
	int whole_pixels_y,
	int whole_pixels_y1,
	image_ram *img_copy)
{
	double percent_n = 1.0-percent;
	unsigned short val;

	if (getIsFloat())
		throw ccd_exception("image_ram::pixelShift only works for unsigned short images");

	for (int xx = xs; xx<xe;xx++)
		{

			for (int yy = ys; yy<ye;yy++)
			{
				if ( (xx+whole_pixels_x)>= xs &&
					(xx+whole_pixels_x) < xe &&
					(yy+whole_pixels_y) >= ys &&
					(yy+whole_pixels_y) < ye)
				{

					val =(unsigned short)(percent *
						((double)(img_copy->getPixi(
							xx+whole_pixels_x,
							yy+whole_pixels_y))));

					val+= (unsigned short) (percent_n *
						((double)(img_copy->getPixi(
							xx+whole_pixels_x1,
							yy+whole_pixels_y1))));

					setPixi(xx,yy,val);
				}
			}
		}

}



/*******************************************************************************
 *
 	This is for shifting a spatial correction map. the idea is to shift image
	due to dead row on the CCD, then do spaitial. Because the spaitkla map is a list
	of numbers that point to pixels on the raw CCD image. We have x map with list
	of numbers that points to X cords on the raw image, and y map that points to
	y coords. We typically shift raw image, then do spatial. To save time, we can
	skip image shift, and then siomply redirect the map to point to the next row or col.
	To shift up, we alter ymap, to shift left we alter xmap.

	we assume that this image is a MAP, and is float data.

		Left right top bottom is determined by

		y
		|		top
		|
		|left			right
		|
		|		bottom
		|
		(0,0)-----------------------x


 ******************************************************************************/
		// Shift entire image or 1/2 of image, or 1/4 of image.
void image_ram::shiftMap(
	direction image_portion,
	direction shift_direction,
	double num_pixels)
{

	int x,y;
	float val;
	double pixel_delta;

	if (!getIsFloat())
		throw ccd_exception("image_ram::shiftMap only works for float images");



	if (shift_direction==left)
	{
		pixel_delta = num_pixels;
	}
	else if (shift_direction == right)
	{
		pixel_delta = 0.0-num_pixels;
	}
	else if (shift_direction == down)
	{
		pixel_delta = num_pixels;
	}
	else if (shift_direction == up)
	{
		pixel_delta = 0.0-num_pixels;
	}

	// Clear the image ram.


	/*
	 * Here we shift whole image.
	 */

	if (image_portion == whole_image)
	{
		for (x=0; x<this->getImgX(); x++)
		{
			for (y=0; y<this->getImgY(); y++)
			{
				val = this->getPix(x,y);
				this->setPix(x,y,(val + pixel_delta));
			}
		}
	}
	else if (image_portion == top)
	{
		// pixel position for 1/2 image siz
		float half_img=(float)(getImgY()/2);

		for (x=0; x<this->getImgX(); x++)
		{
			for (y=0; y<this->getImgY(); y++)
			{
				val = this->getPix(x,y);
				if (val>half_img)
				{
					this->setPix(x,y,(val + pixel_delta));

				}
			}
		}
	}
	else if (image_portion == bottom)
	{

		// pixel position for 1/2 image siz
		float half_img=(float)(getImgY()/2);

		for (x=0; x<this->getImgX(); x++)
		{
			for (y=0; y<this->getImgY(); y++)
			{
				val = this->getPix(x,y);
				if (val<=half_img)
				{
					this->setPix(x,y,(val + pixel_delta));

				}
			}
		}
	}
	else if (image_portion == right)
	{
		// pixel position for 1/2 image siz
		float half_img=(float)(getImgX()/2);

		for (x=0; x<this->getImgX(); x++)
		{
			for (y=0; y<this->getImgY(); y++)
			{
				val = this->getPix(x,y);
				if (val>half_img)
				{
					this->setPix(x,y,(val + pixel_delta));

				}
			}
		}
	}
	else if (image_portion == left)
	{
		// pixel position for 1/2 image siz
		float half_img=(float)(getImgX()/2);

		for (x=0; x<this->getImgX(); x++)
		{
			for (y=0; y<this->getImgY(); y++)
			{
				val = this->getPix(x,y);
				if (val<=half_img)
				{
					this->setPix(x,y,(val + pixel_delta));

				}
			}
		}
	}
	else
	{
		throw ccd_exception("image_ram::shiftImage not implemented");
	}
}


/*******************************************************************************
 *
 *	Class image_ram
*  apply lineary correction in this image.
 * corr table is 4 1x64k arrays, startin in the memory of the table.\
 * order of tables is 4 arrsy in line. topleft, top right bott left bott rught
 * xy=0 0 at top left.

 ******************************************************************************/
		// Add an image (or subratt image with sign = 0)
void image_ram::applyLinTableSing(
	image_ram& table,
	int bias,
	int taper)
		{

	unsigned short *img_pixel;
	float *table_mem0;
	float *table_mem1;
	float *table_mem2;
	float *table_mem3;
	int chanxsize, chanysize, xe,ye,x,y;

	long pixval;
	float pixvalc;
	int k;
	float biasf;

		biasf = (float)bias;

		img_pixel=(unsigned short*)this->getMem();
		table_mem0=(float*)(table.getMem() + taper*262144*sizeof(float));
		table_mem1=table_mem0 + 65536;
		table_mem2=table_mem1 + 65536;
		table_mem3=table_mem2 + 65536;

		chanxsize=this->getImgX()/2;
		chanysize = this->getImgY()/2;
		xe=chanxsize*2;
		ye=chanysize*2;

		for (y=0;y<chanysize;y++)
		{
			for (x=0;x<chanxsize;x++)
			{
				pixval= (long)(*img_pixel);

				pixvalc = table_mem0[pixval];
				// dither before we convert to short...
				// table is floats, and has decimals in it...
				pixvalc = floor(pixvalc + (float)getRand());
				if (pixvalc>65535.0)
					pixvalc=65535.0;

				if (pixvalc<0.0)
					pixvalc=0.0;

				*img_pixel = (unsigned short)pixvalc;
				img_pixel++;
			}
			for (x=chanxsize;x<xe;x++)
			{
				pixval= (long)(*img_pixel);

				pixvalc = table_mem1[pixval];
				// dither before we convert to short...
				// table is floats, and has decimals in it...
				pixvalc = floor(pixvalc + (float)getRand());
				if (pixvalc>65535.0)
					pixvalc=65535.0;


				if (pixvalc<0.0)
					pixvalc=0.0;

				*img_pixel = (unsigned short)pixvalc;
				img_pixel++;
			}
		}

		for (y=chanysize;y<ye;y++)
		{
			for (x=0;x<chanxsize;x++)
			{
				pixval= (long)(*img_pixel);

				pixvalc = table_mem2[pixval];
				// dither before we convert to short...
				// table is floats, and has decimals in it...
				pixvalc = floor(pixvalc + (float)getRand());
				if (pixvalc>65535.0)
					pixvalc=65535.0;


				if (pixvalc<0.0)
					pixvalc=0.0;

				*img_pixel = (unsigned short)pixvalc;
				img_pixel++;
			}
			for (x=chanxsize;x<xe;x++)
			{
					pixval= (long)(*img_pixel);

				pixvalc = table_mem3[pixval];
				// dither before we convert to short...
				// table is floats, and has decimals in it...
				pixvalc = floor(pixvalc + (float)getRand());
				if (pixvalc>65535.0)
					pixvalc=65535.0;


				if (pixvalc<0.0)
					pixvalc=0.0;

				*img_pixel = (unsigned short)pixvalc;
				img_pixel++;
			}
		}


}

/********************************************************************
 *
 *
 *********************************************************************/

double image_ram::aveLine(
	int y_start,
	int y_end,
	int x_start,
	int x_end)
{

	double x_len,y_len,len,x,y;
	double x_mult,y_mult,k;
	int xrnd,yrnd;
	double sum,ped;
	double k2;

	sum=0.0;

	x_len = (double)(x_end-x_start);
	y_len = (double)(y_end-y_start);
	len = sqrt( y_len*y_len + x_len*x_len);
	x_mult = x_len/len;
	y_mult = y_len/len;
	k2 = 0.0;
	for (k=0.0; k<len;k=k+1.0)
	{
		k2 = k2+1.0;
		x=x_start + k*x_mult;
		y=y_start + k*y_mult;
		xrnd=(int)floor(x+0.5);
		yrnd=(int)floor(y+0.5);
		sum+=(double)(this->getPixi(xrnd,yrnd));

	}
	//k=k-1.0;

	ped = sum/k2;
	return(ped);
}



void image_ram::addConstRegion(
	int x_start,
	int x_end,
	int y_start,
	int y_end,
	double delta,
	int is_dither)
{
	int x,y;
	int pixval;
	int delta_sd;

	for (y=y_start; y <= y_end ; y++)
	{
		for (x=x_start;x<= x_end; x++)
		{
			pixval = getPix(x,y);
			if (is_dither)
				delta_sd=(int)floor(delta + getRand());
			else
				delta_sd=(int)delta;

			pixval+=delta_sd;
			setPix(x,y,pixval);
		}
	}

}

// something wrong with this... prob wron eq for SDM
int image_ram::sigmaDelta(double input)
{
	double output,integrator;
	int outputi;

	integrator= input - sigma_delta_error;
	output = floor(integrator);
	sigma_delta_error = integrator-output;
	outputi=(int)output;
	return(outputi);
}
void image_ram::adjustPedistalSing(
	int bias,
	int taper)
{
	int x,y;
	double sum,k;
	double delta;
	int pixval;

/*
 *   ped1   |   ped2
 *          |
 *  -------------------
 *          |
 *  ped3    |   ped4
 */

 // taper = 0, or 3, we have bias lnes at row 1 and row image_y-2
	// else at cols 1 and image_y-2
  // chans are 1 2 // 3 4
	if (taper ==0 || taper==3)
	{
		ped1=aveLine(1,1,
			5,
			(image_x/2)-5);

		ped2=aveLine(1,1,
			(image_x/2)+5,
			image_x-5);

		ped3=aveLine(
			image_y-2,
			image_y-2,
			5,
			(image_x/2) - 5);

		ped4=aveLine(
			image_y-2,
			image_y-2,
			(image_x/2)+5,
			image_x-5);

	}
	else
	{

		ped1=aveLine(
			5,
			(image_y/2)-5,
			1,
			1);


		ped2=aveLine(
			5,
			(image_y/2)-5,
			image_x-2,
			image_x-2);

		ped3=aveLine(
			(image_y/2)+5,
			(image_y)-5,
			1,
			1);

		ped4=aveLine(
			(image_y/2) +5,
			(image_y)-5,
			image_x-2,
			image_x-2);

	}

	// chan1
	delta = (float)bias - ped1;

	addConstRegion(
		0,
		image_x/2 - 1,
		0,
		image_y/2 - 1,
		delta,
		true);

	// chan2
	delta = (float)bias - ped2;

	addConstRegion(
		image_x/2,
		image_x -1,
		0,
		image_y/2 - 1,
		delta,
		true);


		// chan3
	delta = (float)bias - ped3;


	addConstRegion(
		0,
		image_x/2 -1,
		image_y/2,
		image_y - 1,
		delta,
		true);



	// chan4
	delta = (float)bias - ped4;

	addConstRegion(
		image_x/2,
		image_x -1,
		image_y/2,
		image_y - 1,
		delta,
		true);



}

/**************************************************************************************
 *
 * Apply Haar wavelet to all rows  of this image.
 * tell size of subimage...
 ************************************************************************************/
void image_ram::haarRows(int levelsize)
{
	float *scratch= new float[image_x];
	int x,y,k;
	float pixval, pixval1;

	float r2=1.0/sqrt(2.0);

	if (this->getIsFloat()==false)
		this->short2Float(1.0);

	for (y=0; y<levelsize; y++)
	{
		//
		// do LOW freq Scaling function.
		//
		k=0;
		for (x =0; x<levelsize; x=x+2)
		{
			pixval=this->getPix(x,y);
			pixval1=this->getPix(x+1,y);
			scratch[k] = r2*(pixval+pixval1);
			k++;
		}

		//
		// do HI freq Wavelet function.
		//
		for (x =0; x<levelsize; x=x+2)
		{
			pixval=this->getPix(x,y);
			pixval1=this->getPix(x+1,y);
			scratch[k] = r2*(pixval-pixval1);
			k++;
		}

		for (x=0; x<levelsize; x++)
			this->setPix(x,y,scratch[x]);

	}
	delete(scratch);
}



/**************************************************************************************
 *
 * Apply Haar Inverse wavelet to all rows  of this image.
 * tell size of subimage...
 ************************************************************************************/
void image_ram::haarRowsInv(int levelsize)
{
	float *scratch = new float[image_x];
	int x,y,k;
	float pixval, pixval1;

	float r2=1.0/sqrt(2.0);

	if (this->getIsFloat()==false)
		this->short2Float(1.0);

	for (y=0; y<levelsize; y++)
	{
		//
		// do INV LOW freq Scaling function.
		//
		k=0;
		for (x =0; x<levelsize/2; x=x+1)
		{
			pixval=this->getPix(x,y);
			scratch[k] = r2*pixval;
			k++;
			scratch[k] = r2*pixval;
			k++;

		}

		//
		// do INV HI freq Wavelet function.
		//
		k=0;
		for (x =levelsize/2; x<levelsize; x=x+1)
		{
			pixval=this->getPix(x,y);
			scratch[k] = scratch[k] + r2*pixval;
			k++;
			scratch[k] = scratch[k] - r2*pixval;
			k++;
		}

		for (x=0; x<levelsize; x++)
			this->setPix(x,y,scratch[x]);

	}

	delete(scratch);

}



/**************************************************************************************
 *
 * Apply Haar wavelet to all rows and cols of this image.
 * tell how many levels...
 ************************************************************************************/
void image_ram::haar2D(int levels)
{
	int levelsize;
	int k;

	levelsize=image_x;

	for (k=0; k<levels; k++)
	{
		setIsRot(false);
		haarRows(levelsize);
		setIsRot(true);
		haarRows(levelsize);
		levelsize=levelsize/2;
	}
	setIsRot(false);

}



/**************************************************************************************
 *
 * Apply INV Haar wavelet to all rows and cols of this image.
 * tell how many levels...
 ************************************************************************************/
void image_ram::haar2DInv(int levels)
{
	int k;
	int levelsize;
	levelsize=image_x / (int)pow(2.0,(double)(levels-1));


	for (k=0; k<levels; k++)
	{
		setIsRot(false);
		haarRowsInv(levelsize);
		setIsRot(true);
		haarRowsInv(levelsize);
		levelsize=levelsize*2;
	}
	setIsRot(false);
	float2Short(1.0);

}

/**************************************************************************************
 *
 * Haar wavelet method for zinger removal. works for single pixel zingers....
 ************************************************************************************/
int image_ram::waveZing(float threshold)
{

	int levelsize,levelsize2;
	int x,y;
	float thresholdn=0.0-threshold;
	float coef;
	int num_zingers=0;

	// conv to wavelets
	haar2D(1);
	levelsize=image_x;
	levelsize2=levelsize/2;
	// seach hi hi feq waelet coef for zingers.
	for (y=levelsize2; y<levelsize; y++)
	{
		// start at 2 pixels offset because we use pix to left as filler...
		for (x=levelsize2+2; x<levelsize; x++)
		{
			coef=getPix(x,y);
			if (coef>threshold || coef<thresholdn)
			{
				setPix(x,y,0.0);
				setPix(x-levelsize2,y,0.0);
				setPix(x,y-levelsize2,0.0);
				setPix(x-levelsize2,y-levelsize2,getPix(x-levelsize2-1,y-levelsize2));
				// count number of pixels we alter...
				num_zingers++;
			}
		}
	}


	// conv to image pixels
	haar2DInv(1);
	return(num_zingers);
}

/**************************************************************************************
 *
 ************************************************************************************/
double image_ram::checkSum(void)
{
	double sum=0.0;
	int x,y;

	for (y=0; y<image_y; y++)
	{
		for (x=0; x<image_x; x++)
		{
			sum+=this->getPix(x,y);
		}
	}
	return(sum);
}

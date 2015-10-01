/*******************************************************************************
 *
 *    image_ram.h
 *
 *    Author: Tim Madden
 *    Date:    6/30/03
 *    Project:MBC CCD Detector DAQ.
 *
 *
 *
 *
 ******************************************************************************/


/*
 * Include files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>

/*
 * Double incluson protection.
 */
#ifndef _IMAGE_RAM_H
    #define _IMAGE_RAM_H



#ifdef _WINDOWS
 #define EXPORT __declspec(dllexport)
#else
 #define EXPORT

#endif

/*******************************************************************************
 *
 *    Class image_ram
 *
 *
 ******************************************************************************/

class EXPORT image_ram
{
    public:

            // Directions to shift the image.
        enum direction
        {
            up = 0,
            down = 1,
            left = 2,
            right = 3,
            top = 4,
            bottom = 5,
            top_R =6,
            top_L = 7,
            bottom_R = 8,
            bottom_L = 9,
            whole_image= 10
        };


        // make mutex object.
        image_ram();

        // make mutex object.
        image_ram(int x, int y);
        // so we can make float dtat
        image_ram(int x, int y,bool fl);

        // copy constructor- makes copy of same image data too
        image_ram(image_ram &img);

        // destroy mutex object.
        ~image_ram();


        void copy(image_ram &img);
        void copyQuad(image_ram &img, int quadrant);

        // mem_ptr
        unsigned char* getMem(void);
        // mem_ptr
        unsigned char* getMem2(void);

        void putMem(void *ptr);

        // is it floating point?
        bool getIsFloat(void);

        //get pixel vlue as float
        float getPix(int x, int y);

        void setPix(int x, int y, float val);


        unsigned short getPixi(int x, int y);

        void setPixi(int x, int y, unsigned short val);

        //get size
        int getRamSize(void);

        //get size
        int getImgSize(void);

        //get size

        int getImgX(void);

        //get size
        int getImgY(void);

        //set iage size
        void setImageSize(
            int sizex,
            int sizey);

        // set binning mode
        void setBinning(bool mode);

        // check binning
        bool getBinning(void);

        // set image id
        void setImageID(const unsigned long id);

        // get image id
        unsigned long getImageID(void);

        // clearimg
        void clrRam(void);

        // check tag pixels
        bool checkTags(void);

        // swap x,y coords on getPix, setPix
        void setIsRot(bool is_rot);
        // take x=2048-x to reverse x coord
        void setInvX(bool is_inv);
        void setInvY(bool is_inv);

        // Add a const. of sign to the image. sign=true for +
        void addConst(
            float bias);

        // mult a const. of sign to the image. sign=true for +
        void multConst(
            float bias);

        // makes the min value to minv
        void setFloor(float minv);

        // Add an image (or subratt image with sign = 0)
        void addImage(
            image_ram& img,
            bool sign);

        // mult 2 images together. img must be double precision.
        void multImage(
            image_ram& img,
            int bias);
        // all int versionl use fixed point arithmetic. divide answer by 2^bits
        void multImage(
            image_ram& img,
            int bias,
            int bits);


        // Shift entire image or 1/2 of image, or 1/4 of image.
        void shiftImage(
            direction image_portion,
            direction shift_direction,
            double num_pixels);

    void shiftMap(
        direction image_portion,
        direction shift_direction,
        double num_pixels);


        int getNumCcdX(void);
        int getNumCcdY(void);

        void setNumCcdX(int x, int y);


    void float2Short(float multval);
        void short2Float(float multval);

    void applyLinTableSing(
        image_ram& table,
        int bias,
        int taper);

    double aveLine(
        int y_start,
        int y_end,
        int x_start,
        int x_end);

void adjustPedistalSing(
    int bias,
    int taper);

void haar2DInv(int levels);
void haar2D(int levels);
void haarRowsInv(int levelsize);
void haarRows(int levelsize);
int waveZing(float threshold);

double checkSum(void);
int sigmaDelta(double input);

void addConstRegion(
    int x_start,
    int x_end,
    int y_start,
    int y_end,
    double delta,
    int is_dither);

    // make table of raqnd numbers fro m ((0,1.0)
    void makeRandTable(void);
    // get rand form the table, like gen a random number...
    double getRand(void);


    float ped1, ped2, ped3, ped4;


    protected:
    double sigma_delta_error;



        // copy pixels in some region of image from another image_ram
        void pixelCopy(
            int xs,
            int ys,
            int    xe,
            int    ye,
            image_ram *img_copy);

        void pixelShift(
            int xs,
            int ys,
            int    xe,
            int    ye,
            double percent,
            int whole_pixels_x,
            int whole_pixels_x1,
            int whole_pixels_y,
            int whole_pixels_y1,
            image_ram *img_copy);

        // data storage.
        unsigned char *memory;
        unsigned char *memorysave;

        // true if floating point
        bool is_float;

        // size of ram.
        int ram_size;
        // x dim
        int ram_x;
        //y dim
        int ram_y;

        // size of image
        int image_size;
        // x dim
        int image_x;
        //y dim
        int image_y;

        // binnimg mode is true.
        bool is_binning;

        // image id
        unsigned long image_id;

        bool is_img_rot;
        bool is_y_inv;
        bool is_x_inv;

        int numccds_x;
        int numccds_y;
        // default size
        enum
        {
            def_size = 1024

        };

        int ram_bytes;

        // mem to store a set of random doubles
        double *random_numbers;
        enum {rand_tab_size=2048};
        int rand_index;
};

#endif


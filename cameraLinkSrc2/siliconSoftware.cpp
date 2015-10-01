// AncDemoRotationExpDlg.cpp : implementation file
//

#ifdef USE_SISW

//#include "stdafx.h"
#include <iostream>

#include "siliconSoftware.h"

//#include "ccd_exception.h"

//static   char *g_pDesignStr = EXPANSION_RTPRO_ROTATION_STR;

 volatile long siSoftware::frames_to_cpu = 0;
 volatile long siSoftware::frame_count = 0;

 volatile long siSoftware::missed_frames = 0;
 volatile bool siSoftware::is_missed_frame=false;
 volatile int siSoftware::sap_buffer_count = 0;
volatile long siSoftware::recent_missed_frames = 0;

// genCamController* siSoftware::cam_control=0;

siSoftware *siSoftware::mycard=0;

/////////////////////////////////////////////////////////////////////////////
// siSoftwareFPGA dialog

siSoftware::siSoftware(bool is_use_fpga) :
lf("sisoGrabber.log")
{
//    m_bEnableRtPro = is_use_fpga;

lf.enablePrintf(true);

is_edge_globalshut=false;
act_sccmos_version=-1;

is_display=false;

dbg_last_imgnum=0;

   m_IsSignalDetected = TRUE;
   is_force_size=false;

   is_rst_server=false;
  // m_RtproDesign = NULL;
//setFpgaFileName("null");
  
   setConfigFileName("D:/timmaddenedge.mcf");

  siSoftware::frames_to_cpu = 0;
  siSoftware::frame_count = 0;

  siSoftware::missed_frames = 0;
  siSoftware::is_missed_frame=false;
  siSoftware::sap_buffer_count = 0;
 siSoftware::recent_missed_frames = 0;

 num_buffers=100;
is_destroyed=true;

is_doub_width=1;

 siSoftware::mycard=this;

is_blocking=true;

dispId0 =0;
boardNr=0;
fg=0;
  camPort             = PORT_A;
  ptrMem=0;
  totalBufferSize=0;

}


int siSoftware::getGrabberType()
{
    return((int)gSISW);
}



void siSoftware::makeView(void)
{
    //*2 becuse we have 8bit data for 16bit images- 10tap camlink
  dispId0 = CreateDisplay(8,sensor_width*2,sensor_height);
  
  SetBufferWidth(dispId0,sensor_width*2,sensor_height);
  is_display=true;

}
void siSoftware::setNumBuffers(int b)
{
num_buffers=b;
}

bool siSoftware::isFrameAvailable(void)
{
    frame_count = Fg_getLastPicNumberEx(fg,camPort,(dma_mem*)ptrMem);
    
//    int a = Fg_getStatus(fg,NUMBER_OF_ACT_IMAGE,0,0);
//    int b = Fg_getStatus(fg,NUMBER_OF_LAST_IMAGE,0,0);
//    int c = Fg_getStatus(fg,NUMBER_OF_GRABBED_IMAGES,0,0);
//    int d = Fg_getStatus(fg,NUMBER_OF_LOST_IMAGES,0,0);
//    int e = Fg_getStatus(fg,NUMBER_OF_BLOCK_LOST_IMAGES,0,0);
//    int f = Fg_getStatus(fg,NUMBER_OF_IMAGES_IN_PROGRESS,0,0);
    //int lastacq=
    
    if (frame_count==FG_TIMEOUT_ERR)
        return false;

        
    
    
    
    int behind=frame_count-frames_to_cpu;
    
    if (behind>sap_buffer_count)
    {
        recent_missed_frames=behind-sap_buffer_count;
        missed_frames+=recent_missed_frames;
    }    
        
    if (frame_count>frames_to_cpu)
        return true;
        
        

    return false;
}

bool siSoftware::isMissedFrame(void)
{

    return(is_missed_frame);
}

void siSoftware::clearMissedFrames(void)
{
    frame_count = Fg_getLastPicNumberEx(fg,camPort,(dma_mem*)ptrMem);

    frames_to_cpu=frame_count;
    is_missed_frame=false;
    recent_missed_frames=0;
}
long siSoftware::getTotalMissedFrames(void)
{
    return(missed_frames);
}

long siSoftware::getRecentMissedFrames(void)
{
    return(recent_missed_frames);
}



void siSoftware::setCamController(void *cc)
{
    //cam_control=cc;

}


int siSoftware::getWidth(void)
{

    return(sensor_width);
}

int siSoftware::getHeight(void)
{

    return(sensor_height);
}


//

bool siSoftware::getFrame(void *copy_memory,unsigned int *siSoftware_timestamp)
{

    return(
        getFrame(
            copy_memory,
            siSoftware_timestamp,
            (sensor_height*sensor_width*sizeof(unsigned short))));
}

bool siSoftware::getFrame(void *copy_memory,unsigned int *siSoftware_timestamp,int nbytes)
{
    bool sap_result;
    int grab_index;
    void  *image_address;
    char mesx[256];
    int retrycnt=0;
    int buffnum;
    int param;

    if (isFrameAvailable())
    {

        if (is_blocking)
            param=SEL_NEXT_IMAGE;
        else
            param = SEL_NUMBER;


        buffnum= Fg_getImageEx(fg, param,frames_to_cpu+1, camPort, 0,(dma_mem*)ptrMem);
//            buffnum= Fg_getImageEx(fg, SEL_NEXT_IMAGE,0, camPort, 4,(dma_mem*)ptrMem);
        retrycnt=0;
        while (dbg_last_imgnum==(buffnum) && retrycnt<5)
        {
            int aa;
            aa=1;
//            printf("siSoftware::getFrame() - Grabbed same img twice from grabber-buffnum= %d frames_to_cpu+1= %d\n",buffnum,frames_to_cpu+1);
            buffnum= Fg_getImageEx(fg, param,frames_to_cpu+1, camPort, 4,(dma_mem*)ptrMem);
            retrycnt++;
            //trap here
        }
        if (dbg_last_imgnum==buffnum)
        {
            
            sprintf(mesx,"siSoftware::getFrame() - Grabbed same img twice from grabber-buffnum= %d frames_to_cpu+1= %d\n",buffnum,frames_to_cpu+1);
            lf.log(mesx);
        }
        dbg_last_imgnum=buffnum;

        if (buffnum>=0)
        {
                image_address = Fg_getImagePtrEx(fg,buffnum,camPort,(dma_mem*)ptrMem);

            if (image_address!=0)
            {
                
                if (copy_memory!=0)
                    memcpy(copy_memory,image_address,nbytes);


                *siSoftware_timestamp = buffnum;
                Fg_getParameter(fg, FG_TIMESTAMP, siSoftware_timestamp,camPort);
                if (is_display)
                    DrawBuffer(dispId0,image_address,buffnum,"");
                if (is_blocking)
                    Fg_setStatusEx(fg, FG_UNBLOCK, buffnum, camPort, (dma_mem*)ptrMem);
    
                frames_to_cpu++;
            }
            else
            {
                lf.log("SISW returned image address=0 for some reason...\n");
                
                recent_missed_frames++;
                missed_frames++;
                is_missed_frame=true;
            }
        }
        else
        {
             sprintf(mesx,
                "ERROR-SISW Grabber got buffnum=%d, frames2cpu1= %d, param =  %d \n ", 
                buffnum, frames_to_cpu+1,param);
            lf.log(mesx);
        }
    }

    return false;
}

bool siSoftware::getFrame(void *copy_memory)
{

    unsigned int ts;

    return(
        getFrame(
            copy_memory,
            &ts,
            (sensor_height*sensor_width*sizeof(unsigned short))));

}



void siSoftware::setDoubleWidth(int isdw)
{
    is_doub_width=1.0;

    if (isdw==1)
        is_doub_width=2.0;

    if (isdw==2)
        is_doub_width=1.5;

        
}


/////////////////////////////////////////////////////////////////////////////
// siSoftwareFPGA message handlers


//***********************************************************************************
// Initialize Demo Dialog based application
//***********************************************************************************
bool siSoftware::initialize(int size_x, int size_y)
{
    return(initialize(size_x,size_y,false));
}



//***********************************************************************************
// Initialize Demo Dialog based application
//***********************************************************************************
bool siSoftware::initialize(int size_x, int size_y,bool is_force_size)
{
char mesx[256];

        this->is_force_size = is_force_size;


        
  if ((fg = Fg_InitConfig(camera_format_file, boardNr)) == NULL) {
    sprintf(mesx, "error in siSoftware::initialize,Fg_InitConfig: %s\n", Fg_getLastErrorDescription(NULL));
    lf.log(mesx);
    
    return(FALSE);
  }

  //
  // check for special edge global shut fw
    //  
    int IdVersion;

   IdVersion = Fg_getParameterIdByName(fg, "Device1_Process0_version_Value");
   if(IdVersion<0)
   {
    lf.log("siSoftware::initialize HAP_PCOGLSH version not avaiable- assume normal FW");
   }
   else
   {
    int val;
    Fg_getParameterWithType(fg, IdVersion, &val, 0);
    
    sprintf(mesx,"siSoftware::initialize: HAP_PCOGLSH version %d",val);
    lf.log(mesx);
    if (val>=0)
    {
    act_sccmos_version=val;
    is_edge_globalshut=true;
    }
    else
    {
    lf.log("siSoftware::initialize no GS FW- assume normal FW");
    act_sccmos_version=-1;
    is_edge_globalshut=false;
    
    }
   }

   //DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
   //pco_hap_loaded=PCO_SC2_CL_ME4_PCO_HAP_PCOGLSH;
  

  
  
  
  
  if (is_force_size && !is_edge_globalshut)
  {
      
    this->sensor_height=size_y;
    //when we config the grabber- we assume we convert raw 8 bit data to 16bit pixels.
    //10 tap mode assumed...
    // if we have 12 bit data, we have to set is_doub_width to 1.5... a real pain...
    this->sensor_width=(int)(is_doub_width*(double)size_x);

      /*Image width of the acquisition window.*/
      if (Fg_setParameter(fg,FG_WIDTH,(void*)&sensor_width,camPort) < 0 ) {
        sprintf(mesx, "Fg_setParameter(FG_WIDTH) failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
        Fg_FreeGrabber(fg);
        return FALSE;
      }

      /*Image height of the acquisition window.*/
      if (Fg_setParameter(fg,FG_HEIGHT,(void*)&sensor_height,camPort) < 0 ) {
        sprintf(mesx, "Fg_setParameter(FG_HEIGHT) failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
        
        Fg_FreeGrabber(fg);
        return FALSE;
      }
        //set to num pixels...    
      this->sensor_width=size_x;

            //we need to mult by 2 for 16 bit images...sizex and sizey are for 16 bit images
      // we assume grabber is set for 8 bit 10 tyap data, with 2 raw pixels for one 16biyt image piuxel..
               totalBufferSize = 2*sensor_height * sensor_width *num_buffers;

  }
  
  if (is_force_size && is_edge_globalshut)
  {
      
    this->sensor_height=size_y;
    //when we config the grabber- we assume we convert raw 8 bit data to 16bit pixels.
    //10 tap mode assumed...
    // if we have 12 bit data, we have to set is_doub_width to 1.5... a real pain...
    this->sensor_width=(int)(is_doub_width*(double)size_x);

      /*Image width of the acquisition window.*/
      if (Fg_setParameter(fg,FG_WIDTH,(void*)&sensor_width,camPort) < 0 ) {
        sprintf(mesx, "Fg_setParameter(FG_WIDTH) failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
        Fg_FreeGrabber(fg);
        return FALSE;
      }

      /*Image height of the acquisition window.*/
      if (Fg_setParameter(fg,FG_HEIGHT,(void*)&sensor_height,camPort) < 0 ) {
        sprintf(mesx, "Fg_setParameter(FG_HEIGHT) failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
        
        Fg_FreeGrabber(fg);
        return FALSE;
      }
        //set to num pixels...    
      this->sensor_width=size_x;

            //we need to mult by 2 for 16 bit images...sizex and sizey are for 16 bit images
      // we assume grabber is set for 8 bit 10 tyap data, with 2 raw pixels for one 16biyt image piuxel..
               totalBufferSize = 2*sensor_height * sensor_width *num_buffers;
               
               
          lf.log("set_actual_size_global_shutter: Get ID's");


           int Idxoff0, Idxlen0, Idyoff0, Idylen0;
      int Idxoff1, Idxlen1, Idyoff1, Idylen1;
      int Idxoff2, Idxlen2, Idyoff2, Idylen2;
      int Idxoff3, Idxlen3, Idyoff3, Idylen3;
      int Idoffset, Idenable, Idsplitter;
      int ConstLGShift4_Value_Id, ConstHGShift4_Value_Id;
     
     int rcode, nError;
     
      Idxoff0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_XOffset");
      Idxlen0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_XLength");
      Idyoff0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_YOffset");
      Idylen0  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM0_YLength");

      Idxoff1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_XOffset");
      Idxlen1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_XLength");
      Idyoff1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_YOffset");
      Idylen1  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferReset_DRAM1_YLength");

      Idxoff2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_XOffset");
      Idxlen2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_XLength");
      Idyoff2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_YOffset");
      Idylen2  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM0_YLength");

      Idxoff3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_XOffset");
      Idxlen3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_XLength");
      Idyoff3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_YOffset");
      Idylen3  = Fg_getParameterIdByName(fg, "Device1_Process0_ImageBufferData_DRAM1_YLength");

      Idsplitter    = Fg_getParameterIdByName(fg, "Device1_Process0_DoubleFrameSplit_Splitter_ImageHeight");

      Idenable      = Fg_getParameterIdByName(fg, "Device1_Process0_EnableSubImage_Enable_Value");
      Idoffset      = Fg_getParameterIdByName(fg, "Device1_Process0_SubImages_Offset_Value");

      ConstLGShift4_Value_Id = Fg_getParameterIdByName(fg, "Device1_Process0_Match16Bit_ConstLGShift4_Value");
      ConstHGShift4_Value_Id = Fg_getParameterIdByName(fg, "Device1_Process0_Match16Bit_ConstHGShift4_Value");
           
               //==Image Buffer==//
  
  int xoff,yoff,xlen,ylen;
  ylen=size_y;
  xlen=size_x;
  xoff=0;
  yoff=0;
  int height, width;
  height = size_y;
  width = size_x;
  
  
  int x_pre_len = xlen / 10;

  nError=0;
  
  
  rcode = Fg_setParameterWithType(fg, Idxoff0, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen0, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff0, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen0, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  rcode = Fg_setParameterWithType(fg, Idxoff1, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen1, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff1, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen1, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  rcode = Fg_setParameterWithType(fg, Idxoff2, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen2, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff2, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen2, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  rcode = Fg_setParameterWithType(fg, Idxoff3, &xoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idxlen3, &x_pre_len, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idyoff3, &yoff, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;
  rcode = Fg_setParameterWithType(fg, Idylen3, &height, 0, FG_PARAM_TYPE_UINT32_T);
  if(rcode)
   nError++;

  if(nError)
   lf.log("siSoftware::initialize: global shutter Single parameter failed");
  else
   lf.log("siSoftware::initialize: global shutter  Single parameter OK");


   
   
/*============ Image Splitter  ============== */

  height;
 rcode = Fg_setParameterWithType(fg, Idsplitter, &height, 0, FG_PARAM_TYPE_UINT32_T);
 if(rcode)
  nError++;
 else
  lf.log("siSoftware::initialize: global shutter: Idsplitter OK");

 int Value;
  
/*============ HG/LG Mult  ============== */
  Value = 128; //*32 << 2 = 128
  Fg_setParameterWithType(fg, ConstLGShift4_Value_Id, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
   nError++;
  else
  {
   sprintf(mesx,"siSoftware::initialize: global shutter: LG Mult %d (%.02f)",Value,(float)(Value/4.0));
   lf.log(mesx);
   
    }
  
  Value = 13; //*(32/10)=3.2 << 2 = 13 -> 3.25
  Fg_setParameterWithType(fg, ConstHGShift4_Value_Id, &Value, 0, FG_PARAM_TYPE_INT32_T);
  if(rcode)
   nError++;
  else
   {
   sprintf(mesx,"siSoftware::initialize: global shutter: HG Mult %d (%.02f)",Value,(float)(Value/4.0));
   lf.log(mesx);
    }
/*
  if(act_sccmos_version>=HAP_VERSION_NOISE)
  {
   int DoubleShutter_AppendNumber_Id;
   if(doublemode)
    Value=2;
   else
    Value=1;

   DoubleShutter_AppendNumber_Id = Fg_getParameterIdByName(fg,"Device1_Process0_DoubleShutter_AppendNumber");

   rcode = Fg_setParameterWithType(fg,DoubleShutter_AppendNumber_Id, &Value, 0, FG_PARAM_TYPE_UINT32_T);
   if(rcode)
    nError++;
   else
   {
    sprintf(mesx,"siSoftware::initialize: global shutter: Append Images set to %d ",Value);
    lf.log(mesx);
    }
  }//if(act_sccmos_version>=HAP_VERSION_NOISE)
  else
  {
   sprintf(mesx,"siSoftware::initialize: global shutter: doublemode not possible with this version",Value);
   lf.log(mesx);
   
    }
 */
 
 if(nError)
  {
   sprintf(mesx,"siSoftware::initialize: global shutter: Error because some FG_setParameter failed Errcount %d",nError);
   lf.log(mesx);
    return FALSE;
  }

  lf.log("siSoftware::initialize: global shutter: done without error");
  //!!return PCO_NOERROR;
  
  
  }
  
  
  
  
  
  if (!is_force_size)
  {
    if (Fg_getParameter(fg, FG_WIDTH,(void*) &sensor_width, camPort)<0)
    {
        sprintf(mesx, "Fg_getParameter(FG_WIDTH) failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
        Fg_FreeGrabber(fg);
        return FALSE;
    }
    if (Fg_getParameter(fg, FG_HEIGHT, (void*)&sensor_height, camPort)<0)
    {
        sprintf(mesx, "Fg_getParameter(FG_HEIGHT) failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
        Fg_FreeGrabber(fg);
        return FALSE;
    }
        
        sensor_width=sensor_width/2;
        //leave height alone... we /2 because we have 10tap 8 bit data, to convert to 16bit pixels.

        //2 for 16 bit images. silicon grabber is set for 8 bit 10 tap data
       totalBufferSize = 2*sensor_height * sensor_width *num_buffers;

  }


  sprintf(mesx,"To Allocate %d buffers for total of %d bytes",num_buffers,totalBufferSize );
   lf.log(mesx);
   
  ptrMem = Fg_AllocMemEx(fg,totalBufferSize,num_buffers);

  sap_buffer_count = num_buffers;
  if ( ptrMem == NULL) {
    sprintf(mesx, "error in siSoftware::initialize Fg_AllocMem: %s\n", Fg_getLastErrorDescription(fg));
    lf.log(mesx);
    //CloseDisplay(dispId0);
    Fg_FreeGrabber(fg);
    return(FALSE);
  }


//!!
    //makeView();

   // Get current input signal connection status
//   GetSignalStatus();

//   m_Acq->SaveParameters("D:/siSoftwarefiles/current_params.ccf");

    return TRUE;  // return TRUE  unless you set the focus to a control
}


void siSoftware::setConfigFileName(char *name)
{
    char mesx[256];
    sprintf(mesx,"siSoftware::setConfigFileName use %s",name);
    lf.log(mesx);
    
    strcpy(camera_format_file, name);
}



bool siSoftware::CreateObjects()
{


//        if (is_force_size)
//        {
//            m_Acq->SetParameter(CORACQ_PRM_CROP_HEIGHT , sensor_height,false);
//           m_Acq->SetParameter(CORACQ_PRM_CROP_WIDTH , sensor_width,false);
//
//           m_Acq->SetParameter(CORACQ_PRM_SCALE_VERT , sensor_height,false);
//           m_Acq->SetParameter(CORACQ_PRM_SCALE_HORZ , sensor_width,false);
//
//            m_Acq->SetParameter(CORACQ_PRM_VACTIVE , sensor_height,false);
//           m_Acq->SetParameter(CORACQ_PRM_HACTIVE , sensor_width,true);
//
//
//
//
//        }



    return TRUE;
}

bool siSoftware::DestroyObjects()
{    
    
    if (is_display)
        CloseDisplay(dispId0);

    Fg_FreeMemEx(fg,(dma_mem*)ptrMem);
    Fg_FreeGrabber(fg);
    return TRUE;
}

bool siSoftware::DestroyObjectsNoDelete()
{

    return TRUE;
}








//*****************************************************************************************
//
//                    Acquisition Control
//
//*****************************************************************************************

void siSoftware::abort()
{
    Fg_stopAcquireEx(fg, camPort,(dma_mem*)ptrMem,STOP_SYNC);
    //UpdateMenu();
}

void siSoftware::freeze( )
{
    Fg_stopAcquireEx(fg, camPort,(dma_mem*)ptrMem,STOP_SYNC);

//    UpdateMenu();
}

void siSoftware::grab()
{
char mesx[256];

//    clearMissedFrames();
 if (is_blocking==false)
 {
      if (Fg_AcquireEx(fg,camPort,GRAB_INFINITE,ACQ_STANDARD,(dma_mem*)ptrMem) < 0)
      {
        sprintf(mesx, "siSoftware::grab Fg_Acquire() failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
      }
  }
 else
 {
 if (Fg_AcquireEx(fg,camPort,GRAB_INFINITE,ACQ_BLOCK,(dma_mem*)ptrMem) < 0)
      {
        sprintf(mesx, "siSoftware::grab Fg_Acquire() failed: %s\n", Fg_getLastErrorDescription(fg));
        lf.log(mesx);
      }
 }

  clearMissedFrames();
    //UpdateMenu();
}

void siSoftware::snap()
{
char mesx[256];

    clearMissedFrames();
  // m_statusWnd.SetWindowText("");
  if(Fg_AcquireEx(fg,camPort,1,ACQ_STANDARD,(dma_mem*)ptrMem) < 0){
    sprintf(mesx, "siSoftware::grab Fg_Acquire() failed: %s\n", Fg_getLastErrorDescription(fg));
    lf.log(mesx);
  }
    //UpdateMenu();
}

int siSoftware::getNumFreeBuffers(void)
{
    
    return(sap_buffer_count - (frame_count-frames_to_cpu));
}
int siSoftware::getNumBuffers(void)
{
        return(sap_buffer_count);
}


     //inc missed frames counter
   void siSoftware::incMissedFrames(void)
   {
    missed_frames++;
    recent_missed_frames++;
    
   }


//*****************************************************************************************
//
//                    Acquisition Options
//
//*****************************************************************************************






//*****************************************************************************************
//
//                    File Options
//
//*****************************************************************************************


//**************************************************************************************
//
//            Processing Options
//
//**************************************************************************************


void siSoftware::GetSignalStatus()
{
}




void siSoftware::setPin(char* pinstr, int val)
{

    int status,status2;
    //char msg[255];
    //char errbuff[1024];
//char infobuff[1024];



//    siSoftware_log.log("Doing Reset in siSoftware Card");



}



#endif

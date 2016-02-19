
#ifndef H_CXP_H
#define H_CXP_H

#define _STDINT
#include <fgrab_prototyp.h>
#include <fgrab_struct.h>
#include <fgrab_define.h>


// ---------------------------------------------------------------------------
typedef unsigned char  uchar;
typedef unsigned int   uint;
typedef const char     cchar;

// -----------------------------------------------------------------------------------
/** handle structure for boards */
struct CxpBoardHandle;
/** handle structure for cameras */
struct CxpCameraHandle;

/** property type: UNSIGNED INT */
#define GBE_PROPERTY_TYPE_UINT 0x1

/** structure for camera infos */
struct CxpCameraInfo
{
	/** manufactor name */
	char manufactor_name[32];
	/** model name */
	char model_name[32];
	/** device version */
	char device_version[32];
	/** manufacturer info */
	char manufacturer_info[48];
	/** serial number */
	char serial_number[16];
	/** user name */
	char user_name[16];

	/** MAC address */
	uint8_t mac[6];
};

// ----------------------------------------------------------------------------------------------

#define PORTMASK0 1
#define PORTMASK1 2
#define PORTMASK2 4
#define PORTMASK3 8

#define CXP_SPEED_1250 0x28
#define CXP_SPEED_2500 0x30
#define CXP_SPEED_3125 0x38
#define CXP_SPEED_5000 0x40
#define CXP_SPEED_6250 0x88

#ifdef __cplusplus
extern "C"
{
#endif

int  Cxp_initBoard			(Fg_Struct *fg, int init_flag, struct CxpBoardHandle** board_handle_ptr);
void Cxp_freeBoard			(struct CxpBoardHandle *board_handle);

int  Cxp_setDemoMode		(struct CxpBoardHandle *board_handle,bool tag,int width,int height,int divider,struct CxpCameraHandle **camera_handle);

int Cxp_scanPorts			(struct CxpBoardHandle *board_handle, int portMask, int discovery_timeout,int speed);
bool Cxp_isCameraAvailable	(struct CxpBoardHandle *board_handle, int port);

struct CxpCameraInfo* Cxp_getCameraInfo(struct CxpCameraHandle *camera_handle);

int Cxp_getCamera(struct CxpBoardHandle *board_handle, int port, struct CxpCameraHandle **camera_handle_ptr);

int Cxp_connectCamera(struct CxpCameraHandle *camera_handle);
int Cxp_connectCameraWithExternalXml(struct CxpCameraHandle *camera_handle,char* filename);
int Cxp_disconnectCamera(struct CxpCameraHandle *camera_handle);

int Cxp_powerSwitch(struct CxpBoardHandle *board_handle,int portMask);


int Cxp_setIntegerValue(struct CxpCameraHandle *camera_handle, const char* name, int64_t value);
int Cxp_getIntegerValue(struct CxpCameraHandle *camera_handle, const char* name, int64_t* value_ptr);

int Cxp_setBooleanValue(struct CxpCameraHandle *camera_handle, const char* name, unsigned int value);
int Cxp_getBooleanValue(struct CxpCameraHandle *camera_handle, const char* name, unsigned int* value_ptr);

int Cxp_setFloatValue(struct CxpCameraHandle *camera_handle, const char* name, double value);
int Cxp_getFloatValue(struct CxpCameraHandle *camera_handle, const char* name, double* value_ptr);

int Cxp_setStringValue(struct CxpCameraHandle *camera_handle, const char* name, const char* value);
int Cxp_getStringValue(struct CxpCameraHandle *camera_handle, const char* name, const char** value_ptr);

int Cxp_setEnumerationValue(struct CxpCameraHandle *camera_handle, const char* name, int64_t value);
int Cxp_getEnumerationValue(struct CxpCameraHandle *camera_handle, const char* name, int64_t* value_ptr);

int Cxp_executeCommand(struct CxpCameraHandle *camera_handle, const char* name);
int Cxp_isCommandDone(struct CxpCameraHandle *camera_handle, const char* name,bool *value_ptr);

int Cxp_startAcquisition(struct CxpCameraHandle *camera_handle,bool startCamera);
int Cxp_stopAcquisition(struct CxpCameraHandle *camera_handle,bool stopCamera);

int Cxp_getGenICamXML(struct CxpCameraHandle *camera_handle, char *buffer, size_t *buffer_size);
int Cxp_setGenICamXML(struct CxpCameraHandle *camera_handle, char *buffer, size_t buffer_size);

const char* Cxp_getErrorDescription(int errcode);

//int Gbe_lockTLParams(struct CxpCameraHandle *camera_handle);
//int Gbe_unlockTLParams(struct CxpCameraHandle *camera_handle);

//int Cxp_registerReadFromCamera(struct CxpCameraHandle *camera_handle, void* buffer, uint64_t address, size_t length);
//int Cxp_registerWriteToCamera(struct CxpCameraHandle *camera_handle, const void* buffer, uint64_t address, size_t length);
int Cxp_registerReadFromCamera(struct CxpCameraHandle *camera_handle, uint32_t *data, uint64_t address);
int Cxp_registerWriteToCamera(struct CxpCameraHandle *camera_handle, uint32_t data, uint64_t address);

int Cxp_stringReadFromCamera(struct CxpCameraHandle *camera_handle, char *data, uint64_t address,size_t length);

int Cxp_ReadFromCamera(struct CxpCameraHandle *camera_handle, char* data, uint64_t address,int32_t length);
int Cxp_WriteToCamera(struct CxpCameraHandle *camera_handle, char* data, uint64_t address,int32_t length);

#ifdef __cplusplus
}
#endif
#endif

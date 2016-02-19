/**
	\file   gbe.h

	\brief  GigE Vision functions

	\author Silicon Software GmbH
*/
#ifndef __SISO_GBE_H__
#define __SISO_GBE_H__

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <msinttypes/stdint.h>
#else
#include <stdint.h>
#endif

#include <sys/types.h>

/** handle structure for boards */
struct BoardHandle;
/** handle structure for cameras */
struct CameraHandle;

/** property type: UNSIGNED INT */
#define GBE_PROPERTY_TYPE_UINT 0x1

/** structure for camera infos */
struct CameraInfo
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

#ifdef __cplusplus
extern "C" {
#endif

/**
	\brief Initializes the Gbe board
	\param board board id
	\param init_flag initialization flags, should be 0
	\param board_handle_ptr pointer to the board handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_initBoard(int board, int init_flag, struct BoardHandle** board_handle_ptr);

/**
	\brief Releases the Gbe board
	\param board_handle board handle instance
*/
void Gbe_freeBoard(struct BoardHandle *board_handle);

/**
	\brief Scans the network for cameras
	\param board_handle board handle instance
	\param port port number
	\param discovery_timeout timeout for the camera discovery
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_scanNetwork(struct BoardHandle *board_handle, int port, int discovery_timeout);

/**
	\brief Get the number of cameras found on a port
	\param board_handle board handle instance
	\param port port number
	\return number of cameras found
*/
int Gbe_getCameraCount(struct BoardHandle *board_handle, int port);

/**
	\brief Get the first camera found on a port
	\param board_handle board handle instance
	\param port port number
	\param camera_handle_ptr pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getFirstCamera(struct BoardHandle *board_handle, int port, struct CameraHandle **camera_handle_ptr);

/**
	\brief Get camera by MAC on a port
	\param board_handle board handle instance
	\param port port number
	\param mac MAC address of the camera
	\param camera_handle_ptr pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getCameraByMac(struct BoardHandle *board_handle, int port, uint8_t mac[6], struct CameraHandle **camera_handle_ptr);

/**
	\brief Get camera by IP on a port
	\param board_handle board handle instance
	\param port port number
	\param ip IP address of the camera
	\param camera_handle_ptr pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getCameraByIp(struct BoardHandle *board_handle, int port, uint32_t ip, struct CameraHandle **camera_handle_ptr);

/**
	\brief Get camera by camera user name on a port
	\param board_handle board handle instance
	\param port port number
	\param user_name user name of the camera (from the camera bootstrap registers)
	\param camera_handle_ptr pointer to the camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getCameraByUserName(struct BoardHandle *board_handle, int port, char* user_name, struct CameraHandle **camera_handle_ptr);

/**
	\brief Get camera info
	\param camera_handle camera handle instance
	\return camera info structure
	\see CameraInfo
*/
struct CameraInfo* Gbe_getCameraInfo(struct CameraHandle *camera_handle);

/**
	\brief Releases the camera
	\param camera_handle camera handle instance
*/
void Gbe_freeCamera(struct CameraHandle *camera_handle);

/**
	\brief Establish camera connection
	\param camera_handle camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_connectCamera(struct CameraHandle *camera_handle);

/**
	\brief Release camera connection
	\param camera_handle camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_disconnectCamera(struct CameraHandle *camera_handle);

/**
	\brief Switch established camera connection to another board/port
	\param camera_handle camera handle instance
	\param board_handle board handle instance
	\param port port number
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_switchCameraConnection(struct CameraHandle *camera_handle, struct BoardHandle *board_handle, int port);

/**
	\brief Set GenICam Integer value
	\param camera_handle camera handle instance
	\param name feature name
	\param value value to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_setIntegerValue(struct CameraHandle *camera_handle, const char* name, int64_t value);

/**
	\brief Get GenICam Integer value
	\param camera_handle camera handle instance
	\param name feature name
	\param value_ptr pointer to value
	\return Gbe error code
	\see Gbe_getErrorDescription
*/
int Gbe_getIntegerValue(struct CameraHandle *camera_handle, const char* name, int64_t* value_ptr);

/**
	\brief Set GenICam Boolean value
	\param camera_handle camera handle instance
	\param name feature name
	\param value value to write (0 = false, otherwise true)
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_setBooleanValue(struct CameraHandle *camera_handle, const char* name, unsigned int value);

/**
	\brief Get GenICam Boolean value
	\param camera_handle camera handle instance
	\param name feature name
	\param value_ptr pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getBooleanValue(struct CameraHandle *camera_handle, const char* name, unsigned int* value_ptr);

/**
	\brief Set GenICam Float value
	\param camera_handle camera handle instance
	\param name feature name
	\param value value to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_setFloatValue(struct CameraHandle *camera_handle, const char* name, double value);

/**
	\brief Get GenICam Float value
	\param camera_handle camera handle instance
	\param name feature name
	\param value_ptr pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getFloatValue(struct CameraHandle *camera_handle, const char* name, double* value_ptr);

/**
	\brief Set GenICam String value
	\param camera_handle camera handle instance
	\param name feature name
	\param value value to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_setStringValue(struct CameraHandle *camera_handle, const char* name, const char* value);

/**
	\brief Get GenICam String value
	\param camera_handle camera handle instance
	\param name feature name
	\param value_ptr pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getStringValue(struct CameraHandle *camera_handle, const char* name, const char** value_ptr);

/**
	\brief Set GenICam Enumeration value
	\param camera_handle camera handle instance
	\param name feature name
	\param value value to write
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_setEnumerationValue(struct CameraHandle *camera_handle, const char* name, const char* value);

/**
	\brief Get GenICam Enumeration value
	\param camera_handle camera handle instance
	\param name feature name
	\param value_ptr pointer to value
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getEnumerationValue(struct CameraHandle *camera_handle, const char* name, const char** value_ptr);

/**
	\brief Execute GenICam Command
	\param camera_handle camera handle instance
	\param name feature name
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_executeCommand(struct CameraHandle *camera_handle, const char* name);

/**
	\brief Check status of a GenICam Command
	\param camera_handle camera handle instance
	\param name feature name
	\return 0 = command in inactive, otherwise active
*/
int Gbe_isCommandDone(struct CameraHandle *camera_handle, const char* name);

/**
	\brief Start image acquisition
	\param camera_handle camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_startAcquisition(struct CameraHandle *camera_handle);

/**
	\brief Stop image acquisition
	\param camera_handle camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_stopAcquisition(struct CameraHandle *camera_handle);

/**
	\brief Get the GenICam XML data for the camera
	\param camera_handle camera handle instance
	\param buffer user supplied buffer for the xml data (use 0 to get the needed buffer size!)
	\param buffer_size size of the user supplied buffer for the xml data (as return value: real size of the xml data)
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
*/
int Gbe_getGenICamXML(struct CameraHandle *camera_handle, char *buffer, size_t *buffer_size);

/**
	\brief Get error description
	\param errcode error code (defined in gbe_error.h)
	\return error description as string
*/
const char* Gbe_getErrorDescription(int errcode);

/**
	\brief Enable/Disable GenICam checks (i.e. locked/available/implemented)
	\param camera_handle camera handle instance
	\param flag 0 = false, otherwise true
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_enableChecks(struct CameraHandle *camera_handle, unsigned int flag);

/**
	\brief Lock GenICam transport layer parameters
	\param camera_handle camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_lockTLParams(struct CameraHandle *camera_handle);

/**
	\brief Unlock GenICam transport layer parameters
	\param camera_handle camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_unlockTLParams(struct CameraHandle *camera_handle);

/**
	\brief Invalidates the register cache for the camera
	\param camera_handle camera handle instance
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_invalidateCache(struct CameraHandle *camera_handle);

/**
	\brief Read register content from camera
	\param camera_handle camera handle instance
	\param buffer user buffer to be filled
	\param address camera register space address
	\param length size of camera register space to be read
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_registerReadFromCamera(struct CameraHandle *camera_handle, void* buffer, uint64_t address, size_t length);

/**
	\brief Write register content to camera
	\param camera_handle camera handle instance
	\param buffer user buffer to be written
	\param address camera register space address
	\param length size of camera register space to be written
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_registerWriteToCamera(struct CameraHandle *camera_handle, const void* buffer, uint64_t address, size_t length);

/**
	\brief Set board property
	\param board_handle board handle instance
	\param propertyName name of the property
	\param propertyValuePtr pointer to the value of the property
	\param propertyType type of the property (see PROPERTY_TYPE_* defines)
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_setBoardPropertyWithType(struct BoardHandle *board_handle, const char* propertyName, const void* propertyValuePtr, int propertyType);

/**
	\brief Get board property
	\param board_handle board handle instance
	\param propertyName name of the property
	\param propertyValuePtr pointer to the value of the property
	\param propertyType type of the property (see GBE_PROPERTY_TYPE_* defines)
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_getBoardPropertyWithType(struct BoardHandle *board_handle, const char* propertyName, void* propertyValuePtr, int propertyType);

/**
	\brief Set camera property
	\param camera_handle camera handle instance
	\param propertyName name of the property
	\param propertyValuePtr pointer to the value of the property
	\param propertyType type of the property (see PROPERTY_TYPE_* defines)
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_setCameraPropertyWithType(struct CameraHandle *camera_handle, const char* propertyName, const void* propertyValuePtr, int propertyType);

/**
	\brief Get camera property
	\param camera_handle camera handle instance
	\param propertyName name of the property
	\param propertyValuePtr pointer to the value of the property
	\param propertyType type of the property (see GBE_PROPERTY_TYPE_* defines)
	\return Gbe error code (defined in gbe_error.h)
	\see Gbe_getErrorDescription
	\warning expert function
*/
int Gbe_getCameraPropertyWithType(struct CameraHandle *camera_handle, const char* propertyName, void* propertyValuePtr, int propertyType);

#ifdef __cplusplus
}
#endif

#endif

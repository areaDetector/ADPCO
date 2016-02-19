/**
	\file   gbe_error.h

	\brief  Error codes for GigE Vision functions

	\author Silicon Software GmbH
*/
#ifndef __SISO_GBE_ERROR_H__
#define __SISO_GBE_ERROR_H__

/** success */
#define GBE_OK                              0

/** feature not implemented */
#define GBE_ERR_FEATURE_NOT_IMPLEMENTED    -40001
/** feature not available */
#define GBE_ERR_FEATURE_NOT_AVAILABLE      -40002
/** feature locked */
#define GBE_ERR_FEATURE_LOCKED             -40003
/** feature not readable */
#define GBE_ERR_FEATURE_NOT_READABLE       -40004
/** feature not writable */
#define GBE_ERR_FEATURE_NOT_WRITABLE       -40005
/** illegal value */
#define GBE_ERR_ILLEGAL_VALUE              -40006
/** illegal node */
#define GBE_ERR_ILLEGAL_NODE               -40007
/** illegal register length */
#define GBE_ERR_ILLEGAL_REGISTER_LENGTH    -40008
/** math error */
#define GBE_ERR_MATH                       -40009
/** port not available */
#define GBE_ERR_PORT_NOT_AVAILABLE         -40010
/** transport layer exception */
#define GBE_ERR_TRANSPORT_LAYER_EXCEPTION  -40011
/** camera/service connection exception */
#define GBE_ERR_CONNECTION_EXCEPTION       -40012
/** environment not set */
#define GBE_ERR_ENVIRONMENT_NOT_SET        -40013
/** file does not exist */
#define GBE_ERR_FILE_DOES_NOT_EXIST        -40014
/** illegal parameter */
#define GBE_ERR_ILLEGAL_PARAMETER          -40015
/** camera not connected */
#define GBE_ERR_CAMERA_NOT_CONNECTED       -40016
/** camera already connected */
#define GBE_ERR_CAMERA_ALREADY_CONNECTED   -40017
/** board initialization error */
#define GBE_ERR_BOARD_INITIALIZATION_ERROR -40018
/** board does not exist */
#define GBE_ERR_BOARD_DOES_NOT_EXIST       -40019
/** xml read exception */
#define GBE_ERR_XML_READ_EXCEPTION         -40020
/** service connection error */
#define GBE_ERR_SERVICE_CONNECTION_ERROR   -40021
/** camera busy */
#define GBE_ERR_CAMERA_BUSY                -40022
/** no cameras found */
#define GBE_ERR_NO_CAMERAS_FOUND           -40023
/** camera does not exist */
#define GBE_ERR_CAMERA_DOES_NOT_EXIST      -40024
/** buffer too small */
#define GBE_ERR_BUFFER_TOO_SMALL           -40025
/** unknown property */
#define GBE_ERR_UNKNOWN_PROPERTY           -40026
/** invalid property type */
#define GBE_ERR_INVALID_PROPERTY_TYPE      -40027
/** invalid property value */
#define GBE_ERR_INVALID_PROPERTY_VALUE     -40028

/** selected camera is not unique (warning) */
#define GBE_WARN_CAMERA_NOT_UNIQUE         -41000

#endif

/*
 * fgrab_define.h
 *
 * Copyright (c) 2002-2009 Silicon Software GmbH, All Rights Reserved.
 */

/**
 * \file fgrab_define.h
 *
 * \brief Constant and enum definitions.
 *
 * \author Silicon Software GmbH
 */

#ifndef __FGRABDEFINE_H
#define __FGRABDEFINE_H

#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <msinttypes/stdint.h>
#else
#include <stdint.h>
#endif

#include "sisoboards.h"
#include "os_type.h"

enum RowFilterModes {
	_NON_TRIGGERED_EOF_CONTROLLED = 0,
	_NON_TRIGGERED_LINE_COUNT_CONTROLLED = 0x1,
	_TRIGGERED_EOF_CONTROLLED = 0x2,
	_TRIGGERED_LINE_COUNT_CONTROLLED = 0x3
};

enum TriggerMode    {
	_GRABBER_CONTROLLED_ = 0,
	_GRABBER_CONTROLLED_STROBE = 1,
	_GRABBER_CONTROLLED_TRIGGER = 2,
	_SINGLE_SHOT = 4,
	_EXTERNAL_TRIGGER = 6
};

enum LineTriggerMode  {
	_LRM_AUTO = 0,		/**< auto generated line trigger output (VINIT) */
	_LRM_EXT_TRG = 1	/**< external trigger */
};

enum LineTriggerGateMode {
	_LRM_NON_GATED = 0,	/**< no image trigger */
	_LRM_GATED_COUNT = 1,	/**< image trigger rising edge starts sampling */
	_LRM_GATED_PARTIAL = 2,	/**< image sampling during gate active. if the gate
					signal exceeds AreaHeight number of lines, the
					transfer is completed by an EoT, but continues
					(Image Tag Bit 31 is 0 if the next frame belongs
					to the same image strip) */
	_LRM_GATED = 3		/**< image sampling during gate active */
};

enum TriggerSync {
	_LVAL = 0,
	_HDSYNC = 1
};

enum CCsel {
	CC_EXSYNC = 0,
	CC_PRESCALER = 1,
	CC_HDSYNC = CC_PRESCALER,
	CC_EXSYNC2 = CC_PRESCALER,
	CC_STROBEPULSE = 2,
	CC_CLK = 3,
	CC_GND = 4,
	CC_VCC = 5,
	CC_NOT_EXSYNC = 6,
	CC_NOT_PRESCALER = 7,
	CC_NOT_HDSYNC = CC_NOT_PRESCALER,
	CC_NOT_EXSYNC2 = CC_NOT_PRESCALER,
	CC_NOT_STROBEPULSE = 8
};

enum MeTriggerMode{
	FREE_RUN						= 0,
	GRABBER_CONTROLLED				= 1,
	ASYNC_TRIGGER					= 2,
	GRABBER_CONTROLLED_SYNCHRON		= 3,
	ASYNC_SOFTWARE_TRIGGER			= 4,
	ASYNC_GATED						= 5,
	ASYNC_GATED_MULTIFRAME			= 6,
	ASYNC_SOFTWARE_TRIGGER_QUEUED	= 7
};

enum MeLineTriggerMode{
	GRABBER_CONTROLLED_GATED		= 6
};

enum MeShaftMode {
	SOURCE_A		= 0,
	SOURCE_B		= 1
};
enum MeLineShadingMode{
	SHADING_OFF			= 0,
	SHADING_SUB			= 1,
	SHADING_MULT		= 2,
	SHADING_SUB_MULT	= 3
};

enum MeKneeLutMode{
	FG_INDEP		= 0,
	FG_DEP			= 1
};

enum MeAreaTriggerMode{
	AREA_FREE_RUN					= 0,
	AREA_GRABBER_CONTROLLED			= 1,
	AREA_ASYNC_TRIGGER				= 2,
	AREA_GRABBER_CONTROLLED_SYNCHRON= 3,
	AREA_ASYNC_SOFTWARE_TRIGGER		= 4
};

enum MeLineTriggerModeLine{
	LINE_FREE_RUN_LINE						= 0,
	LINE_GRABBER_CONTROLLED_LINE			= 1,
	LINE_ASYNC_TRIGGER_LINE					= 2,
	LINE_ASYNC_GATED_LINE					= 5,
	LINE_GRABBER_CONTROLLED_GATED_LINE		= 6
};


enum MeLineTriggerModeImage{
	LINE_FREE_RUN_IMAGE						= 0,
	LINE_GRABBER_CONTROLLED_IMAGE			= 1,
	LINE_ASYNC_TRIGGER_IMAGE				= 2,
	LINE_GRABBER_CONTROLLED_GATED_IMAGE		= 5,
	LINE_ASYNC_GATED_MULTIBUFFERS_IMAGE		= 6
};



enum MeRgbComponentMapping {
	FG_MAP_PIXEL0 = 0,
	FG_MAP_PIXEL1 = 1,
	FG_MAP_PIXEL2 = 2,
	FG_MAP_PIXEL3 = 3
};

enum  MeCameraLinkFormat {
	FG_CL_SINGLETAP_8_BIT		= 8,
	FG_CL_SINGLETAP_10_BIT		= 10,
	FG_CL_SINGLETAP_12_BIT		= 12,
	FG_CL_SINGLETAP_14_BIT		= 14,
	FG_CL_SINGLETAP_16_BIT		= 16,
	FG_CL_DUALTAP_8_BIT			= 108,
	FG_CL_DUALTAP_10_BIT		= 110,
	FG_CL_DUALTAP_12_BIT		= 112,
	FG_CL_TRIPLETAP_8_BIT		= 120,
	FG_CL_LITE_8_BIT			= 130,
	FG_CL_LITE_10_BIT			= 140,
	FG_CL_RGB					= 500,
	FG_CL_MEDIUM_8_BIT			= 208,
	FG_CL_MEDIUM_10_BIT			= 210,
	FG_CL_MEDIUM_12_BIT			= 212,
	FG_CL_MEDIUM_RGB_24			= 214,
	FG_CL_MEDIUM_RGB_30			= 216,
	FG_CL_MEDIUM_RGB_36			= 218,
	FG_CL_8BIT_FULL_8			= 308,
	FG_CL_8BIT_FULL_10			= 310

};

enum MeCameraTypes {
	FG_AREA_GRAY					= 0,
	FG_AREA_BAYER					= 1,
	FG_LINE_GRAY					= 2,
	FG_SINGLE_LINE_RGB				= 3,
	FG_DUAL_LINE_RGB				= 4,
	FG_SINGLE_AREA_RGB				= 5,
	FG_DUAL_AREA_RGB				= 6,
	FG_AREA_HSI						= 7,
	FG_DUAL_LINE_RGB_SHADING		= 8,
	FG_SINGLE_LINE_RGBHSI			= 9,
	FG_SINGLE_AREA_RGB_SEPARATION	= 10,
	FG_MEDIUM_LINE_RGB				= 11,
	FG_MEDIUM_LINE_GRAY				= 12,
	FG_MEDIUM_AREA_GRAY				= 13,
	FG_MEDIUM_AREA_RGB				= 14,
	FG_AREA_GRAY12					= 15,
	FG_SEQUENCE_EXTRACTOR_A			= 16,
	FG_SEQUENCE_EXTRACTOR_B			= 17,
	FG_LINE_GRAY12					= 18,
	FG_AREA_RGB36					= 19,
	FG_DUAL_LINE_RGB_SORTING		= 20,
	FG_DUAL_LINE_GRAY12				= 21,
	FG_MEDIUM_LINE_GRAY12			= 22,
	FG_SINGLE_AREA_GRAY12			= 23,
	FG_2D_SHADING_12				= 24,
	DIVISOR_1						= 25,
	DIVISOR_2						= 26,
	DIVISOR_4						= 27,
	DIVISOR_8						= 28,
	DIVISOR_3						= 29,
	DIVISOR_16						= 30,
	DIVISOR_6						= 31

};

enum MeSensorReadoutModes2{
	SMODE_UNCHANGED			= 0,
	SMODE_REVERSE			= 3,
	SMODE_TAB2_0			= 1,
	SMODE_TAB2_1			= 4,
	SMODE_TAB2_2			= 6,
	SMODE_TAB4_0			= 2,
	SMODE_TAB4_1			= 5,
	SMODE_TAB4_2			= 7,
	SMODE_TAB4_5			= 8,
	SMODE_TAB4_3			= 9,
	SMODE_TAB4_4			= 10,
	SMODE_TAB4_6			= 11,

    SMODE_TAB8_0			= 30,	/**< |1234>            <8765| */
    SMODE_TAB8_1			= 31,	/**< |1234>            <5678| */
    SMODE_TAB8_2			= 32,	/**< |1234>     5678>       | */
    SMODE_TAB8_3			= 33,	/**< |12>   34>   56>   78> | */
    SMODE_TAB8_4			= 34,	/**< | <87   <65   <43   <21| */
    SMODE_TAB8_5			= 35,	/**< |12>   34>    <56   <78| */

    SMODE_TAB8_6			= 36,
    SMODE_TAB8_7			= 37,
    SMODE_TAB8_8			= 38,
    SMODE_TAB8_9			= 39,
    SMODE_TAB8_10			= 40,
    SMODE_TAB8_11			= 41,
    SMODE_TAB8_12			= 42,
    SMODE_TAB8_13			= 43,
    SMODE_TAB8_14			= 44,
    SMODE_TAB8_15			= 45,
    SMODE_TAB8_16			= 46,
    SMODE_TAB8_17			= 47,
    SMODE_TAB8_18			= 48,
    SMODE_TAB8_19			= 49,
    SMODE_TAB8_20			= 50,
    SMODE_TAB8_21			= 51,
    SMODE_TAB8_22			= 52,
    SMODE_TAB8_23			= 53,
    SMODE_TAB8_24			= 54

};

enum FgParamTypes {
	FG_PARAM_TYPE_INVALID = 0x0,	/**< will always cause an error, for internal use only */
	FG_PARAM_TYPE_INT32_T = 0x1,	/**< signed 32 bit integer (int, int32_t) */
	FG_PARAM_TYPE_UINT32_T = 0x2,	/**< unsigned 32 bit integer (unsigned int, uint32_t) */
	FG_PARAM_TYPE_INT64_T = 0x3,	/**< signed 64 bit integer (long long, int64_t) */
	FG_PARAM_TYPE_UINT64_T = 0x4,	/**< unsigned 64 bit integer (unsigned long long, uint64_t) */
	FG_PARAM_TYPE_DOUBLE = 0x5,	/**< double */
	FG_PARAM_TYPE_CHAR_PTR = 0x6,	/**< char* */
	FG_PARAM_TYPE_SIZE_T = 0x7,	/**< size_t */
	FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS = 0x1000,	/**< struct FieldParameterAccess */
	FG_PARAM_TYPE_STRUCT_FIELDPARAMINT = 0x1002,	/**< struct FieldParameterInt_s */
	FG_PARAM_TYPE_STRUCT_FIELDPARAMDOUBLE = 0x1005,	/**< struct FieldParameterDouble_s */
	FG_PARAM_TYPE_AUTO = -1		/**< will always cause an error, for internal use only */
};

enum FgImageSourceTypes {
	FG_CAMPORT = 0,
	FG_GENERATOR = 1
};

/*
 * Defines
 */

#define FG_NO		0
#define FG_YES		1

#define FG_LOW		0
#define FG_HIGH		1

#define	FG_FALLING	1
#define	FG_RISING	0

#define FG_ON		1
#define FG_OFF		0

#define FG_ZERO		0
#define FG_ONE		1

#define FG_APPLY	1

#define FG_LEFT_ALIGNED			1
#define FG_RIGHT_ALIGNED		0

#define FG_SAVE_LUT_TO_FILE		1
#define FG_LOAD_LUT_FROM_FILE	0

#define FG_0_BIT	0
#define FG_1_BIT	1
#define FG_2_BIT	2
#define FG_3_BIT	3
#define FG_4_BIT	4
#define FG_5_BIT	5
#define FG_6_BIT	6
#define FG_7_BIT	7
#define FG_8_BIT	8
#define FG_9_BIT	9
#define FG_10_BIT	10
#define FG_11_BIT	11
#define FG_12_BIT	12
#define FG_13_BIT	13
#define FG_14_BIT	14
#define FG_15_BIT	15
#define FG_16_BIT	16
#define FG_17_BIT	17
#define FG_18_BIT	18
#define FG_19_BIT	19
#define FG_20_BIT	20
#define FG_21_BIT	21
#define FG_22_BIT	22
#define FG_23_BIT	23
#define FG_24_BIT	24
#define FG_25_BIT	25
#define FG_26_BIT	26
#define FG_27_BIT	27
#define FG_28_BIT	28
#define FG_29_BIT	29
#define FG_30_BIT	30
#define FG_31_BIT	31
#define FG_32_BIT	32
#define FG_36_BIT	36
#define FG_48_BIT	48

#define FG_MSB		0
#define FG_LSB		1

#define MAX_BUF_NR 1048576

#define CONTMODE			0x10
#define HANDSHAKEMODE		0x20
#define BLOCKINGMODE		HANDSHAKEMODE
#define PULSEMODE			0x30

#define FG_GRAY					3
#define FG_GRAY_PLUS_PICNR		30
#define FG_GRAY16				1
#define FG_GRAY16_PLUS_PICNR	10
#define FG_COL24				2
#define FG_COL32				4
#define FG_COL30				5
#define FG_COL48				6
#define FG_BINARY				8
#define FG_GRAY32				20

/* acquire formats */
#define ACQ_STANDARD	0x1
#define ACQ_BLOCK		0x2

#define NUMBER_OF_GRABBED_IMAGES		10	/**< how many frames were already transferred */
#define NUMBER_OF_LOST_IMAGES			20	/**< how many frames were lost */
#define NUMBER_OF_BLOCK_LOST_IMAGES		30	/**< how many frames were lost because all buffers were blocked */
#define NUMBER_OF_BLOCKED_IMAGES		40	/**< how many frames were blocked */
#define NUMBER_OF_ACT_IMAGE			50	/**< index of the frame transferred last */
#define NUMBER_OF_LAST_IMAGE			60	/**< index of the frame requested last */
#define NUMBER_OF_NEXT_IMAGE			70	/**< index of the frame following the last requested one */
#define NUMBER_OF_IMAGES_IN_PROGRESS		80	/**< how many frames were transferred but not yet requested */

#define BUFFER_STATUS					90
#define GRAB_ACTIVE					100	/**< state of transfer (running, stopped) */

#define FG_REVNR						99

#define FG_BLOCK						0x100
#define FG_UNBLOCK						0x200
#define FG_UNBLOCK_ALL					0x220

#define SEL_ACT_IMAGE		200
#define SEL_LAST_IMAGE		210
#define SEL_NEXT_IMAGE		220
#define SEL_NUMBER			230
#define SEL_NEW_IMAGE		240

/* +-- PARAMETER --+ */

#define FG_WIDTH	100
#define FG_HEIGHT	200

#define FG_MAXWIDTH		6100
#define FG_MAXHEIGHT	6200
#define FG_ACTIVEPORT	6300

#define FG_XOFFSET			300
#define FG_YOFFSET			400
#define FG_XSHIFT			500
#define FG_TIMEOUT			600
#define FG_APC_STOP_TIMEOUT		601		/**< how many milliseconds Fg_AcquireEx(Fg, DmaIndex, memHandle, STOP_SYNC_TO_APC) will wait for the APC to return (unsigned int) */
#define FG_FORMAT			700
#define FG_CAMSUBTYP		80
#define FG_FRAMESPERSEC		90
#define FG_MAXFRAMESPERSEC	91
#define FG_MINFRAMESPERSEC	92
#define FG_LINESPERSEC		95
#define FG_LINEPERIOD		96
#define FG_LINEPERIODE		FG_LINEPERIOD /*< duplicated for compatibility issues*/

#define FG_EXPOSURE			10020			/**< Exposure Time in us (Brigthness) (float) */
#define FG_LINEEXPOSURE		10030
#define FG_HDSYNC			10050

#define FG_PRESCALER		FG_HDSYNC
#define FG_LINETRIGGER		FG_HDSYNC

#define FG_RS232PARON		10060


#define FG_PIXELDEPTH				4000
#define FG_BITALIGNMENT				4010
#define FG_LINEALIGNMENT			4020
#define FG_COLOR_SELECT				4030

#define FG_CAMBITWIDTH				5000
#define FG_CAMBITSHIFT				5010
#define FG_SHIFTCAMDATARIGHT		5020
#define FG_ROTATECAMDATA			FG_SHIFTCAMDATARIGHT /* compatibility mode, do not use */
#define FG_USEDVAL					5025
#define FG_SWAPENDIAN				5028
#define FG_MASKCAMDATA				5030
#define FG_ADDOFFSET				5035
#define FG_DROPPEDIMAGEES			5040
#define FG_SENSORREADOUT			5050
#define FG_SENSORREADOUT_TAPS		5051
#define FG_SENSORREADOUT_DIREC		5052

#define FG_TRIGGERMODE				8100
#define FG_LINETRIGGERMODE			8102
#define FG_IMGTRIGGERMODE			8104
#define FG_IMGTRIGGERON				8106
#define FG_TRIGGERINSRC				8110
#define FG_LINETRIGGERINSRC			8112
#define FG_IMGTRIGGERINSRC			8113
#define FG_LINETRIGGERINPOLARITY	8115
#define FG_IMGTRIGGERINPOLARITY		8116
#define FG_TRIGGERINPOLARITY		FG_IMGTRIGGERINPOLARITY
#define FG_IMGTRIGGERGATEDELAY		8118
#define FG_USEROUT					8120
#define FG_EXSYNCINVERT				8200
#define FG_EXSYNCON					8300
#define FG_EXSYNCDELAY				8400
#define FG_EXSYNCPOLARITY			8420
#define FG_DEADTIME					8450
#define FG_DEADTIME_OFFSET			8460
#define FG_BGRRGBORDER				8500
#define FG_FLASHON					8600

#define FG_SENDSOFTWARETRIGGER		8800
#define FG_SETSOFTWARETRIGGER		8801
#define FG_SOFTWARETRIGGER_QUEUE_FILLLEVEL 8802
#define FG_LINETRIGGERDELAY			8900
#define FG_LIMIT_TRIGGER_PULSES		8950
#define FG_TRIGGERMASTERSYNC		9000

#define FG_SHAFTENCODERINSRC		9100
#define FG_SHAFTENCODERON			9110
#define FG_SHAFTENCODERLEADING		9120
#define FG_SHAFTENCODER_COMPCOUNTER	9125

#define FG_RGB_MAP_RED				9200
#define FG_RGB_MAP_GREEN			9210
#define FG_RGB_MAP_BLUE				9220

#define FG_CAMSTATUS			2000
#define FG_CAMSTATUS_EXTENDED	2050
#define FG_TWOCAMMODEL			2100
#define FG_PORT					3000
#define FG_NR_OF_DMAS			3050
#define FG_NR_OF_CAMS			3060
#define FG_NR_OF_PROCESSES		3070
#define FG_DMA_PORT				3080
#define FG_CAM_PORT				3090

#define FG_TRANSFER_LEN			5210

#define FG_STROBEPULSEDELAY		8700
#define FG_STROBEPULSEREDUCE	8710
#define FG_STROBEPULSESRCSEL	8720
#define FG_STROBEPULSEINVERT	8730
#define FG_FLASHTIME			8740		
#define FG_FLASHTIME_SYNC		8750	

#define FG_CAMERA_LINK_CAMTYP			11011
#define FG_CAMERA_LINK_CAMTYPE			FG_CAMERA_LINK_CAMTYP
#define FG_CL_CAMTYP					11011
#define FG_CAMTYP						11011
#define FG_GBE_CAMTYP					11011

#define FG_LOOKUPTABLE					12000
#define FG_LUT_FILE						12010
#define FG_LUT_SAVE_LOAD_FILE			12020

#define FG_KNEE_LUT						12100
#define FG_KNEE_LUT_FILE				12110
#define FG_KNEE_LUT_SAVE_LOAD_FILE		12120
#define FG_KNEE_LUT_MODE				12130
#define FG_KNEE_LUT_ACCESS				12140

#define FG_KNEE_LUT_SCALE				12101
#define FG_KNEE_LUT_OFFSET				12102
#define FG_KNEE_LUT_GAMMA				12103
#define FG_KNEE_LUT_INVERT				12104

#define FG_MEDIAN						12200

#define FG_2DSHADINGPARAMETER			12500

#define FG_SCALINGFACTOR_RED			13000
#define FG_SCALINGFACTOR_BLUE			13010
#define FG_BAYERINIT					13020
#define FG_SCALINGFACTOR_GREEN			13030

#define FG_CCSEL						14000
#define FG_CCSEL0						14001
#define FG_CCSEL1						14002
#define FG_CCSEL2						14003
#define FG_CCSEL3						14004

#define FG_CCSEL_INVERT					14005
#define FG_CCSEL_INVERT0			    14006
#define FG_CCSEL_INVERT1				14007
#define FG_CCSEL_INVERT2				14008
#define FG_CCSEL_INVERT3				14009

#define FG_DIGIO_INPUT					14010
#define FG_DIGIO_OUTPUT					14020

#define FG_IMAGE_TAG					22000
#define FG_TIMESTAMP					22020
#define FG_TIMESTAMP_LONG				22030

#define FG_LICENSESTRING0				23000
#define FG_LICENSESTRING1				23010
#define FG_LICENSESTRING2				23020

#define FG_ACCESS_POINTER				23030

#define FG_ROIX							23100
#define FG_ROIY							23110
#define FG_SHADING_SUBIMAGE				23120
#define FG_SHADING_MULTENABLE			23130
#define FG_SHADING_OFFSETENABLE			23140
#define FG_SHADING_SUBENABLE			FG_SHADING_OFFSETENABLE

#define FG_SHADING_MAX_MULT				23135

#define FG_SHADING_RUNSUBIMAGE0			23121
#define FG_SHADING_RUNSUBIMAGE1			23122
#define FG_SHADING_RUNSUBIMAGE2			23123
#define FG_SHADING_RUNSUBIMAGE3			23124

#define FG_SHADING_ENABLEMULT0			23131
#define FG_SHADING_ENABLEMULT1			23132
#define FG_SHADING_ENABLEMULT2			23133
#define FG_SHADING_ENABLEMULT3			23134

#define FG_SHADING_ENABLESUB0			23141
#define FG_SHADING_ENABLESUB1			23142
#define FG_SHADING_ENABLESUB2			23143
#define FG_SHADING_ENABLESUB3			23144

#define FG_SHADING_FPNENABLE			23150
#define FG_SHADING_ENABLEFPN0			23151
#define FG_SHADING_ENABLEFPN1			23152
#define FG_SHADING_ENABLEFPN2			23153
#define FG_SHADING_ENABLEFPN3			23154


#define FG_SHADING_THRESHOLD0			23156
#define FG_SHADING_THRESHOLD1			23157
#define FG_SHADING_THRESHOLD2			23158
#define FG_SHADING_THRESHOLD3			23159

#define FG_SHADING_MULTFILE0			23160
#define FG_SHADING_SUBFILE0				23170
#define FG_SHADING_FPNFILE0				23180
#define FG_SHADING_MULTFILE1			23210
#define FG_SHADING_SUBFILE1				23225
#define FG_SHADING_FPNFILE1				23230
#define FG_SHADING_MULTFILE2			23240
#define FG_SHADING_SUBFILE2				23250
#define FG_SHADING_FPNFILE2				23260
#define FG_SHADING_MULTFILE3			23270
#define FG_SHADING_SUBFILE3				23280
#define FG_SHADING_FPNFILE3				23290

#define FG_CONTRAST						23200
#define FG_BRIGHTNESS					23220

#define FG_DOWNSCALE					24040
#define FG_LINE_DOWNSCALE				FG_DOWNSCALE
#define FG_LINE_DOWNSCALEINIT			24050
#define FG_FLASH_POLARITY				24060
#define FG_FLASHDELAY					FG_STROBEPULSEDELAY

#define FG_LOAD_SHADINGDATA				24070
#define FG_CLEAR_SHADINGDATA			24080


#define FG_LINESHADINGPARAMETER			24081
#define FG_1DSHADINGPARAMETER			FG_LINESHADINGPARAMETER

#define FG_LINESHADING_SUB_ENABLE		24082
#define FG_LINESHADING_MULT_ENABLE		24083
#define FG_ENABLEDISABLE_SHADING		FG_LINESHADING_MULT_ENABLE
#define FG_SHADING_WIDTH					24089
#define FG_AUTO_SHADING_WIDTH			24090
#define	FG_WRITE_SHADING_12				24091

#define FG_LINESHADING_MULT_FILENAME	24084
#define FG_LINESHADING_SUB_FILENAME		24085
#define FG_LINESHADING_LOAD_FROM_FILE	24086
#define FG_LINESHADING_MODE				24087

#define FG_DMASTATUS					24092
#define FG_LINEVALID_SIGNAL_COUNT		24093
#define FG_FRAMEVALID_SIGNAL_COUNT		24094

#define FG_1DSHADING_FILE				FG_LINESHADING_MULT_FILENAME
#define FG_LOAD_1DSHADINGDATA			FG_LINESHADING_LOAD_FROM_FILE

#define FG_BURSTLENGTH					24097
#define FG_SUPERFRAME					24098

#define FG_PLX_CLK						24102
#define FG_MEASURED_PCIE_CLK			24103
#define FG_FPGA_CLK						24104
#define FG_HAP_FILE						24108

#define FG_GLOBAL_ACCESS				24110
#define FG_DOC_URL						24112
#define FG_PARAM_DESCR					24114
#define FG_REG_VALUE_STRING				24115

#define	FG_CAMPORT_CONFIG				30000
#define	FG_CAMERA_TYPE					30001
#define	FG_COLOR_FLAVOUR				30002

#define	FG_GEN_ENABLE					30099
#define	FG_GEN_PASSIVE					30100
#define	FG_GEN_ACTIVE					30101
#define	FG_GEN_WIDTH					30102
#define	FG_GEN_LINE_WIDTH				30103
#define	FG_GEN_HEIGHT					30104

#define	FG_GEN_LINE_GAP					30105
#define	FG_GEN_FREQ						30106
#define	FG_GEN_ACCURACY					30107

#define	FG_GEN_TAP1						30108
#define	FG_GEN_TAP2						30109
#define	FG_GEN_TAP3						30110
#define	FG_GEN_TAP4						30111
#define	FG_GEN_ROLL						30112


/* defines from 200000 to 210000 are reserved for customer projects */


#define FG_APPLET_ID			24010
#define FG_APPLET_VERSION		24020
#define FG_APPLET_REVISION		24030

#define FG_DESIGNCLK			24040
#define FG_ALL					24050

#define FG_THRESHOLD_H_MIN		25000
#define FG_THRESHOLD_H_MAX		25010

#define FG_THRESHOLD_S_MIN		25020
#define FG_THRESHOLD_S_MAX		25030

#define FG_THRESHOLD_I_MIN		25040
#define FG_THRESHOLD_I_MAX		25050

#define FG_DO_THRESHOLD_S		25060
#define FG_DO_THRESHOLD_I		25070

#define FG_SHADING_H			25080
#define FG_SHADING_S			25090
#define FG_SHADING_I			25100

#define FG_FASTCONFIG_SEQUENCE	30010
#define FG_FASTCONFIG_PAGECMD	30020
#define FG_FASTCONFIG_PAGECMD_PTR	30030
#define FG_FASTCONFIG_PULSEDIGIO 30040

#define FG_IMG_SELECT_PERIOD	25110
#define FG_IMG_SELECT			25111

#define FG_NROFEXTERN_TRIGGER		30110
#define FG_ACTIVATE_EXTERN_TRIGGER	30120
#define FG_READ_EXTERN_TRIGGER		30130

#define FG_NB_QUAD_IMG				30300
#define FG_NB_STD_IMG				30310

#define	FG_GEN_ENABLE				30099
#define	FG_GEN_PASSIVE				30100
#define	FG_GEN_ACTIVE				30101
#define	FG_GEN_WIDTH				30102
#define	FG_GEN_LINE_WIDTH			30103
#define	FG_GEN_HEIGHT				30104
#define	FG_GEN_START				30113

#define	FG_GEN_LINE_GAP				30105
#define	FG_GEN_FREQ					30106
#define	FG_GEN_ACCURACY				30107

#define	FG_GEN_TAP1					30108
#define	FG_GEN_TAP2					30109
#define	FG_GEN_TAP3					30110
#define	FG_GEN_TAP4					30111
#define	FG_GEN_ROLL					30112

#define FG_BOARD_INFORMATION		42042

#define FG_LOGGING					43010
#define FG_LOG_FLUSH				43020
#define FG_LOG_CONSOLE				43030
#define FG_CREATE_DUMP				43040

/**
 * \brief definitions of available information related to the grabber-board itself
 */
enum BOARD_INFORMATION_SELECTOR {
	BINFO_BOARDTYPE = 0,    /**< get type of installed board (see corresponding defines in sisoboards.h) */
	BINFO_POCL = 1,         /**< get information about board support PoCL (Power over CameraLink) */
	BINFO_PCIE_PAYLOAD = 2  /**< get information about current BIOS setting for PCIe payload size configured for current design */
};


/**
 * \brief definitions of available system informations
 */
enum Fg_Info_Selector {
	INFO_APPLET_CAPABILITY_TAGS = 1,                     /**< list of key value pairs describing the features supported by the applet */
	INFO_TIMESTAMP_FREQUENCY = 100,                      /**< frequency of the timestamp counter in counts per second */
	INFO_NR_OF_BOARDS = 1000,                            /**< get the number of frame grabbers in the system */
	INFO_BOARDNAME = 1010,                               /**< get the typename of the according board identified by param1*/
	INFO_BOARDTYPE = 1011,                               /**< get the type of the according board identified by param1*/
	INFO_BOARDSERIALNO = 1012,                           /**< get the serialNo. of the according board identified by param1*/
	INFO_FIRMWAREVERSION = 1015,                         /**< get the firmware version of the according board identified by param1 as string*/
	INFO_BOARDSTATUS = 1020,                             /**< get the board status */
	INFO_DRIVERVERSION = 1100,                           /**< get the driver version used for the according board identified by param1 as string*/
	INFO_DRIVERARCH = 1101,                              /**< get the driver architecture used for the according board identified by param1 as string*/
	INFO_DRIVERFULLVERSION = 1102,                       /**< get the driver version and architecture used for the according board identified by param1 as string*/
	INFO_STATUS_PCI_TRAINING_STATUS = 2001,              /**< get the currently speed of the PCIe connection, possible values: 0,1,4,8 */
	INFO_STATUS_PCI_PAYLOAD_MODE = 2002,                 /**< get information about current BIOS setting for PCIe payload size configured for current design. possible values: 0: payload 128, 1 Payload 256 (availablity: firmware dependend)*/
	INFO_STATUS_PCI_NROF_INVALID_8B10B_CHARS = 2101,     /**< get the number of invalid 8b10b character PCI errors (availability: firmware dependent)*/
	INFO_STATUS_PCI_NROF_8B10B_DISPARITY_ERRORS = 2102,  /**< get the number of 8b10b disparity PCI errors (availability:  firmware dependent)*/

};


/**
 * \brief definitions for querying information about a certain field
 */
enum FgProperty{
	PROP_ID_VALUE = 0,         /**< request the current value of a parameter, information,.. */
	PROP_ID_DATATYPE = 1,      /**< request the data type of a parameter, information, */
	PROP_ID_NAME = 2,          /**< request the symbolic name of a parameter, information, */
	PROP_ID_PARAMETERNAME = 3, /**< request the Name for Software access */
	PROP_ID_VALUELLEN = 4,     /**< request the current length of data */
	PROP_ID_ACCESS_ID = 5,     /**< request the parameter id for access parameter*/
	PROP_ID_MIN_ID = 6,        /**< request the parameter id for min parameter*/
	PROP_ID_MAX_ID = 7         /**< request the parameter id for max parameter*/
};


#define FG_CABLE_SELECT		1001010
#define FG_IMAGE_ENABLE		1001020
#define FG_STAT_ENABLE		1001030
#define FG_MIN_DX			1001040
#define FG_THR1				1001050
#define FG_THR2				1001060
#define FG_MEDIAN_ON		1001070
#define FG_DMA_WRITE		1001080
#define FG_FAST_CONFIG		1001090
#define FG_SYNC				1001100
#define FG_NODMA1IR			1001110


/* LUT defines */
#define LUT_RED			0
#define LUT_GREEN 		1
#define LUT_BLUE		2
#define LUT_GRAY 		3

/* CAM-Port constants */
#define PORT_A		0
#define PORT_B		1
#define PORT_C		2
#define PORT_D		3
#define PORT_AB		4

/* RGB Colour IDs */
#define FG_RED		0
#define FG_GREEN	1
#define FG_BLUE		2

/* Trigger*/
#define TRGINSOFTWARE -1
#define TRGINSRC_0	0
#define TRGINSRC_1	1
#define TRGINSRC_2	2
#define TRGINSRC_3	3
#define TRGINSRC_4	4
#define TRGINSRC_5	5
#define TRGINSRC_6	6
#define TRGINSRC_7	7

#define GRAB_INFINITE ((frameindex_t) -1)
#define GRAB_ALL_BUFFERS ((frameindex_t) -2)

/**
  * \brief Flags for controlling the behavior of Fg_StopAcquireEx() function
  */
enum FgStopAcquireFlags {
	STOP_ASYNC = 0x00,      /**< Stops the images acquisition immediately without any synchronization to image data flow*/
	STOP_SYNC_TO_APC = 0x04,/**< Synchronizes the Fg_StopAcquireEx() with the pending APC callbacks by using the Timeout defined by FG_APC_STOP_TIMEOUT */
	STOP_SYNC = 0x80000000  /**< Reserved for stopping acquisition synchronously to image data flow*/
};

/* Status codes */
#define		FG_OK 							0
#define		FG_INIT_OK						1
#define		FG_SOFTWARE_TRIGGER_PENDING		8803

/* Error codes */
#define		FG_ERROR 							-1
#define		FG_DUMMY_BUFFER						-1
#define		FG_NO_PICTURE_AVAILABLE 			-2
#define		FG_SISODIR5_NOT_SET					-5

#define		FG_ALR_INIT							-10
#define		FG_NOT_AVAILABLE					-12
#define		FG_NO_BOARD_AVAILABLE				-20
#define		FG_INVALID_BOARD_NUMBER				-21
#define		FG_BOARD_INIT_FAILED				-22
#define		FG_INVALID_CLOCK					-23
#define		FG_INVALID_DESIGN_NAME				-26
#define		FG_SYSTEM_LOCKED					-27
#define		FG_RESSOURCES_STILL_IN_USE			-28
#define		FG_CLOCK_INIT_FAILED				-29
#define		FG_WRONG_ARCHITECTURE 				-50
#define		FG_SOFTWARE_TRIGGER_BUSY			-60
#define		FG_INVALID_PORT_NUMBER 				-61
#define		FG_HAP_FILE_NOT_LOAD				-100

#define		FG_MICROENABLE_NOT_INIT				-110
#define		FG_DLL_NOT_LOAD						-120
#define		FG_REG_KEY_NOT_FOUND				-121
#define		FG_VASDLL_NOT_LOAD					-122
#define		FG_ERROR_LOADING_MODULE -123
#define		FG_UNEXPECTED_HAPLOAD				-130
#define		FG_SIZE_ERROR						-200
#define		FG_PTR_INVALID						-300
#define		FG_RANGE_ERR						-400
#define		FG_OVERFLOW_ERR						-401
#define		FG_NOT_ENOUGH_MEM					-500
#define		FG_DMATRANSFER_INVALID				-600
#define		FG_HAP_FILE_DONT_MATCH				-700
#define		FG_VERSION_MISMATCH					-701
#define     FG_ACCESS_DENIED					-702

#define		FG_NOT_INIT							-2001
#define		FG_WRONG_SIZE						-2002
#define		FG_WRONG_NUMBER_OF_BUFFER			-2010
#define		FG_TOO_MANY_BUFFER					-2011
#define		FG_NOT_ENOUGH_MEMORY				-2020
#define		FG_MEMORY_ALREADY_ALLOCATED			-2024
#define		FG_CANNOT_WRITE_MEM_CONFIG_FAILED	-2026
#define		FG_INTERNAL_STATUS_ERROR			-2030
#define		FG_INTERNAL_ERROR					-2031
#define		FG_CANNOT_START						-2040
#define		FG_CANNOT_STOP						-2042
#define		FG_SYNC_ACQUIRE_NOT_SUPPORTED		-2045
#define		FG_INVALID_DESIGN					-2050
#define		FG_CONFIGURE_FAILED					-2052
#define		FG_RECONFIGURE_FAILED				-2053
#define		FG_NO_APPLET_ID						-2055

#define		FG_INVALID_MEMORY -2060
#define		FG_INVALID_PARAMETER -2070      /**< the applet-parameter is not valid or a function call contains invalid data / arguments*/ 
#define		FG_ILLEGAL_WHILE_APC -2071      /**< there are no event types available at the grabber / applet*/
#define		FG_APC_PRIORITY_ERROR -2072		  /**< FG_APC_HIGH_PRIORITY was requested but setting the priority failed */
#define		FG_APC_ALREADY_REGISTERED -2073	/**< a APC callback handler is already registered*/

#define		FG_INVALID_VALUE					-2075
#define		FG_INVALID_FILENAME					-2076
#define		FG_INVALID_REGISTER					-7040
#define		FG_INVALID_MODULO					-7080
#define		FG_INVALID_CONFIGFILE				-5000
#define		FG_INVALID_CONFIGFILEEXT			FG_INVALID_CONFIGFILE
#define		FG_FILE_ACCESS_DENIED				-5001

#define		FG_ERROR_FREQUENCY_TOO_HIGH_FOR_PFG	-2077
#define		FG_ERROR_VALUE_TOO_LOW_FOR_FPS_OR_WIDTH_OR_DELAY -2078
#define		FG_ERROR_VALUE_TOO_HIGH_FOR_FPS_OR_WIDTH_OR_DELAY -2079

#define		FG_NOT_LOAD -2080
#define		FG_ALREADY_STARTED -2090
#define		FG_OPERATION_ABORTED -2091
#define		FG_STILL_ACTIVE -2100
#define		FG_NO_VALID_DESIGN -2110
#define		FG_TIMEOUT_ERR -2120
#define		FG_NOT_IMPLEMENTED -2130
#define		FG_WRONG_TRIGGER_MODE -2140
#define		FG_NOT_WRONG_TRIGGER_MODE FG_WRONG_TRIGGER_MODE
#define		FG_WRONG_TRIGGER_STATE -2141
#define		FG_ALL_BUFFER_BLOCKED -2150

#define		FG_NO_EVENTS_FOUND -2160 /**< there are no event types available at the grabber / applet*/
#define		FG_CANNOT_COMBINE_DATA_EVENTS -2161 /**< different event types, which transport additional information cannot be combined to one event mask*/
#define		FG_INVALID_EVENTMASK -2162 /**< event mask does not match to available event types*/

#define		FG_CANNOT_INIT_MICROENABLE			-3000
#define		FG_TRANSFER_NOT_ACTIVE				-3010
#define		FG_CLOCK_NOT_LOCKED					-3120
#define		FG_STILL_NOT_STARTED				-4000
#define		FG_VALUE_OUT_OF_RANGE				-6000
#define		FG_CANNOT_CHANGE_DISPLAY_WIDTH		-7000
#define		FG_CANNOT_CHANGE_DISPLAY_HEIGHT		-7005
#define		FG_CANNOT_CHANGE_DISPLAY_SIZE		-7010
#define		FG_NO_VALID_LICENSE					-7020
#define		FG_CANNOT_CHANGE_CAMERA_FORMAT		-7030
#define		FG_REGISTER_INIT_FAILED				-7050
#define		FG_INVALID_SHADING_CORRECTION_FILE	-7060
#define		FG_WRITE_LINE_SHADING_TIMEOUT		-7070
#define		FG_INVALID_IMAGE_DIMENSIONS			-7071
#define		FG_CANNOT_CHANGE_DURING_ACQU		-7090
#define		FG_TOKEN_NOT_FOUND_ERROR			-8000
#define		FG_WRITE_ACCESS_DENIED				-8010
#define		FG_REGISTER_UPDATE_FAILED			-8020
#define		FG_DEVICE_IO_ERROR					-9000
#define		FG_INVALID_CONFIG_REGION			-9001


/* applets*/
#define SINGLE_AREA_GRAY                        0x10
#define SINGLE_AREA_2DSHADING           		0x11
#define DUAL_AREA_GRAY                          0x20
#define SINGLE_AREA_BAYER                       0x30
#define DUAL_AREA_BAYER                         0x31
#define SINGLE_AREA_GRAY_SHADING        		0x40
#define SDRAM_ACCESS                            0x41
#define SINGLE_LINE_GRAY                        0x50
#define SINGLE_LINE_RGB                         0x60
#define DUAL_LINE_RGB                           0x61
#define DUAL_LINE_RGB_SHADING           		0x62
#define DUAL_LINE_GRAY                          0x70
#define VISIGLAS                                0x80
#define TRUMPFINESS                             0x81
#define SOUDRONIC                               0x82
#define SINGLEHIGHPRECISION                     0x83
#define SINGLE_AREA_GRAY_OFFSET         		0x84
#define SINGLE_AREA_HSI                         0x90
#define SINGLE_AREA_RGB                         0xa0
#define DUAL_AREA_RGB                           0xb0
#define SINGLE_AREA_RGB_SEPARATION      		0xb1
#define MEDIUM_LINE_RGB                         0xb2
#define MEDIUM_LINE_GRAY                        0xb3
#define SINGLE_FAST_CONFIG                      0xb5
#define FASTCONFIG_SINGLE_AREA_GRAY SINGLE_FAST_CONFIG

#define SINGLE_AREA_GRAY_XXL            		0x110
#define SINGLE_AREA_2DSHADING_XXL       		0x111
#define DUAL_AREA_GRAY_XXL                      0x120
#define SINGLE_AREA_BAYER_XXL           		0x130
#define DUAL_AREA_BAYER_XXL                     0x131
#define SINGLE_AREA_GRAY_SHADING_XXL 			0x140
#define SDRAM_ACCESS_XXL                        0x141
#define SINGLE_LINE_GRAY_XXL            		0x150
#define SINGLE_LINE_RGB_XXL                     0x160
#define DUAL_LINE_RGB_XXL                       0x161
#define DUAL_LINE_RGB_SHADING_XXL       		0x162
#define DUAL_LINE_GRAY_XXL                      0x170
#define SINGLE_AREA_HSI_XXL                     0x190
#define SINGLE_AREA_RGB_XXL                     0x1a0
#define DUAL_AREA_RGB_XXL                       0x1b0
#define SINGLE_AREA_RGB_SEPARATION_XXL  		0x1b1
#define MEDIUM_LINE_RGB_XXL                     0x1b2
#define MEDIUM_LINE_GRAY_XXL            		0x1b3
#define MEDIUM_AREA_GRAY_XXL            		0x1b4
#define MEDIUM_AREA_RGB_XXL                     0x1b5
#define SINGLE_AREA_BAYER12_XXL         		0x1c0
#define DUAL_AREA_GRAY12_XXL            		0x1d0
#define SINGLE_LINE_GRAY12_XXL          		0x1d1
#define DUAL_AREA_RGB36_XXL                     0x1d2
#define DUAL_LINE_GRAY12_XXL            		0x1d3
#define MEDIUM_LINE_GRAY12_XXL          		0x1d4
#define SINGLE_AREA_GRAY12_XXL          		0x1d5
#define DUAL_LINE_RGB36_XXL                     0x1d6
#define SINGLE_AREA_RGB36_XXL           		0x1d7
#define SINGLE_LINE_RGB36_XXL           		0x1d8
#define DUAL_AREA_BAYER12_XXL           		0x1d9
#define SINGLE_AREA_2DSHADING12_XXL     		0x1da
#define SINGLE_LINE_RGB24_XXL           		0x1db

#define LSC1020XXL                              0x500
#define LSC1020JPGXXL                           0x501
#define CLSC2050                                0x502
#define CLSC2050JPGXXL                          0x503
#define SEQUENCE_EXTRACTOR                      0x510
#define SAG_COMPRESSION                         0x520
#define MEDIUM_LINE_GRAY_FIR_XXL        		0x530
#define DUAL_LINE_RGB_SORTING_XXL       		0x540
#define SINGLE_LINE_GRAY_2X12_XXL       		0x550
#define MEDIUM_LINE_GRAY12                      0x560
#define SINGLE_LINE_RGB36PIPELINE2_XXL          0x570
#define DUAL_AREA_GRAY_16                       0x580

#define DUAL_AREA_GRAY16_ME4BASEX1				0xa400010
#define DUAL_AREA_RGB48_ME4BASEX1				0xa400020
#define DUAL_LINE_GRAY16_ME4BASEX1				0xa400030
#define DUAL_LINE_RGB48_ME4BASEX1				0xa400040
#define MEDIUM_AREA_GRAY16_ME4BASEX1			0xa400050
#define MEDIUM_AREA_RGB36_ME4BASEX1				0xa400060
#define MEDIUM_LINE_GRAY16_ME4BASEX1			0xa400070
#define MEDIUM_LINE_RGB36_ME4BASEX1				0xa400080

#define DUAL_AREA_BAYER12_ME4FULLX1				0xa410010
#define DUAL_AREA_GRAY16_ME4FULLX1				0xa410020
#define DUAL_AREA_RGB48_ME4FULLX1				0xa410030
#define DUAL_LINE_GRAY16_ME4FULLX1				0xa410040
#define DUAL_LINE_RGB30_ME4FULLX1				0xa410050
#define FULL_AREA_GRAY8_ME4FULLX1				0xa410060
#define FULL_LINE_GRAY8_ME4FULLX1				0xa410070
#define MEDIUM_AREA_GRAY16_ME4FULLX1			0xa410080
#define MEDIUM_AREA_RGB36_ME4FULLX1				0xa410090
#define MEDIUM_LINE_GRAY16_ME4FULLX1			0xa4100a0
#define MEDIUM_LINE_RGB36_ME4FULLX1				0xa4100b0
#define SINGLE_AREA_BAYERHQ_ME4FULLX1			0xa4100c0
#define SINGLE_AREA_GRAY2DSHADING_ME4FULLX1		0xa4100d0

#define DUAL_AREA_BAYER12_ME4FULLX4				0xa440010
#define DUAL_AREA_GRAY16_ME4FULLX4				0xa440020
#define DUAL_AREA_RGB48_ME4FULLX4				0xa440030
#define DUAL_LINE_GRAY16_ME4FULLX4				0xa440040
#define DUAL_LINE_RGB30_ME4FULLX4				0xa440050
#define FULL_AREA_GRAY8_ME4FULLX4				0xa440060
#define FULL_LINE_GRAY8_ME4FULLX4				0xa440070
#define MEDIUM_AREA_GRAY16_ME4FULLX4			0xa440080
#define MEDIUM_AREA_RGB36_ME4FULLX4				0xa440090
#define MEDIUM_LINE_GRAY16_ME4FULLX4			0xa4400a0
#define MEDIUM_LINE_RGB36_ME4FULLX4				0xa4400b0
#define SINGLE_AREA_BAYERHQ_ME4FULLX4			0xa4400c0
#define SINGLE_AREA_GRAY2DSHADING_ME4FULLX4		0xa4400d0

#define QUAD_AREA_BAYER24_ME4GBEX4				0xe440010
#define QUAD_AREA_GRAY16_ME4GBEX4				0xe440020
#define	QUAD_AREA_RG24_ME4GBEX4					0xe440030
#define QUAD_AREA_RGB48_ME4GBEX4				0xe440040
#define QUAD_AREA_GRAY8_ME4GBEX4				0xe440050
#define QUAD_LINE_GRAY16_ME4GBEX4				0xe440060
#define QUAD_LINE_RGB24_ME4GBEX4				0xe440070
#define QUAD_LINE_GRAY8_ME4GBEX4				0xe440080

#define FULL_AREA_GRAY8_HS_ME4VD4	((PN_MICROENABLE4VD4CL << 16) + 0x00e0)
#define FULL_AREA_GRAY8_HS_ME4AD4	((PN_MICROENABLE4AD4CL << 16) + 0x00e0)

/**
 * \struct fg_apc_data
 * placeholder for user defined APC function argument.
 *
 * This struct is not defined anywhere within Silicon Software code, it is only passed
 * to the apcfunc to give it the working context. Just define the struct in your software
 * with the values you need.
 *
 * \see Fg_registerApcHandler()
 */
struct fg_apc_data;

/**
 * interface of user defined APC handler
 *
 * \see Fg_registerApcHandler()
 */
typedef int (*Fg_ApcFunc_t)(frameindex_t imgNr, struct fg_apc_data *data);

/**
 * \brief controls the behaviour of the APC thread
 */
enum Fg_Apc_Flag {
	FG_APC_DEFAULTS = 0,			/**< use APC default mode (handler is called for every transfer, grabbing is stopped on timeout) */
	FG_APC_BATCH_FRAMES = 0x1,		/**< when multiple images arrived only call APC handler once with the highest image number */
	FG_APC_IGNORE_TIMEOUTS = 0x2,		/**< ignore image timeouts */
	FG_APC_IGNORE_APCFUNC_RETURN = 0x4,	/**< do not stop acquisition if apcfunc returns != 0 */
	FG_APC_IGNORE_STOP = 0x8,		/**< do not stop the APC handler if acquisition stops */
	FG_APC_HIGH_PRIORITY = 0x10		/**< increase the priority of the APC thread */
};

/**
 * \brief control flags for Fg_registerApcHandler()
 */
enum FgApcControlFlags {
	FG_APC_CONTROL_BASIC = 0		/**< default behaviour */
};

/**
 * \struct fg_event_data
 * placeholder for user defined event function argument to be passed to the event callback.
 *
 * This struct is not defined anywhere within Silicon Software code, it is only passed
 * to the event handler function to give it some working context. Just define the struct
 * in your software with the values you need.
 *
 * \see Fg_registerEventHandler()
 */
struct fg_event_data;

struct fg_event_info;
/**
 * interface of user defined event handler
 *
 * If this function returns a value != 0 the event handler for the given event
 * is automatically unregistered.
 *
 * \see Fg_registerEventCallback()
 *
 * \param events mask of events that were received
 * \param data context data passed to Fg_registerEventCallback() which will be
 * passed to the event-handling callback function
 * \param info the info data passed to Fg_registerEventCallback()
 *   filled with information for the returned events
 *
 * You may not change the contents of info from within the handler.
 * You must consider the data within that struct as volatile, i.e.
 * it is only guaranteed to be unchanged until your handler function
 * returns.
 */
typedef int (*Fg_EventFunc_t)(uint64_t events, struct fg_event_data *data, const struct fg_event_info *info);

/**
 * \brief controls the behaviour of Fg_waitEvent()
 */
enum FgEventControlFlags {
	FG_EVENT_DEFAULT_FLAGS = 0,	///< default behaviour (i.e. none of the other flags)
	FG_EVENT_BATCHED = 0x1		///< if more than one event is recorded only return once
};

/**
 * \brief the status flags passed in notify member of struct fg_event_info
 */
enum FgEventNotifiers {
	FG_EVENT_NOTIFY_JOINED = 0x1,     /**< events were joined in mode FG_EVENT_BATCHED */
	FG_EVENT_NOTIFY_TIMESTAMP = 0x2,  /**< timestamp contains valid data */
	FG_EVENT_NOTIFY_PAYLOAD = 0x4,    /**< the event has payload, use the payload member of the union */
	FG_EVENT_NOTIFY_LOST = 0x8        /**< there was data lost before this event */
};

/**
 * \brief	initialize a struct fg_event_info
 *
 * Never write any values to struct fg_event_info directly,
 * always use this macro to reset this to a clean state.
 * This will make sure that you get the proper behaviour even
 * if a newer runtime environment will extend the struct.
 */
#define FG_EVENT_INFO_INIT(x) \
		do { \
			unsigned int i; \
			struct fg_event_info *_x = (x); \
			_x->version = 2; \
			_x->_pad = 0; \
			for (i = 0; i<  64; i++) { \
				_x->notify[i] = 0; \
				_x->timestamp[i] = 0; \
			} \
			_x->length = 0; \
			for (i = 0; i<  254; i++) { \
				_x->data[i] = 0; \
			} \
		} while (0)

/* ACQ APPLETS --- PARAMETER IDS */

#define FG_FILLLEVEL 110086
#define FG_OVERFLOW 110087
#define FG_NOISEFILTER 110016
#define FG_LUT_TYPE 110017
#define FG_LUT_CUSTOM_FILE 300000
#define FG_LUT_SAVE_FILE 110021
#define FG_PROCESSING_GAIN 300002
#define FG_PROCESSING_GAMMA 300003
#define FG_PROCESSING_OFFSET 300004
#define FG_PROCESSING_INVERT 300005
#define FG_LUT_IMPLEMENTATION_TYPE 300006
#define FG_SHADING_GAIN_ENABLE 300100
#define FG_SHADING_GRAY_FILENAME 300101
#define FG_SHADING_OFFSET_ENABLE 300102
#define FG_SHADING_BLACK_FILENAME 300103
#define FG_SHADING_GAIN_CORRECTION_MODE 300106
#define FG_SHADING_APPLY_SETTINGS 300107
#define FG_AREATRIGGERMODE 300200
#define FG_TRIGGERSTATE 300201
#define FG_TRIGGER_FRAMESPERSECOND FG_FRAMESPERSEC
#define FG_TRIGGER_EXCEEDED_PERIOD_LIMITS 300202
#define FG_TRIGGER_EXCEEDED_PERIOD_LIMITS_CLEAR 300203
#define FG_TRIGGERIN_DEBOUNCE 300204
#define FG_TRIGGERIN_SRC FG_TRIGGERINSRC
#define FG_TRIGGERIN_POLARITY FG_TRIGGERINPOLARITY
#define FG_SOFTWARETRIGGER_IS_BUSY 110075
#define FG_TRIGGERIN_DOWNSCALE 300205
#define FG_TRIGGERIN_DOWNSCALE_PHASE 300206
#define FG_TRIGGERIN_STATS_PULSECOUNT 300207
#define FG_TRIGGERIN_STATS_PULSECOUNT_CLEAR 300208
#define FG_TRIGGERIN_STATS_FREQUENCY 300209
#define FG_TRIGGERIN_STATS_MINFREQUENCY 300210
#define FG_TRIGGERIN_STATS_MAXFREQUENCY 300211
#define FG_TRIGGERIN_STATS_MINMAXFREQUENCY_CLEAR 300212
#define FG_TRIGGER_MULTIPLY_PULSES 300213
#define FG_TRIGGERQUEUE_MODE 300214
#define FG_TRIGGERQUEUE_FILLLEVEL 300215
#define FG_TRIGGER_PULSEFORMGEN0_DOWNSCALE 300216
#define FG_TRIGGER_PULSEFORMGEN0_DOWNSCALE_PHASE 300217
#define FG_TRIGGER_PULSEFORMGEN0_DELAY 300218
#define FG_TRIGGER_PULSEFORMGEN0_WIDTH 300219
#define FG_TRIGGER_PULSEFORMGEN1_DOWNSCALE 300220
#define FG_TRIGGER_PULSEFORMGEN1_DOWNSCALE_PHASE 300221
#define FG_TRIGGER_PULSEFORMGEN1_DELAY 300222
#define FG_TRIGGER_PULSEFORMGEN1_WIDTH 300223
#define FG_TRIGGER_PULSEFORMGEN2_DOWNSCALE 300224
#define FG_TRIGGER_PULSEFORMGEN2_DOWNSCALE_PHASE 300225
#define FG_TRIGGER_PULSEFORMGEN2_DELAY 300226
#define FG_TRIGGER_PULSEFORMGEN2_WIDTH 300227
#define FG_TRIGGER_PULSEFORMGEN3_DOWNSCALE 300228
#define FG_TRIGGER_PULSEFORMGEN3_DOWNSCALE_PHASE 300229
#define FG_TRIGGER_PULSEFORMGEN3_DELAY 300230
#define FG_TRIGGER_PULSEFORMGEN3_WIDTH 300231
#define FG_TRIGGEROUT_SELECT0 300232
#define FG_TRIGGEROUT_SELECT1 300233
#define FG_TRIGGEROUT_SELECT2 300234
#define FG_TRIGGEROUT_SELECT3 300235
#define FG_TRIGGEROUT_STATS_SOURCE 300236
#define FG_TRIGGEROUT_STATS_PULSECOUNT 300237
#define FG_TRIGGEROUT_STATS_PULSECOUNT_CLEAR 300238
#define FG_TRIGGERIN_EVENT_SRC 300239
#define FG_TRIGGER_QUEUE_FILLLEVEL_EVENT_ON_THRESHOLD 300240
#define FG_TRIGGER_QUEUE_FILLLEVEL_EVENT_OFF_THRESHOLD 300241
#define FG_TRIGGER_OUTPUT_EVENT_SELECT 300242
#define FG_TRIGGERIN_BYPASS_SRC 300243
#define FG_TRIGGEROUT_SELECT4 300244
#define FG_TRIGGEROUT_SELECT5 300245
#define FG_TRIGGEROUT_SELECT6 300246
#define FG_TRIGGEROUT_SELECT7 300247
#define FG_LUT_BASEPOINTS 300012
#define FG_LUT_IN_BITS 300007
#define FG_LUT_OUT_BITS 300008
#define FG_LUT_VALUE 300001
#define FG_LUT_VALUE_RED 300009
#define FG_LUT_VALUE_GREEN 300010
#define FG_LUT_VALUE_BLUE 300011
#define FG_SHADING_DEAD_PIXEL_INTERPOLATION_ENABLE 300104
#define FG_SHADING_DEAD_PIXEL_INTERPOLATION_THRESHOLD 300105
#define FG_MISSING_CAMERA_FRAME_RESPONSE 300248
#define FG_MISSING_CAMERA_FRAME_RESPONSE_CLEAR 300249
#define FG_TRIGGERCC_SELECT0 300250
#define FG_TRIGGERCC_SELECT1 300251
#define FG_TRIGGERCC_SELECT2 300252
#define FG_TRIGGERCC_SELECT3 300253
#define FG_TRIGGER_LEGACY_MODE 300254

/* ACQ APPLETS --- SYMBOLIC ENUMERATION VALUES */

#define LUT_TYPE_PROCESSING 3
#define LUT_TYPE_CUSTOM 0
#define LUT_IMPLEMENTATION_FULL_LUT 0
#define LUT_IMPLEMENTATION_KNEELUT 1
#define FG_MAX_VALUE 0
#define FG_MEAN_VALUE 1
#define FG_MAX_RANGE 2
#define FG_APPLY 1
#define ATM_GENERATOR 1
#define ATM_EXTERNAL 2
#define ATM_SOFTWARE 4
#define TS_ACTIVE 0
#define TS_ASYNC_STOP 1
#define TS_SYNC_STOP 2
#define FG_ONE 1
#define FG_ZERO 0
#define IS_BUSY 1
#define IS_NOT_BUSY 0
#define CC_PULSEGEN0 0
#define CC_PULSEGEN1 1
#define CC_PULSEGEN2 2
#define CC_PULSEGEN3 3
#define CC_NOT_PULSEGEN0 6
#define CC_NOT_PULSEGEN1 7
#define CC_NOT_PULSEGEN2 8
#define CC_NOT_PULSEGEN3 9
#define CC_INPUT_BYPASS 10
#define CC_NOT_INPUT_BYPASS 11
#define PULSEGEN0 0
#define PULSEGEN1 1
#define PULSEGEN2 2
#define PULSEGEN3 3
#define GND 4
#define VCC 5
#define NOT_PULSEGEN0 6
#define NOT_PULSEGEN1 7
#define NOT_PULSEGEN2 8
#define NOT_PULSEGEN3 9
#define INPUT_BYPASS 10
#define NOT_INPUT_BYPASS 11
#define ATM_SYNCHRONIZED 3
#define FG_CL_FULL_8_TAP_8_BIT FG_CL_8BIT_FULL_8
#define FG_CL_FULL_10_TAP_8_BIT FG_CL_8BIT_FULL_10

/* 
* \brief	ordering of colors at bayer format
*/
enum BayerOrdering{
	GreenFollowedByRed = 3,
	GreenFollowedByBlue = 0,
	RedFollowedByGreen = 2,
	BlueFollowedByGreen = 1
};

#define HIGH_ACTIVE 0
#define LOW_ACTIVE 1

/* 
 * \brief	available format of pixels at GigE Vision
*/
enum GigEPixelFormat{
	MONO8 = 0,
	MONO8_SIGNED = 1,
	MONO10 = 2,
	MONO10_PACKED = 3,
	MONO12 = 4,
	MONO12_PACKED = 5,
	MONO16 = 6
};


#endif

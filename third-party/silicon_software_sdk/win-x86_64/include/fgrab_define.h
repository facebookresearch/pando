/**
 * fgrab_define.h
 *
 * Copyright (c) 2002-2017 Silicon Software GmbH, All Rights Reserved.
 *
 *
 * \file fgrab_define.h
 *
 * \brief Constant and enum definitions.
 *
 * \author Silicon Software GmbH
 */

#ifndef __FGRABDEFINE_H
#define __FGRABDEFINE_H

#pragma warning(push)
#pragma warning(disable: 4005)
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <msinttypes/stdint.h>
#else
#include <stdint.h>
#endif
#pragma warning(pop)

#include "sisoboards.h"
#include "os_type.h"

enum RowFilterModes {
	_NON_TRIGGERED_EOF_CONTROLLED        = 0,
	_NON_TRIGGERED_LINE_COUNT_CONTROLLED = 0x1,
	_TRIGGERED_EOF_CONTROLLED            = 0x2,
	_TRIGGERED_LINE_COUNT_CONTROLLED     = 0x3
};

enum TriggerMode    {
	_GRABBER_CONTROLLED_        = 0,
	_GRABBER_CONTROLLED_STROBE  = 1,
	_GRABBER_CONTROLLED_TRIGGER = 2,
	_SINGLE_SHOT                = 4,
	_EXTERNAL_TRIGGER           = 6 
};

enum LineTriggerMode  {
	_LRM_AUTO    = 0, /**< auto generated line trigger output (VINIT) */
	_LRM_EXT_TRG = 1  /**< external trigger */
};

enum LineTriggerGateMode {
	_LRM_NON_GATED = 0,     /**< no image trigger */
	_LRM_GATED_COUNT = 1,   /**< image trigger rising edge starts sampling */
	_LRM_GATED_PARTIAL = 2, /**< image sampling during gate active. if the gate
	                        signal exceeds AreaHeight number of lines, the
	                        transfer is completed by an EoT, but continues
	                        (Image Tag Bit 31 is 0 if the next frame belongs
	                        to the same image strip) */
	_LRM_GATED = 3          /**< image sampling during gate active */
};

enum TriggerSync {
	_LVAL   = 0,
	_HDSYNC = 1
};

enum MeTriggerMode{
	FREE_RUN                      = 0,
	GRABBER_CONTROLLED            = 1,
	ASYNC_TRIGGER                 = 2,
	GRABBER_CONTROLLED_SYNCHRON   = 3,
	ASYNC_SOFTWARE_TRIGGER        = 4,
	ASYNC_GATED                   = 5,
	ASYNC_GATED_MULTIFRAME        = 6,
	ASYNC_SOFTWARE_TRIGGER_QUEUED = 7
};

enum MeLineTriggerMode{
	GRABBER_CONTROLLED_GATED = 6
};

enum MeShaftMode {
	SOURCE_A = 0,
	SOURCE_B = 1
};
enum MeLineShadingMode{
	SHADING_OFF = 0,
	SHADING_SUB = 1,
	SHADING_MULT = 2,
	SHADING_SUB_MULT = 3
};

enum MeKneeLutMode{
	FG_INDEP = 0,
	FG_DEP = 1
};

enum MeAreaTriggerMode{
	AREA_FREE_RUN                     = 0,
	AREA_GRABBER_CONTROLLED           = 1,
	AREA_ASYNC_TRIGGER                = 2,
	AREA_GRABBER_CONTROLLED_SYNCHRON  = 3,
	AREA_ASYNC_SOFTWARE_TRIGGER       = 4
};

enum MeLineTriggerModeLine{
	LINE_FREE_RUN_LINE                 = 0,
	LINE_GRABBER_CONTROLLED_LINE       = 1,
	LINE_ASYNC_TRIGGER_LINE            = 2,
	LINE_ASYNC_GATED_LINE              = 5,
	LINE_GRABBER_CONTROLLED_GATED_LINE = 6
};


enum MeLineTriggerModeImage{
	LINE_FREE_RUN_IMAGE                 = 0,
	LINE_GRABBER_CONTROLLED_IMAGE       = 1,
	LINE_ASYNC_TRIGGER_IMAGE            = 2,
	LINE_GRABBER_CONTROLLED_GATED_IMAGE = 5,
	LINE_ASYNC_GATED_MULTIBUFFERS_IMAGE = 6
};


enum MeRgbComponentMapping {
	FG_MAP_PIXEL0 = 0,
	FG_MAP_PIXEL1 = 1,
	FG_MAP_PIXEL2 = 2,
	FG_MAP_PIXEL3 = 3
};

enum  MeCameraLinkFormat {
	FG_CL_SINGLETAP_8_BIT      = 8,
	FG_CL_SINGLETAP_10_BIT     = 10,
	FG_CL_SINGLETAP_12_BIT     = 12,
	FG_CL_SINGLETAP_14_BIT     = 14,
	FG_CL_SINGLETAP_16_BIT     = 16,
	FG_CL_DUALTAP_8_BIT        = 108,
	FG_CL_DUALTAP_10_BIT       = 110,
	FG_CL_DUALTAP_12_BIT       = 112,
	FG_CL_TRIPLETAP_8_BIT      = 120,
	FG_CL_LITE_8_BIT           = 130,
	FG_CL_LITE_10_BIT          = 140,
	FG_CL_RGB                  = 500,
	FG_CL_MEDIUM_8_BIT         = 208,
	FG_CL_MEDIUM_10_BIT        = 210,
	FG_CL_MEDIUM_12_BIT        = 212,
	FG_CL_MEDIUM_3_TAP_10_BIT  = 219,
	FG_CL_MEDIUM_3_TAP_12_BIT  = 220,
	FG_CL_MEDIUM_RGB_24        = 214,
	FG_CL_MEDIUM_RGB_30        = 216,
	FG_CL_MEDIUM_RGB_36        = 218,
	FG_CL_8BIT_FULL_8          = 308,
	FG_CL_8BIT_FULL_10         = 310,
	FG_CL_FULL_8_TAP_10_BIT    = 311,
	FG_CL_FULL_8_TAP_RGB_24    = 320,
	FG_CL_FULL_10_TAP_RGB_24   = 321,
	FG_CL_FULL_8_TAP_RGB_30    = 322
};

enum MeCameraTypes {
	FG_AREA_GRAY                  = 0,
	FG_AREA_BAYER                 = 1,
	FG_LINE_GRAY                  = 2,
	FG_SINGLE_LINE_RGB            = 3,
	FG_DUAL_LINE_RGB              = 4,
	FG_SINGLE_AREA_RGB            = 5,
	FG_DUAL_AREA_RGB              = 6,
	FG_AREA_HSI                   = 7,
	FG_DUAL_LINE_RGB_SHADING      = 8,
	FG_SINGLE_LINE_RGBHSI         = 9,
	FG_SINGLE_AREA_RGB_SEPARATION = 10,
	FG_MEDIUM_LINE_RGB            = 11,
	FG_MEDIUM_LINE_GRAY           = 12,
	FG_MEDIUM_AREA_GRAY           = 13,
	FG_MEDIUM_AREA_RGB            = 14,
	FG_AREA_GRAY12                = 15,
	FG_SEQUENCE_EXTRACTOR_A       = 16,
	FG_SEQUENCE_EXTRACTOR_B       = 17,
	FG_LINE_GRAY12                = 18,
	FG_AREA_RGB36                 = 19,
	FG_DUAL_LINE_RGB_SORTING      = 20,
	FG_DUAL_LINE_GRAY12           = 21,
	FG_MEDIUM_LINE_GRAY12         = 22,
	FG_SINGLE_AREA_GRAY12         = 23,
	FG_2D_SHADING_12              = 24,
	DIVISOR_1                     = 25,
	DIVISOR_2                     = 26,
	DIVISOR_4                     = 27,
	DIVISOR_8                     = 28,
	DIVISOR_3                     = 29,
	DIVISOR_16                    = 30,
	DIVISOR_6                     = 31

};

enum MeSensorReadoutModes2{
	SMODE_UNCHANGED      = 0,
	SMODE_REVERSE        = 3,
	SMODE_TAB2_0         = 1,
	SMODE_TAB2_1         = 4,
	SMODE_TAB2_2         = 6,
	SMODE_TAB4_0         = 2,
	SMODE_TAB4_1         = 5,
	SMODE_TAB4_2         = 7,
	SMODE_TAB4_5         = 8,
	SMODE_TAB4_3         = 9,
	SMODE_TAB4_4         = 10,
	SMODE_TAB4_6         = 11,

	SMODE_TAB8_0         = 30, /**< |1234>            <8765| */
	SMODE_TAB8_1         = 31, /**< |1234>            <5678| */
	SMODE_TAB8_2         = 32, /**< |1234>     5678>       | */
	SMODE_TAB8_3         = 33, /**< |12>   34>   56>   78> | */
	SMODE_TAB8_4         = 34, /**< | <87   <65   <43   <21| */
	SMODE_TAB8_5         = 35, /**< |12>   34>    <56   <78| */

	SMODE_TAB8_6         = 36,
	SMODE_TAB8_7         = 37,
	SMODE_TAB8_8         = 38,
	SMODE_TAB8_9         = 39,
	SMODE_TAB8_10        = 40,
	SMODE_TAB8_11        = 41,
	SMODE_TAB8_12        = 42,
	SMODE_TAB8_13        = 43,
	SMODE_TAB8_14        = 44,
	SMODE_TAB8_15        = 45,
	SMODE_TAB8_16        = 46,
	SMODE_TAB8_17        = 47,
	SMODE_TAB8_18        = 48,
	SMODE_TAB8_19        = 49,
	SMODE_TAB8_20        = 50,
	SMODE_TAB8_21        = 51,
	SMODE_TAB8_22        = 52,
	SMODE_TAB8_23        = 53,
	SMODE_TAB8_24        = 54,
	SMODE_TAB10_1        = 60, /** 10 Tap Sensor Correction: |1> 2> 3> 4> 5> 6> 7> 8> 9> 10> |*/
	SMODE_TAB10_2        = 61,  /** 10 Tap Sensor Correction reverse sensor: | <10 <9 <8 <7 <6 <5 <4 <3 <2 <1 |*/
	SMODE_TAB10_4        = 63,
	SMODE_TAB10_3        = 62,
};

enum FgParamTypes {
	FG_PARAM_TYPE_INVALID                 = 0x0, /**< will always cause an error, for internal use only */
	FG_PARAM_TYPE_INT32_T                 = 0x1, /**< signed 32 bit integer (int, int32_t) */
	FG_PARAM_TYPE_UINT32_T                = 0x2, /**< unsigned 32 bit integer (unsigned int, uint32_t) */
	FG_PARAM_TYPE_INT64_T                 = 0x3, /**< signed 64 bit integer (long long, int64_t) */
	FG_PARAM_TYPE_UINT64_T                = 0x4, /**< unsigned 64 bit integer (unsigned long long, uint64_t) */
	FG_PARAM_TYPE_DOUBLE                  = 0x5, /**< double */
	FG_PARAM_TYPE_CHAR_PTR                = 0x6, /**< char* */
	FG_PARAM_TYPE_SIZE_T                  = 0x7, /**< size_t */
	FG_PARAM_TYPE_CHAR_PTR_PTR            = 0x16, /**< char** */
	FG_PARAM_TYPE_STRUCT_FIELDPARAMACCESS = 0x1000, /**< struct FieldParameterAccess */
	FG_PARAM_TYPE_STRUCT_FIELDPARAMINT    = 0x1002, /**< struct FieldParameterInt_s */
	FG_PARAM_TYPE_STRUCT_FIELDPARAMINT64  = 0x1003, /**< struct FieldParameterAccess with 64 bit type */
	FG_PARAM_TYPE_STRUCT_FIELDPARAMDOUBLE = 0x1005, /**< struct FieldParameterDouble_s */
	FG_PARAM_TYPE_COMPLEX_DATATYPE        = 0x2000, /**< complex structure */
	FG_PARAM_TYPE_AUTO                    = -1 /**< will always cause an error, for internal use only */
};

enum MeInitFlags {
    FG_INIT_FLAG_DEFAULT = 0x00,  /** < default behavior */
    FG_INIT_FLAG_SLAVE   = 0x01,  /** < init as slave */
    FG_INIT_FLAGS_VALID_MASK   = ~(FG_INIT_FLAG_DEFAULT 
                                    | FG_INIT_FLAG_SLAVE) /** < for internal use to check for invalid flags */
};

#define FG_PARAM_TYPE_CHAR_PTR_MAX_SIZE 655360 /**< maximum size currently handled by fglib for string parameters (VA up to 3.0.6, Rt up to 5.5.1)*/

#ifdef _WIN32
#ifdef _WIN64
#define FG_PARAM_TYPE_FRAMEINDEX FG_PARAM_TYPE_INT64_T
#else /* _WIN64 */
#ifdef _MSC_VER
#define FG_PARAM_TYPE_FRAMEINDEX FG_PARAM_TYPE_INT32_T
#else /* _MSC_VER */
#define FG_PARAM_TYPE_FRAMEINDEX FG_PARAM_TYPE_INT32_T
#endif /* _MSC_VER */
#endif /* _WIN64 */
#else /* _WIN32 */
#if INTPTR_MAX == INT32_MAX
#define FG_PARAM_TYPE_FRAMEINDEX FG_PARAM_TYPE_INT32_T
#else
#define FG_PARAM_TYPE_FRAMEINDEX FG_PARAM_TYPE_INT64_T
#endif
#endif /* _WIN32 */

/**
 * \brief definitions of values for parameter FG_GEN_ENABLE / FG_CAMERASIMULATOR_ENABLE
 * These values describe the data source for images of the applet.
 * 
 */
enum FgImageSourceTypes {
	FG_CAMPORT         = 0, /**< data source is the camera */
  FG_CAMERASIMULATOR = 1, /**< data source is the image generator of the applet (naming convention of newer applets)*/
	FG_GENERATOR       = 1  /**< data source is the image generator of the applet (naming convention of older applet, kept for compatibility only)*/
};

/*
 * Defines
 */

#define FG_NO 0
#define FG_YES 1

#define FG_LOW 0
#define FG_HIGH 1

#define HIGH_ON_ZERO_LOW 1
#define HIGH_ON_ZERO_HIGH 0

#define FG_FALSE 0
#define FG_TRUE 1

#define	FG_FALLING 1
#define	FG_RISING 0

#define FG_ON 1
#define FG_OFF 0

#define FG_ZERO 0
#define FG_ONE 1

#define FG_APPLY 1

#define FG_LEFT_ALIGNED 1
#define FG_RIGHT_ALIGNED 0

#define FG_SAVE_LUT_TO_FILE 1
#define FG_LOAD_LUT_FROM_FILE 0


/**
 * \brief definitions of pixel format for parameter FG_GBE_CAMTYPE
 * These values describe the pixel format of the camera at the applet input 
 * specific for GigE Vision protocol.
 */
enum FgParamEnumGbeCamType{
  RGB8_PACKED  = 0,  /**< pixel format: red followed by green and blue without alignment */
  BGR8_PACKED  = 1,  /**< pixel format: blue followed by green and red without alignment */
  RGBA8_PACKED = 2,  /**< pixel format: red followed by green and blue plus alpha channel */
  BGRA8_PACKED = 3   /**< pixel format: blue followed by green and red plus alpha channel */
};


/**
 * \brief additional definitions
 */
#define FG_0_BIT 0
#define FG_1_BIT 1
#define FG_2_BIT 2
#define FG_3_BIT 3
#define FG_4_BIT 4
#define FG_5_BIT 5
#define FG_6_BIT 6
#define FG_7_BIT 7
#define FG_8_BIT 8
#define FG_9_BIT 9
#define FG_10_BIT 10
#define FG_11_BIT 11
#define FG_12_BIT 12
#define FG_13_BIT 13
#define FG_14_BIT 14
#define FG_15_BIT 15
#define FG_16_BIT 16
#define FG_17_BIT 17
#define FG_18_BIT 18
#define FG_19_BIT 19
#define FG_20_BIT 20
#define FG_21_BIT 21
#define FG_22_BIT 22
#define FG_23_BIT 23
#define FG_24_BIT 24
#define FG_25_BIT 25
#define FG_26_BIT 26
#define FG_27_BIT 27
#define FG_28_BIT 28
#define FG_29_BIT 29
#define FG_30_BIT 30
#define FG_31_BIT 31
#define FG_32_BIT 32
#define FG_36_BIT 36
#define FG_48_BIT 48

#define FG_MSB  0
#define FG_LSB  1

#define MAX_BUF_NR 1048576

#define CONTMODE      0x10
#define HANDSHAKEMODE 0x20
#define BLOCKINGMODE  HANDSHAKEMODE
#define PULSEMODE     0x30

/**
 * \brief definitions of image formats for parameter FG_FORMAT
 * These constants define the format at the applet output.
 */
/* image formats */
#define FG_GRAY               3  /**< 8 bit per Pixel, gray */
#define FG_GRAY_PLUS_PICNR    30 /**< 8 bit per Pixel, gray */
#define FG_GRAY16             1  /**< 16 bit per Pixel, gray*/
#define FG_GRAY16_PLUS_PICNR  10 /**< 16 bit per Pixel, gray*/
#define FG_GRAY10             21 /**< 10 bit per Pixel, gray, packed (components not aligned to bytes)*/
#define FG_GRAY12             22 /**< 12 bit per Pixel, gray, packed (components not aligned to bytes)*/
#define FG_GRAY14             23 /**< 14 bit per Pixel, gray, packed (components not aligned to bytes)*/
#define FG_GRAY32             20 /**< 32 bit per Pixel, gray*/
#define FG_COL24              2  /**< 8 bit per color component, 3 color components */
#define FG_COL32              4  /**< 8 bit per color component, 4 color components */
#define FG_COL30              5  /**< 10 bit per color component, 3 color components, packed (components not aligned to bytes) */
#define FG_COL36              7  /**< 12 bit per color component, 3 color components, packed (components not aligned to bytes) */
#define FG_COL42              9  /**< 14 bit per color component, 3 color components, packed (components not aligned to bytes) */
#define FG_COL48              6  /**< 16 bit per color component, 3 color components */
#define FG_RGBX32             408 /**< 32 bit per 4-component pixel, R  8bit G  8bit B  8bit X  8bit, where X represents an additional component */
#define FG_RGBX40             410 /**< 40 bit per 4-component pixel, R 10bit G 10bit B 10bit X 10bit, where X represents an additional component, packed (components not aligned to bytes) */
#define FG_RGBX48             412 /**< 48 bit per 4-component pixel, R 12bit G 12bit B 12bit X 12bit, where X represents an additional component, packed (components not aligned to bytes) */
#define FG_RGBX56             414 /**< 56 bit per 4-component pixel, R 14bit G 14bit B 14bit X 14bit, where X represents an additional component, packed (components not aligned to bytes) */
#define FG_RGBX64             416 /**< 64 bit per 4-component pixel, R 16bit G 16bit B 16bit X 16bit, where X represents an additional component */
#define FG_BINARY             8  /**< 1 bit per pixel, packed (8 pixel in one byte)*/        
#define FG_RAW                50 /**< user / camera defined, not aligned by frame grabber*/  

/**
 * \brief definition of synchronization flags for Fg_InitLibrariesEx()
 */
/* synchronization flags */
#define FG_INIT_LIBRARIES_SINGLE                 0 /**< Standard init, no synchronization (same as calling Fg_InitLibraries()) */
#define FG_INIT_LIBRARIES_MASTER                 1 /**< Master init, prepare slave synchronization */
#define FG_INIT_LIBRARIES_SLAVE                  2 /**< Slave init, wait for master */
#define FG_INIT_LIBRARIES_WAIT_FOR_SERVICE   0x004 /**< Wait for service to be started (valid only for single application and master modes) */
#define FG_INIT_LIBRARIES_SEQUENTIAL         0x008 /**< Strictly sequential startup (slaves must use unique priority; number of slaves must be given on master init) */
#define FG_INIT_LIBRARIES_AUTOSTART_ON_INIT  0x010 /**< Start next slave on Fg_Init() */
#define FG_INIT_LIBRARIES_SET_MODE(n) (n & 0x3) /**< Set mode (single, master, slave)*/
#define FG_INIT_LIBRARIES_GET_MODE(n) (n & 0x3) /**< Get mode (single, master, slave)*/
#define FG_INIT_LIBRARIES_SET_SLAVE_PRIORITY(n) ((n & 0x3f) << 12) /** Set slave priority (allowed values are 1..63; priority 0 will select priority automatically)*/
#define FG_INIT_LIBRARIES_GET_SLAVE_PRIORITY(n) ((n >> 12) & 0x3f) /** Get slave priority (allowed values are 1..63;)*/
#define FG_INIT_LIBRARIES_SET_NUMBER_OF_SLAVES(n) ((n & 0x3f) << 22) /** Set number of slaves (allowed values are 1..63)*/
#define FG_INIT_LIBRARIES_GET_NUMBER_OF_SLAVES(n) ((n >> 22) & 0x3f) /** Get number of slaves (allowed values are 1..63)*/

/**
 * \brief definition of acquisition modes for Fg_Acquire()
 */
/* acquire formats */
#define ACQ_STANDARD    0x1     /**< Standard acquisition mode, without lock of frame buffers */
#define ACQ_BLOCK       0x2     /**< Blocking acquisition mode, prevents previous frames to be overwritten*/
#define ACQ_MODE_MASK   0xffff
#define ACQ_NO_AUTOSTOP 0x10000


/**
 * \brief definitions of arguments for Fg_getStatusEx()
 */
#define NUMBER_OF_GRABBED_IMAGES     10   /**< how many frames were already transferred */
#define NUMBER_OF_LOST_IMAGES        20   /**< how many frames were lost */
#define NUMBER_OF_BLOCK_LOST_IMAGES  30   /**< how many frames were lost because all buffers were blocked */
#define NUMBER_OF_BLOCKED_IMAGES     40   /**< how many frames were blocked */
#define NUMBER_OF_ACT_IMAGE          50   /**< index of the frame transferred last */
#define NUMBER_OF_LAST_IMAGE         60   /**< index of the frame requested last */
#define NUMBER_OF_NEXT_IMAGE         70   /**< index of the frame following the last requested one */
#define NUMBER_OF_IMAGES_IN_PROGRESS 80   /**< how many frames were transferred but not yet requested */

#define BUFFER_STATUS                90   /**< state of transfer (running, stopped) */
#define GRAB_ACTIVE                  100  /**< state of transfer (running, stopped) */

 /**
 * \brief Definitions of arguments for Fg_setStatusEx()
 */
#define FG_BLOCK                     0x100
#define FG_UNBLOCK                   0x200 /**< unblock a particular buffer */
#define FG_UNBLOCK_ALL               0x220 /**< unblock all buffers */

#define SEL_ACT_IMAGE                200   /**< */
#define SEL_LAST_IMAGE               210   /**< Select / read frame number of last completely transferred image */
#define SEL_NEXT_IMAGE               220   /**< Select / read successor of latest selected image */
#define SEL_NUMBER                   230   /**< Select / read a certain image defined by frame number */
#define SEL_NEW_IMAGE                240   /**< Select / read frame number of currently transferred image  */

/* +-- PARAMETER --+ */

/**
 * \brief Definitions of virtual parameter IDs
 * These values can be used to get additional information about a parameter by adding them to the parameter ID and call one of the
 * Fg_getParameter... functions.
 */
#define FG_PARAMETER_PROPERTY_ACCESS 0x80000000 /**< Get the access mode of a parameter by adding this value to the parameter ID; this is always an int parameter! */
#define FG_PARAMETER_PROPERTY_MIN    0xC0000000 /**< Get the minimum value of a parameter by adding this value to the parameter ID; same type as parameter */
#define FG_PARAMETER_PROPERTY_MAX    0x40000000 /**< Get the maximum value of a parameter by adding this value to the parameter ID; same type as parameter */
#define FG_PARAMETER_PROPERTY_STEP   0xE0000000 /**< Get the step value of a parameter by adding this value to the parameter ID; same type as parameter */

/**
 * \brief Possible flags for parameter access property
 * The value of the access property for a parameter can be any combination of the values below ORed
 */
#define FP_PARAMETER_PROPERTY_ACCESS_READ   0x1 /**< The register can be accessed for reading */
#define FP_PARAMETER_PROPERTY_ACCESS_WRITE  0x2 /**< The register can be accessed for writing */
#define FP_PARAMETER_PROPERTY_ACCESS_MODIFY 0x4 /**< The register value can be modified during acquisition  */
#define FP_PARAMETER_PROPERTY_ACCESS_LOCKED 0x8 /**< The register value is locked */

/**
 * \brief definitions of various parameter IDs
 * These values can be read by using Fg_GetParameterIdByName(<ParameterName>)
 */
#define FG_REVNR                     99
#define FG_WIDTH                     100  /**< image width of the output image, cannot changed during acquisition*/
#define FG_HEIGHT                    200  /**< image height of the output image, cannot changed during acquisition */

#define FG_MAXWIDTH                  6100
#define FG_MAXHEIGHT                 6200
#define FG_ACTIVEPORT                6300

#define FG_XOFFSET                   300
#define FG_YOFFSET                   400
#define FG_XSHIFT                    500
#define FG_TIMEOUT                   600
#define FG_APC_STOP_TIMEOUT          601  /**< how many milliseconds Fg_AcquireEx(Fg, DmaIndex, memHandle, STOP_SYNC_TO_APC) will wait for the APC to return (unsigned int) */
#define FG_STOP_TIMEOUT              602  /**< how many seconds Fg_stopAcquire with STOP_SYNC flag will wait for end of current picture before stopping the acquisition */
#define FG_FORMAT                    700
#define FG_CAMSUBTYP                 80
#define FG_FRAMESPERSEC              90
#define FG_MAXFRAMESPERSEC           91
#define FG_MINFRAMESPERSEC           92
#define FG_LINESPERSEC               95
#define FG_LINEPERIOD                96
#define FG_LINEPERIODE               FG_LINEPERIOD /*< duplicated for compatibility issues*/

#define FG_EXPOSURE                  10020 /**< Exposure Time in us (Brightness) (float) */
#define FG_LINEEXPOSURE              10030
#define FG_HDSYNC                    10050

#define FG_PRESCALER                 FG_HDSYNC
#define FG_LINETRIGGER               FG_HDSYNC

#define FG_RS232PARON                10060
#define FG_MTU_SIZE                  200351 /**< MTU size to use. Has to be equal to the MTU size defined for the link. GEV and specific applets only*/

#define FG_PIXELDEPTH                4000
#define FG_BITALIGNMENT              4010
#define FG_LINEALIGNMENT             4020
#define FG_COLOR_SELECT              4030

#define FG_SWAP_CHANNELS             200350 /**< Swap the link channels.*/

#define FG_CAMBITWIDTH               5000
#define FG_CAMBITSHIFT               5010
#define FG_CAMERA_WIDTH              110133 /**< Length of lines transfered from the camera to the frame grabber.*/

#define FG_SHIFTCAMDATARIGHT         5020
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

#define FG_CAMSTATUS          2000 /**< returns the status information of a particular camera link chip */
#define FG_CAMSTATUS_EXTENDED 2050 /**< returns the status information of a particular camera link chip: Bits [0] Clock, [1] LVAL, [2] FVAL, [3] ExSync, [4] ExTrg, [5] Overflow, [6:7] BufLvl */
#define FG_TWOCAMMODEL        2100
#define FG_PORT               3000
#define FG_NR_OF_DMAS         3050 /**< returns the number of DMA channels for a given process */ 
#define FG_TURBO_DMA_MODE     3051 /**< activate most high speed DMA transfer. Available at certain applets only*/
#define FG_NR_OF_CAMS         3060 /**< returns the number of camera ports for a given process */
#define FG_NR_OF_PROCESSES    3070 /**< returns the number of processes of the current applet */
#define FG_DMA_PORT           3080
#define FG_DMA_STARTED        3081
#define FG_CAM_PORT           3090

#define FG_RESET_GIGE_PORT_0  3100
#define FG_RESET_GIGE_PORT_1  3101
#define FG_RESET_GIGE_PORT_2  3102
#define FG_RESET_GIGE_PORT_3  3103


#define FG_TRANSFER_LEN       5210  /**< returns the length of the transferred image. Use Fg_getParameterEx(<frameNo>) to read */

#define FG_STROBEPULSEDELAY   8700
#define FG_STROBEPULSEREDUCE  8710
#define FG_STROBEPULSESRCSEL  8720
#define FG_STROBEPULSEINVERT  8730
#define FG_FLASHTIME          8740
#define FG_FLASHTIME_SYNC     8750

#define FG_CAMERA_LINK_CAMTYPE     11011
#define FG_CAMERA_LINK_CAMTYP      FG_CAMERA_LINK_CAMTYPE /*< duplicated for compatibility issues*/
#define FG_CL_CAMTYP               11011
#define FG_CAMTYP                  11011
#define FG_GBE_CAMTYPE             11011 /**< Input format for GEV frame grabbers*/
#define FG_GBE_CAMTYP              FG_GBE_CAMTYPE /*< duplicated for compatibility issues*/
#define FG_CAMERA_LINK_CORE_RESET		11012 /**< Reset Camera Core. */ 
#define FG_CAMERA_LINK_PIXEL_CLOCK		11013 /**< Returns the currently measured pixel clock.*/
#define FG_CAMERA_LINK_PIXEL_CLOCK_X	11014 /**< Returns the currently measured pixel clock for Camera Link chip X.*/
#define FG_CAMERA_LINK_PIXEL_CLOCK_Y	11015 /**< Returns the currently measured pixel clock for Camera Link chip Y.*/
#define FG_CAMERA_LINK_PIXEL_CLOCK_Z	11016 /**< Returns the currently measured pixel clock for Camera Link chip Z.*/

#define FG_LOOKUPTABLE             12000
#define FG_LUT_FILE                12010
#define FG_LUT_SAVE_LOAD_FILE      12020
#define FG_LUT_ENABLE              12030 /**< In some applets it is possible to disable the LUT*/

#define FG_KNEE_LUT                12100
#define FG_KNEE_LUT_FILE           12110
#define FG_KNEE_LUT_SAVE_LOAD_FILE 12120
#define FG_KNEE_LUT_MODE           12130
#define FG_KNEE_LUT_ACCESS         12140

#define FG_KNEE_LUT_SCALE          12101
#define FG_KNEE_LUT_OFFSET         12102
#define FG_KNEE_LUT_GAMMA          12103
#define FG_KNEE_LUT_INVERT         12104

#define FG_MEDIAN                  12200

#define FG_2DSHADINGPARAMETER      12500

#define FG_SCALINGFACTOR_RED       13000
#define FG_SCALINGFACTOR_BLUE      13010
#define FG_BAYERINIT               13020
#define FG_SCALINGFACTOR_GREEN     13030

#define FG_CCSEL                   14000
#define FG_CCSEL0                  14001
#define FG_CCSEL1                  14002
#define FG_CCSEL2                  14003
#define FG_CCSEL3                  14004

#define FG_CCSEL_INVERT            14005
#define FG_CCSEL_INVERT0           14006
#define FG_CCSEL_INVERT1           14007
#define FG_CCSEL_INVERT2           14008
#define FG_CCSEL_INVERT3           14009

#define FG_CC1_SOURCE              14041  /**< Source for CC1 output */   
#define FG_CC1_POLARITY            14031  /**< Polarity of CC1 source */     
#define FG_CC2_SOURCE              14042  /**< Source for CC2 output */     
#define FG_CC2_POLARITY            14032  /**< Polarity of CC2 source */        
#define FG_CC3_SOURCE              14043  /**< Source for CC3 output */     
#define FG_CC3_POLARITY            14033  /**< Polarity of CC3 source */        
#define FG_CC4_SOURCE              14044  /**< Source for CC4 output */     
#define FG_CC4_POLARITY            14034  /**< Polarity of CC4 source */        

#define FG_DIGIO_INPUT             14010 /**< Read the state of digital inputs of the frame grabber*/
#define FG_DIGIO_OUTPUT            14020 /**< Set the state of digital outputs of the frame grabber*/

#define FG_IMAGE_TAG               22000 /**< Tag and status information of a particular frame. Bit 0..15: unique tag (starting with 0), Bit 16..24: reserved, Bit 24..31: status flags (applet specific)*/
#define FG_TIMESTAMP               22020 /**< Timestamp of a transmitted frame in milliseconds */
#define FG_TIMESTAMP_LONG          22030 /**< Timestamp of a transmitted frame as 64 bit value; use FG_TIMESTAMP_LONG_FREQUENCY to calculate time */
#define FG_TIMESTAMP_LONG_FREQUENCY 22031 /**< Timestamp frequency as 64 bit value */

#define FG_LICENSESTRING0          23000
#define FG_LICENSESTRING1          23010
#define FG_LICENSESTRING2          23020

#define FG_ACCESS_POINTER          23030

#define FG_ROIX                    23100
#define FG_ROIY                    23110
#define FG_SHADING_SUBIMAGE        23120
#define FG_SHADING_MULTENABLE      23130
#define FG_SHADING_OFFSETENABLE    23140
#define FG_SHADING_SUBENABLE       FG_SHADING_OFFSETENABLE /*< duplicated for compatibility issues*/
#define FG_SHADING_MAX_MULT        23135

#define FG_SHADING_RUNSUBIMAGE0    23121
#define FG_SHADING_RUNSUBIMAGE1    23122
#define FG_SHADING_RUNSUBIMAGE2    23123
#define FG_SHADING_RUNSUBIMAGE3    23124

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

#define FG_CONTRAST                   23200
#define FG_BRIGHTNESS                 23220

#define FG_DOWNSCALE                  24040
#define FG_LINE_DOWNSCALE             FG_DOWNSCALE
#define FG_LINE_DOWNSCALEINIT         24050
#define FG_FLASH_POLARITY             24060
#define FG_FLASHDELAY                 FG_STROBEPULSEDELAY

#define FG_LOAD_SHADINGDATA           24070
#define FG_CLEAR_SHADINGDATA          24080

#define FG_LINESHADINGPARAMETER       24081
#define FG_1DSHADINGPARAMETER         FG_LINESHADINGPARAMETER

#define FG_LINESHADING_SUB_ENABLE     24082
#define FG_LINESHADING_MULT_ENABLE    24083
#define FG_ENABLEDISABLE_SHADING      FG_LINESHADING_MULT_ENABLE
#define FG_SHADING_WIDTH              24089
#define FG_AUTO_SHADING_WIDTH         24090
#define	FG_WRITE_SHADING_12           24091

#define FG_LINESHADING_MULT_FILENAME  24084
#define FG_LINESHADING_SUB_FILENAME   24085
#define FG_LINESHADING_LOAD_FROM_FILE 24086
#define FG_LINESHADING_MODE           24087

#define FG_DMASTATUS                  24092
#define FG_LINEVALID_SIGNAL_COUNT     24093
#define FG_FRAMEVALID_SIGNAL_COUNT    24094

#define FG_1DSHADING_FILE             FG_LINESHADING_MULT_FILENAME
#define FG_LOAD_1DSHADINGDATA         FG_LINESHADING_LOAD_FROM_FILE

#define FG_BURSTLENGTH                24097
#define FG_SUPERFRAME                 24098

#define FG_PLX_CLK                    24102
#define FG_MEASURED_PCIE_CLK          24103
#define FG_FPGA_CLK                   24104
#define FG_HAP_FILE                   24108

#define FG_GLOBAL_ACCESS              24110
#define FG_DOC_URL                    24112
#define FG_PARAM_DESCR                24114
#define FG_REG_VALUE_STRING           24115

#define	FG_CAMPORT_CONFIG             30000
#define	FG_CAMERA_TYPE                30001
#define	FG_COLOR_FLAVOUR              30002



/**
 * \brief Camera simulation (microEnable IV)
*/
#define	FG_GEN_ENABLE              30099 /**< Select Camera simulation or Camera port input*/
#define	FG_GEN_PASSIVE             30100 /**< */
#define	FG_GEN_ACTIVE              30101 /**< */
#define	FG_GEN_WIDTH               30102 /**< Width of the simulated image (area scan)*/
#define	FG_GEN_LINE_WIDTH          30103 /**< Width of the simulated image (line scan)*/
#define	FG_GEN_HEIGHT              30104 /**< Height of the simulated image*/
#define	FG_GEN_START               30113 /**< Start simulation / stop simulation*/
#define	FG_GEN_LINE_GAP            30105 /**< */
#define	FG_GEN_FREQ                30106 /**< Frequency of frame grabber simulation [MHz]*/
#define	FG_GEN_ACCURACY            30107 /**< */
#define	FG_GEN_ROLL                30112 /**< Simulate rolling image */
#define	FG_GEN_TAP1                30108 /**< CameraLink simulation: Enable TAP 1*/
#define	FG_GEN_TAP2                30109 /**< CameraLink simulation: Enable TAP 2*/
#define	FG_GEN_TAP3                30110 /**< CameraLink simulation: Enable TAP 3*/
#define	FG_GEN_TAP4                30111 /**< CameraLink simulation: Enable TAP 4*/


/**
 * \brief Camera simulation parameter(microEnable 5)
*/
#define FG_CAMERASIMULATOR_ENABLE          FG_GEN_ENABLE   /**< Enable or disable the camera simulator to replace the frame grabber input data from camera.*/
#define FG_CAMERASIMULATOR_WIDTH           FG_GEN_WIDTH    /**< The image width of the generate images.*/
#define FG_CAMERASIMULATOR_HEIGHT          200322          /**< The image height of the generated images.*/
#define FG_CAMERASIMULATOR_LINE_GAP        FG_GEN_LINE_GAP /**< The line gap between two image lines. Unit pixel.*/
#define FG_CAMERASIMULATOR_FRAME_GAP       200325          /**< The frame gap between two image line. Unit lines.*/
#define FG_CAMERASIMULATOR_PATTERN         200326          /**< Select the pattern, the simulator has to generate.*/
#define FG_CAMERASIMULATOR_ROLL            200327          /**< Activate or disable a rolling simulator image. Pixel values will be increased with every new frame.*/
#define FG_CAMERASIMULATOR_SELECT_MODE     200328          /**< Select the operation mode of the camera simulator. Pixel frequency, line rate or frame rate.*/
#define FG_CAMERASIMULATOR_PIXEL_FREQUENCY FG_GEN_FREQ     /**< Define the pixel frequency of the simulator.*/
#define FG_CAMERASIMULATOR_LINERATE        200329          /**< Define the line rate of the simulator.*/
#define FG_CAMERASIMULATOR_FRAMERATE       200352          /**< Define the frame rate of the simulator.*/
#define FG_CAMERASIMULATOR_ACTIVE          FG_GEN_ACTIVE   /**< Ratio of active clock cycles.*/
#define FG_CAMERASIMULATOR_PASSIVE         FG_GEN_PASSIVE  /**< Ratio of passive clock cycles.*/
#define FG_CAMERASIMULATOR_TRIGGER_MODE    200355          /**< Select free run or trigger module to trigger the camera simulator.*/
#define FG_CAMERASIMULATOR_PATTERN_OFFSET  200356          /**< Define an offset for pattern generation.*/
#define FG_CAMERASIMULATOR_FPS             200358          /**< Defines the framerate of the generated images.*/

/**
 * \brief Camera simulation Enumerations / values (microEnable 5)
*/

/**
 * \brief Camera simulation: pattern generation
*/
#define FG_HORIZONTAL       1 /**< Horizontal Pattern*/
#define FG_VERTICAL         2 /**< Vertical Pattern*/
#define FG_DIAGONAL         3 /**< Diagonal Pattern*/

/**
 * \brief Camera simulation: definition of simulation speed
*/
#define FG_PIXEL_FREQUENCY  0 /**< Define simulation frequency by pixel rate.*/
#define FG_LINERATE         1 /**< Define simulation frequency by line rate.*/
#define FG_FRAMERATE        2 /**< Define simulation frequency by frame rate.*/


/**
 * \brief Camera simulation: Image triggering
*/
enum CameraSimulatorTriggerMode{
  SIMULATION_FREE_RUN           = 0, /**< Data is generated automatically*/
  RISING_EDGE_TRIGGERS_LINE     = 8, /**< Line data is generated on Rising Edge of grabber's trigger module*/
  RISING_EDGE_TRIGGERS_FRAME    = 9  /**< Frame data is generated on Rising Edge of grabber's trigger module*/
};


/* defines from 200000 to 210000 are reserved for customer projects */

/**
 * \brief Applet identification
*/
#define FG_APPLET_ID               24010 /**< Identifies the type of applet*/
#define FG_APPLET_VERSION          24020 /**< Indicates the applet version (major version)*/
#define FG_APPLET_REVISION         24030 /**< Indicates the applet version (major version)*/
#define FG_APPLET_BUILD_TIME       24011 /**< The time and date the hardware applet was build (me 5 only).*/

#define FG_DESIGNCLK               24040

/**
 * \brief Others 
*/

#define FG_ALL               24050

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


#define FG_BOARD_INFORMATION		42042

#define FG_LOGGING					43010 /**< DEPRECATED */
#define FG_LOG_FLUSH				43020 /**< DEPRECATED */
#define FG_LOG_CONSOLE				43030 /**< DEPRECATED */
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
	INFO_APPLET_CAPABILITY_TAGS                 = 1,   /**< list of key value pairs describing the features supported by the applet */
	INFO_TIMESTAMP_FREQUENCY                    = 100, /**< frequency of the timestamp counter in counts per second */
	INFO_OWN_BOARDINDEX                         = 101, /**< get the own board index */
	INFO_NR_OF_BOARDS                           = 1000, /**< get the number of frame grabbers in the system */
	INFO_MAX_NR_OF_BOARDS                       = 1001, /**< get the maximum number of frame grabbers allowed in the system */
	INFO_BOARDNAME                              = 1010, /**< get the typename of the according board identified by param1*/
	INFO_BOARDTYPE                              = 1011, /**< get the type of the according board identified by param1*/
	INFO_BOARDSERIALNO                          = 1012, /**< get the serialNo. of the according board identified by param1*/
	INFO_BOARDSUBTYPE                           = 1013, /**< get the sub-type of the according board identified by param1*/
	INFO_FIRMWAREVERSION                        = 1015, /**< get the firmware version of the according board identified by param1 as string*/
	INFO_HARDWAREVERSION                        = 1016, /**< get the hardware version of the according board identified by param1 as string*/
	INFO_PHYSICAL_LOCATION                      = 1017, /**< get the physical location of the according board identified by param1 as string*/
	INFO_BOARDSTATUS                            = 1020, /**< get the board status (see meaning of the bits in Fg_BoardStatus_Bits) */
	INFO_PIXELPLANT_PRESENT                     = 1030, /**< presence of PixelPlant expansion board*/
	INFO_CAMERA_INTERFACE                       = 1040, /**< Type of camera interface */
	INFO_DRIVERVERSION                          = 1100, /**< get the driver version used for the according board identified by param1 as string*/
	INFO_DRIVERARCH                             = 1101, /**< get the driver architecture used for the according board identified by param1 as string*/
	INFO_DRIVERFULLVERSION                      = 1102, /**< get the driver version and architecture used for the according board identified by param1 as string*/
	INFO_BOARDNODENUMBER                        = 1103, /**< get the board NUMA node number*/
	INFO_DRIVERGROUPAFFINITY                    = 1104, /**< get the driver IRQ group affinity*/
	INFO_DRIVERAFFINITYMASK                     = 1105, /**< get the driver IRQ processor affinity mask as unsigned 64-bit integer (hex)*/
	INFO_DESIGN_ID                              = 1200, /**< get the HAP id for the design of the according board identified by param1 as string*/
	INFO_BITSTREAM_ID                           = 1201, /**< get the bit stream id for the design of the according board identified by param1 as string*/
	INFO_APPLET_DESIGN_ID                       = 1202, /**< get the HAP id for the design of the applet identified on input as string in the buffer; full applet path required */
	INFO_APPLET_BITSTREAM_ID                    = 1203, /**< get the bit stream id for the design of applet identified on input as string in the buffer; full applet path required */
	INFO_FPGA_BITSTREAM_ID                      = 1204, /**< get the bit stream id for the FPGA of the according board identified by param1 as string*/
	INFO_APPLET_FULL_PATH                       = 1300, /**< get the full applet path according to fg as string*/
	INFO_APPLET_FILE_NAME                       = 1301, /**< get the applet file name according to fg as string*/
	INFO_APPLET_TYPE                            = 1302, /**< get the applet type according to fg (0:HAP, 1:DLL/SO)*/
	INFO_STATUS_PCI_LINK_WIDTH                  = 2001, /**< get the number of lanes of the PCIe connection, possible values: 0,1,2,4,8,12,16,32 */
	INFO_STATUS_PCI_PAYLOAD_MODE                = 2002, /**< get information about current BIOS setting for PCIe payload size configured for current design, possible values: 0: payload 128, 1 Payload 256 (availability: firmware dependend)*/
	INFO_STATUS_PCI_LINK_SPEED                  = 2003, /**< get the link speed (PCIe generation) of the PCIe connection, possible values: 0,1,2,3 */
	INFO_STATUS_PCI_PAYLOAD_SIZE                = 2004, /**< get information about current BIOS setting for PCIe payload size configured for current design.*/
	INFO_STATUS_PCI_EXPECTED_LINK_WIDTH         = 2005, /**< get the number of lanes of the PCIe connection as expected by the firmware, possible values: 0,1,2,4,8,12,16,32 */
	INFO_STATUS_PCI_EXPECTED_LINK_SPEED         = 2006, /**< get the link speed (PCIe generation) of the PCIe connection as expected by the firmware, possible values: 0,1,2,3 */
	INFO_STATUS_PCI_NATIVE_LINK_SPEED           = 2007, /**< get the link speed (PCIe generation) of the PCIe connection as reported by the mainboard, possible values: 0,1,2,3 */
	INFO_STATUS_PCI_REQUEST_SIZE                = 2008, /**< get information about current BIOS setting for PCIe request size configured for current design.*/
	INFO_STATUS_PCI_NROF_INVALID_8B10B_CHARS    = 2101, /**< get the number of invalid 8b10b character PCI errors (availability: firmware dependent)*/
	INFO_STATUS_PCI_NROF_8B10B_DISPARITY_ERRORS = 2102, /**< get the number of 8b10b disparity PCI errors (availability:  firmware dependent)*/
	INFO_SERVICE_ISRUNNING                      = 3001  /**< check if Silicon Software service is running */
};

/**
 * \brief definitions of board status bits
 */
enum Fg_BoardStatus_Bits {
	INFO_BOARDSTATUS_CONFIGURED      = 0x00000001, /**< FPGA is configured */
	INFO_BOARDSTATUS_LOCKED          = 0x00000002, /**< resources are locked and in use */
	INFO_BOARDSTATUS_DEAD_1          = 0x00008000, /**< dead and gone (mE4) */
	INFO_BOARDSTATUS_RECONFIGURING   = 0x10000000, /**< board is being reconfigured (mE5) */
	INFO_BOARDSTATUS_REBOOT_REQUIRED = 0x20000000, /**< reconfiguring failed and a reboot is required (mE5) */
	INFO_BOARDSTATUS_OVERTEMP        = 0x40000000, /**< FPGA is too hot for safe operation */
	INFO_BOARDSTATUS_DEAD_2          = 0x80000000, /**< dead and gone (mE5) */
	INFO_BOARDSTATUS_DEAD = INFO_BOARDSTATUS_DEAD_1 | INFO_BOARDSTATUS_DEAD_2
};

/**
 * \brief definitions for querying information about a certain field
 */
enum FgProperty {
	PROP_ID_VALUE         = 0,  /**< request the current value of a parameter, information,.. */
	PROP_ID_DATATYPE      = 1,  /**< request the data type of a parameter, information, */
	PROP_ID_NAME          = 2,  /**< request the symbolic name of a parameter, information, */
	PROP_ID_PARAMETERNAME = 3,  /**< request the Name for Software access */
	PROP_ID_VALUELLEN     = 4,  /**< request the current length of data */
	PROP_ID_ACCESS_ID     = 5,  /**< request the parameter id for access parameter*/
	PROP_ID_MIN_ID        = 6,  /**< request the parameter id for min parameter*/
	PROP_ID_MAX_ID        = 7,  /**< request the parameter id for max parameter*/
	PROP_ID_STEP_ID       = 8,  /**< request the parameter id for step parameter*/
	PROP_ID_ACCESS        = 9,  /**< request the parameter id for access parameter*/
	PROP_ID_MIN           = 10, /**< request the min value*/
	PROP_ID_MAX           = 11, /**< request the max value*/
	PROP_ID_STEP          = 12, /**< request the value for step size*/
	PROP_ID_IS_ENUM       = 13, /**< request if the int parameter is an enumeration; the value returned as string is the length of the buffer needed for the enum values; 0 means it's not an enum*/
	PROP_ID_ENUM_VALUES   = 14, /**< request the enumeration values*/
	PROP_ID_FIELD_SIZE    = 15, /**< request the size of a field parameter*/
};

/**
 * \brief structure for requesting enumeration values
 */
struct FgPropertyEnumValues {
    int32_t value;
    char name[1];
};

#define FG_PROP_GET_NEXT_ENUM_VALUE(pev) ((FgPropertyEnumValues *)(((char *)pev) + 5 + strlen(pev->name)))



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
#define LUT_RED   0
#define LUT_GREEN 1
#define LUT_BLUE  2
#define LUT_GRAY  3

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
#define TRGINSRC_GPI_0 0
#define TRGINSRC_GPI_1 1
#define TRGINSRC_GPI_2 2
#define TRGINSRC_GPI_3 3
#define TRGINSRC_GPI_4 4
#define TRGINSRC_GPI_5 5
#define TRGINSRC_GPI_6 6
#define TRGINSRC_GPI_7 7
#define TRGINSRC_FRONT_GPI_0 16
#define TRGINSRC_FRONT_GPI_1 17
#define TRGINSRC_FRONT_GPI_2 18
#define TRGINSRC_FRONT_GPI_3 19

#define GRAB_INFINITE    -1
#define GRAB_ALL_BUFFERS -2

/**
  * \brief Flags for controlling the behavior of Fg_StopAcquireEx() function
  */
enum FgStopAcquireFlags {
	STOP_ASYNC          = 0x00,       /**< Stops the images acquisition immediately without any synchronization to image data flow*/
	STOP_SYNC_TO_APC    = 0x04,       /**< Synchronizes the Fg_StopAcquireEx() with the pending APC callbacks by using the Timeout defined by FG_APC_STOP_TIMEOUT */
	STOP_ASYNC_FALLBACK = 0x40000000, /**< Fall back to STOP_ASYNC if STOP_SYNC fails*/
	STOP_SYNC           = 0x80000000  /**< Reserved for stopping acquisition synchronously to image data flow*/
};

/* Status codes */
#define		FG_OK                                 0 /**< Indication of success*/
#define		FG_INIT_OK                            1
#define		FG_SOFTWARE_TRIGGER_PENDING        8803

/**
  * \brief Error codes returned by calls to Fg_xxx functions
*/
/* Error codes */
#define		FG_ERROR                             -1
#define		FG_DUMMY_BUFFER                      -1
#define		FG_NO_PICTURE_AVAILABLE              -2
#define		FG_SISODIR5_NOT_SET                  -5
#define		FG_INVALID_HANDLE                    -6

#define		FG_ALR_INIT                         -10
#define		FG_NOT_AVAILABLE                    -12
#define		FG_NO_BOARD_AVAILABLE               -20
#define		FG_INVALID_BOARD_NUMBER             -21
#define		FG_BOARD_INIT_FAILED                -22
#define		FG_INVALID_CLOCK                    -23
#define		FG_INVALID_DESIGN_NAME              -26
#define		FG_SYSTEM_LOCKED                    -27
#define		FG_RESSOURCES_STILL_IN_USE          -28
#define		FG_CLOCK_INIT_FAILED                -29
#define		FG_WRONG_ARCHITECTURE               -50
#define		FG_WRONG_FIRMWARE_VERSION           -51 /**<The firmware in the frame grabber board does not match the requirements of the applet*/
#define		FG_WRONG_RUNTIME_VERSION            -52 /**<The applet requires a different runtime version*/
#define		FG_SOFTWARE_TRIGGER_BUSY            -60
#define		FG_INVALID_PORT_NUMBER              -61

#define		FG_EXCEPTION_IN_APPLET              -99 /**<An exception occurred while accessing an applet function */
#define		FG_HAP_FILE_NOT_LOAD               -100 /**<File cannot be loaded due to various reasons */
#define		FG_FILE_NOT_FOUND                  -101 /**<File does not exist */
#define		FG_APPLET_NOT_ACCEPTED             -102 /**<Applet is not flashed on the board */
#define		FG_RECONFIGURATION_DISABLED        -103 /**<Reconfiguration crash protection was enabled; please use microDiagnostics to disable */
#define		FG_RESTART_REQUIRED                -104 /**<Reconfiguration requires a restart of the system */
#define		FG_POWERCYCLE_REQUIRED             -105 /**<Reconfiguration requires a powercycle of the system */

#define		FG_MICROENABLE_NOT_INIT            -110
#define		FG_DLL_NOT_LOAD                    -120
#define		FG_REG_KEY_NOT_FOUND               -121
#define		FG_VASDLL_NOT_LOAD                 -122
#define		FG_ERROR_LOADING_MODULE            -123
#define		FG_UNEXPECTED_HAPLOAD              -130
#define		FG_SIZE_ERROR                      -200
#define		FG_PTR_INVALID                     -300
#define		FG_RANGE_ERR                       -400
#define		FG_OVERFLOW_ERR                    -401
#define		FG_NOT_ENOUGH_MEM                  -500
#define		FG_DMATRANSFER_INVALID             -600
#define		FG_HAP_FILE_DONT_MATCH             -700
#define		FG_VERSION_MISMATCH                -701
#define		FG_ACCESS_DENIED                   -702
#define		FG_RUNTIME_VERSION_TOO_OLD         -703

#define		FG_NOT_INIT                        -2001
#define		FG_WRONG_SIZE                      -2002
#define		FG_WRONG_NUMBER_OF_BUFFER          -2010
#define		FG_TOO_MANY_BUFFER                 -2011
#define		FG_NOT_ENOUGH_MEMORY               -2020
#define		FG_MEMORY_ALREADY_ALLOCATED        -2024
#define		FG_CANNOT_WRITE_MEM_CONFIG_FAILED  -2026
#define		FG_INTERNAL_STATUS_ERROR           -2030
#define		FG_INTERNAL_ERROR                  -2031
#define		FG_CANNOT_START                    -2040
#define		FG_CANNOT_STOP                     -2042
#define		FG_SYNC_ACQUIRE_NOT_SUPPORTED      -2045
#define		FG_INVALID_DESIGN                  -2050 /**<  mismatch between applet and required firmware version, mismatch applet/hardware fpga type, mismatch fpga no, CRC error during fpga configuration*/ 
#define		FG_CONFIGURE_FAILED                -2052
#define		FG_RECONFIGURE_FAILED              -2053
#define		FG_NO_APPLET_ID                    -2055

#define		FG_INVALID_MEMORY                  -2060
#define		FG_MEMORY_IN_USE                   -2061 /**< the memory is already in use by another DMA channel */
#define		FG_INVALID_PARAMETER               -2070 /**< the applet-parameter is not valid or a function call contains invalid data / arguments*/ 
#define		FG_ILLEGAL_WHILE_APC               -2071 /**< there are no event types available at the grabber / applet*/
#define		FG_APC_PRIORITY_ERROR              -2072 /**< FG_APC_HIGH_PRIORITY was requested but setting the priority failed */
#define		FG_APC_ALREADY_REGISTERED          -2073 /**< a APC callback handler is already registered*/

#define		FG_INVALID_VALUE                   -2075 /**< An applet parameter was set to an invalid value*/
#define		FG_INVALID_FILENAME                -2076 /**< An applet parameter was set to an non existing filename*/
#define		FG_INVALID_FILESIZE                -2077 /**< An applet parameter was set to a file of the wrong size*/
#define		FG_INVALID_TYPE                    -2078 /**< An applet parameter was set using an invalid type*/

#define		FG_INVALID_REGISTER                -7040
#define		FG_INVALID_MODULO                  -7080
#define		FG_INVALID_CONFIGFILE              -5000
#define		FG_INVALID_CONFIGFILEEXT            FG_INVALID_CONFIGFILE
#define		FG_FILE_ACCESS_DENIED              -5001

#define		FG_ERROR_FREQUENCY_TOO_HIGH_FOR_PFG               -2077
#define		FG_ERROR_VALUE_TOO_LOW_FOR_FPS_OR_WIDTH_OR_DELAY  -2078
#define		FG_ERROR_VALUE_TOO_HIGH_FOR_FPS_OR_WIDTH_OR_DELAY -2079

#define		FG_NOT_LOAD                         -2080
#define		FG_ALREADY_STARTED                  -2090
#define		FG_OPERATION_ABORTED                -2091
#define		FG_STILL_ACTIVE                     -2100
#define		FG_NO_VALID_DESIGN                  -2110
#define		FG_TIMEOUT_ERR                      -2120 /**< Acquisition timeout, no more frames */
#define		FG_NOT_IMPLEMENTED                  -2130
#define		FG_WRONG_TRIGGER_MODE               -2140
#define		FG_NOT_WRONG_TRIGGER_MODE            FG_WRONG_TRIGGER_MODE
#define		FG_WRONG_TRIGGER_STATE              -2141
#define		FG_ALL_BUFFER_BLOCKED               -2150

#define		FG_NO_EVENTS_FOUND                  -2160 /**< there are no event types available at the grabber / applet*/
#define		FG_CANNOT_COMBINE_DATA_EVENTS       -2161 /**< different event types, which transport additional information cannot be combined to one event mask*/
#define		FG_INVALID_EVENTMASK                -2162 /**< event mask does not match to available event types*/

#define		FG_CANNOT_INIT_MICROENABLE          -3000
#define		FG_TRANSFER_NOT_ACTIVE              -3010
#define		FG_CLOCK_NOT_LOCKED                 -3120
#define		FG_STILL_NOT_STARTED                -4000
#define		FG_VALUE_OUT_OF_RANGE               -6000
#define		FG_CANNOT_CHANGE_DISPLAY_WIDTH      -7000
#define		FG_CANNOT_CHANGE_DISPLAY_HEIGHT     -7005
#define		FG_CANNOT_CHANGE_DISPLAY_SIZE       -7010
#define		FG_NO_VALID_LICENSE                 -7020
#define		FG_CANNOT_CHANGE_CAMERA_FORMAT      -7030
#define		FG_REGISTER_INIT_FAILED             -7050
#define		FG_INVALID_SHADING_CORRECTION_FILE  -7060
#define		FG_WRITE_LINE_SHADING_TIMEOUT       -7070
#define		FG_INVALID_IMAGE_DIMENSIONS         -7071  /**< Image dimensions of a loaded file don't match*/
#define   FG_ERR_INVALID_FILE_DATA            -7072
#define   FG_ERR_RANGE_ERROR                  -7073
#define		FG_CANNOT_CHANGE_DURING_ACQU        -7090
#define		FG_TOKEN_NOT_FOUND_ERROR            -8000
#define		FG_WRITE_ACCESS_DENIED              -8010
#define		FG_REGISTER_UPDATE_FAILED           -8020
#define		FG_DEVICE_IO_ERROR                  -9000
#define		FG_INVALID_CONFIG_REGION            -9001
#define		FG_DEVICE_REMOVED                   -9002
#define		FG_PARAMETER_NOT_IN_FILE            -9003
/**
  * \brief applets microEnable III
*/
#define SINGLE_AREA_GRAY                        0x10
#define SINGLE_AREA_2DSHADING                   0x11
#define DUAL_AREA_GRAY                          0x20
#define SINGLE_AREA_BAYER                       0x30
#define DUAL_AREA_BAYER                         0x31
#define SINGLE_AREA_GRAY_SHADING                0x40
#define SDRAM_ACCESS                            0x41
#define SINGLE_LINE_GRAY                        0x50
#define SINGLE_LINE_RGB                         0x60
#define DUAL_LINE_RGB                           0x61
#define DUAL_LINE_RGB_SHADING                   0x62
#define DUAL_LINE_GRAY                          0x70
#define VISIGLAS                                0x80
#define TRUMPFINESS                             0x81
#define SOUDRONIC                               0x82
#define SINGLEHIGHPRECISION                     0x83
#define SINGLE_AREA_GRAY_OFFSET                 0x84
#define SINGLE_AREA_HSI                         0x90
#define SINGLE_AREA_RGB                         0xa0
#define DUAL_AREA_RGB                           0xb0
#define SINGLE_AREA_RGB_SEPARATION              0xb1
#define MEDIUM_LINE_RGB                         0xb2
#define MEDIUM_LINE_GRAY                        0xb3
#define SINGLE_FAST_CONFIG                      0xb5
#define FASTCONFIG_SINGLE_AREA_GRAY SINGLE_FAST_CONFIG

#define SINGLE_AREA_GRAY_XXL                    0x110
#define SINGLE_AREA_2DSHADING_XXL               0x111
#define DUAL_AREA_GRAY_XXL                      0x120
#define SINGLE_AREA_BAYER_XXL                   0x130
#define DUAL_AREA_BAYER_XXL                     0x131
#define SINGLE_AREA_GRAY_SHADING_XXL            0x140
#define SDRAM_ACCESS_XXL                        0x141
#define SINGLE_LINE_GRAY_XXL                    0x150
#define SINGLE_LINE_RGB_XXL                     0x160
#define DUAL_LINE_RGB_XXL                       0x161
#define DUAL_LINE_RGB_SHADING_XXL               0x162
#define DUAL_LINE_GRAY_XXL                      0x170
#define SINGLE_AREA_HSI_XXL                     0x190
#define SINGLE_AREA_RGB_XXL                     0x1a0
#define DUAL_AREA_RGB_XXL                       0x1b0
#define SINGLE_AREA_RGB_SEPARATION_XXL          0x1b1
#define MEDIUM_LINE_RGB_XXL                     0x1b2
#define MEDIUM_LINE_GRAY_XXL                    0x1b3
#define MEDIUM_AREA_GRAY_XXL                    0x1b4
#define MEDIUM_AREA_RGB_XXL                     0x1b5
#define SINGLE_AREA_BAYER12_XXL                 0x1c0
#define DUAL_AREA_GRAY12_XXL                    0x1d0
#define SINGLE_LINE_GRAY12_XXL                  0x1d1
#define DUAL_AREA_RGB36_XXL                     0x1d2
#define DUAL_LINE_GRAY12_XXL                    0x1d3
#define MEDIUM_LINE_GRAY12_XXL                  0x1d4
#define SINGLE_AREA_GRAY12_XXL                  0x1d5
#define DUAL_LINE_RGB36_XXL                     0x1d6
#define SINGLE_AREA_RGB36_XXL                   0x1d7
#define SINGLE_LINE_RGB36_XXL                   0x1d8
#define DUAL_AREA_BAYER12_XXL                   0x1d9
#define SINGLE_AREA_2DSHADING12_XXL             0x1da
#define SINGLE_LINE_RGB24_XXL                   0x1db

#define LSC1020XXL                              0x500
#define LSC1020JPGXXL                           0x501
#define CLSC2050                                0x502
#define CLSC2050JPGXXL                          0x503
#define SEQUENCE_EXTRACTOR                      0x510
#define SAG_COMPRESSION                         0x520
#define MEDIUM_LINE_GRAY_FIR_XXL                0x530
#define DUAL_LINE_RGB_SORTING_XXL               0x540
#define SINGLE_LINE_GRAY_2X12_XXL               0x550
#define MEDIUM_LINE_GRAY12                      0x560
#define SINGLE_LINE_RGB36PIPELINE2_XXL          0x570
#define DUAL_AREA_GRAY_16                       0x580

#define DUAL_AREA_GRAY16_ME4BASEX1              0xa400010
#define DUAL_AREA_RGB48_ME4BASEX1               0xa400020
#define DUAL_LINE_GRAY16_ME4BASEX1              0xa400030
#define DUAL_LINE_RGB48_ME4BASEX1               0xa400040
#define MEDIUM_AREA_GRAY16_ME4BASEX1            0xa400050
#define MEDIUM_AREA_RGB36_ME4BASEX1             0xa400060
#define MEDIUM_LINE_GRAY16_ME4BASEX1            0xa400070
#define MEDIUM_LINE_RGB36_ME4BASEX1             0xa400080

#define DUAL_AREA_BAYER12_ME4FULLX1             0xa410010
#define DUAL_AREA_GRAY16_ME4FULLX1              0xa410020
#define DUAL_AREA_RGB48_ME4FULLX1               0xa410030
#define DUAL_LINE_GRAY16_ME4FULLX1              0xa410040
#define DUAL_LINE_RGB30_ME4FULLX1               0xa410050
#define FULL_AREA_GRAY8_ME4FULLX1               0xa410060
#define FULL_LINE_GRAY8_ME4FULLX1               0xa410070
#define MEDIUM_AREA_GRAY16_ME4FULLX1            0xa410080
#define MEDIUM_AREA_RGB36_ME4FULLX1             0xa410090
#define MEDIUM_LINE_GRAY16_ME4FULLX1            0xa4100a0
#define MEDIUM_LINE_RGB36_ME4FULLX1             0xa4100b0
#define SINGLE_AREA_BAYERHQ_ME4FULLX1           0xa4100c0
#define SINGLE_AREA_GRAY2DSHADING_ME4FULLX1     0xa4100d0

#define DUAL_AREA_BAYER12_ME4FULLX4             0xa440010
#define DUAL_AREA_GRAY16_ME4FULLX4              0xa440020
#define DUAL_AREA_RGB48_ME4FULLX4               0xa440030
#define DUAL_LINE_GRAY16_ME4FULLX4              0xa440040
#define DUAL_LINE_RGB30_ME4FULLX4               0xa440050
#define FULL_AREA_GRAY8_ME4FULLX4               0xa440060
#define FULL_LINE_GRAY8_ME4FULLX4               0xa440070
#define MEDIUM_AREA_GRAY16_ME4FULLX4            0xa440080
#define MEDIUM_AREA_RGB36_ME4FULLX4             0xa440090
#define MEDIUM_LINE_GRAY16_ME4FULLX4            0xa4400a0
#define MEDIUM_LINE_RGB36_ME4FULLX4             0xa4400b0
#define SINGLE_AREA_BAYERHQ_ME4FULLX4           0xa4400c0
#define SINGLE_AREA_GRAY2DSHADING_ME4FULLX4     0xa4400d0

#define QUAD_AREA_BAYER24_ME4GBEX4              0xe440010
#define QUAD_AREA_GRAY16_ME4GBEX4               0xe440020
#define	QUAD_AREA_RG24_ME4GBEX4                 0xe440030
#define QUAD_AREA_RGB48_ME4GBEX4                0xe440040
#define QUAD_AREA_GRAY8_ME4GBEX4                0xe440050
#define QUAD_LINE_GRAY16_ME4GBEX4               0xe440060
#define QUAD_LINE_RGB24_ME4GBEX4                0xe440070
#define QUAD_LINE_GRAY8_ME4GBEX4                0xe440080

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
    FG_APC_DEFAULTS              = 0x0,  /**< use APC default mode (handler is called for every transfer, grabbing is stopped on timeout) */
    FG_APC_BATCH_FRAMES          = 0x1,  /**< when multiple images arrived only call APC handler once with the highest image number */
    FG_APC_IGNORE_TIMEOUTS       = 0x2,  /**< ignore image timeouts */
    FG_APC_IGNORE_APCFUNC_RETURN = 0x4,  /**< do not stop acquisition if apcfunc returns != 0 */
    FG_APC_IGNORE_STOP           = 0x8,  /**< do not stop the APC handler if acquisition stops */
    FG_APC_HIGH_PRIORITY         = 0x10, /**< increase the priority of the APC thread */
    FG_APC_DELIVER_ERRORS        = 0x20  /**< deliver any errors to APC handler first */
};

/**
 * \brief control flags for Fg_registerApcHandler()
 */
enum FgApcControlFlags {
	FG_APC_CONTROL_BASIC = 0    /**< default behaviour */
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
	FG_EVENT_DEFAULT_FLAGS = 0,  /**< default behaviour (i.e. none of the other flags)*/
	FG_EVENT_BATCHED       = 0x1 /**< if more than one event is recorded only return once*/
};

/**
 * \brief the status flags passed in notify member of struct fg_event_info
 */
enum FgEventNotifiers {
	FG_EVENT_NOTIFY_JOINED    = 0x1, /**< events were joined in mode FG_EVENT_BATCHED */
	FG_EVENT_NOTIFY_TIMESTAMP = 0x2, /**< timestamp contains valid data */
	FG_EVENT_NOTIFY_PAYLOAD   = 0x4, /**< the event has payload, use the payload member of the union */
	FG_EVENT_NOTIFY_LOST      = 0x8  /**< there was data lost before this event */
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

/**
 * interface of user defined asynchronous notification handler
 *
 * If this function returns a value != 0 the notification handler is automatically unregistered.
 *
 * \see Fg_registerAsyncNotifyCallback()
 *
 * \param boardIndex the board index (will be -1 on system notifications)
 * \param notification the notification
 * \param pl 32-bit parameter, or low part of 64-bit parameter
 *        depends on notification
 * \param ph 32-bit parameter, or high part of 64-bit parameter
 *        depends on notification
 * \param context the context data that was supplied through Fg_registerAsyncNotificationCallback()
 *
 */
typedef int (*Fg_AsyncNotifyFunc_t)(int boardIndex, unsigned long notification, unsigned long pl, unsigned long ph, void * context);

/* ACQ APPLETS --- PARAMETER IDS */
/* 
* \brief	Acquisition Applets: Parameter IDs
*/
#define FG_FILLLEVEL                                     110086                 /**< Show the filllevel of the fpga*/
#define FG_OVERFLOW                                      110087                 /**< */
#define FG_NOISEFILTER                                   110016                 /**< */
#define FG_LUT_TYPE                                      110017                 /**< */
#define FG_LUT_CUSTOM_FILE                               300000                 /**< */
#define FG_LUT_SAVE_FILE                                 110021                 /**< */
#define FG_PROCESSING_GAIN                               300002                 /**< */
#define FG_PROCESSING_GAMMA                              300003                 /**< */
#define FG_PROCESSING_OFFSET                             300004                 /**< */
#define FG_PROCESSING_INVERT                             300005                 /**< */
#define FG_LUT_IMPLEMENTATION_TYPE                       300006                 /**< */
#define FG_SHADING_GAIN_ENABLE                           300100                 /**< */
#define FG_SHADING_GRAY_FILENAME                         300101                 /**< */
#define FG_SHADING_OFFSET_ENABLE                         300102                 /**< */
#define FG_SHADING_BLACK_FILENAME                        300103                 /**< */
#define FG_SHADING_GAIN_CORRECTION_MODE                  300106                 /**< */
#define FG_SHADING_APPLY_SETTINGS                        300107                 /**< */
#define FG_SHADING_GAIN_NORMALIZATION_VALUE              300108                 /**< */
#define FG_AREATRIGGERMODE                               300200                 /**< */
#define FG_TRIGGERSTATE                                  300201                 /**< */
#define FG_TRIGGER_FRAMESPERSECOND                       FG_FRAMESPERSEC        /**< */
#define FG_TRIGGER_EXCEEDED_PERIOD_LIMITS                300202                 /**< */
#define FG_TRIGGER_EXCEEDED_PERIOD_LIMITS_CLEAR          300203                 /**< */
#define FG_TRIGGERIN_DEBOUNCE                            300204                 /**< */
#define FG_TRIGGERIN_SRC                                 FG_TRIGGERINSRC        /**< */
#define FG_TRIGGERIN_POLARITY                            FG_TRIGGERINPOLARITY   /**< */
#define FG_SOFTWARETRIGGER_IS_BUSY                       110075                 /**< */
#define FG_TRIGGERIN_DOWNSCALE                           300205                 /**< */
#define FG_TRIGGERIN_DOWNSCALE_PHASE                     300206                 /**< */
#define FG_TRIGGERIN_STATS_PULSECOUNT                    300207                 /**< */
#define FG_TRIGGERIN_STATS_PULSECOUNT_CLEAR              300208                 /**< */
#define FG_TRIGGERIN_STATS_FREQUENCY                     300209                 /**< */
#define FG_TRIGGERIN_STATS_MINFREQUENCY                  300210                 /**< */
#define FG_TRIGGERIN_STATS_MAXFREQUENCY                  300211                 /**< */
#define FG_TRIGGERIN_STATS_MINMAXFREQUENCY_CLEAR         300212                 /**< */
#define FG_TRIGGER_MULTIPLY_PULSES                       300213                 /**< */
#define FG_TRIGGERQUEUE_MODE                             300214                 /**< */
#define FG_TRIGGERQUEUE_FILLLEVEL                        300215                 /**< */
#define FG_TRIGGER_PULSEFORMGEN0_DOWNSCALE               300216                 /**< */
#define FG_TRIGGER_PULSEFORMGEN0_DOWNSCALE_PHASE         300217                 /**< */
#define FG_TRIGGER_PULSEFORMGEN0_DELAY                   300218                 /**< */
#define FG_TRIGGER_PULSEFORMGEN0_WIDTH                   300219                 /**< */
#define FG_TRIGGER_PULSEFORMGEN1_DOWNSCALE               300220                 /**< */
#define FG_TRIGGER_PULSEFORMGEN1_DOWNSCALE_PHASE         300221                 /**< */
#define FG_TRIGGER_PULSEFORMGEN1_DELAY                   300222                 /**< */
#define FG_TRIGGER_PULSEFORMGEN1_WIDTH                   300223                 /**< */
#define FG_TRIGGER_PULSEFORMGEN2_DOWNSCALE               300224                 /**< */
#define FG_TRIGGER_PULSEFORMGEN2_DOWNSCALE_PHASE         300225                 /**< */
#define FG_TRIGGER_PULSEFORMGEN2_DELAY                   300226                 /**< */
#define FG_TRIGGER_PULSEFORMGEN2_WIDTH                   300227                 /**< */
#define FG_TRIGGER_PULSEFORMGEN3_DOWNSCALE               300228                 /**< */
#define FG_TRIGGER_PULSEFORMGEN3_DOWNSCALE_PHASE         300229                 /**< */
#define FG_TRIGGER_PULSEFORMGEN3_DELAY                   300230                 /**< */
#define FG_TRIGGER_PULSEFORMGEN3_WIDTH                   300231                 /**< */
#define FG_TRIGGEROUT_SELECT0                            300232                 /**< */
#define FG_TRIGGEROUT_SELECT1                            300233                 /**< */
#define FG_TRIGGEROUT_SELECT2                            300234                 /**< */
#define FG_TRIGGEROUT_SELECT3                            300235                 /**< */
#define FG_TRIGGEROUT_STATS_SOURCE                       300236                 /**< */
#define FG_TRIGGEROUT_STATS_PULSECOUNT                   300237                 /**< */
#define FG_TRIGGEROUT_STATS_PULSECOUNT_CLEAR             300238                 /**< */
#define FG_TRIGGERIN_EVENT_SRC                           300239                 /**< */
#define FG_TRIGGER_QUEUE_FILLLEVEL_EVENT_ON_THRESHOLD    300240                 /**< */
#define FG_TRIGGER_QUEUE_FILLLEVEL_EVENT_OFF_THRESHOLD   300241                 /**< */
#define FG_TRIGGER_OUTPUT_EVENT_SELECT                   300242                 /**< */
#define FG_TRIGGERIN_BYPASS_SRC                          300243                 /**< */
#define FG_TRIGGEROUT_SELECT4                            300244                 /**< */
#define FG_TRIGGEROUT_SELECT5                            300245                 /**< */
#define FG_TRIGGEROUT_SELECT6                            300246                 /**< */
#define FG_TRIGGEROUT_SELECT7                            300247                 /**< */
#define FG_LUT_BASEPOINTS                                300012                 /**< */
#define FG_LUT_IN_BITS                                   300007                 /**< */
#define FG_LUT_OUT_BITS                                  300008                 /**< */
#define FG_LUT_VALUE                                     300001                 /**< */
#define FG_LUT_VALUE_RED                                 300009                 /**< */
#define FG_LUT_VALUE_GREEN                               300010                 /**< */
#define FG_LUT_VALUE_BLUE                                300011                 /**< */
#define FG_SHADING_DEAD_PIXEL_INTERPOLATION_ENABLE       300104                 /**< */
#define FG_SHADING_DEAD_PIXEL_INTERPOLATION_THRESHOLD    300105                 /**< */
#define FG_MISSING_CAMERA_FRAME_RESPONSE                 300248                 /**< */
#define FG_MISSING_CAMERA_FRAME_RESPONSE_CLEAR           300249                 /**< */
#define FG_TRIGGERCC_SELECT0                             300250                 /**< */
#define FG_TRIGGERCC_SELECT1                             300251                 /**< */
#define FG_TRIGGERCC_SELECT2                             300252                 /**< */
#define FG_TRIGGERCC_SELECT3                             300253                 /**< */
#define FG_TRIGGER_LEGACY_MODE                           300254                 /**< Enables a compatibility mode (CL-Area Applets only).*/
#define FG_SC_SUBSENSORCOUNT                             110118                 /**< Number of sub-sensors the sensor is divided into.*/
#define FG_SC_SENSORLENGTH                               110119                 /**< The total number of pixel transferred from the sensor to the frame grabber.*/
#define FG_SC_TAPCOUNT                                   110120                 /**< The total number of taps the camera uses to transfer the pixel from the sensor to the frame grabber. */
#define FG_SC_ROTATEDSENSOR                              110121                 /**< Set to FG_TRUE for rotated sensors i.e. read from right to left e.g. reverse movement direction.*/
#define FG_SC_READOUTDIRECTION                           110122                 /**< The readoutdirection of each subsensor. Forward = 0 = left-to-right. Backward = 1 = right-to-left */
#define FG_SC_PIXELORDER                                 110123                 /**< Sub-sensor readout order if multiple taps are used for one sub-sensor.*/
#define FG_SC_UPDATESCHEME                               110124                 /**< Update the frame grabber with the sensor correction parameter values. Error for illegal settings.*/
#define FG_IMAGEHEIGHT                                   110061                 /**< Height of image in case of some extra data lines.*/
#define FG_SHAFTENCODERMODE                              110065                 /**< Operation mode of the shaft encoder.*/
#define FG_IMGTRIGGER_IS_BUSY                            110066                 /**< Indicates if a requested frame from the camera has not been completely send to the frame grabber.*/
#define FG_IMGTRIGGERDEBOUNCING                          110064                 /**< Debouncing time for image trigger inputs.*/

#define FG_TRIGGERCAMERA_GPO0                            200330                 /**< Select the trigger source for the GPO0 output to the camera. */
#define FG_TRIGGERCAMERA_GPO1                            200331                 /**< Select the trigger source for the GPO1 output to the camera. */
#define FG_TRIGGERCAMERA_GPO2                            200332                 /**< Select the trigger source for the GPO2 output to the camera. */
#define FG_TRIGGERCAMERA_GPO3                            200333                 /**< Select the trigger source for the GPO3 output to the camera. */
#define FG_TRIGGERCAMERA_GPO4                            200334                 /**< Select the trigger source for the GPO4 output to the camera. */
#define FG_TRIGGERCAMERA_GPO5                            200335                 /**< Select the trigger source for the GPO5 output to the camera. */
#define FG_TRIGGERCAMERA_GPO6                            200336                 /**< Select the trigger source for the GPO6 output to the camera. */
#define FG_TRIGGERCAMERA_GPO7                            200337                 /**< Select the trigger source for the GPO7 output to the camera. */
#define FG_TRIGGERCAMERA_OUT_SELECT                      200338                 /**< Select the trigger source for the trigger camera output. */
#define FG_TRIGGERCAMERA_SOURCE                          FG_TRIGGERCAMERA_OUT_SELECT /**< Select the trigger source for the trigger camera output. */
#define FG_TRIGGERCAMERA_POLARITY                        200354                 /**< Polarity of the camera trigger output. */
#define FG_TRIGGERCAMERA_INPUT_MONITOR                   200339                 /**< Shows the trigger value send from the camera to the frame grabber. */
#define FG_TRIGGERCAMERA_GPI_MONITOR                     200340                 /**< General purpose channel value from camera to frame grabber. */

#define FG_SYSTEMMONITOR_FPGA_TEMPERATURE                200341                 /**< Shows the fpga Temperature */
#define FG_SYSTEMMONITOR_FPGA_VCC_INT                    200342                 /**< Shows the fpga internal supply voltage*/
#define FG_SYSTEMMONITOR_FPGA_VCC_AUX                    200343                 /**< Shows the fpga auxiliary supply voltage*/
#define FG_SYSTEMMONITOR_BOARD_POWER                     200344                 /**< Shows the fpga board power*/
#define FG_SYSTEMMONITOR_CXP_CHIP_TEMPERATURE            200345                 /**< Shows the CXP-Chip Temperature */
#define FG_SYSTEMMONITOR_RAM_CHIP_TEMPERATURE            200346                 /**< Shows the RAM-Chip Temperature */
#define FG_SYSTEMMONITOR_CXP_POWER_REGULATOR_TEMPERATURE 200347                 /**< Shows the CXP Power Regulator Temperature */
#define FG_SYSTEMMONITOR_POWER_REGULATOR_TEMPERATURE     200348                 /**< Shows the Power Regulator Temperature */
#define FG_SYSTEMMONITOR_FPGA_DNA                        200349                 /**< Shows the FPGA's DNA Signature */
#define FG_SYSTEMMONITOR_CHANNEL_CURRENT                 200350                 /**< Shows the CXP Link Current */
#define FG_SYSTEMMONITOR_CHANNEL_VOLTAGE                 200351                 /**< Shows the CXP Link Voltage */
#define FG_SYSTEMMONITOR_CHANNEL_STATE                   200353                 /**< Shows the CXP Link State as Enumerator */
#define FG_SC                                            110138                 /**< */
#define FG_SAMPLING_RATE                                 200365                 /**< Sampling rate for bilinear line applets. */
#define FG_PIXELFORMAT                                   200368                 /**< */
#define FG_CXP_TRIGGER_PACKET_MODE                       200369                 /**< CoaXPress Trigger Packed Mode. Allows non-CXP Standard use of the trigger. Check the documentation for more inoformation.*/
#define FG_SHADING_ENABLE                                300109                 /**< */
#define FG_SHAFTENCODER_COMPENSATION_ENABLE              200370                 /**< */
#define FG_SHAFTENCODER_COMPENSATION_COUNT               200371                 /**< */

#define FG_REVERSE_X                                        300110              /**< Line mirror for some applet. Activate with this parameter. Check documentation for further details */
#define FG_TAPGEOMETRY                                      300111              /**< Select the tap geometry according to the SFNC */
#define FG_VANTAGEPOINT                                     300112              /**< Select the vantage point according to the SFNC */
#define FG_SENSORWIDTH                                      200220              /**< Defines the sensor with i.e. transfer width from camera to grabber. */
#define FG_SENSORHEIGHT                                     200221              /**< Defines the sensor height i.e. transfer height from camera to grabber. */

#define FG_SYSTEMMONITOR_FPGA_VCC_BRAM 						200372				/**< */
#define FG_SYSTEMMONITOR_CURRENT_LINK_WIDTH 				200373				/**< */
#define FG_SYSTEMMONITOR_CURRENT_LINK_SPEED 				200374				/**< */
#define FG_SYSTEMMONITOR_PCIE_LINK_GEN2_CAPABLE 			200375				/**< */
#define FG_SYSTEMMONITOR_PCIE_LINK_PARTNER_GEN2_CAPABLE 	200376				/**< */
#define FG_SYSTEMMONITOR_PCIE_TRAINED_PAYLOAD_SIZE 			200377				/**< */
#define FG_SYSTEMMONITOR_EXTENSION_CONNECTOR_PRESENT 		200378				/**< */
#define FG_SYSTEMMONITOR_POCL_STATE_PORT_A 					200379				/**< */
#define FG_SYSTEMMONITOR_POCL_STATE_PORT_B 					200380				/**< */
#define FG_ALTERNATIVE_BOARD_DETECTION	 					200381				/**< System state of the alternative frame grabber board detection */
#define FG_CUSTOM_BIT_SHIFT_RIGHT 							200396				/**< */
#define FG_LINETRIGGERDEBOUNCING 							110063				/**< */	
#define FG_IMGTRIGGER_ASYNC_HEIGHT 							110067				/**< */
#define FG_GPI 												140100				/**< */
#define FG_FRONT_GPI 										200382				/**< */
#define FG_TRIGGERIN_STATS_SOURCE 							200398				/**< */
#define FG_TRIGGERIN_STATS_POLARITY 						200399				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_0 							200384				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_1 							200385				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_2 							200386				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_3 							200387				/**< */
#define FG_TRIGGEROUT_SELECT_FRONT_GPO_0 					200392				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_4 							200388				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_5 							200389				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_6 							200390				/**< */
#define FG_TRIGGEROUT_SELECT_GPO_7 							200391				/**< */
#define FG_TRIGGEROUT_SELECT_FRONT_GPO_1 					200393				/**< */	
#define FG_OUTPUT_APPEND_NUMBER							 	200372				/**< */
#define FG_LINE_PAYLOAD_SIZE								200400				/**< */
#define FG_CLHS_TRIGGER_PULSE_MESSAGE						200401				/**< */

#define FG_TRIGGEROUT_GPO_0_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_0
#define FG_TRIGGEROUT_GPO_0_POLARITY                       200410
#define FG_TRIGGEROUT_GPO_1_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_1
#define FG_TRIGGEROUT_GPO_1_POLARITY                       200411
#define FG_TRIGGEROUT_GPO_2_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_2
#define FG_TRIGGEROUT_GPO_2_POLARITY                       200412
#define FG_TRIGGEROUT_GPO_3_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_3
#define FG_TRIGGEROUT_GPO_3_POLARITY                       200413
#define FG_TRIGGEROUT_GPO_4_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_4
#define FG_TRIGGEROUT_GPO_4_POLARITY                       200414
#define FG_TRIGGEROUT_GPO_5_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_5
#define FG_TRIGGEROUT_GPO_5_POLARITY                       200415
#define FG_TRIGGEROUT_GPO_6_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_6
#define FG_TRIGGEROUT_GPO_6_POLARITY                       200416
#define FG_TRIGGEROUT_GPO_7_SOURCE                         FG_TRIGGEROUT_SELECT_GPO_7
#define FG_TRIGGEROUT_GPO_7_POLARITY                       200417
#define FG_TRIGGEROUT_FRONT_GPO_0_SOURCE                   FG_TRIGGEROUT_SELECT_FRONT_GPO_0
#define FG_TRIGGEROUT_FRONT_GPO_0_POLARITY                 200418
#define FG_TRIGGEROUT_FRONT_GPO_1_SOURCE                   FG_TRIGGEROUT_SELECT_FRONT_GPO_1
#define FG_TRIGGEROUT_FRONT_GPO_1_POLARITY                 200419

#define FG_SIGNAL_ANALYZER_0_SOURCE                        200420     
#define FG_SIGNAL_ANALYZER_0_POLARITY                      200421     
#define FG_SIGNAL_ANALYZER_0_PERIOD_CURRENT                200422     
#define FG_SIGNAL_ANALYZER_0_PERIOD_MAX                    200423     
#define FG_SIGNAL_ANALYZER_0_PERIOD_MIN                    200424     
#define FG_SIGNAL_ANALYZER_0_PULSE_COUNT                   200425     
#define FG_SIGNAL_ANALYZER_1_SOURCE                        200430     
#define FG_SIGNAL_ANALYZER_1_POLARITY                      200431     
#define FG_SIGNAL_ANALYZER_1_PERIOD_CURRENT                200432     
#define FG_SIGNAL_ANALYZER_1_PERIOD_MAX                    200433     
#define FG_SIGNAL_ANALYZER_1_PERIOD_MIN                    200434     
#define FG_SIGNAL_ANALYZER_1_PULSE_COUNT                   200435     
#define FG_SIGNAL_ANALYZER_PULSE_COUNT_DIFFERENCE          200439     
#define FG_SIGNAL_ANALYZER_CLEAR                           200438     
#define FG_CUSTOM_SIGNAL_EVENT_0_SOURCE                    200440     
#define FG_CUSTOM_SIGNAL_EVENT_0_POLARITY                  200441     
#define FG_CUSTOM_SIGNAL_EVENT_1_SOURCE                    200442     
#define FG_CUSTOM_SIGNAL_EVENT_1_POLARITY                  200443     



/* ACQ APPLETS --- SYMBOLIC ENUMERATION VALUES */

/* 
* \brief	Acquisition Applets: enumeration values
*/

#define LUT_TYPE_PROCESSING                         3   /**< */
#define LUT_TYPE_CUSTOM                             0   /**< */
#define LUT_IMPLEMENTATION_FULL_LUT                 0   /**< */
#define LUT_IMPLEMENTATION_KNEELUT                  1   /**< */
#define FG_MAX_VALUE                                0   /**< */
#define FG_MEAN_VALUE                               1   /**< */
#define FG_MAX_RANGE                                2   /**< */
#define FG_CUSTOM_VALUE                             3   /**< */
#define FG_APPLY                                    1   /**< */
#define ATM_GENERATOR                               1   /**< */
#define ATM_EXTERNAL                                2   /**< */
#define ATM_SOFTWARE                                4   /**< */
#define TS_ACTIVE                                   0   /**< */
#define TS_ASYNC_STOP                               1   /**< */
#define TS_SYNC_STOP                                2   /**< */
#define FG_ONE                                      1   /**< */
#define FG_ZERO                                     0   /**< */
#define IS_BUSY                                     1   /**< */
#define IS_NOT_BUSY                                 0   /**< */

/**
 * \brief Definition for CameraLink CameraControl Signals
 */
 
enum CCsel { // this is the basic CC signal value set
	CC_EXSYNC             = 0,                /**< use exposure pulse at CL CC line */
	CC_PRESCALER          = 1,                /**< use secondary exposure pulse at CL CC line */
	CC_HDSYNC             = CC_PRESCALER,     /**< use secondary exposure pulse at CL CC line */
	CC_EXSYNC2            = CC_PRESCALER,     /**< use secondary exposure pulse at CL CC line */
	CC_STROBEPULSE        = 2,                /**< use strobes pulse at CL CC line */
	CC_CLK                = 3,                /**< use clock pulse at CL CC line */
	CC_GND                = 4,                /**< use ground corresponding to low signal at CL CC line */
	CC_VCC                = 5,                /**< use Vcc corresponding to high signal at CL CC line */
	CC_NOT_EXSYNC         = 6,                /**< use inverted exposure pulse at CL CC line */
	CC_NOT_PRESCALER      = 7,                /**< use inverted secondary exposure pulse at CL CC line */
	CC_NOT_HDSYNC         = CC_NOT_PRESCALER, /**< use inverted secondary exposure pulse at CL CC line */
	CC_NOT_EXSYNC2        = CC_NOT_PRESCALER, /**< use inverted secondary exposure pulse at CL CC line */
	CC_NOT_STROBEPULSE    = 8,                /**< use inverted strobes pulse at CL CC line */
    FG_OTHER              = -1                /**< Other signal. Value cannot be represented with the given values of the enumerator.*/
};

enum SignalSelectLine { // this is the signal select enum used for example in marathon line applets 
    FG_SIGNAL_CAM0_EXSYNC            = 2000,  /**< Cam 0 exsync signal to trigger the camera */
    FG_SIGNAL_CAM0_EXSYNC2           = 2001,  /**< Cam 0 exsync 2 signal (delayed exsync) */
    FG_SIGNAL_CAM0_FLASH             = 2002,  /**< Cam 0 flash signal */
    FG_SIGNAL_CAM0_LVAL              = 2007,  /**< Cam 0 line valid. Camera line transfer active*/
    FG_SIGNAL_CAM0_FVAL              = 2008,  /**< Cam 0 frame valid. Frame transfer time after trigger module*/
    FG_SIGNAL_CAM0_LINE_START        = 2100,  /**< Cam 0 line start pulse*/
    FG_SIGNAL_CAM0_LINE_END          = 2101,  /**< Cam 0 line end pulse*/
    FG_SIGNAL_CAM0_FRAME_START       = 2102,  /**< Cam 0 frame start pulse*/
    FG_SIGNAL_CAM0_FRAME_END         = 2103,  /**< Cam 0 frame end pulse*/
    FG_SIGNAL_CAM1_EXSYNC            = 2010,  /**< Cam 1 exsync signal to trigger the camera */
    FG_SIGNAL_CAM1_EXSYNC2           = 2011,  /**< Cam 1 exsync 2 signal (delayed exsync) */
    FG_SIGNAL_CAM1_FLASH             = 2012,  /**< Cam 1 flash signal */
    FG_SIGNAL_CAM1_LVAL              = 2017,  /**< Cam 1 line valid. Camera line transfer active*/
    FG_SIGNAL_CAM1_FVAL              = 2018,  /**< Cam 1 frame valid. Frame transfer time after trigger module*/
    FG_SIGNAL_CAM1_LINE_START        = 2110,  /**< Cam 1 line start pulse*/
    FG_SIGNAL_CAM1_LINE_END          = 2111,  /**< Cam 1 line end pulse*/
    FG_SIGNAL_CAM1_FRAME_START       = 2112,  /**< Cam 1 frame start pulse*/
    FG_SIGNAL_CAM1_FRAME_END         = 2113,  /**< Cam 1 frame end pulse*/
    FG_SIGNAL_CAM2_EXSYNC            = 2020,  /**< Cam 2 exsync signal to trigger the camera */
    FG_SIGNAL_CAM2_EXSYNC2           = 2021,  /**< Cam 2 exsync 2 signal (delayed exsync) */
    FG_SIGNAL_CAM2_FLASH             = 2022,  /**< Cam 2 flash signal */
    FG_SIGNAL_CAM2_LVAL              = 2027,  /**< Cam 2 line valid. Camera line transfer active*/
    FG_SIGNAL_CAM2_FVAL              = 2028,  /**< Cam 2 frame valid. Frame transfer time after trigger module*/
    FG_SIGNAL_CAM2_LINE_START        = 2120,  /**< Cam 2 line start pulse*/
    FG_SIGNAL_CAM2_LINE_END          = 2121,  /**< Cam 2 line end pulse*/
    FG_SIGNAL_CAM2_FRAME_START       = 2122,  /**< Cam 2 frame start pulse*/
    FG_SIGNAL_CAM2_FRAME_END         = 2123,  /**< Cam 2 frame end pulse*/
    FG_SIGNAL_CAM3_EXSYNC            = 2030,  /**< Cam 3 exsync signal to trigger the camera */
    FG_SIGNAL_CAM3_EXSYNC2           = 2031,  /**< Cam 3 exsync 2 signal (delayed exsync) */
    FG_SIGNAL_CAM3_FLASH             = 2032,  /**< Cam 3 flash signal */
    FG_SIGNAL_CAM3_LVAL              = 2037,  /**< Cam 3 line valid. Camera line transfer active*/
    FG_SIGNAL_CAM3_FVAL              = 2038,  /**< Cam 3 frame valid. Frame transfer time after trigger module*/
    FG_SIGNAL_CAM3_LINE_START        = 2130,  /**< Cam 3 line start pulse*/
    FG_SIGNAL_CAM3_LINE_END          = 2131,  /**< Cam 3 line end pulse*/
    FG_SIGNAL_CAM3_FRAME_START       = 2132,  /**< Cam 3 frame start pulse*/
    FG_SIGNAL_CAM3_FRAME_END         = 2133,  /**< Cam 3 frame end pulse*/
    FG_SIGNAL_GPI_0                  = 1001,  /**< GPI 3 signal*/
    FG_SIGNAL_GPI_1                  = 1011,  /**< GPI 1 signal*/
    FG_SIGNAL_GPI_2                  = 1021,  /**< GPI 2 signal*/
    FG_SIGNAL_GPI_3                  = 1031,  /**< GPI 3 signal*/
    FG_SIGNAL_GPI_4                  = 1041,  /**< GPI 4 signal*/
    FG_SIGNAL_GPI_5                  = 1051,  /**< GPI 5 signal*/
    FG_SIGNAL_GPI_6                  = 1061,  /**< GPI 6 signal*/
    FG_SIGNAL_GPI_7                  = 1071,  /**< GPI 7 signal*/
    FG_SIGNAL_FRONT_GPI_0            = 1081,  /**< Front GPI 0 signal*/
    FG_SIGNAL_FRONT_GPI_1            = 1091,  /**< Front GPI 1 signal*/
    FG_SIGNAL_FRONT_GPI_2            = 1101,  /**< Front GPI 2 signal*/
    FG_SIGNAL_FRONT_GPI_3            = 1111   /**< Front GPI 3 signal*/
};

enum CcSignalMappingArea { // this is the enumerator for adv. acq. area applets
	CC_PULSEGEN0            = 0,    /**< Pulse Generator 0 for Camera Control Signal Mapping */
	CC_PULSEGEN1            = 1,    /**< Pulse Generator 1 for Camera Control Signal Mapping */
	CC_PULSEGEN2            = 2,    /**< Pulse Generator 2 for Camera Control Signal Mapping */
	CC_PULSEGEN3            = 3,    /**< Pulse Generator 3 for Camera Control Signal Mapping */
	CC_NOT_PULSEGEN0        = 6,    /**< Not Pulse Generator 0 for Camera Control Signal Mapping */
	CC_NOT_PULSEGEN1        = 7,    /**< Not Pulse Generator 1 for Camera Control Signal Mapping */
	CC_NOT_PULSEGEN2        = 8,    /**< Not Pulse Generator 2 for Camera Control Signal Mapping */
	CC_NOT_PULSEGEN3        = 9,    /**< Not Pulse Generator 3 for Camera Control Signal Mapping */
	CC_INPUT_BYPASS         = 10,   /**< Use a possible bypass as CC signal */
	CC_NOT_INPUT_BYPASS     = 11    /**< Use a possible bypass as CC signal */
};

enum CcSignalMappingLineExtended { // this is the extension of enum CCSel
	CC_GPI_0                = 1001, /**< General Purpose Input 0 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_0            = 1000, /**< General Purpose Input 0 NOT Select for Camera Control Signal Mapping */
	CC_GPI_1                = 1011, /**< General Purpose Input 1 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_1            = 1010, /**< General Purpose Input 1 NOT Select for Camera Control Signal Mapping */
	CC_GPI_2                = 1021, /**< General Purpose Input 2 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_2            = 1020, /**< General Purpose Input 2 NOT Select for Camera Control Signal Mapping */
	CC_GPI_3                = 1031, /**< General Purpose Input 3 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_3            = 1030, /**< General Purpose Input 3 NOT Select for Camera Control Signal Mapping */
	CC_GPI_4                = 1041, /**< General Purpose Input 4 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_4            = 1040, /**< General Purpose Input 4 NOT Select for Camera Control Signal Mapping */
	CC_GPI_5                = 1051, /**< General Purpose Input 5 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_5            = 1050, /**< General Purpose Input 5 NOT Select for Camera Control Signal Mapping */
	CC_GPI_6                = 1061, /**< General Purpose Input 6 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_6            = 1060, /**< General Purpose Input 6 NOT Select for Camera Control Signal Mapping */
	CC_GPI_7                = 1071, /**< General Purpose Input 7 Select for Camera Control Signal Mapping */
	CC_NOT_GPI_7            = 1070, /**< General Purpose Input 7 NOT Select for Camera Control Signal Mapping */
	CC_FRONT_GPI_0          = 1081, /**< General Purpose Front Input 0 Select for Camera Control Signal Mapping */
	CC_NOT_FRONT_GPI_0      = 1080, /**< General Purpose Front Input 0 NOT Select for Camera Control Signal Mapping */
	CC_FRONT_GPI_1          = 1091, /**< General Purpose Front Input 1 Select for Camera Control Signal Mapping */
	CC_NOT_FRONT_GPI_1      = 1090, /**< General Purpose Front Input 1 NOT Select for Camera Control Signal Mapping */
	CC_FRONT_GPI_2          = 1101, /**< General Purpose Front Input 2 Select for Camera Control Signal Mapping */
	CC_NOT_FRONT_GPI_2      = 1100, /**< General Purpose Front Input 2 NOT Select for Camera Control Signal Mapping */
	CC_FRONT_GPI_3          = 1111, /**< General Purpose Front Input 3 Select for Camera Control Signal Mapping */
	CC_NOT_FRONT_GPI_3      = 1110  /**< General Purpose Front Input 3 NOT Select for Camera Control Signal Mapping */
};

#define PULSEGEN0                                   0   /**< */
#define PULSEGEN1                                   1   /**< */
#define PULSEGEN2                                   2   /**< */
#define PULSEGEN3                                   3   /**< */
#define GND                                         4   /**< */
#define VCC                                         5   /**< */
#define NOT_PULSEGEN0                               6   /**< */
#define NOT_PULSEGEN1                               7   /**< */
#define NOT_PULSEGEN2                               8   /**< */
#define NOT_PULSEGEN3                               9   /**< */
#define CAM_A_PULSEGEN0                            50   /**< */
#define CAM_A_PULSEGEN1                            51   /**< */
#define CAM_A_PULSEGEN2                            52   /**< */
#define CAM_A_PULSEGEN3                            53   /**< */
#define CAM_A_NOT_PULSEGEN0                        60   /**< */
#define CAM_A_NOT_PULSEGEN1                        61   /**< */
#define CAM_A_NOT_PULSEGEN2                        62   /**< */
#define CAM_A_NOT_PULSEGEN3                        63   /**< */
#define CAM_B_PULSEGEN0                            54   /**< */
#define CAM_B_PULSEGEN1                            55   /**< */
#define CAM_B_PULSEGEN2                            56   /**< */
#define CAM_B_PULSEGEN3                            57   /**< */
#define CAM_B_NOT_PULSEGEN0                        64   /**< */
#define CAM_B_NOT_PULSEGEN1                        65   /**< */
#define CAM_B_NOT_PULSEGEN2                        66   /**< */
#define CAM_B_NOT_PULSEGEN3                        67   /**< */

#define CAM_C_PULSEGEN0                            68   /**< */
#define CAM_C_PULSEGEN1                            69   /**< */
#define CAM_C_PULSEGEN2                            70   /**< */
#define CAM_C_PULSEGEN3                            71   /**< */
#define CAM_C_NOT_PULSEGEN0                        76   /**< */
#define CAM_C_NOT_PULSEGEN1                        77   /**< */
#define CAM_C_NOT_PULSEGEN2                        78   /**< */
#define CAM_C_NOT_PULSEGEN3                        79   /**< */
#define CAM_D_PULSEGEN0                            72   /**< */
#define CAM_D_PULSEGEN1                            73   /**< */
#define CAM_D_PULSEGEN2                            74   /**< */
#define CAM_D_PULSEGEN3                            75   /**< */
#define CAM_D_NOT_PULSEGEN0                        80   /**< */
#define CAM_D_NOT_PULSEGEN1                        81   /**< */
#define CAM_D_NOT_PULSEGEN2                        82   /**< */
#define CAM_D_NOT_PULSEGEN3                        83   /**< */



#define INPUT_BYPASS                                10  /**< */
#define NOT_INPUT_BYPASS                            11  /**< */
#define BYPASS_GPI_0                                10  /**< */
#define NOT_BYPASS_GPI_0                            20  /**< */
#define BYPASS_GPI_1                                11  /**< */
#define NOT_BYPASS_GPI_1                            21  /**< */
#define BYPASS_GPI_2                                12  /**< */
#define NOT_BYPASS_GPI_2                            22  /**< */
#define BYPASS_GPI_3                                13  /**< */
#define NOT_BYPASS_GPI_3                            23  /**< */
#define BYPASS_GPI_4                                14  /**< */
#define NOT_BYPASS_GPI_4                            24  /**< */
#define BYPASS_GPI_5                                15  /**< */
#define NOT_BYPASS_GPI_5                            25  /**< */
#define BYPASS_GPI_6                                16  /**< */
#define NOT_BYPASS_GPI_6                            26  /**< */
#define BYPASS_GPI_7                                17  /**< */
#define NOT_BYPASS_GPI_7                            27  /**< */
#define BYPASS_FRONT_GPI_0                          30  /**< */
#define NOT_BYPASS_FRONT_GPI_0                      40  /**< */
#define BYPASS_FRONT_GPI_1                          31  /**< */
#define NOT_BYPASS_FRONT_GPI_1                      41  /**< */
#define BYPASS_FRONT_GPI_2                          32  /**< */
#define NOT_BYPASS_FRONT_GPI_2                      42  /**< */
#define BYPASS_FRONT_GPI_3                          33  /**< */
#define NOT_BYPASS_FRONT_GPI_3                      43  /**< */

#define ATM_SYNCHRONIZED                            3   /**< */
#define FG_CL_FULL_8_TAP_8_BIT FG_CL_8BIT_FULL_8
#define FG_CL_FULL_10_TAP_8_BIT FG_CL_8BIT_FULL_10
#define FG_CL_MEDIUM_4_TAP_8_BIT FG_CL_MEDIUM_8_BIT
#define FG_CL_MEDIUM_4_TAP_10_BIT FG_CL_MEDIUM_10_BIT
#define FG_CL_MEDIUM_4_TAP_12_BIT FG_CL_MEDIUM_12_BIT
#define FG_SINGLE                                   1   /**< Sampling Rate Single*/
#define FG_DOUBLE                                   2   /**< Sampling Rate Double */

#define FILTER_X1                                   1   /**< See applet documentation for usage.*/
#define FILTER_X2                                   2   /**< See applet documentation for usage.*/
#define FILTER_X4                                   3   /**< See applet documentation for usage.*/

#define HIGH_ACTIVE 0 /**< Take care, this is flipped 0=1 due to intenal inversion */
#define LOW_ACTIVE 1 /**< Take care, this is flipped 1=0 due to intenal inversion */

#define FG_CUSTOM_BIT_SHIFT_MODE				2
#define ASYNC_TRIGGER_MULTIFRAME 				8
#define CAMERA_FVAL_MULTIFRAME                  9

#define FG_PULSE_IMMEDIATE						1
#define FG_PULSE_WIDTH							23

/**
 * \brief definitions for Power over CameraLink (PoCL) states
 */
#define FG_INITIALIZE 							0   /**< CL : inititialize detection for CL or PoCL camera */
#define FG_POCL_CONNECTION_SENSE 				1 	/**< CL : detection in progress */
#define FG_POCL_CAMERA_DETECTED 				2   /**< CL : A PoCL camera and cable is detected*/
#define FG_POCL_CAMERA_CLOCK_DETECTED 			3   /**< CL : A PoCL camera, cable and pixel clock is detected */
#define FG_CL_CAMERA_DETECTED 					4   /**< CL : A Camera Link camera without power over function is detected */
#define FG_CL_CAMERA_CLOCK_DETECTED 			5   /**< CL : A Camera Link camera and clock without power over funcion is detected */
#define FG_POCL_WAIT_FOR_CONNECTION				6	/**< CL : PoCL camera detected. Waiting for stable connection */
#define FG_CL_WAIT_FOR_CONNECTION				7	/**< CL : Camera Link camera without power over function detected. Waiting for stable connection */
#define FG_POCL_DISABLED						8	/**< CL : PoCL is disabled for this frame grabber */

/**
 * \brief definitions for Power over CoaXPress states
 */
enum Fg_PoCXPState{
	BOOTING = 0x001,		/**< Power over CXP status : Booting, not initialized */
	NOCABLE = 0x002,		/**< Power over CXP status : no cable connected */
	NOPOCXP = 0x004,		/**< Power over CXP status : no Power over CXP */
	POCXPOK = 0x008,		/**< Power over CXP status : Power over CXP OK */
	MIN_CURR = 0x010,		/**< Power over CXP status : minimum current */
	MAX_CURR = 0x020,		/**< Power over CXP status : maximum current */
	LOW_VOLT = 0x040,		/**< Power over CXP status : low voltage */
	OVER_VOLT = 0x080,		/**< Power over CXP status : over voltage */
	ADC_Chip_Error = 0x100	/**< Power over CXP status : ADC Chip Error */
};

#define FG_POCXP_STATE_BOOTING                    BOOTING		 /**< Power over CXP status : Booting, not initialized */
#define FG_POCXP_STATE_NOCABLE                    NOCABLE		 /**< Power over CXP status : no cable connected */
#define FG_POCXP_STATE_NOPOCXP                    NOPOCXP		 /**< Power over CXP status : no Power over CXP */
#define FG_POCXP_STATE_POCXPOK                    POCXPOK		 /**< Power over CXP status : Power over CXP OK */
#define FG_POCXP_STATE_MIN_CURR                   MIN_CURR		 /**< Power over CXP status : minimum current */
#define FG_POCXP_STATE_MAX_CURR                   MAX_CURR		 /**< Power over CXP status : maximum current */
#define FG_POCXP_STATE_LOW_VOLT                   LOW_VOLT		 /**< Power over CXP status : low voltage */
#define FG_POCXP_STATE_OVER_VOLT                  OVER_VOLT		 /**< Power over CXP status : over voltage */
#define FG_POCXP_STATE_ADC_CHIP_ERROR             ADC_Chip_Error  /**< Power over CXP status : ADC Chip Error */


/**
 * \brief	Vantage Point. Used for tap geometry and mirroring. See GinICam SFNC
 */
 enum VantagePointNamingConvention{
    FG_VANTAGEPOINT_TOP_LEFT =     0,            /**< Top-Left*/
	FG_VANTAGEPOINT_TOP_RIGHT =    1,            /**< Top-Right*/
	FG_VANTAGEPOINT_BOTTOM_LEFT =  2,            /**< Bottom-Left*/
	FG_VANTAGEPOINT_BOTTOM_RIGHT = 3             /**< Bottom-Right*/
};
/**
 * \brief	Avilable Tap Geometry Formats. Format according to GenICam SFNC. Values not defined in SFNC
 * Scheme for Tap Geometry:
 * Bit 0to3:   YZones Readout direction: % = 0, E = 1, M = 2 
 * Bit 4to7:   YZones Interleaved: 1 to 2 
 * Bit 8to11   YZones Number 1 to 2 
 * Bit 12to15: Spare 
 * Bit 16to19: XZones Readout direction: % = 0, E = 1, M = 2 
 * Bit 20to23: XZones Interleaved: 1 to 10 
 * Bit 24to27  XZones Number 1 to 10 
 * Bit 28to31: Spare 
 * For formats with X-Zone only, bits for y-Zones are set to 0 (avoid redifinition)
 */
 enum TapGeometryNamingConvention{
    FG_GEOMETRY_1X =     0x01100000,            /**< 1X*/
	FG_GEOMETRY_1X2 =    0x01200000,           /**< 1X2*/
	FG_GEOMETRY_2X =     0x02100000,            /**< 2X*/
	FG_GEOMETRY_2XE =    0x02110000,           /**< 2XE*/
	FG_GEOMETRY_2XM =    0x02120000,           /**< 2XM*/
	FG_GEOMETRY_1X3 =    0x01300000,           /**< 1X3*/
	FG_GEOMETRY_3X =     0x03100000,            /**< 3X*/
	FG_GEOMETRY_1X4 =    0x01400000,           /**< 1X4*/
	FG_GEOMETRY_4X =     0x04100000,            /**< 4X*/
	FG_GEOMETRY_4XE =    0x04110000,           /**< 4XE*/	
	FG_GEOMETRY_2X2 =    0x02200000,           /**< 2X2*/
	FG_GEOMETRY_2X2E =   0x02210000,          /**< 2X2E*/
	FG_GEOMETRY_2X2M =   0x02220000,          /**< 2X2M*/
	FG_GEOMETRY_1X8 =    0x01800000,           /**< 1X8*/
	FG_GEOMETRY_8X =     0x08100000,            /**< 8X*/
	FG_GEOMETRY_1X10 =   0x01A00000,          /**< 1X10*/
	FG_GEOMETRY_10X =    0x0A100000,           /**< 10X*/
	FG_GEOMETRY_4X2 =    0x04200000,           /**< 4X2*/
	FG_GEOMETRY_4X2E =   0x04210000,           /**< 4X2E*/	
	FG_GEOMETRY_5X2 =    0x05200000,           /**< 5X2*/
	
	FG_GEOMETRY_1X_1Y =  0x01100110,         /**< 1X-1Y*/
	FG_GEOMETRY_1X_2Y =  0x01100210,         /**< 1X-2Y*/
	FG_GEOMETRY_1X_2YE = 0x01100211,        /**< 1X-2YE*/
	FG_GEOMETRY_2X_1Y =  0x02100110,         /**< 2X-1Y*/
	FG_GEOMETRY_2XE_1Y = 0x02110110,        /**< 2XE-1Y*/
	FG_GEOMETRY_2XM_1Y = 0x02120110,        /**< 2XM-1Y*/
	FG_GEOMETRY_2X_2Y =  0x02100210,         /**< 2X-2Y*/
	FG_GEOMETRY_2X_2YE = 0x02100211,        /**< 2X-2YE*/
	FG_GEOMETRY_2XE_2Y = 0x02110210,        /**< 2XE-2Y*/
	FG_GEOMETRY_2XE_2YE = 0x02110211,       /**< 1X2E*/
	FG_GEOMETRY_2XM_2Y = 0x02120210,        /**< 2XM-2Y*/
	FG_GEOMETRY_2XM_2YE = 0x02120211,       /**< 2XM-2YE*/
	FG_GEOMETRY_4X_1Y = 0x04100110,         /**< 4X-1Y*/
	FG_GEOMETRY_1X2_1Y = 0x01200110,        /**< 1X2-1Y*/
	FG_GEOMETRY_1X3_1Y = 0x01300110,        /**< 1X3-1Y*/
	FG_GEOMETRY_1X4_1Y = 0x01400110,        /**< 1X4-1Y*/
	FG_GEOMETRY_2X2_1Y = 0x02200110,        /**< 2X2-1Y*/
	FG_GEOMETRY_2X2E_1Y = 0x02210110,       /**< 2X2E-1Y*/
	FG_GEOMETRY_2X2M_1Y = 0x02220110,       /**< 2X2M-1Y*/
	FG_GEOMETRY_1X2_2YE = 0x01200211        /**< 1X2-2YE*/
};

/*
 * \brief    CLHS RGB Vertical ordering of colors in lines 
 */
#define FG_LINEORDER 920000
#define FG_LINEORDER_RGB 0
#define FG_LINEORDER_BGR 1

/*
* \brief    pixel format naming convention, pmsb is removed since CXP 1.1.1
*/
enum PixelFormatNamingConvention{
	Mono8 = 257,
	Mono10 = 258,
	Mono12 = 259,
	Mono14 = 260,
	Mono16 = 261,
	BayerGR8 = 785,
	BayerGR10 = 786,
	BayerGR12 = 787,
	BayerGR14 = 788,
	BayerRG8 = 801,
	BayerRG10 = 802,
	BayerRG12 = 803,
	BayerRG14 = 804,
	BayerGB8 = 817,
	BayerGB10 = 818,
	BayerGB12 = 819,
	BayerGB14 = 820,
	BayerBG8 = 833,
	BayerBG10 = 834,
	BayerBG12 = 835,
	BayerBG14 = 836,
	RGB8 = 1025,
	RGB10 = 1026,
	RGB12 = 1027,
	RGB14 = 1028,
	RGB16 = 1029,
	RGBA8 =  1281,
	RGBA10 = 1282,
	RGBA12 = 1283,
	RGBA14 = 1284,
	RGBA16 = 1285 
};

/* 
* \brief	ordering of colors at bayer format
*/
enum BayerOrdering{
	GreenFollowedByRed  = 3,
	GreenFollowedByBlue = 0,
	RedFollowedByGreen  = 2,
	BlueFollowedByGreen = 1
};

/* 
* \brief	ordering of colors for bilinear linescan bayer formats
*/
enum BayerBilinearLineOrdering{
	RedBlueLineFollowedByGreenLine  = 10,
	BlueRedLineFollowedByGreenLine = 11,
	GreenLineFollowedByRedBlueLine  = 12,
	GreenLineFollowedByBlueRedLine = 13
};

/* 
 * \brief	available format of pixels at GigE Vision
*/
enum GigEPixelFormat{
	MONO8         = 0,
	MONO8_SIGNED  = 1,
	MONO10        = 2,
	MONO10_PACKED = 3,
	MONO12        = 4,
	MONO12_PACKED = 5,
	MONO14        = 7,
	MONO16        = 6
};

/* 
 * \brief Trigger packet modes for CXP. Defined with parameter FG_CXP_TRIGGER_PACKET_MODE
 * Allows the user to send only one package for the rising edge of the trigger instead of rising and falling edge.
 * This can increase the maximum possible triggerrate. 
 * The camera needs to be capable to support this non-standard operation mode.
*/
enum CXPTriggerPackedModes{
	FG_STANDARD  = 0,
	FG_RISING_EDGE_ONLY = 1
};

/*
 * \brief Asynychronous notifications
 */
enum {
	ASYNC_NOTIFY_DRIVER_CLOSED,
	ASYNC_NOTIFY_DEVICE_REMOVED,
	ASYNC_NOTIFY_DEVICE_ARRIVED,
	ASYNC_NOTIFY_DEVICE_ALARM,
};

#define	DEVICE_ALARM_TEMPERATURE 0x00000001ul   // Temperature alarm
#define	DEVICE_ALARM_PHY         0x00000002ul   // PHY[0..3] management interrupt, shared
#define	DEVICE_ALARM_POE         0x00000004ul   // POE Chip Fault Interrupt
#define	DEVICE_ALARM_ACL_0       0x00000010ul   // Action command lost on channel [0]
#define	DEVICE_ALARM_ACL_1       0x00000020ul   // Action command lost on channel [1]
#define	DEVICE_ALARM_ACL_2       0x00000040ul   // Action command lost on channel [2]
#define	DEVICE_ALARM_ACL_3       0x00000080ul   // Action command lost on channel [3]

#define DEVICE_STATUS_CONFIGURED 0x00000001ul
#define DEVICE_STATUS_LOCKED     0x00000002ul
#define DEVICE_STATUS_OVERTEMP   0x40000000ul
#define DEVICE_STATUS_DEAD       0x80000000ul


/* Applet INFO */

typedef void * Fg_AppletIteratorType;
typedef void * Fg_AppletIteratorItem;

/**
 * \brief Applet Iterator Sources
 */
enum FgAppletIteratorSource
{
    FG_AIS_BOARD,       /**< Partitions on the board (default)*/
    FG_AIS_FILESYSTEM,  /**< Applets on the filesystem for this type of board*/
};

/** @name Applet Flags
 *  \anchor AppletFlags
 */
/*@{*/
const int64_t FG_AF_IS_AVAILABLE = 0x1;         /**< Applet is available on the filesystem*/
const int64_t FG_AF_IS_CORRECT_PLATFORM = 0x2;  /**< Applet has the correct platform (E.g. Win 32/64 or Linux 32/64 Bit)*/
const int64_t FG_AF_IS_VALID_LICENSE = 0x4;     /**< Applet has a valid license*/
const int64_t FG_AF_IS_ACTIVE = 0x8;            /**< Applet is currently active on the board*/
const int64_t FG_AF_IS_LOADABLE = 0x10;         /**< Applet can be loaded via Fg_Init*/
const int64_t FG_AF_IS_POWERUP_APPLET = 0x20;   /**< Applet is loaded when the system powers up*/
const int64_t FG_AF_IS_COMPATIBLE = 0x40;       /**< This file is compatible with the flashed design (same FPGA UUID) (FLASHED & COMPATIBLE CANNOT BE BOTH 1)*/
const int64_t FG_AF_IS_FLASHED = 0x80;          /**< This exact file is flashed (same FPGA & APPLET UUID) (FLASHED & COMPATIBLE CANNOT BE BOTH 1)*/
const int64_t FG_AF_IS_SUPPORTED_BY_RUNTIME = 0x100; /**< This applet is supported by the current Runtime version*/
/*@}*/


/** @name Applet Info
 *  \anchor AppletInfo
 */
/*@{*/
const int64_t FG_AI_IS_COLOR = 0x1;                     /**< Color applet (1 = COLOR, 0 = GRAY)*/
const int64_t FG_AI_IS_AREA = 0x2;                      /**< Area applet (1 = AREA, 0 = LINE)*/

const int64_t FG_AI_IS_BASE_X1 = 0x10;                  /**< Base (or x1) applet*/
const int64_t FG_AI_IS_MEDIUM_X2 = 0x20;                /**< Medium (or x2) applet*/
const int64_t FG_AI_IS_FULL_X4 = 0x40;                  /**< Full (or x4) applet*/
const int64_t FG_AI_IS_DUAL_FULL_X8 = 0x80;             /**< Dual Full (or x8) applet*/
const int64_t FG_AI_CAMERA_INTERFACE = 0xF0;            /**< Camera Interface (or number of Links per camera)*/

const int64_t FG_AI_IS_VAS = 0x100;                     /**< VAS applet*/
const int64_t FG_AI_IS_CUSTOM = 0x200;                  /**< Custom applet*/
const int64_t FG_AI_IS_SMART = 0x400;                   /**< Smart applet*/
const int64_t FG_AI_IS_BLOB = 0x800;                    /**< Blob applet*/
const int64_t FG_AI_IS_ADVANCED_ACQ_APPLET = 0x1000;    /**< Advanced ACQ Applet*/

const int64_t FG_AI_IS_VALID = 0x80000000;              /**< info valid (If 0, then the file could not be correctly read; OS not supported, corrupted file ..., then all other properties are not valid)*/
/*@}*/

/**
 * \brief	Available Integer Applet Properties
 * \anchor  FgAppletIntProperty
 */
enum FgAppletIntProperty {
    FG_AP_INT_FLAGS,                /**< \ref AppletFlags "Applet Flags"*/
    FG_AP_INT_INFO,                 /**< \ref AppletInfo "Applet Info"*/
    FG_AP_INT_PARTITION,            /**< the board partition where the applet is flashed*/
    FG_AP_INT_NR_OF_DMA,            /**< DMA Count*/
    FG_AP_INT_NR_OF_CAMS,           /**< Cam Port Count*/
    FG_AP_INT_GROUP_CODE,           /**< Group Code*/
    FG_AP_INT_USER_CODE,            /**< User Code*/
    FG_AP_INT_BOARD_GROUP_MASK,     /**< Board Group Mask*/
    FG_AP_INT_BOARD_USER_CODE,      /**< Board User Code*/
    FG_AP_INT_ICON_SIZE,            /**< Icon Size*/
    FG_AP_INT_LAG,                  /**< LAG*/
    FG_AP_INT_DESIGN_VERSION,       /**< Design Version Number*/
    FG_AP_INT_DESIGN_REVISION,      /**< Design Revision Number*/
};

/**
 * \brief	Available String Applet Properties
 * \anchor  FgAppletStringProperty
 */
enum FgAppletStringProperty {
    FG_AP_STRING_APPLET_UID,            /**< Applet UID*/
    FG_AP_STRING_BITSTREAM_UID,         /**< Applet Bitstream Uid*/
    FG_AP_STRING_DESIGN_NAME,           /**< Design Name*/
    FG_AP_STRING_APPLET_NAME,           /**< Applet Name*/
    FG_AP_STRING_DESCRIPTION,           /**< Applet Description*/
    FG_AP_STRING_CATEGORY,              /**< Applet Category*/
    FG_AP_STRING_APPLET_PATH,           /**< Applet Path*/
    FG_AP_STRING_ICON,                  /**< Icon Data*/
    FG_AP_STRING_SUPPORTED_PLATFORMS,   /**< Supported Platforms (Comma separated)*/
    FG_AP_STRING_TAGS,                  /**< Tags*/
    FG_AP_STRING_VERSION,               /**< Version*/
    FG_AP_STRING_APPLET_FILE,           /**< File Name of Applet*/
    FG_AP_STRING_RUNTIME_VERSION,       /**< Required Runtime version*/
};



#endif

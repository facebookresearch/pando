/**
 * fgrab_struct.h
 *
 * Copyright (c) 2002-2017 Silicon Software GmbH, All Rights Reserved.
 *
 *
 * \file fgrab_struct.h
 *
 * \brief struct declarations.
 *
 * \author Silicon Software GmbH
 */

#ifndef FGRAB_STRUCT_H
#define FGRAB_STRUCT_H

#pragma warning(push)
#pragma warning(disable: 4005)
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <msinttypes/stdint.h>
#else
#include <stdint.h>
#endif
#pragma warning(pop)

#ifdef _MSC_VER
#pragma pack(push)
#pragma pack(8)
#endif /* _WIN32 */

#ifdef __GNUC__
#if __WORDSIZE == 64
#define FGLIB_PTRALIGN(x) unsigned int x;
#else /* __WORDSIZE == 64 */
#define FGLIB_PTRALIGN(x)
#endif /* BITS_PER_LONG > 32 */
#else /* __GNUC__ */
#define FGLIB_PTRALIGN(x)
#endif /* __GNUC__ */

#include "fgrab_define.h"

typedef struct LookupTable_s {
	unsigned int	*lut;		/**< pointer to field data, maximum 32 Bit */
	unsigned int	id;		/**< ID (#LUT_RED, #LUT_GREEN, #LUT_BLUE, #LUT_GRAY, etc. */
	unsigned int	nrOfElements;	/**< Number of field elements in the lut field */
	unsigned int	format;		/**< reserved for future use */
	unsigned char	number;		/**< the log number of LUT, if more than one LUT of the same id is used */
} LookupTable;

typedef struct KneeLookupTable_s {
	double *value;			/**< pointer to field data */
	double *reserved;		/**< reserved for future use */
	unsigned int	id;		/**< ID (#LUT_RED, #LUT_GREEN, #LUT_BLUE, #LUT_GRAY, etc. */
	unsigned int	nrOfElements;	/**< Number of field elements in the lut field */
	unsigned int	format;		/**< reserved for future use */
	unsigned char	number;		/**< the log number of LUT, if more than one LUT of the same id is used */
} KneeLookupTable;


typedef struct ShadingParameter_s {
	unsigned char	*offset;	/**< pointer to field data, maximum 8 Bit */
	unsigned char	*cmult;		/**< pointer to field mult, range 0.0-<2.0 */
	float	*mult;			/**< pointer to field mult, range 0.0-<2.0 */
	unsigned int	nrOfElements;	/**< Number of field elements in the offset,mult fields */
	int		width;		/**< width */
	int		height;		/**< height */
	int		set;		/**< nr of set */
} ShadingParameter;

typedef struct LineShadingParameter_s {
	unsigned int mShadingData[4096]; /**< pointer to field data, maximum 24 Bit */
	int		mNoOfPixelsInit;
} LineShadingParameter;

typedef struct FieldParameterInt_s {
	uint32_t value;
	unsigned int index;
} FieldParameterInt;

typedef struct FieldParameterDouble_s {
	double value;
	unsigned int index;
} FieldParameterDouble;

/**
 * accessor struct for array parameters
 *
 * This struct allows access to array parameters of any type in a
 * flexible way. Both range accesses as well as single value accesses
 * are possible.
 *
 * \code
 * struct FieldParameterAccess singleaccess;
 * struct FieldParameterAccess rangeaccess;
 * uint64_t primes[7] = { 1, 2, 3, 5, 7, 11, 13 };
 * uint32_t answer = 42;
 *
 * // set up single value access
 * singleaccess.vtype = FG_PARAM_TYPE_UINT32_T;
 * singleaccess.index = 17;
 * singleaccess.count = 1;
 * singleaccess.p_uint32_t = &answer;
 *
 * // set up range access
 * rangeaccess.vtype = FG_PARAM_TYPE_UINT64_T;	// every data value is an uint64_t
 * rangeaccess.index = 0;
 * rangeaccess.count = 7;
 * rangeaccess.p_uint64_t = primes;
 *
 * \endcode
 */
struct FieldParameterAccess {
	enum FgParamTypes vtype;	/**< gives the type of the included data */
	unsigned int index;		/**< gives the first index in the range */
	unsigned int count;		/**< gives the value count of the range */
	FGLIB_PTRALIGN(_pad)
	union {
		int32_t *p_int32_t;	/**< a range of signed 32 bit integer values */
		uint32_t *p_uint32_t;	/**< a range of unsigned 32 bit integer values */
		int64_t *p_int64_t;	/**< a range of signed 64 bit integer values */
		uint64_t *p_uint64_t;	/**< a range of unsigned 64 bit integer values */
		double *p_double;	/**< a range of double values */
		void *ptr;	/**< for internal access of generalized type */
	};
};

#ifndef __cplusplus
typedef struct FieldParameterAccess FieldParameterAccess;
#endif

struct FgApcControl {
	unsigned int version;		/**< struct layout version. Must be 0 */
	FGLIB_PTRALIGN(_pad)
	Fg_ApcFunc_t func;		/**< function handler called when transfers arrive */
	struct fg_apc_data *data;	/**< value passed to apcfunc */
	unsigned int timeout;		/**< seconds to wait for new images (must not be 0) */
	unsigned int flags;		/**< operation mode of APC thread */
};

#ifndef __cplusplus
typedef struct FgApcControl FgApcControl;
#endif

/**
 * \typedef	Fg_Struct
 *
 * \brief	The structure for a framegrabber.
 *
 * This struct is used as a handle to the currently opened board and design.
 */
typedef struct Fg_Struct_s Fg_Struct;

struct fg_event_info {
	unsigned int version;		/**< version of this struct. Do not set directly, use FG_EVENT_INFO_INIT() */
	unsigned int _pad;		/**< currently unused, inserted to allow 64 bit aligning of following fields */
	unsigned int notify[64];	/**< see enum FgEventNotifiers for the meaning of the bits */
	uint64_t timestamp[64];		/**< the timestamp when the events were recorded (always 0 in version 5.1.2) */
	unsigned int length;		/**< valid entries in data */
	uint16_t data[254];		/**< payload data of the event */
};


/**
 * \typedef	dma_mem
 *
 * \brief	DMA buffer head control object.
 *
 * A variable of this type contains the control information for DMA buffers.
 * Take this as an opaque value and just pass it around to the functions that need it.
 */

/**
 * \typedef	ShadingMaster
 *
 * \brief	Shading control object.
 *
 * A variable of this type contains the control information for a specific shading set.
 * Take this as an opaque value and just pass it around to the functions that need it.
 */

#ifdef _MSC_VER
#pragma pack(pop)
#endif /* _WIN32 */
#undef FGLIB_PTRALIGN

#endif

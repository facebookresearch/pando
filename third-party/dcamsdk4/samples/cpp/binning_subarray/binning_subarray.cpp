/**
 @file binning_subarray.cpp
 @brief		sample code to set binning and subarray
 @details	This program sets the binning and subarray. The set value is defined by the directive.
 @details	Each value is checked whether the connected camera supports or not.
 @details	dcamprop_queryvalue
 @details	dcamprop_setvalue
 */

#include "../misc/console4.h"
#include "../misc/common.h"

/**
 @def BINNING_VALUE
 *
 *n:	This value is set as a binning parameter.
 *		This value is set to the property DCAM_IDPROP_BINNING.
 */
#define BINNING_VALUE	2

/**
 @def	SUBARRAY_HORZ_OFFSET
 *
 *n:	This value is set as a horizontal offset of subbarray.
 *		This value is set to the property DCAM_IDPROP_SUBARRAYHPOS.
 *		This value means the position of the sensor coordinate. In other words, this position is the coordinate of the binning 1x1.
 */
#define SUBARRAY_HORZ_OFFSET	128

/**
 @def	SUBARRAY_HORZ_SIZE
 *
 *n:	This value is set as a horizontal length of subbarray.
 *		This value is set to the property DCAM_IDPROP_SUBARRAYHSIZE.
 *		This value means the length of the sensor coordinate. In other words, this position is the coordinate of the binning 1x1.
 */
#define SUBARRAY_HORZ_SIZE		512

/**
 @def	SUBARRAY_VERT_OFFSET
 *
 *n:	This value is set as a vertical offset of subbarray.
 *		This value is set to the property DCAM_IDPROP_SUBARRAYVPOS.
 *		This value means the position of the sensor coordinate. In other words, this position is the coordinate of the binning 1x1.
 */
#define SUBARRAY_VERT_OFFSET	128

/**
 @def	SUBARRAY_VERT_SIZE
 *
 *n:	This value is set as a vertical length of subbarray.
 *		This value is set to the property DCAM_IDPROP_SUBARRAYVSIZE.
 *		This value means the length of the sensor coordinate. In other words, this position is the coordinate of the binning 1x1.
 */
#define SUBARRAY_VERT_SIZE		512



/**
 @brief	set binning
 @param hdcam:		DCAM handle
 @param binning:	value of binning
 @return	result of setting to binning parameter
 */
BOOL set_binning( HDCAM hdcam, int32 binning )
{
	DCAMERR err;
	
	// check whether the camera supports or not. (refer sample program propertylist to get the list of support values)
	double v = binning;
	err = dcamprop_queryvalue( hdcam, DCAM_IDPROP_BINNING, &v );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_queryvalue()", "IDPROP:BINNING, VALUE:%d", binning ); 
		return FALSE;
	}

	// set binning value to the camera
	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_BINNING, binning );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:BINNING, VALUE:%d", binning ); 
		return FALSE;
	}

	return TRUE;
}

/**
 @brief set subarray
 @param hdcam:		DCAM handle
 @param hpos:		horizontal offset
 @param hsize:		horizontal size
 @param vpos:		vertical offset
 @param vsize:		vertical size
 @return	result of setting to subarray paramter
 */
BOOL set_subarray( HDCAM hdcam, int32 hpos, int32 hsize, int32 vpos, int32 vsize )
{
	DCAMERR err;

	// set subarray mode off. This setting is not mandatory, but you have to control the setting order of offset and size when mode is on. 
	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYMODE, DCAMPROP_MODE__OFF );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBARRAYMODE, VALUE:OFF" );
		return FALSE;
	}

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYHPOS, hpos );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBARRAYHPOS, VALUE:%d", hpos );
		return FALSE;
	}

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYHSIZE, hsize );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBARRAYHSIZE, VALUE:%d", hsize );
		return FALSE;
	}

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYVPOS, vpos );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBARRAYVPOS, VALUE:%d", vpos );
		return FALSE;
	}

	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYVSIZE, vsize );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBARRAYVSIZE, VALUE:%d", vsize );
		return FALSE;
	}

	// set subarray mode on. The combination of offset and size is checked on this timing.
	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBARRAYMODE, DCAMPROP_MODE__ON );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBARRAYMODE, VALUE:ON" );
		return FALSE;
	}

	return TRUE;
}

int main( int argc, char* const argv[] )
{
	printf( "PROGRAM START\n" );

	int	ret = 0;

	DCAMERR err;

	// initialize DCAM-API and open device
	HDCAM hdcam;
	hdcam = dcamcon_init_open();
	if( hdcam != NULL )
	{
		// show device information
		dcamcon_show_dcamdev_info( hdcam );

		// set binning
		if( set_binning( hdcam, BINNING_VALUE ) )
		{
			// set subarray
			if( set_subarray( hdcam, SUBARRAY_HORZ_OFFSET, SUBARRAY_HORZ_SIZE, SUBARRAY_VERT_OFFSET, SUBARRAY_VERT_SIZE ) )
			{
				int32 number_of_buffer = 10;
				// allocate the buffer to receive image. the image geometry is fixed.
				err = dcambuf_alloc( hdcam, number_of_buffer );
				if( failed(err) )
				{
					dcamcon_show_dcamerr( hdcam, err, "dcambuf_alloc()" );
					ret = 1;
				}
				else
				{
					// start capture
					err = dcamcap_start( hdcam, DCAMCAP_START_SEQUENCE );
					if( failed(err) )
					{
						dcamcon_show_dcamerr( hdcam, err, "dcamcap_start()" );
						ret = 1;
					}
					else
					{
						printf( "\nStart Capture\n" );

						// TODO: add your process to wait and access image

						// stop capture
						dcamcap_stop( hdcam );
						printf( "Stop Capture\n" );
					}

					// release buffer
					dcambuf_release( hdcam );
				}
			}
		}

		// close DCAM handle
		dcamdev_close( hdcam );
	}
	else
	{
		ret = 1;
	}

	// finalize DCAM-API
	dcamapi_uninit();

	printf( "PROGRAM END\n" );
	return ret;
}
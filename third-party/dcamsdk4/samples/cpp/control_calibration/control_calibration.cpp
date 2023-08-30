/**
  @file control_calibration.cpp
  @brief	sample code to capture and store calibration data.
  @remarks	dcamprop_setvalue
  @remarks	dcamwait_start
 */

#define	DARKCALIB_TARGETPAGE	1	// 0: not access page
									// other: save dark calibration data to target page, or load dark calibration data from target page before capturing shading data when subtract mode isn't enabled.
#define CAPTURE_DARKCALIB		1	// 0: not capture dark calibration data
									// 1: capture dark calibration data
#define	SHADINGCALIB_TARGETPAGE	1	// 0: not access page
									// other: save shading calibration data to target page.
#define CAPTURE_SHADINGCALIB	1	// 0: not capture sharding calibration data
									// 1: capture shading calibration data

#include "../misc/console4.h"
#include "../misc/common.h"

BOOL capture_darkcalibdata( HDCAM hdcam, HDCAMWAIT hwait )
{
	DCAMERR err;
	
	// set capture mode
	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_CAPTUREMODE, DCAMPROP_CAPTUREMODE__DARKCALIB );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:CAPTUREMODE, VALUE:DARKCALIB" );
		return FALSE;
	}

	// allocate buffer
	int32 number_of_buffer = 10;
	err = dcambuf_alloc( hdcam, number_of_buffer );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_alloc()" );
		return FALSE;
	}

	// TO DO: set exposure time that you use

	BOOL bRes = TRUE;

	// start capture. it is necessary to make darkness before capturing.
	err = dcamcap_start( hdcam, DCAMCAP_START_SEQUENCE );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamcap_start()" );
		bRes = FALSE;
	}
	else
	{
		printf( "\nStart Capture for Dark Calibration\n" );

		// set wait param
		DCAMWAIT_START waitstart;
		memset( &waitstart, 0, sizeof(waitstart) );
		waitstart.size	= sizeof(waitstart);
		waitstart.eventmask	= DCAMWAIT_CAPEVENT_STOPPED;
		waitstart.timeout	= 10000;	// tentative. There is possibility to set long time by camera specification.

		// wait to create the data to calibrate dark noise.
		err = dcamwait_start( hwait, &waitstart );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamwait_start()" );

			// stop capture forcedly
			dcamcap_stop( hdcam );
			bRes = FALSE;
		}
		else
		{
#if DARKCALIB_TARGETPAGE
			int32 page = DARKCALIB_TARGETPAGE;
			err = dcamprop_setvalue( hdcam, DCAM_IDPROP_STORESUBTRACTIMAGETOMEMORY, page );
			if( failed(err) )
			{
				dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:STORESUBTRACTIMAGETOMEMORY, VALUE:%d", page );

				bRes = FALSE;
			}
#endif
		}
	}

	dcambuf_release( hdcam );

	return bRes;
}

BOOL capture_shadingcalibdata( HDCAM hdcam, HDCAMWAIT hwait )
{
	DCAMERR err;

	// it is necessary that DCAM_IDPROP_SUBTRACT is DCAMPROP_MODE__ON.
	double v;
	err = dcamprop_getvalue( hdcam, DCAM_IDPROP_SUBTRACT, &v );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_getvalue()", "IDPROP:SUBTRACT" );
		return FALSE;
	}
	
	if( v == DCAMPROP_MODE__OFF )
	{
#if DARKCALIB_TARGETPAGE
		int32 page = DARKCALIB_TARGETPAGE;
		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBTRACTIMAGEMEMORY, page );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBTRACTIMAGEMEMORY, VALUE:%d", page );
			return FALSE;
		}

		err = dcamprop_setvalue( hdcam, DCAM_IDPROP_SUBTRACT, DCAMPROP_MODE__ON );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:SUBTRACT, VALUE:ON" );
			return FALSE;
		}
#else
		printf( "SUBTRACT isn't ON before capturing shading calibration data.\n" );
		return FALSE;
#endif
	}

	// set capture mode
	err = dcamprop_setvalue( hdcam, DCAM_IDPROP_CAPTUREMODE, DCAMPROP_CAPTUREMODE__SHADINGCALIB );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:CAPTUREMODE, VALUE:SHADINGCALIB" );
		return FALSE;
	}

	// allocate buffer
	int32 number_of_buffer = 10;
	err = dcambuf_alloc( hdcam, number_of_buffer );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_alloc()" );
		return FALSE;
	}

	// TO DO: set exposure time that you use

	BOOL bRes = TRUE;

	// start capture. it is necessary to set flat brightness before capturing.
	err = dcamcap_start( hdcam, DCAMCAP_START_SEQUENCE );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamcap_start()" );
		bRes = FALSE;
	}
	else
	{
		printf( "\nStart Capture for Shading Calibration\n" );

		// set wait param
		DCAMWAIT_START waitstart;
		memset( &waitstart, 0, sizeof(waitstart) );
		waitstart.size	= sizeof(waitstart);
		waitstart.eventmask	= DCAMWAIT_CAPEVENT_STOPPED;
		waitstart.timeout	= 10000;	// tentative. There is possibility to set long time by camera specification.

		// wait to create the data to calibrate shading.
		err = dcamwait_start( hwait, &waitstart );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamwait_start()" );

			// stop capture forcedly
			dcamcap_stop( hdcam );
			bRes = FALSE;
		}
		else
		{
#if SHADINGCALIB_TARGETPAGE
			int32 page = SHADINGCALIB_TARGETPAGE;
			err = dcamprop_setvalue( hdcam, DCAM_IDPROP_STORESHADINGCALIBDATATOMEMORY, page );
			if( failed(err) )
			{
				dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:STORESHADINGCALIBDATATOMEMORY, VALUE:%d", page );

				bRes = FALSE;
			}
#endif
		}
	}

	dcambuf_release( hdcam );

	return bRes;
}

BOOL capture_cablibration_data( HDCAM hdcam, HDCAMWAIT hwait )
{
#if CAPTURE_DARKCALIB
	if( !capture_darkcalibdata( hdcam, hwait ) )
		return FALSE;
#endif

#if CAPTURE_SHADINGCALIB
	if( !capture_shadingcalibdata( hdcam, hwait ) )
		return FALSE;
#endif

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

		// open wait handle
		DCAMWAIT_OPEN	waitopen;
		memset( &waitopen, 0, sizeof(waitopen) );
		waitopen.size	= sizeof(waitopen);
		waitopen.hdcam	= hdcam;

		err = dcamwait_open( &waitopen );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamwait_open()" );
			ret = 1;
		}
		else
		{
			HDCAMWAIT hwait = waitopen.hwait;

			// Some camera needs to set SENSORMODE.

			// create calibration data
			if( !capture_cablibration_data( hdcam, hwait ) )
				ret = 1;

			// close wait handle
			dcamwait_close( hwait );
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
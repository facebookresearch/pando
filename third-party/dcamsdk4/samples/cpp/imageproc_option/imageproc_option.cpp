// console/imageproc_option
//

#include "../misc/console4.h"
#include "../misc/common.h"

void sample_access_image_with_procoption( HDCAM hdcam )
{
	DCAMERR err;

	// check capability
	DCAMDEV_CAPABILITY_FRAMEOPTION capframeoption;
	memset( &capframeoption, 0, sizeof(capframeoption) );
	capframeoption.hdr.size		= sizeof(capframeoption);
	capframeoption.hdr.domain	= DCAMDEV_CAPDOMAIN__FRAMEOPTION;

	err = dcamdev_getcapability( hdcam, &capframeoption.hdr );
	if( failed(err) )
		dcamcon_show_dcamerr( hdcam, err, "dcamdev_getcapbility()", "DOMAIN:FRAMEOPTION" );
	else
	if( capframeoption.supportproc == 0 )
		printf( "not support proc option.\n" );
	else
	if( capframeoption.hdr.capflag == 0 )
		printf( "currently disable option(0x%08x)\n", capframeoption.supportproc );
	else
	{
		ASSERT( capframeoption.hdr.capflag & DCAMBUF_PROCTYPE__HIGHCONTRASTMODE );

		// transferinfo param
		DCAMCAP_TRANSFERINFO captransferinfo;
		memset( &captransferinfo, 0, sizeof(captransferinfo) );
		captransferinfo.size	= sizeof(captransferinfo);

		// get number of captured image
		err = dcamcap_transferinfo( hdcam, &captransferinfo );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcamcap_transferinfo()" );
			return;
		}

		if( captransferinfo.nFrameCount < 1 )
		{
			printf( "not capture image\n" );
			return;
		}

		// prepare frame param
		DCAMBUF_FRAME	bufframe;
		memset( &bufframe, 0, sizeof(bufframe) );
		bufframe.size	= sizeof(bufframe);
		bufframe.iFrame	= captransferinfo.nNewestFrameIndex;
		// set proc option
		bufframe.option	= DCAMBUF_FRAME_OPTION__PROC_HIGHCONTRAST;

		// access image
		err = dcambuf_lockframe( hdcam, &bufframe );
		if( failed(err) )
		{
			dcamcon_show_dcamerr( hdcam, err, "dcambuf_lockframe()" );
			return;
		}

		{
			// TODO: add your process
		}
	}
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

			// allocate buffer
			int32 number_of_buffer = 10;
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

					// set wait param
					DCAMWAIT_START waitstart;
					memset( &waitstart, 0, sizeof(waitstart) );
					waitstart.size		= sizeof(waitstart);
					waitstart.eventmask	= DCAMWAIT_CAPEVENT_FRAMEREADY;
					waitstart.timeout	= 1000;

					err = dcamwait_start( hwait, &waitstart );
					if( failed(err) )
					{
						dcamcon_show_dcamerr( hdcam, err, "dcamwait_start()" );
						ret = 1;
					}

					// stop capture
					dcamcap_stop( hdcam );
					printf( "Stop Capture\n" );

					// access image
					sample_access_image_with_procoption( hdcam );
				}

				// release buffer
				dcambuf_release( hdcam );
			}
		
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
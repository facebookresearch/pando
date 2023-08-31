// console/attachmetadatabuf
//

#include "../misc/console4.h"
#include "../misc/common.h"

#define ATTACH_PRIMARYBUFFER	1	// 0: attach pointer array to store meta data, 1: attach primary buffer to store meta data

void attach_primary_timestampbuffer( HDCAM hdcam, DCAM_TIMESTAMP* pTimeStamp, int32 timestamp_count )
{
	DCAMERR err;

	DCAMBUF_ATTACH	bufattach;
	memset( &bufattach, 0, sizeof(bufattach) );

	bufattach.size			= sizeof(bufattach);
	bufattach.iKind			= DCAMBUF_ATTACHKIND_PRIMARY_TIMESTAMP;
	bufattach.buffer		= (void**)&pTimeStamp;
	bufattach.buffercount	= timestamp_count;

	err = dcambuf_attach( hdcam, &bufattach );
	if( failed(err) )
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_attach()", "KIND:PRIMARY_TIMESTAMP" );
}

void attach_timestampbuffer( HDCAM hdcam, void** pTimeStamp, int32 timestamp_count )
{
	DCAMERR err;

	DCAMBUF_ATTACH	bufattach;
	memset( &bufattach, 0, sizeof(bufattach) );

	bufattach.size			= sizeof(bufattach);
	bufattach.iKind			= DCAMBUF_ATTACHKIND_TIMESTAMP;
	bufattach.buffer		= pTimeStamp;
	bufattach.buffercount	= timestamp_count;

	err = dcambuf_attach( hdcam, &bufattach );
	if( failed(err) )
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_attach()", "KIND:TIMESTAMP" );
}

void attach_primary_framestampbuffer( HDCAM hdcam, int32* pFrameStamp, int32 framestamp_count )
{
	DCAMERR err;

	DCAMBUF_ATTACH	bufattach;
	memset( &bufattach, 0, sizeof(bufattach) );

	bufattach.size			= sizeof(bufattach);
	bufattach.iKind			= DCAMBUF_ATTACHKIND_PRIMARY_FRAMESTAMP;
	bufattach.buffer		= (void**)&pFrameStamp;
	bufattach.buffercount	= framestamp_count;

	err = dcambuf_attach( hdcam, &bufattach );
	if( failed(err) )
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_attach()", "KIND:PRIMARY_FRAMESTAMP" );
}

void attach_framestampbuffer( HDCAM hdcam, void** pFrameStamp, int32 framestamp_count )
{
	DCAMERR err;

	DCAMBUF_ATTACH	bufattach;
	memset( &bufattach, 0, sizeof(bufattach) );

	bufattach.size			= sizeof(bufattach);
	bufattach.iKind			= DCAMBUF_ATTACHKIND_FRAMESTAMP;
	bufattach.buffer		= pFrameStamp;
	bufattach.buffercount	= framestamp_count;

	err = dcambuf_attach( hdcam, &bufattach );
	if( failed(err) )
		dcamcon_show_dcamerr( hdcam, err, "dcambuf_attach()", "KIND:FRAMESTAMP" );
}

void show_metadata_information( HDCAM hdcam, int32 number_of_metadata, DCAM_TIMESTAMP* timestamp_array, int32* framestamp_array )
{
	DCAMERR err;

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
		printf( "no image.\n" );
		return;
	}

	int32 iStart = captransferinfo.nFrameCount - number_of_metadata;
	if( iStart < 0 )
		iStart = 0;

	printf( "index\tframe stamp\ttime stamp\n" );

	int i;
	for( i = iStart; i < captransferinfo.nFrameCount; i++ )
	{
		int32 ind = i % number_of_metadata;
		printf( "%d\t%d\t%d.%06d\n", i, framestamp_array[ind], timestamp_array[ind].sec, timestamp_array[ind].microsec );
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
				// allocate the buffer for meta data
				int32 number_of_metadata = 100;
				DCAM_TIMESTAMP*	timestamp_array = new DCAM_TIMESTAMP[ number_of_metadata ];
				memset( timestamp_array, 0, sizeof(DCAM_TIMESTAMP) * number_of_metadata );

				int32* framestamp_array = new int32[ number_of_metadata ];
				memset( framestamp_array, 0, sizeof(int32) * number_of_metadata );

#if ATTACH_PRIMARYBUFFER
				// attach primary buffer for meta data
				attach_primary_timestampbuffer( hdcam, timestamp_array, number_of_metadata );
				attach_primary_framestampbuffer( hdcam, framestamp_array, number_of_metadata );
#else
				// prepare pointer array to store pointer of meta data
				void** pTimeStamp	= new void*[ number_of_metadata ];
				void** pFrameStamp	= new void*[ number_of_metadata ];

				int i;
				for( i = 0; i < number_of_metadata; i++ )
				{
					pTimeStamp[i]	= timestamp_array + i;
					pFrameStamp[i]	= framestamp_array + i;
				}

				// attach buffer pointer array for meta data
				attach_timestampbuffer( hdcam, pTimeStamp, number_of_metadata );
				attach_framestampbuffer( hdcam, pFrameStamp, number_of_metadata );
#endif

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
					Sleep( 3000 );

					// stop capture
					dcamcap_stop( hdcam );
					printf( "Stop Capture\n" );
				}

				show_metadata_information( hdcam, number_of_metadata, timestamp_array, framestamp_array );

#if ATTACH_PRIMARYBUFFER
#else
				delete pTimeStamp;
				delete pFrameStamp;
#endif

				delete timestamp_array;
				delete framestamp_array;

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
// console/open_show_modelinfo
//

#include "../misc/console4.h"
#include "../misc/common.h"

int main( int argc, char* const argv[] )
{
	printf( "PROGRAM START\n" );

	int	ret = 0;
	DCAMERR err;

	// initialize DCAM-API
	DCAMAPI_INIT	apiinit;
	memset( &apiinit, 0, sizeof(apiinit) );
	apiinit.size	= sizeof(apiinit);

	err = dcamapi_init( &apiinit );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( NULL, err, "dcamapi_init()" );
		ret = 1;
	}
	else
	{
		int32	nDevice = apiinit.iDeviceCount;
		printf( "dcamapi_init() found %d device(s).\n", nDevice );

		int32 iDevice;
		for( iDevice = 0; iDevice < nDevice; iDevice++ )
		{
			printf( "#%d: ", iDevice );

			// open device
			DCAMDEV_OPEN	devopen;
			memset( &devopen, 0, sizeof(devopen) );
			devopen.size	= sizeof(devopen);
			devopen.index	= iDevice;

			err = dcamdev_open( &devopen );
			if( failed(err) )
			{
				dcamcon_show_dcamerr( (HDCAM)(intptr_t)iDevice, err, "dcamdev_open()" );
				ret = 1;
			}
			else
			{
				HDCAM hdcam = devopen.hdcam;

				dcamcon_show_dcamdev_info( hdcam );
				
				// close device
				dcamdev_close( hdcam );
			}
		}
	}

	// finalaize DCAM-API
	dcamapi_uninit();	// recommended call dcamapi_uninit() when dcamapi_init() is called even if it failed.

	printf( "PROGRAM END\n" );
	return ret;
}


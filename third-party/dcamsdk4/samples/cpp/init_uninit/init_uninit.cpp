// console/init_uninit
//

#include "../misc/console4.h"
#include "../misc/common.h"

#define USE_INITOPTION	0	// set DCAMAPI_INITOPTION when the value isn't 0.
#define USE_INITGUID	0	// set GUID parameter when the value isn't 0.

#if USE_INITGUID
#include "../misc/dcamapix.h"
#endif

int main( int argc, char* const argv[] )
{
	printf( "PROGRAM START\n" );

	int ret = 0;
	DCAMERR err;

	// initialize DCAM-API
	DCAMAPI_INIT	apiinit;
	memset( &apiinit, 0, sizeof(apiinit) );
	apiinit.size	= sizeof(apiinit);

#if USE_INITOPTION
	// set option of initialization
	int32 initoption[] = {
							DCAMAPI_INITOPTION_APIVER__LATEST,
							DCAMAPI_INITOPTION_ENDMARK			// it is necessary to set as the last value.
						 };

	apiinit.initoption		= initoption;
	apiinit.initoptionbytes	= sizeof(initoption);
#endif

#if USE_INITGUID
	// set GUID parameter
	DCAM_GUID	guid = DCAM_GUID_MYAPP;

	apiinit.guid	= &guid;
#endif
	
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
			dcamcon_show_dcamdev_info( (HDCAM)(intptr_t)iDevice );
		}
	}

	// finalize DCAM-API
	dcamapi_uninit();	// recommended call dcamapi_uninit() when dcamapi_init() is called even if it failed.

	printf( "PROGRAM END\n" );
	return ret;
}
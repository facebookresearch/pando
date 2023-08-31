// console/setdata_lut
//

#include "../misc/console4.h"
#include "../misc/common.h"

BOOL dcamcon_show_capability_lut( HDCAM hdcam )
{
	DCAMERR err;

	// get capability of LUT
	DCAMDEV_CAPABILITY_LUT	caplut;
	memset( &caplut, 0, sizeof(caplut) );
	caplut.hdr.size		= sizeof(caplut);
	caplut.hdr.domain	= DCAMDEV_CAPDOMAIN__DCAMDATA;
	caplut.hdr.kind		= DCAMDATA_KIND__LUT;

	err = dcamdev_getcapability( hdcam, &caplut.hdr );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamdev_getcapability()", "DOMAIN:DCAMDATA, KIND:LUT" );
		return FALSE;
	}

	printf( "\n" );

	int32 luttype	= (caplut.hdr.capflag & DCAMDATA_LUTTYPE__BODYMASK);
	int32 attribute	= (caplut.hdr.capflag & DCAMDATA_LUTTYPE__ATTRIBUTEMASK);

	printf( "Support LUT type\n" );
	if( luttype == DCAMDATA_LUTTYPE__NONE )
	{
		printf( "\tNone\n" );
		return FALSE;
	}
	else
	{
		if( luttype & DCAMDATA_LUTTYPE__SEGMENTED_LINEAR )
		{
			printf( "\tSegumented Linear(Max Point of Linear:%d)\n", caplut.linearpointmax );
			luttype -= DCAMDATA_LUTTYPE__SEGMENTED_LINEAR;
		}
		
		if( luttype != 0 )
		{
			printf( "\tUnknown Type(0x%08x)\n", luttype );
		}
	}

	printf( "LUT attribute\n" );
	printf( "\tAccess Busy:" );
	if( attribute & DCAMDATA_LUTTYPE__ACCESSBUSY )
		printf( "\tOK\n" );
	else
		printf( "\tNG\n" );

	printf( "\tAccess Ready:" );
	if( attribute & DCAMDATA_LUTTYPE__ACCESSREADY )
		printf( "\tOK\n" );
	else
		printf( "\tNG\n" );

	printf( "\n" );

	return TRUE;
}

void set_linearpoint( DCAMDATA_LINEARLUT* p, int32& nNextIndex, int32 value_in, int32 value_out )
{
	p[nNextIndex].lutin		= value_in;
	p[nNextIndex].lutout	= value_out;

	nNextIndex++;
}

BOOL dcamcon_set_lutparam( HDCAM hdcam, int32 nPage )
{
	DCAMERR err;

	// get capability of LUT
	DCAMDEV_CAPABILITY_LUT	caplut;
	memset( &caplut, 0, sizeof(caplut) );
	caplut.hdr.size		= sizeof(caplut);
	caplut.hdr.domain	= DCAMDEV_CAPDOMAIN__DCAMDATA;
	caplut.hdr.kind		= DCAMDATA_KIND__LUT;

	err = dcamdev_getcapability( hdcam, &caplut.hdr );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamdev_getcapability()", "DOMAIN:DCAMDATA, KIND:LUT" );
		return FALSE;
	}

	int32 luttype	= (caplut.hdr.capflag & DCAMDATA_LUTTYPE__BODYMASK);
	
	if( luttype & DCAMDATA_LUTTYPE__SEGMENTED_LINEAR )
	{
		// set LUT with segmented linear
		int32 nMaxPoint = caplut.linearpointmax;
		if( caplut.linearpointmax <= 0 )
		{
			printf( "linearpointmax is invalid value!.\n" );
			return FALSE;
		}

		DCAMDATA_LINEARLUT* pLinearlut = new DCAMDATA_LINEARLUT[ nMaxPoint ];
		memset( pLinearlut, 0, sizeof(DCAMDATA_LINEARLUT) * nMaxPoint );

		int32 nSetPoint = 0;
		// set linear point. the maximum is linearpointmax of DCAMDEV_CAPABILITY_LUT.
		// it is necessary to set larger value than previous point. both ends are clipped.
		set_linearpoint( pLinearlut, nSetPoint, 0, 0 );
		set_linearpoint( pLinearlut, nSetPoint, 65535, 4095 );

		ASSERT( nSetPoint <= nMaxPoint );

		DCAMDATA_LUT	lutdata;
		memset( &lutdata, 0, sizeof(lutdata) );
		lutdata.hdr.size	= sizeof(lutdata);
		lutdata.hdr.iKind	= DCAMDATA_KIND__LUT;
		lutdata.type		= DCAMDATA_LUTTYPE__SEGMENTED_LINEAR;
		lutdata.page		= nPage;
		lutdata.data		= pLinearlut;
		lutdata.datasize	= sizeof(DCAMDATA_LINEARLUT) * nSetPoint;

		err = dcamdev_setdata( hdcam, &lutdata.hdr );
		if( failed(err) )
			dcamcon_show_dcamerr( hdcam, err, "dcamdev_setdata()", "KIND:LUT, TYPE:SEGEMENTED_LINEAR" );

		delete pLinearlut;

		return !failed(err);
	}
	else
		printf( "Unknown LUT type. Not implement sample.\n" );

	return FALSE;
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

		// show capability lut
		if( dcamcon_show_capability_lut( hdcam ) )
		{
			int32 nTargetPage = 1;	// it is necessary to check the supported page by dcamprop_getattr() is set DCAM_IDPROP_INTENSITYLUT_PAGE to iProp of DCAMPROP_ATTR.

			// set lut paramter
			if( dcamcon_set_lutparam( hdcam, nTargetPage ) )
			{
				// set lut page
				err = dcamprop_setvalue( hdcam, DCAM_IDPROP_INTENSITYLUT_PAGE, nTargetPage );
				if( !failed(err) )
				{
					// enable lut table
					err = dcamprop_setvalue( hdcam, DCAM_IDPROP_INTENSITYLUT_MODE, DCAMPROP_INTENSITYLUT_MODE__PAGE );
					if( !failed( err ) )
					{
						// TODO: add your process to get image
					}
					else
						dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:INTENSITYLUT_MODE, VALUE:PAGE" );
				}
				else
					dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:INTENSITYLUT_PAGE, VALUE:%d", nTargetPage );
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
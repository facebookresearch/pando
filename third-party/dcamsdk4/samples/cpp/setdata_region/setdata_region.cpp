// console/setdata_region
//

#include "../misc/console4.h"
#include "../misc/common.h"

#define	USE_BYTEARRAYMASK	0	// 0: use rectangle array to set region , 1: use byte array mask to set region

BOOL dcamcon_show_capability_region( HDCAM hdcam )
{
	DCAMERR err;

	// get capability of region
	DCAMDEV_CAPABILITY_REGION	capregion;
	memset( &capregion, 0, sizeof(capregion) );
	capregion.hdr.size		= sizeof(capregion);
	capregion.hdr.domain	= DCAMDEV_CAPDOMAIN__DCAMDATA;
	capregion.hdr.kind		= DCAMDATA_KIND__REGION;

	err = dcamdev_getcapability( hdcam, &capregion.hdr );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamdev_getcapability()", "DOMAIN:DCAMDATA, KIND:REGION" );
		return FALSE;
	}

	printf( "\n" );

	int32 regiontype	= (capregion.hdr.capflag & DCAMDATA_REGIONTYPE__BODYMASK);
	int32 attribute		= (capregion.hdr.capflag & DCAMDATA_REGIONTYPE__ATTRIBUTEMASK);

	printf( "Support region type\n" );
	if( regiontype == DCAMDATA_REGIONTYPE__NONE )
	{
		printf( "\tNone\n" );
		return FALSE;
	}
	else
	{
		if( regiontype & DCAMDATA_REGIONTYPE__RECT16ARRAY )
		{
			printf( "\tRectangle Array\n" );
			regiontype -= DCAMDATA_REGIONTYPE__RECT16ARRAY;
		}

		if( regiontype & DCAMDATA_REGIONTYPE__BYTEMASK )
		{
			printf( "\tByte Mask\n" );
			regiontype -= DCAMDATA_REGIONTYPE__BYTEMASK;
		}

		if( regiontype != 0 )
		{
			printf( "Unknown Type(0x%08x)\n", regiontype );
		}
	}

	printf( "Region Unit\n" );	// region is spreaded by the unit.
	printf( "\tHorizontal\t%d\n", capregion.horzunit );
	printf( "\tVertical\t%d\n", capregion.vertunit );

	printf( "Region attribute\n" );
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

void get_maxsize( HDCAM hdcam, int32& width, int32& height )
{
	DCAMERR err;

	DCAMPROP_ATTR	propattr;
	memset( &propattr, 0, sizeof(propattr) );
	propattr.cbSize	= sizeof(propattr);
	propattr.iProp	= DCAM_IDPROP_IMAGE_WIDTH;

	err = dcamprop_getattr( hdcam, &propattr );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_getattr()", "IDPROP:IMAGE_WIDTH" );
		return;
	}
	
	width = (int32)propattr.valuemax;

	memset( &propattr, 0, sizeof(propattr) );
	propattr.cbSize	= sizeof(propattr);
	propattr.iProp	= DCAM_IDPROP_IMAGE_HEIGHT;

	err = dcamprop_getattr( hdcam, &propattr );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamprop_getattr()", "IDPROP:IMAGE_HEIGHT" );
		return;
	}
	
	height = (int32)propattr.valuemax;
}

BOOL dcamcon_set_regionparam_bytearray( HDCAM hdcam )
{
	DCAMERR err;

	// get capability of region
	DCAMDEV_CAPABILITY_REGION	capregion;
	memset( &capregion, 0, sizeof(capregion) );
	capregion.hdr.size		= sizeof(capregion);
	capregion.hdr.domain	= DCAMDEV_CAPDOMAIN__DCAMDATA;
	capregion.hdr.kind		= DCAMDATA_KIND__REGION;

	err = dcamdev_getcapability( hdcam, &capregion.hdr );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamdev_getcapability()", "DOMAIN:DCAMDATA", "KIND:REGION" );
		return FALSE;
	}

	int32 regiontype	= (capregion.hdr.capflag & DCAMDATA_REGIONTYPE__BODYMASK);
	int32 attribute		= (capregion.hdr.capflag & DCAMDATA_REGIONTYPE__ATTRIBUTEMASK);

	if( regiontype & DCAMDATA_REGIONTYPE__BYTEMASK )
	{
		int32 width, height;
		get_maxsize( hdcam, width, height );

		int32 nPixcount = width * height;

		BYTE* pMask = new BYTE[ nPixcount ];
		memset( pMask, 0, sizeof(BYTE) * nPixcount );

		// make mask (0,0) - (99, 99). 100 x 100
		BYTE* pB = pMask;
		int h, w;
		for( h=0; h<100; h++ )
		{
			for( w=0; w<100; w++ )
			{
				pB[w] = 1;
			}

			pB += width;
		}

		if( attribute & DCAMDATA_REGIONTYPE__HASVIEW )
		{
			// set to each view
			double v;
			err = dcamprop_getvalue( hdcam, DCAM_IDPROP_NUMBEROF_VIEW, &v );
			ASSERT( !failed(err) && v > 1 );

			int32 nView = (int32)v;

			int i;
			for( i=0; i<=nView; i++ )
			{
				// set region with BYTEMASK
				DCAMDATA_REGION	regiondata;
				memset( &regiondata, 0, sizeof(regiondata) );
				regiondata.hdr.size		= sizeof(regiondata);
				regiondata.hdr.iKind	= DCAMDATA_KIND__REGION;
				regiondata.hdr.option	= DCAMDATA_OPTION__VIEW__STEP * i;
				regiondata.type			= DCAMDATA_REGIONTYPE__BYTEMASK;
				regiondata.data			= pMask;
				regiondata.datasize		= sizeof(BYTE) * nPixcount;

				err = dcamdev_setdata( hdcam, &regiondata.hdr );
				if( failed(err) )
				{
					dcamcon_show_dcamerr( hdcam, err, "dcamdev_setdata()", "KIND:REGION, OPTION:VIEW_%d, TYPE:BYTEMASK", i );
					break;
				}
			}
		}
		else
		{
			// set region with BYTEMASK
			DCAMDATA_REGION	regiondata;
			memset( &regiondata, 0, sizeof(regiondata) );
			regiondata.hdr.size		= sizeof(regiondata);
			regiondata.hdr.iKind	= DCAMDATA_KIND__REGION;
			regiondata.type			= DCAMDATA_REGIONTYPE__BYTEMASK;
			regiondata.data			= pMask;
			regiondata.datasize		= sizeof(BYTE) * nPixcount;

			err = dcamdev_setdata( hdcam, &regiondata.hdr );
			if( failed(err) )
				dcamcon_show_dcamerr( hdcam, err, "dcamdev_setdata()", "KIND:REGION, TYPE:BYTEMASK"  );
		}

		delete pMask;

		return !failed(err);
	}
	else
		printf( "Not Support Byte Mask\n" );

	return FALSE;
}

void set_rectparam( DCAMDATA_REGIONRECT* pRect, int32& iNextRect, short left, short top, short right, short bottom )
{
	pRect[ iNextRect ].left		= left;
	pRect[ iNextRect ].top		= top;
	pRect[ iNextRect ].right	= right;
	pRect[ iNextRect ].bottom	= bottom;

	iNextRect++;
}

BOOL dcamcon_set_regionparam_rectarray( HDCAM hdcam )
{
	DCAMERR err;

	// get capability of region
	DCAMDEV_CAPABILITY_REGION	capregion;
	memset( &capregion, 0, sizeof(capregion) );
	capregion.hdr.size		= sizeof(capregion);
	capregion.hdr.domain	= DCAMDEV_CAPDOMAIN__DCAMDATA;
	capregion.hdr.kind		= DCAMDATA_KIND__REGION;

	err = dcamdev_getcapability( hdcam, &capregion.hdr );
	if( failed(err) )
	{
		dcamcon_show_dcamerr( hdcam, err, "dcamdev_getcapability()", "DOMAIN:DCAMDATA, KIND:REGION" );
		return FALSE;
	}

	int32 regiontype	= (capregion.hdr.capflag & DCAMDATA_REGIONTYPE__BODYMASK);
	int32 attribute		= (capregion.hdr.capflag & DCAMDATA_REGIONTYPE__ATTRIBUTEMASK);

	if( regiontype & DCAMDATA_REGIONTYPE__RECT16ARRAY )
	{
		// not limit the number of rectangle.
		DCAMDATA_REGIONRECT	rect[16];
		memset( rect, 0, sizeof(rect) );

		if( attribute & DCAMDATA_REGIONTYPE__HASVIEW )
		{
			// set to each view
			double v;
			err = dcamprop_getvalue( hdcam, DCAM_IDPROP_NUMBEROF_VIEW, &v );
			ASSERT( !failed(err) && v > 1 );

			int32 nView = (int32)v;

			int i;
			for( i=1; i<=nView; i++ )
			{
				memset( rect, 0, sizeof(rect) );

				int32 nRect = 0;
				// not normalize position.
				set_rectparam( rect, nRect, 0, 0, 99, 99 );	// 100 x 100

				ASSERT( nRect <= 16 );

				// set region with REGIONRECT
				DCAMDATA_REGION	regiondata;
				memset( &regiondata, 0, sizeof(regiondata) );
				regiondata.hdr.size		= sizeof(regiondata);
				regiondata.hdr.iKind	= DCAMDATA_KIND__REGION;
				regiondata.hdr.option	= DCAMDATA_OPTION__VIEW__STEP * i;
				regiondata.type			= DCAMDATA_REGIONTYPE__RECT16ARRAY;
				regiondata.data			= rect;
				regiondata.datasize		= sizeof(DCAMDATA_REGIONRECT) * nRect;

				err = dcamdev_setdata( hdcam, &regiondata.hdr );
				if( failed(err) )
				{
					dcamcon_show_dcamerr( hdcam, err, "dcamdev_setdata()", "KIND:REGION, OPTION:VIEW_%d, TYPE:RECT16ARRAY", i );
					break;
				}
			}
		}
		else
		{
			int32 nRect = 0;
			
			// not normalize position.
			set_rectparam( rect, nRect, 0, 0, 99, 99 );	// 100 x 100

			ASSERT( nRect <= 16 );

			// set region with REGIONRECT
			DCAMDATA_REGION	regiondata;
			memset( &regiondata, 0, sizeof(regiondata) );
			regiondata.hdr.size		= sizeof(regiondata);
			regiondata.hdr.iKind	= DCAMDATA_KIND__REGION;
			regiondata.type			= DCAMDATA_REGIONTYPE__RECT16ARRAY;
			regiondata.data			= rect;
			regiondata.datasize		= sizeof(DCAMDATA_REGIONRECT) * nRect;

			err = dcamdev_setdata( hdcam, &regiondata.hdr );
			if( failed(err) )
				dcamcon_show_dcamerr( hdcam, err, "dcamdev_setdata()", "KIND:REGION, TYPE:RECT16ARRAY" );
		}

		return !failed(err);
	}
	else
		printf( "Not Support Rectangle 16 Array\n" );

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

		// show capability region
		if( dcamcon_show_capability_region( hdcam ) )
		{
			// set region parameter
#if USE_BYTEARRAYMASK
			if( dcamcon_set_regionparam_bytearray( hdcam ) )
#else
			if( dcamcon_set_regionparam_rectarray( hdcam ) )
#endif
			{
				// enable extraction mode
				err = dcamprop_setvalue( hdcam, DCAM_IDPROP_EXTRACTION_MODE, DCAMPROP_MODE__ON );
				if( !failed(err) )
				{
					// TODO: add your process to get image
				}
				else
					dcamcon_show_dcamerr( hdcam, err, "dcamprop_setvalue()", "IDPROP:EXTRACTION_MODE, VALUE:ON" );
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
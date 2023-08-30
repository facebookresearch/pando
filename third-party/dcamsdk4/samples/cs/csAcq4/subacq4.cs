// subacq4.cs

// implementation of subacq.copydib() function. SUBACQ4.DLL is not necessary.

using System;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Drawing.Imaging;
using Hamamatsu.DCAM4;

#if ! USE_SUBACQ_DLL

namespace Hamamatsu.subacq4
{
    public enum SUBACQERR : uint
    {
        // error
        NO_DLL = 0x80000000,
        INVALID_ARG = 0x80000001,
        INVALID_DST = 0x80000011,
        INVALID_DSTPIXELFORMAT = 0x80000012,
        INVALID_SRC = 0x80000021,
        INVALID_SRCPIXELTYPE = 0x80000022,

        NOSUPPORT_LUTARRAY = 0x80000101,

        // success
        SUCCESS = 1
    }

    public class subacq
    {
        public static SUBACQERR copydib(ref Bitmap bitmap, DCAMBUF_FRAME src, ref Rectangle rect, int lutmax, int lutmin)
        {
            int w = rect.Width;
            int h = rect.Height;
            if (w > bitmap.Width) w = bitmap.Width;
            if (h > bitmap.Height) h = bitmap.Height;
            if (w > src.width) w = src.width;
            if (h > src.height) h = src.height;

            BitmapData dst = bitmap.LockBits(rect, ImageLockMode.ReadWrite, PixelFormat.Format32bppRgb);
            
            SUBACQERR err;
            if (src.type == DCAM_PIXELTYPE.MONO16)
                err = copydib_rgb32_from_mono16( dst.Scan0, dst.Stride, src.buf, src.rowbytes, w, h, lutmax, lutmin );
            else
                err = SUBACQERR.INVALID_SRCPIXELTYPE;
            bitmap.UnlockBits(dst);

            return err;
        }

        private static SUBACQERR copydib_rgb32_from_mono16( IntPtr dst, Int32 dstrowbytes, IntPtr src, Int32 srcrowbytes, Int32 width, Int32 height, Int32 lutmax, Int32 lutmin )
        {
            Int16[] s = new Int16[width];
            Int32[] d = new Int32[width];

            double gain = 0;
            double inBase = 0;

            if (lutmax != lutmin)
            {
                if (lutmin < lutmax)
                {
                    gain = 256.0 / (lutmax - lutmin + 1);
                    inBase = lutmin;
                }
                else
                if (lutmin > lutmax)
                {
                    gain = 256.0 / (lutmax - lutmin - 1);
                    inBase = lutmax;
                }
            }
            else
            if (lutmin > 0)    // binary threshold
            {
                gain = 0;
                inBase = lutmin;
            }

            Int32 y;
            for (y = 0; y < height; y++)
            {
                Int32 offset;

                offset = srcrowbytes * y;
                Marshal.Copy((IntPtr)(src.ToInt64() + offset), s, 0, width);
                
                copydibline_rgb32_from_mono16(d, s, width, gain, inBase);

                offset = dstrowbytes * y;
                Marshal.Copy(d, 0, (IntPtr)(dst.ToInt64() + offset), width);
            }
            return SUBACQERR.SUCCESS;
        }

        private static void copydibline_rgb32_from_mono16( Int32[] d, Int16[] s, Int32 width, double gain, double inBase )
        {
            Int32   x;
            if( gain != 0 )
            {
		        for( x = 0; x < width; x++ )
		        {
                    UInt16  u = (UInt16)s[x];

                    double v = gain * (u - inBase);

                    Byte c;
			        if( v > 255 )
				        c = 255;
			        else
			        if( v < 0 )
				        c = 0;
			        else
				        c = (Byte)v;

                    d[x] = Color.FromArgb(c, c, c).ToArgb();
		        }
	        }
            else            
            if( inBase > 0 )    // binary threshold
            {
		        for( x = 0; x < width; x++ )
		        {
                    UInt16  u = (UInt16)s[x];

                    Byte    c = (Byte)(u >= inBase ? 255 : 0);

                    d[x] = Color.FromArgb(c, c, c).ToArgb();
                }
	        }
	        else
            {
		        for( x = 0; x < width; x++ )
		        {
                    UInt16  u = (UInt16)s[x];

                    Byte c = (Byte)(u >> 8);

                    d[x] = Color.FromArgb(c, c, c).ToArgb();
                }
            }
		}
    }
}

#endif // ! USE_SUBACQ_DLL

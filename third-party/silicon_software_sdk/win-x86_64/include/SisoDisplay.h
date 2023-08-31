/**
 * SisoDisplay.h
 *
 * Copyright (c) 2002-2017 Silicon Software GmbH, All Rights Reserved.
 *
 * \file SisoDisplay.h
 *
 * \brief Header file for all display lib public functions.
 *
 * \author Silicon Software GmbH
 */

#ifndef __SISO_DISPLAY_H
#define __SISO_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief	Create Display
 *
 * The function CreateDisplay() creates a display window. It can display gray and color images up to 2GB of size.
 * 
 * \param	nDepth	 	Color depth  b/w: 1bit  gray: 8bit, 16bit color: 24bit, 48bit  Use the value 565 for 16Bit RGB 565
 * \param	nWidth	 	Width of window in pixels.
 * \param	nHeight	 	Height of window in pixels.
 *
 * \returns Display ID on success, or negative value in case of an error.
 * \retval 0..n display ID
 * \retval #FG_NOT_ENOUGH_MEM out of memory
 * \retval #FG_INVALID_PARAMETER invalid depth
 * \retval #FG_INVALID_IMAGE_DIMENSIONS image is larger than 2GB
 */

int CreateDisplay(const unsigned int nDepth, const unsigned int nWidth, const unsigned int nHeight);

/**
 * \brief	Sets a buffer width.
 *
 * The function SetBufferWidth() configures the size of the frame buffer. Hereby a window can be displayed, which is smaller than the frame buffer.
 *
 * \param	nId			ID of the display.
 * \param	nWidth		Width of the window in pixels.
 * \param	nHeight		Height of the window in pixels.
 */

void SetBufferWidth(int nId, const unsigned int nWidth, const unsigned int nHeight);

/**
 * \brief	Draw buffer.
 *
 * The function DrawBuffer() displays an image. The current pointer of an image will be returned by the function Fg_getImagePtr().
 *
 * \param	nId		ID of the created window.
 * \param	ulpBuf		Pointer to the frame buffer, which is to be displayed.
 * \param	nNr		Number of the image.
 * \param	cpStr		A string, which is displayed in the windows bar.
 */

void DrawBuffer(int nId, const void *ulpBuf, const int nNr, const char *cpStr);

/**
 * \brief	Closes a display.
 *
 * The function CloseDisplay() closes the window and releases the allocated resources.
 *
 * \param	nId	ID of the display.
 */

void CloseDisplay(int nId);

/**
 * \brief	Sets a display depth.
 *
 * SetDisplayDepth() has been added for Linux systems. It enables a user specific setting (at RGB 24bit to use 32bit).
 *
 * \param	nId		ID of the display.
 * \param	depth		32bit, if neccessary.
 *
 * \retval	Not Documented!!
 */

int SetDisplayDepth(int nId, const unsigned int depth);

#ifdef __cplusplus
}
#endif

#endif /* __FGRAB_PROTOTYP_H */

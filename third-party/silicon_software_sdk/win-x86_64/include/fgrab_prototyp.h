/**
 * fgrab_prototyp.h
 *
 * Copyright (c) 2002-2017 Silicon Software GmbH, All Rights Reserved.
 *
 * \file fgrab_prototyp.h
 *
 * \brief Header file for all functions exported.
 *
 * \author Silicon Software GmbH
 */

#ifndef __FGRAB_PROTOTYP_H
#define __FGRAB_PROTOTYP_H

#pragma warning(push)
#pragma warning(disable: 4005)
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <msinttypes/stdint.h>
#else
#include <stdint.h>
#endif
#pragma warning(pop)

#include <sys/types.h>

#include "fgrab_struct.h"
#include "fgrab_define.h"
#include "os_type.h"

#include "SisoDisplay.h"

#ifdef __cplusplus
#include <string>
#include <sstream>
#include <new>
#include <cstring>
extern "C"
{
class dma_mem;
class ShadingMaster;
#else
struct dma_mem_s;
typedef struct dma_mem_s dma_mem;
struct ShadingMaster_s;
typedef struct ShadingMaster_s ShadingMaster;
#endif

struct fg_event_data;
struct fg_event_info;



/** @name Library initialization and release
 *
 * These functions will initialize the library.
 * Calling these functions is completely optional, but can speed up enumeration of devices in certain circumstances
 * significantly. Note however, that if you call Fg_InitLibraries(), you must also call Fg_FreeLibraries.
 */
/*@{*/

/**
 * \brief The function Fg_InitLibraries() initializes internal structures of the library.
 * \anchor  Fg_InitLibraries
 *
 * This function initializes internal structures in the library and keeps them in memory until
 * Fg_FreeLibraries() is called. This can help to speed up certain operations like enumerating
 * boards or getting information about a board before calling Fg_Init().
 * It is recommended to call this function in the main() method before any other calls to fglib5,
 * or any other library supplied with the Silicon Software Runtime.
 * When Fg_InitLibraries() returns 0, you must call #Fg_FreeLibraries() before exiting your program.
 *
 * \param   sisoDir      User supplied directory to Runtime installation (optional; can be NULL)
 *
 * \returns 0 on success, or a non-zero error value otherwise
 * \retval #FG_OK                   success
 * \retval #FG_NOT_IMPLEMENTED      function is not implemented
 * \retval #FG_SISODIR5_NOT_SET     no search path available
 * \retval #FG_OPERATION_ABORTED    synchronization has been aborted
 *
 */
int Fg_InitLibraries(const char * sisoDir);

/**
 * \brief The function Fg_InitLibrariesEx() initializes internal structures of the library and synchronizes the startup of the application(s).
 * \anchor  Fg_InitLibrariesEx
 *
 * This function initializes internal structures in the library and keeps them in memory until
 * Fg_FreeLibraries() is called. This can help to speed up certain operations like enumerating
 * boards or getting information about a board before calling Fg_Init().
 * Furthermore, the startup of the application can be synchronized with the startup of the
 * Generic Service, or with a Master or the other Slaves in a Master/Slave setup.
 * It is recommended to call this function in the main() method before any other calls to fglib5,
 * or any other library supplied with the Silicon Software Runtime.
 * When Fg_InitLibraries() returns 0, you must call #Fg_FreeLibraries() before exiting your program.
 *
 * Note: microDisplay can be used as a master and you can synchronize your own slave application against microDisplay.
 * The id of microDisplay is siso-microdisplay-master and it uses (#FG_INIT_LIBRARIES_MASTER | #FG_INIT_LIBRARIES_AUTOSTART_ON_INIT).
 * To synchronize your slave application against microDisplay, use the same id and #FG_INIT_LIBRARIES_SLAVE.
 * Strictly sequential startup is not supported by microDisplay, and thus slave priorities are not allowed.
 *
 * Note: The following flags and macros can be used to control the synchronization:
 * #FG_INIT_LIBRARIES_SINGLE                     Standard init, no synchronization (same as calling Fg_InitLibraries())
 * #FG_INIT_LIBRARIES_MASTER                     Master init, prepare slave synchronization
 * #FG_INIT_LIBRARIES_SLAVE                      Slave init, wait for master
 * #FG_INIT_LIBRARIES_WAIT_FOR_SERVICE           Wait for service to be started (valid only for single application and master modes)
 * #FG_INIT_LIBRARIES_SEQUENTIAL                 Strictly sequential startup (slaves must use unique priority; number of slaves must be given on master init)
 * #FG_INIT_LIBRARIES_AUTOSTART_ON_INIT          Start next slave on Fg_Init()
 * #FG_INIT_LIBRARIES_SET_MODE(n)                Set mode (single, master, slave)
 * #FG_INIT_LIBRARIES_SET_SLAVE_PRIORITY(n)      Set slave priority (allowed values are 1..63; priority 0 will select priority automatically)
 * #FG_INIT_LIBRARIES_SET_NUMBER_OF_SLAVES(n)    Set number of slaves (allowed values are 1..63)
 *
 * \param sisoDir      User supplied directory to Runtime installation (optional; can be NULL)
 * \param flags        Additional flags to control the synchronization
 * \param id           The identifier which should be used for synchronization (Master and Slaves must use the same identifier; not used in single mode)
 * \param timeout      Timeout in milliseconds to wait for synchronization
 *
 * \returns 0 on success, or a non-zero error value otherwise
 * \retval #FG_OK                   success
 * \retval #FG_NOT_IMPLEMENTED      function is not implemented
 * \retval #FG_SISODIR5_NOT_SET     no search path available
 * \retval #FG_OPERATION_ABORTED    synchronization has been aborted
 * \retval #FG_TIMEOUT              waiting for synchronization timed out
 *
 */
int Fg_InitLibrariesEx(const char * sisoDir, unsigned int flags, const char * id, unsigned int timeout);

/**
 * \brief The function Fg_AbortInitLibraries() aborts and fails the initialization of internal structures
 * and the synchronization of the startup of the application(s).
 * \anchor  Fg_AbortInitLibraries
 *
 * This function aborts the initialization of internal structures in the library and interrupts any pending
 * synchronization that is requested by the user calling Fg_InitLibraries() or Fg_InitLibrariesEx().
 * This function can be called from within a signal handler or any other mechanism that terminates the
 * application asynchronously to make sure the main thread will not hang indefinitely.
 * If the call to Fg_InitLibraries() or Fg_InitLibrariesEx() was aborted, those functions will return
 * FG_OPERATION_ABORTED.
 *
 */
void Fg_AbortInitLibraries();

/**
 * \brief The function Fg_InitLibrariesStartNextSlave() signals the next slave process to start.
 * \anchor  Fg_InitLibrariesStartNextSlave
 *
 * This function is required for synchronization of the startup of processes in master/slave mode when
 * FG_INIT_LIBRARIES_AUTOSTART_SLAVE was not set in Fg_IitLibrariesEx. This is generally needed if the process requires further
 * initialization beyond Fg_Init() that must be synchronized with other slave processes or the master process. Most likely, this
 * will be applicable to initialization in the master process, but should generally not be the case for slave processes, as these
 * should be using completely separate resources (DMA, parameters, ...).
 *
 */
void Fg_InitLibrariesStartNextSlave();

/**
 * \brief The function Fg_FreeLibraries() releases internal structures of the library.
 * \anchor  Fg_FreeLibraries
 *
 * This function releases all internal structures that were initialized by a successful call
 * to #Fg_InitLibraries().
 * It is recommended to call this function just before the program leaves the main() method,
 * or exits in any other way.
 *
 */
void Fg_FreeLibraries();
/*@}*/

/** @name Board initialization and release
 *
 * These functions will initialize the requested device. They need to
 * be called before any other operation on the device can be performed.
 * The only functions that can be called independent of the device
 * are some functions to query software or device information.
 */
/*@{*/

/**
 * \brief The function Fg_findApplet() returns the file name of the currently active applet for ME5 board.
 * \anchor  Fg_findApplet
 *
 * The applet file is searched within the active Silicon Software Runtime environment (e.g. SISODIR5)
 * If the applet file name exceeds the size of the buffer of returning the 
 * information, FG_NOT_ENOUGH_MEMORY is returned.
 * This function can be used only with ME5 boards. Otherwise, FG_INVALID_BOARD_NUMBER
 * is returned.
 *
 * \param   BoardIndex   Logical number of the board.
 * \param   Path         Buffer for returned string
 * \param   Size         Size of the buffer in bytes 
 *
 * \returns name of the applet as a 0 terminated string.
 *
 * \retval  #FG_OK                   success, Path contains the applet path
 * \retval  #FG_INVALID_PARAMETER    buffer or buffer size are invalid
 * \retval  #FG_INVALID_BOARD_NUMBER invalid board number (or board type)
 * \retval  #FG_DLL_NOT_LOAD unknown board type
 * \retval  #FG_SISODIR5_NOT_SET     no search path available
 * \retval  #FG_NOT_ENOUGH_MEMORY    applet file name exceeds the size of the buffer 
 *
 */
int Fg_findApplet(const unsigned int BoardIndex, char * Path, size_t Size);

/**
 * \brief	The function Fg_Init() initializes the frame grabber.
 * \anchor  Fg_Init
 * 
 * This functions is equivalent to calling Fg_InitEx(FileName, BoardIndex, 0). Please see the
 * documentation of Fg_InitEx() for a detailed description of the parameters.
 *
 * An initialized frame grabber resource by Fg_Init() must be released by Fg_FreeGrabber().
 *
 * \param	FileName	Applet name for initialization.
 * \param	BoardIndex	Logical number of the board.
 *
 * \returns	Initialized pointer on the data structure of the FrameFrabber.
 * \retval	NULL The initialization failed.
 *
 * If this function returns NULL you may call Fg_getLastErrorNumber(NULL) to get the error code.
 */
Fg_Struct *Fg_Init(const char *FileName, unsigned int BoardIndex);

/**
 * \brief	The function Fg_InitConfig() initializes the frame grabber and loads a parameter file (microEnable configuration file *.mcf).
 * \anchor  Fg_InitConfig
 * 
 * If more than one microEnable frame grabber is available within PC system, each hardware card will
 * differ by a logical number, which is distributed by initialization. Sorting of the logical board
 * number is influenced by each hardware's serial number. The serial number is printed on each board
 * and can be displayed by microDiagnostics or microDisplay. An additional possibility for identifying
 * several grabbers is to use a function to ask for serial number via Fg_getSerialNumber().
 *
 * Attention: Each frame grabber card will be handled uniquely by a separate data structure Fg_Struct,
 * after initialization. The selection of frame grabbers is sorted by the increasing number of slots
 * within PC. Please note, that the unique ID can differ from PC to PC.
 *
 * An initialized frame grabber resource by Fg_InitConfig() will released by Fg_FreeGrabber().
 *
 * \param	Config_Name	Applet name for initialization.
 * \param	BoardIndex	Logical number of the board.
 *
 * \returns	Initialized pointer on the data structure of the FrameFrabber.
 *
 * \retval	NULL The initialization failed.
 */
Fg_Struct *Fg_InitConfig(const char *Config_Name, unsigned int BoardIndex);


/**
 * \brief	Initialize a frame grabber in master or slave mode.
 * \anchor  Fg_InitEx
 * 
 * This function initializes a framegrabber with a given applet. Both standard applets as well
 * as VisualApplets generated HAP files are supported. The BoardIndex parameter is used to address
 * multiple framegrabbers in the same system.
 *
 * Board numbers depend on the system architecture and operating system. In general the board numbers
 * depend on the PCI setup, i.e. boards with lower PCI bus and slot numbers will have lower board
 * numbers. On Windows systems meIII grabbers will always get lower numbers than meIV devices. On
 * Linux the order depends only on the PCI bus numbers, resulting in meIII devices usually appearing
 * on higher device numbers as PCI buses are usually the last child bus of PCI Express bridges. The
 * easiest way to find out the mapping from boards to their board id is using microDiagnostics which
 * shows board type, serial number, and board index.
 *
 * A grabber that is initialized in "master" mode will configure the FPGA. A grabber initialized as
 * slave will not configure the FPGA. In fact it checks if the FPGA is already configured with the
 * same design as requested. Not all applets and platforms support slave configurations, e.g. currently
 * VA generated HAPs, mE4AD1-CL boards, and mE4AS1-CL boards do not support slave mode.
 *
 * A grabber initialized as slave has access to all resources of the board. Locking for concurrent
 * accesses is left to the user to allow maximum flexibility. On the other hand some special resources
 * will always protect themselves against concurrent usage, e.g DMA channels can't be started if they are
 * already running.
 *
 * A board can only be opened in slave mode if the master is already opened, otherwise the error code
 * is set to #FG_NOT_AVAILABLE ( #FG_INVALID_PARAMETER , if slave modes isn't supported at all for this applet)
 *
 * An initialized frame grabber resource by Fg_InitEx() must be released by Fg_FreeGrabber().
 *
 * \param	FileName	Applet name for initialization.
 * \param	BoardIndex	Logical number of the board.
 * \param	flags		Flags to modify the initialization behavior.
 * 
 * \returns	Initialized pointer on the data structure of the FrameFrabber.
 * \retval	NULL The initialization failed.
 *
 * If this function returns NULL you may call Fg_getLastErrorNumber(NULL) to get the error code.
 */
Fg_Struct *Fg_InitEx(const char *FileName, unsigned int BoardIndex, int flags);

/**
 * \brief	The function Fg_InitConfigEx() initializes the frame grabber in master or slave mode and loads a parameter file (microEnable configuration file *.mcf).
 * \anchor  Fg_InitConfigEx
 * 
 * If more than one microEnable frame grabber is available within PC system, each hardware card will
 * differ by a logical number, which is distributed by initialization. Sorting of the logical board
 * number is influenced by each hardware's serial number. The serial number is printed on each board
 * and can be displayed by microDiagnostics or microDisplay. An additional possibility for identifying
 * several grabbers is to use a function to ask for serial number via Fg_getSerialNumber().
 *
 * Attention: Each frame grabber card will be handled uniquely by a separate data structure Fg_Struct,
 * after initialization. The selection of frame grabbers is sorted by the increasing number of slots
 * within PC. Please note, that the unique ID can differ from PC to PC.
 *
 * A grabber that is initialized in "master" mode will configure the FPGA. A grabber initialized as
 * slave will not configure the FPGA. In fact it checks if the FPGA is already configured with the
 * same design as requested. Not all applets and platforms support slave configurations, e.g. currently
 * VA generated HAPs, mE4AD1-CL boards, and mE4AS1-CL boards do not support slave mode.
 *
 * A grabber initialized as slave has access to all resources of the board. Locking for concurrent
 * accesses is left to the user to allow maximum flexibility. On the other hand some special resources
 * will always protect themselves against concurrent usage, e.g DMA channels can't be started if they are
 * already running.
 *
 * A board can only be opened in slave mode if the master is already opened, otherwise the error code
 * is set to #FG_NOT_AVAILABLE ( #FG_INVALID_PARAMETER , if slave modes isn't supported at all for this applet)
 *
 * Please note that a configuration has to be edited manually before it can be applied in slave mode safely.
 * Each save is supposed to touch only parameters that no other slave or the master will touch. A configuration
 * saved from microDisplay contains all parameters, and is not master/slave aware.
 *
 * An initialized frame grabber resource by Fg_InitConfig() will released by Fg_FreeGrabber().
 *
 * \param	Config_Name	Applet name for initialization.
 * \param	BoardIndex	Logical number of the board.
 * \param	flags		Flags to modify the initialization behavior (see MeInitFlags).
 *
 * \returns	Initialized pointer on the data structure of the FrameFrabber.
 *
 * \retval	NULL The initialization failed.
 */
Fg_Struct *Fg_InitConfigEx(const char *Config_Name, unsigned int BoardIndex, int flags);

/**
 * \brief	Releasing frame grabber resources.
 * \anchor  Fg_FreeGrabber
 * 
 * The function Fg_FreeGrabber() stops the operating frame grabber and releases the resources.
 * Either, an already allocated frame buffer by Fg_AllocMem() will be released. The function Fg_FreeMem()
 * will be executed internally. Otherwise, an allocated frame buffer by Fg_AllocMemEx() will be released.
 * In this case, the function Fg_FreeMemEx() will be executed internally. All internal resources will be released.
 *
 * \param	Fg	frame grabber to release
 *
 * \retval	#FG_OK The operating frame grabber has been stopped successfully.
 * \retval	#FG_NOT_INIT Frame grabber has not been initialized correctly.
 * \retval	#FG_FILE_NOT_FOUND The file cannot be found
 * \retval	#FG_INVALID_FILENAME The filename is not a valid filename
 * \retval	#FG_SISODIR5_NOT_SET The environment variable SISODIR5 is not set
 * \retval	#FG_NOT_ENOUGH_MEM The system memory is insufficient for loading control structures
 * \retval	#FG_CANNOT_INIT_MICROENABLE The frame grabber cannot be initialized
 * \retval	#FG_HAP_FILE_NOT_LOAD The Applet file (HAP file) is not a valid HAP file
 
 */
int Fg_FreeGrabber(Fg_Struct *Fg);
/*@}*/

/** @name Memory Management
 *
 * These functions allow fine grained control about the memory
 * used to transfer data to and from the frame grabber.
 */
/*@{*/
/**
 * \brief	The function Fg_AllocMem() reserves a defined area of the user memory as frame grabber memory and blocks it in the system memory.
 * \anchor  Fg_AllocMem
 * 
 * The frame buffer can be subdivided in several subbuffers.
 *
 * The frame buffer is organized as a ring buffer. It means, if an image is written in the last subbuffer of the frame buffer,
 * the next image will be transferred in the first subbuffer of the frame buffer. Existent images will be overwritten.
 *
 * Allocated memory can be released with Fg_FreeMem().
 *
 * Please note, that there are memory limits existing!
 *
 * \param	Fg	 	frame grabber to use
 * \param	Size	 	Size of all image memory in byte.
 * \param	BufCnt	 	Number of internal buffer. Value must be 2, when using double-buffering.
 * \param	DmaIndex 	Logical number of used DMA channel. channel 1 = 0, channel 2 = 1 ... and so on.
 *
 * \returns	A pointer to the allocated frame memory / image data.
 * \retval	NULL An error occured. Use Fg_getLastErrorDescription() for more information.
 */

void *Fg_AllocMem(Fg_Struct *Fg, const size_t Size, const frameindex_t BufCnt, const unsigned int DmaIndex);

/**
 * \brief	The function Fg_AllocMemEx() reserves an area of the main memory as frame buffer, blocks it and makes it available for the user.
 * \anchor  Fg_AllocMemEx
 * 
 * The frame buffer is organized as a ring buffer which is subdivided in several subbuffers. It is possible
 * to allocate up to 256 different frame buffers as long as the maximum size of the configurable frame
 * buffer is not exceeded.
 *
 * The assignment between acquired images and subbuffers, which contain the images within PC memory,
 * depends on the acquisition mode.
 *
 * Allocated memory can be released with Fg_FreeMemEx().
 * The returned pointer represents the administrative data structure for all frame buffers. 
 *
 * Please note, that there are memory limits existing!
 *
 * \param	Fg		frame grabber to use
 * \param	Size		Size of all image memory in byte.
 * \param	BufCnt		Number of subbuffers.
 *
 * \returns	A pointer to the allocated memory.
 *
 * \retval	NULL An error occured. Use Fg_getLastErrorDescription() for more information.
 */
dma_mem *Fg_AllocMemEx(Fg_Struct *Fg, const size_t Size, const frameindex_t BufCnt);

/**
 * \brief	Releasing a frame buffer.
 * \anchor  Fg_FreeMem
 * 
 * The function Fg_FreeMem() stops the frame grabber and releases the used memory which has been allocated with Fg_AllocMem() before.
 *
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	Logical number of used DMA channel. channel 1 = 0, channel 2 = 1 ... and so on.
 *
 * \retval	#FG_OK Image memory has been released successfully.
 * \retval	#FG_NOT_INIT A failed try to release. Although, frame grabber has not been initialized correctly.
 * \retval	#FG_STILL_ACTIVE Transfer of an image is still active. The transfer has to be stopped before releasing the frame grabber buffer.
 */
int Fg_FreeMem(Fg_Struct *Fg, const unsigned int DmaIndex);

/**
 * \brief	Releasing frame buffers.
 * \anchor  Fg_FreeMemEx
 * 
 * The function Fg_FreeMemEx() releases the used memory which has been reserved with Fg_AllocMemEx() before.
 * If the acquiring and access on the memory is still active, the function can not releases the memory area and returns
 * with an error message.
 *
 * \param	Fg	frame grabber to use
 * \param	mem	Handle on memory area which has been reserved with with Fg_AllocMemEx().
 *
 * \retval	#FG_OK Image memory has been released successfully.
 * \retval	#FG_NOT_INIT A failed try to release. Although, frame grabber has not been initialized correctly.
 * \retval	#FG_STILL_ACTIVE Transfer of an image is still active. The transfer has to be stopped before releasing the frame grabber buffer.
 */
int Fg_FreeMemEx(Fg_Struct *Fg, dma_mem *mem);

/**
 * \brief	Use an allocated user memory as frame buffer.
 * \anchor  Fg_AllocMemHead
 * 
 * To transfer frames into user provided memory these memory areas have to be announced to
 * the framegrabber system. Fg_AllocMemHead() creates the internal control stuctures to handle
 * these memory. Afterwards you have to add the different memory buffers using Fg_AddMem().
 * Single buffers can be released with Fg_DelMem() from the buffer as long as this buffer is not
 * currently used for frame transfer. The control structure must be released with Fg_FreeMemHead().
 *
 * Please see the chapter "General memory limits" in the framegrabber documentation for general
 * restrictions about the usable memory.
 *
 * \code
 * #define BUFFERS 4
 * #define BUFFERSIZE (1024 * 1024)
 * char buf[BUFFERS + 1][BUFFERSIZE];
 *
 * dma_mem *mhead = Fg_AllocMemHead(Fg, BUFFERSIZE * BUFFERS, BUFFERS);
 * if (mhead == NULL)
 * 	...
 *
 * for (int i = 0; i < BUFFERS; i++) {
 * 	int ret = Fg_AddMem(Fg, buf[i], BUFFERSIZE, i, mhead);
 * 	if (ret < 0)
 * 		...
 * }
 *
 * // replace buf[0] with buf[4] for frame transfer
 * Fg_DelMem(Fg, mhead, 0);
 * Fg_AddMem(Fg, buf[BUFFERS], BUFFERSIZE, 0, mhead);
 *
 * Fg_AcquireEx(Fg, ..., mhead);
 *
 * Fg_FreeMemHead(Fg, mhead);
 * \endcode
 *
 * \param	Fg		frame grabber to use
 * \param	Size		Size of all image memory in byte.
 * \param	BufCnt		Number of subbuffers.
 *
 * \returns	A valid handle on frame buffer.
 *
 * \retval	NULL An error has occured. Use Fg_getLastErrorDescription() for more information.A return value of 
 * FG_INVALID_MODULO indicates, that the requested memory size does not match to the requirements. 
 */
dma_mem *Fg_AllocMemHead(Fg_Struct *Fg, const size_t Size, const frameindex_t BufCnt);

/**
 * \brief	Releasing memory.
 * \anchor  Fg_FreeMemHead
 * 
 * The function Fg_FreeMemHead() releases a handle on a frame buffer that has been added with Fg_AllocMemHead().
 * If the acquisition and access to the memory is still active, the function is not able to release the memory
 * area and returns with an error message.
 *
 * You do not need to remove the frame memory using Fg_DelMem() before calling this function as they will
 * automatically be removed.
 *
 * Please note that this function will only remove the internal control structures and will not free the memory
 * passed to Fg_AddMem().
 *
 * \param	Fg		frame grabber to use
 * \param	memHandle 	Handle on memory area, which has been reserved by Fg_AllocMemEx() or Fg_AllocMemHead().
 *
 * \retval	#FG_OK Image memory has been released successfully.
 * \retval	#FG_NOT_INIT A failed try to release. Although, frame grabber has not been initialized correctly.
 * \retval	#FG_STILL_ACTIVE Transfer of an image is still active. The transfer has to be stopped before releasing the frame grabber buffer.
 */
int Fg_FreeMemHead(Fg_Struct *Fg, dma_mem *memHandle);

/**
 * \brief	Adding user memory.
 * \anchor fg_addmem
 *
 * To use an allocated user memory as subbuffer, a handle on the frame buffer must have been created with the
 * function Fg_AllocMemHead(), before. Then, the function Fg_AddMem() adds an allocated memory area to the frame buffer management.
 *
 * The assignment between acquired images and subbuffers, which contain the images within PC memory, depends on the acquisition mode.
 *
 * Please note that memory limits depend on several factors, including but not limited to the amount of memory installed in the computer and operating system used.
 *
 * \param	Fg		frame grabber to use
 * \param	pBuffer		Pointer to user memory to be added.
 * \param	Size		Size of user memory in byte.
 * \param	bufferIndex	Index of subbuffer to add (range 0 to BufCnt as passed to Fg_AllocMemHead())
 * \param	memHandle	Pointer to the handle on frame buffer.
 *
 * \returns	Index of subbuffer.
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been used.
 */
int Fg_AddMem(Fg_Struct *Fg, void *pBuffer, const size_t Size, const frameindex_t bufferIndex, dma_mem *memHandle);

/**
 * \brief	Deleting user memory,
 * \anchor  Fg_DelMem
 * 
 * With the function Fg_DelMem() a user memory area of the image memory management can
 * be removed which has been created with Fg_AllocMemHead() and added with Fg_AddMem() before.
 * The memory area won't be freed, it will only be removed from frame memory queues. Releasing of
 * the memory are is up to the user.
 *
 * \param	Fg		frame grabber to use
 * \param	memHandle	Pointer to the handle on frame buffer.
 * \param	bufferIndex	Index of subbuffers, the user memory should be deleted from.
 *
 * \retval	#FG_OK User memory has been deleted correctly.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been used.
 */
int Fg_DelMem(Fg_Struct *Fg, dma_mem *memHandle, const frameindex_t bufferIndex);

/**
 * \brief	NUMA aware allocation of memory.
 * \anchor  Fg_NumaAllocDmaBuffer
 * 
 * Allocated memory can be released with Fg_NumaFreeDmaBuffer().
 *
 * Please note, that there are memory limits existing! However, this function
 * will never fail, but give you memory from other nodes, or even the page file,
 * if you exceed the node memory limit.
 *
 * \param	Fg	 	frame grabber to use
 * \param	Size	 	Size of all image memory in byte.
 *
 * \returns	A pointer to the allocated frame memory / image data.
 *
 * \retval	NULL An error occured. Use Fg_getLastErrorDescription() for more information.
 */
void * Fg_NumaAllocDmaBuffer(Fg_Struct * Fg, size_t Size);

/**
 * \brief	NUMA aware freeing of memory.
 * \anchor  Fg_NumaAllocDmaBuffer
 * 
 * Only use this function to release memory allocated with Fg_NumaAllocateDmaBuffer().
 *
 * \param	Fg	 	frame grabber to use
 * \param	Buffer	pointer to frame memory
 *
 * \returns	0 on success.
 *
 * \retval	<0 An error occured. (Use Fg_getLastErrorDescription() for more information.)
 */
int Fg_NumaFreeDmaBuffer(Fg_Struct * Fg, void * Buffer);

/**
 * \brief	NUMA aware pinning of thread affinity.
 * \anchor  Fg_NumaPinThread
 * 
 * \param	Fg	 	frame grabber to use
 *
 * \returns	0 on success.
 *
 * \retval	<0 An error occured. (Use Fg_getLastErrorDescription() for more information.)
 */
int Fg_NumaPinThread(Fg_Struct * Fg);

/*@}*/

/**
 *
 * \brief	The function Fg_getNrOfParameter() returns the number of parameters available within the loaded applet.
 * \anchor  Fg_getNrOfParameter 
 * 
 * \param	Fg	 frame grabber to use
 *
 * \returns	The number of parameters.
 *
 * \retval	<0 An error occured. (Use Fg_getLastErrorDescription() for more information.)
 */

int Fg_getNrOfParameter(Fg_Struct *Fg);

/**
 * \brief	The function Fg_getParameterId() returns the parameter ID of the parameter referenced by index.
 * \anchor  Fg_getParameterId
 * 
 * The value index is the index of the parameter. Its range is from 0 up to the value returned by Fg_getNrOfParameter() (-1).
 *
 * \param	fg	frame grabber to use
 * \param	index	Parameter index.
 *
 * \returns	The parameter ID.
 *
 * \retval	<0 The parameter couldn't be found.
 */

int Fg_getParameterId(Fg_Struct *fg, int index);

/**
 * \brief	The function Fg_getParameterName() informs about the name of a special parameter by index.
 * \anchor  Fg_getParameterName
 * 
 * \param	fg	frame grabber to use
 * \param	index	Parameter index.
 *
 * \returns	A char pointer to the parameter full name string.
 */

const char *const Fg_getParameterName(Fg_Struct *fg, int index);

/**
 * \brief	The function Fg_getParameterIdByName() searchs for the ID of the parameter referenced by the string name.
 * \anchor  Fg_getParameterIdByName
 * 
 * The string name is the full name of the parameter. The full name of the parameters is determined by VisualApplets during
 * Applet creation. It consists of the process name, the module name in which the parameter has been defined, and the simple
 * parameter name. Each part of the full name is separated by an underscore ('_'). Example: The parameter XOffset in module
 * Buffer1 in process 0 is accessed by the full name "Process0_Buffer1_XOffset".
 *
 * When hierarchical boxes are used, the name(s) of the box(es) have to be added in front of the module name in which the parameter
 * has been defined. Example: The parameter XOffset in module Buffer1 inside hierarchical box HBox1 in process 0 is accessed
 * by the full name "Process0_HBox1_Buffer1_XOffset"
 *
 * Important: Parameters in microEnable IV applets get an additional prefix "Device1". The full name of the above example
 * is on a microEnable IV board is "Device1_Process0_Buffer1_XOffset". Furthermore, for PixelPlant applets the prefix
 * changes to "Device2".
 *
 * To get information about the parameters available inside the applet and their names, see the following snippet of code:
 * \code
 * int i;
 * for (i = 0; i < nrOfParameter; i++) {
 *	const char *name = Fg_getParameterName(fg, i);
 *	fprintf(stdout, " Param %d: %s, 0x%x\n", i, name, Fg_getParameterId(fg, i));
 * }
 * \endcode
 *
 * \param	fg	frame grabber to use
 * \param	name	Full Parameter name.
 *
 * \returns	The parameter ID.
 *
 * \retval	<0 The parameter couldn't be found.
 */

int Fg_getParameterIdByName(Fg_Struct *fg, const char *name);

/**
 * \brief The function Fg_getParameterNameById() informs about the name of a parameter by its id.
 * \anchor  Fg_getParameterName
 * 
 * \param	fg	The frame grabber to use
 * \param	id	Parameter id.
 * \param	dma	The dma, process or port.
 *
 * \returns	A char pointer to the parameter full name string.
 */

const char *const Fg_getParameterNameById(Fg_Struct *fg, const unsigned int id, const unsigned int dma);

/**
 * \brief	Setting frame grabber parameters.
 * \anchor  Fg_setParameter
 * 
 * Each applet, loaded by Fg_Init() or Fg_InitConfig(), has a set of parameters to change and control the applets operation.
 * The number of available parameters, their names and functions depend on the design created with VisualApplets.
 * The available parameters are determined by the VisualApplets operators used within the design.
 *
 * Applet parameters are accessed by their ID. The ID is an integer value, which can be determined using two SDK functions.
 * These functions are: Fg_getParameterId() and Fg_getParameterIdByName(). Having the parameter ID, enables to set the
 * parameter value inside the applet with Fg_setParameter() or to read the parameter value with Fg_getParameter(). The
 * number of available parameters within the applet can be determined by calling Fg_getNrOfParameter(), the name of
 * each parameter with Fg_getParameterName().
 *
 * Setting applet parameters is done with Fg_setParameter(). This function takes the Fg_Struct pointer, as it has
 * been returned by Fg_Init or Fg_InitConfig, the parameter ID, the new value, and the associated DMA number.
 *
 * A number of parameter IDs are already pre-defined and generally available. They can be accessed with pre-defined
 * constants. These parameters are summarized in the following table:
 *
 * <TABLE>
 * <TR><TD><B>Expected Input Type</B></TD><TD><B>Flag</B></TD><TD><B>Value</B></TD><TD><B>Description</B></TD></TR>
 * <TR><TD>int</TD><TD>FG_TIMEOUT</TD><TD>min: 1 sec. max: 2,147,483,646 sec.</TD><TD>Time in seconds until device driver displays a timeout.</TD></TR>
 * <TR><TD>int</TD><TD>FG_GLOBAL_ACCESS</TD><TD>auto = 0, read = 1, read write = 3, read write change = 7</TD><TD>Access flag for plausibilities.</TD></TR>
 * </TABLE>
 *
 * \param	Fg		frame grabber to use
 * \param	Parameter	The parameter ID. Can be either the value returned by Fg_getParameterId() or Fg_getParameterIdByName() or the ID of a generally available parameter. See table below for generally available parameters ...
 * \param	Value		Pointer to required value.
 * \param	DmaIndex	Logical number of used DMA channel. channel 1 = 0, channel 2 = 1 ... and so on.
 *
 * \retval	#FG_OK The parameter has been set correctly.
 * \retval	#FG_NOT_INIT Initialization has failed.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been entered.
 * \retval	#FG_VALUE_OUT_OF_RANGE The entered value is besides valid ranges.
 */

int Fg_setParameter(Fg_Struct *Fg, const int Parameter, const void *Value, const unsigned int DmaIndex);

/**
 * \brief	Setting frame grabber parameters with type information.
 * \anchor  Fg_setParameterWithType
 * 
 * This function behaves like Fg_setParameter() but allows to specify the type of the given
 * value. For compatibility reasons Fg_setParameter() only accepts 32 bit values for integer
 * parameters. If you need to set a 64 bit int parameter you need this function or the upper
 * 32 bit of the passed value will be lost.
 */
int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const void *Value, const unsigned int DmaIndex, const enum FgParamTypes type);

#ifdef __cplusplus
}

#if defined(__GNUC__) || defined(_MSC_VER) || defined(__clang__)
#define INLINE inline
#else /* __GNUC__ || _MSC_VER || __clang__ */
#define INLINE
#endif /* __GNUC__ || _MSC_VER || __clang__ */

/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const uint64_t Value, const unsigned int DmaIndex)
{
	return Fg_setParameterWithType(Fg, Parameter, &Value, DmaIndex, FG_PARAM_TYPE_UINT64_T);
}

/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const int64_t Value, const unsigned int DmaIndex)
{
	return Fg_setParameterWithType(Fg, Parameter, &Value, DmaIndex, FG_PARAM_TYPE_INT64_T);
}

/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const uint32_t Value, const unsigned int DmaIndex)
{
	return Fg_setParameterWithType(Fg, Parameter, &Value, DmaIndex, FG_PARAM_TYPE_UINT32_T);
}

/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const int32_t Value, const unsigned int DmaIndex)
{
	return Fg_setParameterWithType(Fg, Parameter, &Value, DmaIndex, FG_PARAM_TYPE_INT32_T);
}

/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const float Value, const unsigned int DmaIndex)
{
	double val = Value;
	return Fg_setParameterWithType(Fg, Parameter, &val, DmaIndex, FG_PARAM_TYPE_DOUBLE);
}

/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const double Value, const unsigned int DmaIndex)
{
	return Fg_setParameterWithType(Fg, Parameter, &Value, DmaIndex, FG_PARAM_TYPE_DOUBLE);
}


/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const char *Value, const unsigned int DmaIndex)
{
	return Fg_setParameterWithType(Fg, Parameter, Value, DmaIndex, FG_PARAM_TYPE_CHAR_PTR);
}

/**
 * \overload
 */
static INLINE int Fg_setParameterWithType(Fg_Struct *Fg, const int Parameter, const std::string & Value, const unsigned int DmaIndex)
{
	return Fg_setParameterWithType(Fg, Parameter, Value.c_str(), DmaIndex, FG_PARAM_TYPE_CHAR_PTR);
}

extern "C" {
#endif /* __cplusplus */

/**
 * \brief	Getting special parameter settings.
 * \anchor  Fg_getParameter
 * 
 * Parameters can be read with Fg_getParameter(). The usage of this function is similar to Fg_setParameter().
 * Again parameters are accessed by their ID which can be determined in the way described above.
 *
 * In addition to the general available parameters described in Fg_setParameter(),
 * a number of read only parameters can always be accessed with Fg_getParameter() and Fg_getParameterEx().
 * These parameters are listed in the following table:
 *
 * <TABLE>
 * <TR><TD><B>Output Type</B></TD><TD><B>Flag</B></TD><TD><B>Description</B></TD></TR>
 * <TR><TD>int</TD><TD>FG_CAMSTATUS</TD><TD>If a camera signal is on microEnable's physical port, result is 1. If there are two pixel clocks recognized, result is 2. If no camera is connected, result is 0.</TD></TR>
 * <TR><TD>int</TD><TD>FG_IMAGE_TAG</TD><TD>Image tag on a buffer.</TD></TR>
 * <TR><TD>int</TD><TD>FG_TIMEOUT</TD><TD>Time in seconds until device driver displays a timeout of the frame grabber.</TD></TR>
 * <TR><TD>int</TD><TD>FG_TIMESTAMP</TD><TD>The time stamp.</TD></TR>
 * <TR><TD>uint64_t</TD><TD>FG_TIMESTAMP_LONG</TD><TD>High performance time stamp.</TD></TR>
 * <TR><TD>size_t</TD><TD>FG_TRANSFER_LEN</TD><TD>Returns the length of the last DMA transfer.</TD></TR>
 * <TR><TD>int</TD><TD>FG_GLOBAL_ACCESS</TD><TD>Returns the value for the set plausibility flag.</TD></TR>
 * </TABLE>
 *
 * \param	Fg		frame grabber to use
 * \param	Parameter	The parameter ID. Can be either the value returned by Fg_getParameterId() or Fg_getParameterIdByName() or the ID of a generally available parameter. See table below for generally available parameters ...
 * \param	Value		Pointer to required value.
 * \param	DmaIndex	Logical number of used DMA channel. channel 1 = 0, channel 2 = 1 ... and so on.
 *
 * \retval	#FG_OK The parameter value has been read correctly.
 * \retval	#FG_NOT_INIT Initialization has failed.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been entered.
 */

int Fg_getParameter(Fg_Struct *Fg, int Parameter, void *Value, const unsigned int DmaIndex);

/**
* \brief	Getting frame grabber parameters with type information.
* \anchor   Fg_getParameterWithType
*
* \param Fg The frame grabber handle
* \param Parameter The parameter ID
* \param Value A pointer to the variable for the value
* \param DmaIndex The DMA, frame grabber port, or applet process index
* \param type The parameter type
*
* This function behaves like Fg_getParameter() but allows to specify the type of the given
* value. For compatibility reasons Fg_getParameter() only accepts 32 bit values for integer
* parameters. If you need to get a 64 bit int parameter you need this function or the upper
* 32 bit of the passed value will never be changed.
* Please not that when querying string parameters with type FG_PARAM_TYPE_CHAR_PTR_PTR,
* value must be a pointer to a char pointer variable. The memory to hold the string will
* be allocated and must be release using Fg_freeStringParameterWithType() after use.
*/
int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, void *Value, const unsigned int DmaIndex, const enum FgParamTypes type);

/**
* \brief	Release previously allocated frame grabber string parameter.
* \anchor   Fg_freeParameterStringWithType
* 
* \param Fg The frame grabber handle
* \param Parameter The parameter ID
* \param Value A pointer to the variable for the value
* \param DmaIndex The DMA, frame grabber port, or applet process index
* \param type The parameter type
*
* This function releases the value from a string paramater that was previously allocated
* through a call to Fg_getParameterWithType with type FG_PARAM_TYPE_CHAR_PTR_PTR. All
* parameters must match the previous call to Fg_getParameterWithType(), except for the
* value pointer, which must be the char pointer to be released (and not a pointer to the
* char pointer).
*/
int Fg_freeParameterStringWithType(Fg_Struct *Fg, const int Parameter, void *Value, const unsigned int DmaIndex, const enum FgParamTypes type);

#ifdef __cplusplus
}

/**
 * \overload
 */
static INLINE int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, uint64_t *Value, const unsigned int DmaIndex)
{
	return Fg_getParameterWithType(Fg, Parameter, Value, DmaIndex, FG_PARAM_TYPE_UINT64_T);
}

/**
 * \overload
 */
static INLINE int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, int64_t *Value, const unsigned int DmaIndex)
{
	return Fg_getParameterWithType(Fg, Parameter, Value, DmaIndex, FG_PARAM_TYPE_INT64_T);
}

/**
 * \overload
 */
static INLINE int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, uint32_t *Value, const unsigned int DmaIndex)
{
	return Fg_getParameterWithType(Fg, Parameter, Value, DmaIndex, FG_PARAM_TYPE_UINT32_T);
}

/**
 * \overload
 */
static INLINE int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, int32_t *Value, const unsigned int DmaIndex)
{
	return Fg_getParameterWithType(Fg, Parameter, Value, DmaIndex, FG_PARAM_TYPE_INT32_T);
}

/**
 * \overload
 */
static INLINE int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, float *Value, const unsigned int DmaIndex)
{
	double val = 0;
	int result = Fg_getParameterWithType(Fg, Parameter, &val, DmaIndex, FG_PARAM_TYPE_DOUBLE);
	if (result == FG_OK)
		*Value = static_cast<float>(val);
	return result;
}

/**
 * \overload
 */
static INLINE int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, double *Value, const unsigned int DmaIndex)
{
	return Fg_getParameterWithType(Fg, Parameter, Value, DmaIndex, FG_PARAM_TYPE_DOUBLE);
}

/**
 * \overload
 */
static INLINE int Fg_getParameterWithType(Fg_Struct *Fg, const int Parameter, std::string & Value, const unsigned int DmaIndex)
{
	char * cstr = NULL;
	int result = Fg_getParameterWithType(Fg, Parameter, &cstr, DmaIndex, FG_PARAM_TYPE_CHAR_PTR_PTR);
	if (result != 0) {
		return result;
	}

	Value = cstr;
	Fg_freeParameterStringWithType(Fg, Parameter, cstr, DmaIndex, FG_PARAM_TYPE_CHAR_PTR_PTR);
	return 0;
}

extern "C" {
#endif /* __cplusplus */

/**
 * \brief	Getting special parameter settings.
 * \anchor  Fg_getParameterEx
 * 
 * Fg_getParameterEx() allows querying of values directly related to image transfers. These
 * values are queried from the dma_mem pointer passed as argument. These values are available
 * as long as the buffer memory is still available, i.e. even if the DMA transfer is no longer
 * running or a DMA transfer is running with a different DMA buffer.
 *
 * As all of these values have special characteristics only a small set of parameter ids may
 * be queried using this interface:
 *
 * <table>
 * <tr><th>Output Type</th><th>Parameter id</th><th>Description</th></tr>
 * <tr><td>unsigned int</td><td>FG_IMAGE_TAG</td><td>Image tag on a buffer.</td></tr>
 * <tr><td>unsigned int</td><td>FG_TIMESTAMP</td><td>The time stamp.</td></tr>
 * <tr><td>uint64_t</td><td>FG_TIMESTAMP_LONG</td><td>High performance time stamp.</td></tr>
 * <tr><td>size_t</td><td>FG_TRANSFER_LEN</td><td>Returns the length of the last DMA transfer.</td></tr>
 * </table>
 *
 * \param	Fg		frame grabber to use
 * \param	Parameter	The parameter ID.
 * \param	Value		Pointer to memory where requested value will be stored.
 * \param	DmaIndex	Ignored, should be 0.
 * \param	pMem		Handle on memory area which has been reserved.
 * \param	ImgNr		Image number to query.
 *
 * \retval	#FG_OK The parameter value has been read correctly.
 * \retval	#FG_NOT_INIT Initialization has failed.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been entered.
 */

int Fg_getParameterEx(Fg_Struct *Fg, const int Parameter, void *Value, const unsigned int DmaIndex, dma_mem *pMem, const frameindex_t ImgNr);


/** Get Parameter information in XML format
 * \brief Get parameter description in a Genicam compatible XML format.
 * \anchor Fg_getParameterInfoXML
 * 
 * This function delivers information about all parameters in a GenICam compatible
 * XMl format. It can be used together with Genicam tools to access the parameters
 * of an applet. Therefore the parameter ID is handled as an address. Read/write accesses
 * are intended to be done with Fg_getParameter and Fg_setParameter with the delivered
 * address as parameter ID. 
 * The buffer is filled with a null-terminated string containing the 
 * XML data. To be compliant against XML validation, the terminating 0 needs to be removed, when
 * saving the data to a XML file
 * 
 * \param Fg frame grabber to use
 * \param port Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param infoBuffer Pointer to a buffer where the XML information will be placed.
 * \param infoBufferSize Input: The max available space of the infoBuffer in byte. Output:
 * 		                      The number of bytes copied into the infoBuffer. In case the infoBuffer
 * 													is a NULL-pointer the output is the required infoBuffer space in byte.
 * \retval #FG_OK The operation was successful.
 * \retval #FG_INTERNAL_ERROR The internal parameter structures are not existing
 * \retval #FG_NOT_ENOUGH_MEM The infoBuffer size contained in infoBufferSize is not
 * 		                           sufficient.
 * 
*/
int Fg_getParameterInfoXML(Fg_Struct *Fg, int port, char * infoBuffer, size_t *infoBufferSize);

/**
 * \brief Calculate the bits per pixel for a specific output format (FG_GRAY, FG_COL24, etc.).
 *
 * \param format the output format
 * 
 * \returns The bits per pixel of the output format. #FG_INVALID_PARAMETER if the pixelsize can not be derived
 *          from the format or the format is invalid or unknown.
 */
int Fg_getBitsPerPixel(int format);

/**
 * \brief	Saving a frame grabber configuration
 * \anchor  Fg_saveConfig
 * 
 * The frame grabber's configuration can be saved into a file by Fg_saveConfig(). Parameterization will be
 * handled by loading of this file. The frame grabber has to be initialized first!
 *
 * A configuration file can also be loaded with Fg_loadConfig().
 *
 * The extension of the microEnable configuration file is always *.mcf.
 *
 * \param	Fg		frame grabber to use
 * \param	Filename	Name and path of the frame grabber configuration file.
 *
 * \retval	#FG_OK  Transmission has been finished successfully.
 * \retval	#FG_NOT_INIT Frame grabber hasn't been initialized correctly.
 * \retval	#FG_INVALID_FILENAME Filename hasn't been found.
 */

int Fg_saveConfig(Fg_Struct *Fg, const char *Filename);

/**
 * \brief	Saving applet field parameter
 * 
 * 
 * The applet's fields parameters can be saved into a file by Fg_saveFieldParameterToFile()
 *
 * A field parameter file can also be loaded with Fg_loadFieldParameterFromFile().
 *
 * The extension of the field parameter is always *.mfs.
 *
 * \param	Fg		frame grabber to use
 * \param       Parameter       parameter id
 * \param       DmaIndex        cam port
 * \param	Filename	Name and path of field parameter file.
 *
 * \retval	#FG_OK  Saving parameter done successfully.
 * \retval      #FG_NOT_INIT The Frame grabber handle is invalid or null
 * \retval      #FG_INVALID_CONFIGFILE File format is incorrect or corrupted data if it already exists
 * \retval      #FG_INVALID_PARAMETER The requested parameter is not a field parameter or parameter id is negative and invalid or parameter id is not in applet at all
 * \retval      #FG_FILE_NOT_FOUND File name is invalid
 * \retval      #FG_FILE_ACCESS_DENIED Unexpected internal error by reading from or writing in file
 * \retval      #FG_ERROR Unexpected internal error
 */

int Fg_saveFieldParameterToFile(Fg_Struct * Fg, const int Parameter, const unsigned int DmaIndex, const char * FileName);

/**
 * \brief	Loading a frame grabber configuration.
 * \anchor  Fg_loadConfig
 * 
 * Having initialized the frame grabber, an existing camera configuration file can be loaded with
 * the function Fg_loadConfig(). There must be a valid frame grabber to use
 *
 * A configuration file can also be saved with Fg_saveConfig().
 *
 * The extension of the microEnable configuration file is always *.mcf.
 *
 * \param	Fg		frame grabber to use
 * \param	Filename	Name and path to microEnable configuration file.
 *
 * \retval	#FG_OK  Transmission has been finished successfully.
 * \retval	#FG_NOT_INIT Frame grabber hasn't been initialized correctly.
 * \retval	#FG_INVALID_FILENAME Filename hasn't been found.
 * \retval	#FG_INVALID_CONFIGFILE Configuration file is invalid.
 * \retval  #FG_INVALID_IMAGE_DIMENSIONS external image data does not match to Applets needs
 */

int Fg_loadConfig(Fg_Struct *Fg, const char *Filename);

/**
 * \brief	Loading field parameter from file.
 *
 *
 * A field parameter file can also be saved with Fg_saveFieldParameterToFile().
 *
 * The extension of the field parameter file is always *.mfs.
 *
 * \param	Fg		frame grabber to use
 * \param       Parameter       parameter id
 * \param       DmaIndex        cam port
 * \param	Filename	Name and path to field parameter file.
 *
 * \retval	#FG_OK  Loading parameter done successfully.
 * \retval      #FG_PARAMETER_NOT_IN_FILE Parameter is not in file
 * \retval      #FG_NOT_INIT The Frame grabber handle is invalid or null
 * \retval      #FG_INVALID_CONFIGFILE File format is incorrect or corrupted data if it already exists
 * \retval      #FG_INVALID_PARAMETER The requested parameter is not a field parameter or parameter id is negative and invalid or parameter id is not in applet at all
 * \retval      #FG_FILE_NOT_FOUND File name is invalid or not exists
 * \retval      #FG_FILE_ACCESS_DENIED Unexpected internal error by reading from or writing in file
 * \retval      #FG_ERROR Unexpected internal error
 */

int Fg_loadFieldParameterFromFile(Fg_Struct * Fg, const int Parameter, const unsigned int DmaIndex, const char * FileName);

/** @name Acquisition control
 *
 * These functions control sending and receiving data frames to and from the
 * device. The memory used to transfer the frames needs to be allocated with
 * functions from the \b Memory \b Management section first.
 */
/*@{*/
/**
 * \brief	Starting image grabbing.
 * \anchor  Fg_Acquire
 * 
 * The function Fg_Acquire() starts a continuous grabbing. Having started the exact number of images
 * will be grabbed (PicCount). If a timeout occurs, the grabbing will be stopped. If a continuous grabbing
 * with manual stopping is needed (explicit stopping of the grabbing with Fg_stopAcquire()),
 * the maximum image number has to be set to #GRAB_INFINITE. When all buffers should be used exactly once
 * #GRAB_ALL_BUFFERS can be used. This is equivalent to passing the buffer count.
 *
 * In a multi DMA configuration the image grabbing can be started and stopped independently for each DMA channel.
 *
 * The grabbing mode is set to #ACQ_STANDARD. The image memory is used as a ring buffer and all subbuffer will be
 * overwritten cyclically.
 *
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	PicCount	Maximum number of images to be grabbed, GRAB_INFINITE, means infinite grabbing.
 *
 * \retval	#FG_OK Grabbing has been started correctly. (when grabbing synchronously also finished correctly).
 * \retval	#FG_NOT_INIT Frame grabber was not initialised correctly.
 * \retval	#FG_NO_VALID_DESIGN No valid design was loaded.
 * \retval	#FG_INVALID_MEMORY Image memory was not allocated.
 * \retval	#FG_MEMORY_IN_USE Image memory is already used by another DMA channel.
 * \retval	#FG_NOT_ENOUGH_MEMORY Insufficient image memory was allocated for specified image parameter.
 * \retval	#FG_ALREADY_STARTED Grabbing is already started and can't be started twice.
 * \retval	#FG_INVALID_PARAMETER Invalid parameter is set.
 */

int Fg_Acquire(Fg_Struct *Fg, const unsigned int DmaIndex, const frameindex_t PicCount);

/**
 * \brief	Stopping image grabbing.
 * \anchor  Fg_stopAcquire
 * 
 * The function Fg_stopAcquire() stops the running image grabbing that has been started
 * with Fg_Acquire(). In a dual grabber configuration, each port can be stopped
 * separately. In a single grabber configuration only the active port.
 *
 * The function Fg_stopAcquire() stopps the acquiring of images asynchronous, that means, running
 * image transfer won't be finished in every case.
 *
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 *
 * \retval	#FG_OK Transmission has been finished successfully.
 * \retval	#FG_NOT_INIT Frame grabber was not initialised correctly.
 * \retval	#FG_TRANSFER_NOT_ACTIVE Transmission was not started.
 * \retval	#FG_INVALID_PARAMETER Invalid parameter is set.
 * \retval	#FG_CANNOT_STOP Frame grabber could not stop.
 */

int Fg_stopAcquire(Fg_Struct *Fg, const unsigned int DmaIndex);

/**
 * \brief	Requesting the current image number (blocking)
 * \anchor  Fg_getLastPicNumberBlocking
 * 
 * The number of the last, completely transferred image can be requested with a blocking call of
 * Fg_getLastPicNumberBlocking().
 *
 * In contrary to Fg_getLastPicNumber() the function waits, until the transmission of the preselected
 * image number is successfully finished or a maximum time is elapsed. If the image number is obtained
 * before, the function returns immediately. The first image number value will be 1, if no error occurs.
 *
 * Attention: When working with extreme high framerates, the returned value of the function can be bigger
 * than the preselected image number. In this case it is important to allocate enough frame buffers
 *
 * \param	Fg		frame grabber to use
 * \param	PicNr		Requested image number.
 * \param	DmaIndex	Logical number of used camera port camera port A = 0 and camera port B = 1
 * \param	Timeout		Timeout in seconds.
 *
 * \returns	Current image number, beginning with 1.
 *
 * \retval	#FG_TIMEOUT_ERR Timeout error occurred.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

frameindex_t Fg_getLastPicNumberBlocking(Fg_Struct *Fg, const frameindex_t PicNr, const unsigned int DmaIndex, const int Timeout);

/**
 * \brief	Requesting the current image number (non-blocking).
 * \anchor  Fg_getLastPicNumber
 * 
 * The number of the last, completely transferred image can be requested with a non-blocking call
 * of Fg_getLastPicNumber(). The first completely transferred image has
 * the image number 1. An image number 0 means, that a complete image is still not transferred.
 *
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 *
 * \returns  Number of the last completely transferred image, transmission is running.
 *
 * \retval	0 Transmission is running, no image has been completely transfered, yet.
 * \retval	#FG_TIMEOUT_ERR Timeout occurs.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

frameindex_t Fg_getLastPicNumber(Fg_Struct *Fg, const unsigned int DmaIndex);

/**
 * \brief	Requesting the current image number (blocking)
 * \anchor  Fg_getLastPicNumberBlockingEx
 * 
 * The number of the last, completely transferred image can be requested with a blocking call of
 * Fg_getLastPicNumberBlockingEx().
 *
 * In contrary to Fg_getLastPicNumberEx() the function waits, until the transmission of the preselected
 * image number is successfully finished or a maximum time is elapsed. If the image number is obtained
 * before, the function returns immediately. The first image number value will be 1, if no error occurs.
 *
 * Attention: When working with extreme high framerates, the returned value of the function can be bigger
 * than the preselected image number. In this case it is important to allocate enough frame buffers
 *
 * \param	Fg		frame grabber to use
 * \param	PicNr		Requested image number.
 * \param	DmaIndex	Logical number of used camera port camera port A = 0 and camera port B = 1
 * \param	Timeout		Timeout in seconds.
 * \param	pMem		Pointer to variable buffer.
 *
 * \returns	 Current image number, beginning with 1.
 *
 * \retval	#FG_TIMEOUT_ERR Timeout error occurred.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

frameindex_t Fg_getLastPicNumberBlockingEx(Fg_Struct *Fg, const frameindex_t PicNr, const unsigned int DmaIndex, const int Timeout, dma_mem *pMem);

/**
 * \brief	Requesting the current image number (non-blocking).
 * \anchor  Fg_getLastPicNumberEx
 * 
 * The number of the last, completely transferred image can be requested with a non-blocking call
 * of Fg_getLastPicNumberEx(). The first completely transferred image has
 * the image number 1. An image number 0 means, that a complete image is still not transferred.
 *
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	pMem		Pointer to variable buffer.
 *
 * \returns  Number of the last completely transferred image, transmission is running.
 *
 * \retval	0 Transmission is running, no image has been completely transfered, yet.
 * \retval	#FG_TIMEOUT_ERR Timeout occurs.
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

frameindex_t Fg_getLastPicNumberEx(Fg_Struct *Fg, const unsigned int DmaIndex, dma_mem *pMem);

/**
 * \brief	Access on frame buffer.
 * \anchor  Fg_getImagePtr
 * 
 * With the function Fg_getImagePtr() a pointer to an image in the frame buffer is created.
 *
 * Reference: The function will return only the correct image, if it still exists in the frame buffer.
 *
 * Example:
 *
 * <CODE>
 * Framebuffer=2;<BR>
 * Fg_getImagePtr(Fg, 3, dma0) 	// returns a pointer of the third frame buffer;<BR>
 * // by the use of two frame buffers,<BR>
 * // the pointer to frame buffer 1 is identical with the third.<BR>
 * </CODE>
 *
 * \param	Fg		frame grabber to use
 * \param	PicNr		Number of frame buffer or picture number.
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 *
 * \returns	Pointer to current image position.
 */
void *Fg_getImagePtr(Fg_Struct *Fg, const frameindex_t PicNr, const unsigned int DmaIndex);

/**
 * \brief	Access on frame buffer.
 * \anchor  Fg_getImagePtrEx
 * 
 * With the function Fg_getImagePtrEx() a pointer to an image in the frame buffer is created.
 *
 * Reference: The function will return only the correct image, if it still exists in the frame buffer.
 *
 * Example:
 *
 * <CODE>
 * Framebuffer=2;<BR>
 * Fg_getImagePtr(Fg, 3, dma0) 	// returns a pointer of the third frame buffer;<BR>
 * // by the use of two frame buffers,<BR>
 * // the pointer to frame buffer 1 is identical with the third.<BR>
 * </CODE>
 *
 * \param	Fg		frame grabber to use
 * \param	PicNr		Number of frame buffer or picture number.
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	pMem		Pointer to variable buffer.
 *
 * \returns	Pointer to current image position.
 */
void *Fg_getImagePtrEx(Fg_Struct *Fg, const frameindex_t PicNr, const unsigned int DmaIndex, dma_mem *pMem);

/**
 * \brief	Starting image grabbing.
 * \anchor  Fg_AcquireEx
 * 
 * The function Fg_AcquireEx() starts a continuous grabbing. Having started the exact number of images will be
 * grabbed (PicCount). If a timeout occurs, the grabbing will be stopped. If a continuous grabbing with manual
 * stopping is needed (explicit stopping of the grabbing with Fg_stopAcquireEx()), the maximum image number has
 * to be set to #GRAB_INFINITE. When all buffers should be used exactly once #GRAB_ALL_BUFFERS can be used. This
 * is equivalent to passing the buffer count.
 *
 * In a multi DMA configuration the image grabbing can be started and stopped independently for each DMA channel.
 *
 * The grabbing mode is set by a flag. You can choose between #ACQ_STANDARD and #ACQ_BLOCK. Is the mode
 * #ACQ_STANDARD set, the image memory is used as a ring buffer and all subbuffer will be overwritten cyclicly.
 * Is the mode #ACQ_BLOCK chosen, the acquired frame buffer is blocked as long as it is explicitly released by
 * the user. If all subbuffers are blocked, the acquired images will be stored in an additional, optional buffer
 * (DummyBuffer), which is not blockable. There is no unique assignment between image number and subbuffer number
 * in this grabbing model.
 *
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	PicCount	Maximum number of images to be grabbed, #GRAB_INFINITE, means infinite grabbing.
 * \param	nFlag		#ACQ_STANDARD, #ACQ_BLOCK
 * \param	memHandle	Pointer to handle of frame buffer.
 *
 * \retval	#FG_OK Grabbing has been started correctly. (when grabbing synchronously also finished correctly).
 * \retval	#FG_NOT_INIT Frame grabber was not initialised correctly.
 * \retval	#FG_INVALID_MEMORY Image memory was not allocated.
 * \retval	#FG_MEMORY_IN_USE Image memory is already used by another DMA channel.
 * \retval	#FG_NOT_ENOUGH_MEMORY Insufficient image memory was allocated for specified image parameter.
 * \retval	#FG_ALREADY_STARTED Grabbing is already started and can't be started twice.
 * \retval	#FG_INVALID_PARAMETER Invalid parameter is set.
 * \retval	#FG_ILLEGAL_WHILE_APC An APC from a previous acquisition is still active
 */
int Fg_AcquireEx(Fg_Struct *Fg, const unsigned int DmaIndex, const frameindex_t PicCount, const int nFlag, dma_mem *memHandle);

/**
 * \brief	Sending images from Software to the applet
 * \anchor  Fg_sendImageEx
 * 
 * This function provides an interface in order to send images from a client software to the applet. This functions uses frame buffers
 * which are allocated by using the Fg_allocMem() function. It shouldn't be used in combination with the Fg_allocMemEx() functions.
 * Please note that when using this function, special applets which support this possibility are needed
 * \param	Fg			frame grabber to use
 * \param	startImage	first image to be sent as index of the frame buffer
 * \param	PicCount	number of images to be sent, 
 * \param	nFlag		reserved, should be 0
 * \param	DmaIndex	Logical number of the DMA's depending on the applet. Indizes starting with 0 
 *
 * \retval	various		FG-error codes, call Fg_getLastError to obtain the error
 */
int Fg_sendImage(Fg_Struct *Fg, const frameindex_t startImage, const frameindex_t PicCount, const int nFlag, const unsigned int DmaIndex);
/**
 * \brief	Sending images from Software to the applet
 * \anchor  Fg_sendImageEx
 * 
 * This function provides an interface in order to send images from a client software to the applet. 
 * Please note that when using this function, special applets which support this possibility are needed
 * \param	Fg			frame grabber to use
 * \param	startImage	first image to be sent as index of the frame buffer
 * \param	PicCount	number of images to be sent, 
 * \param	nFlag		reserved, should be 0
 * \param	DmaIndex	Logical number of the DMA's depending on the applet. Indizes starting with 0 
 * \param	memHandle	pointer to memory structure, which has been allocated be Fg_allocMemEx
 *
 * \retval	various		FG-error codes, call Fg_getLastError to obtain the error
 */
int Fg_sendImageEx(Fg_Struct *Fg, const frameindex_t startImage, const frameindex_t PicCount, const int nFlag, const unsigned int DmaIndex, dma_mem *memHandle);


/**
 * \brief	Stopping image grabbing
 * \anchor  Fg_stopAcquireEx
 * 
 * The function Fg_stopAcquireEx() stop the running image grabbing that has been
 * started with Fg_AquireEx(). In a dual grabber configuration, each port can be stopped separately.
 * In a single grabber configuration only the active port.
 *
 * The function Fg_stopAcquireEx() stopps the image acquiring asynchronous or synchronous. nFlag
 * indicates the stop mode and also the duration of timeout in seconds. Timeout is the maximum time,
 * the function will wait in synchronous mode to stop the acquiring.
 *
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	memHandle	Pointer to handle of frame buffer.
 * \param	nFlag		Selecting stopping mode and timeout in seconds (STOP_SYNC, STOP_ASYNC) .
 *                      When using STOP_SYNC: The timeout is defined by the parameter FG_STOP_TIMEOUT
 *
 * \retval	#FG_OK Transmission has been finished successfully.
 * \retval	#FG_NOT_INIT Frame grabber was not initialised correctly.
 * \retval	#FG_TRANSFER_NOT_ACTIVE Transmission was not started.
 * \retval	#FG_INVALID_PARAMETER Invalid parameter is set.
 * \retval	#FG_CANNOT_STOP Frame grabber could not stop.
 */
int Fg_stopAcquireEx(Fg_Struct *Fg, const unsigned int DmaIndex, dma_mem *memHandle, int nFlag);

/**
 * \brief	Getting the image.
 * \anchor  Fg_getImage
 * 
 * After image acquisition, the buffer number of an image can be explicitly gotten by the help of the function
 * Fg_getImage(). It is needed for blocking acquisition mode. But it can also be used for standard acquisition mode.
 *
 * The first buffer has the number 0.
 *
 * Depending on the parameter, the internal image counters for the last gotten image will be updated or not. A
 * pointer to the image will be gotten by the function Fg_getImagePtr().
 *
 * <TABLE>
 * <TR><TD><B>Param</B></TD><TD><B>Meaning</B></TD></TR>
 * <TR><TD>SEL_NEW_IMAGE</TD><TD>Selection of current acquired image (running process).</TD></TR>
 * <TR><TD>SEL_ACT_IMAGE</TD><TD>Selection of last acquired image.</TD></TR>
 * <TR><TD>SEL_NEXT_IMAGE</TD><TD>Selection of next image after last get image.</TD></TR>
 * <TR><TD>SEL_NUMBER</TD><TD>Selection of an image by image number, not available in blocking mode!</TD></TR>
 * </TABLE>
 *
 * \param	Fg		frame grabber to use
 * \param	Param		See table below ...
 * \param	PicNr		0 in blocking mode, image number in default grabbing mode.
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	Timeout		Duration of timeout in seconds.
 *
 * \returns	Buffer number, beginning with 0.
 *
 * \retval	#FG_TIMEOUT_ERR Timeout occurs.
 * \retval	#FG_INVALID_PARAMETER This error occurs, either if the image number is <= 0 or if the blocking mode has been chosen in case of flag SEL_NUMBER.
 */
frameindex_t Fg_getImage(Fg_Struct *Fg, const int Param, const frameindex_t PicNr, const unsigned int DmaIndex, const unsigned int Timeout);

/**
 * \brief	Getting the image.
 * \anchor  Fg_getImageEx
 * 
 * After image acquisition, the buffer number of an image can be explicitly gotten by the help of the function
 * Fg_getImageEx(). It is needed for blocking acquisition mode. But it can also be used for standard acquisition mode.
 *
 * The first buffer has the number 0.
 *
 * Depending on the parameter, the internal image counters for the last gotten image will be updated or not. A
 * pointer to the image will be gotten by the function Fg_getImagePtrEx().
 *
 * <TABLE>
 * <TR><TD><B>Param</B></TD><TD><B>Meaning</B></TD></TR>
 * <TR><TD>SEL_NEW_IMAGE</TD><TD>Selection of current acquired image (running process).</TD></TR>
 * <TR><TD>SEL_ACT_IMAGE</TD><TD>Selection of last acquired image.</TD></TR>
 * <TR><TD>SEL_NEXT_IMAGE</TD><TD>Selection of next image after last get image.</TD></TR>
 * <TR><TD>SEL_NUMBER</TD><TD>Selection of an image by image number, not available in blocking mode!</TD></TR>
 * </TABLE>
 *
 * \param	Fg		frame grabber to use
 * \param	Param		See table below ...
 * \param	PicNr		0 in blocking mode, image number in default grabbing mode.
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	Timeout		Duration of timeout in seconds.
 * \param	pMem		Pointer to variable buffer.
 *
 * \returns	Buffer number, beginning with 0.
 *
 * \retval	#FG_TIMEOUT_ERR Timeout occurs.
 * \retval	#FG_INVALID_PARAMETER This error occurs, either if the image number is <= 0 or if the blocking mode has been chosen in case of flag SEL_NUMBER.
 */
frameindex_t Fg_getImageEx(Fg_Struct *Fg, const int Param, const frameindex_t PicNr, const unsigned int DmaIndex, const unsigned int Timeout, dma_mem *pMem);

/**
 * \brief	Register an APC handler for the given DMA channel.
 * \anchor  Fg_registerApcHandler
 * 
 * \param	Fg		frame grabber to use
 * \param	DmaIndex	DMA channel number to listen on
 * \param	control		control structure for APC operations
 * \param	flags		how to handle the control structure
 *
 * \returns	#FG_OK						if the handler was successfully registered, error code otherwise.
 * \retval	#FG_NOT_INIT				Fg is NULL
 * \retval	#FG_INVALID_PORT_NUMBER		DmaIndex is invalid
 * \retval	#FG_INVALID_PARAMETER		One parameter or the combination of parameters is invalid
 * \retval	#FG_ALREADY_STARTED			an APC handler for this DMA channel is already registered
 * \retval	#FG_NOT_ENOUGH_MEMORY		creation of the internal control structures or the worker thread failed
 * \retval	#FG_APC_PRIORITY_ERROR		the control structure requested #FG_APC_HIGH_PRIORITY but setting the priority failed
 * \retval	#FG_ACCESS_DENIED			setting the Priority fails due to insufficient permissions
 * \retval	#FG_APC_ALREADY_REGISTERED	an APC handler is already registered
 *
 * This function registers an APC (asynchronous procedure call) handler to be called once
 * images on the given DMA channel arrive.
 *
 \verbatim
   This function does not start the acquisition itself, this needs to be done using
   Fg_Acquire() or Fg_AcquireEx(). If an APC handler is registered for a DMA channel you
   can not use Fg_getLastPicNumberBlocking() or Fg_getLastPicNumberBlockingEx() to wait
   for images on that channel. The DMA channel will not be stopped by the APC handler
   thread on any circumstances even when the handler will terminate itself by those.\endverbatim
 *
 * Once the DMA is started the APC handler function will get called for new images. The
 * exact behaviour can be controlled by setting the flags member of the control struct
 * using the constants defined in enum #Fg_Apc_Flag in fgrab_define.h. If you are unsure
 * just pass Fg_Apc_Flag::FG_APC_DEFAULTS here.
 *
 * The handler function is stored as func in the control struct. It will be called with the
 * image number to work on as first and the value given for data in the control struct as
 * second argument.
 * \see struct fg_apc_data
 *
 * If you request #FG_APC_HIGH_PRIORITY through the flag member of the control structure,
 * your application must have the necessary privileges to change the thread scheduling and priority.
 * Note that on Linux this generally means you have to run the application as root or use setuid.
 *
 * The flags parameter allows future expansions of the APC operation mode. The only value
 * value currently valid is #FG_APC_CONTROL_BASIC. You must set the version number in the
 * control struct.
 *
 * If you want to unregister an APC handler for a given channel call this function as:
 * \code
 * Fg_registerApcHandler(Fg, dma, NULL, FG_APC_CONTROL_BASIC);
 * \endcode
 *
 * You must always unregister the APC handler from a DMA channel even if the APC thread
 * was automatically stopped. This will happen e.g. if a image timeout occurs and
 * Fg_Apc_Flag::FG_APC_IGNORE_TIMEOUTS was not set or if your handler function returns
 * with error code and Fg_Apc_Flag::FG_APC_IGNORE_APCFUNC_RETURN was not set. You do not
 * need to unregister the APC handler if registering failed with an error code.
 *
 * \code
 * class my_class;
 *
 * struct fg_apc_data {
 *     Fg_Struct *fg;
 *     unsigned int dma;
 *     dma_mem *mem;
 *     my_class *my;
 * };
 *
 * class my_class {
 * private:
 *     struct fg_apc_data apcdata;
 * public:
 *     my_class(Fg_Struct *fg, unsigned int dma);
 *     ~my_class();
 *
 *     int registerApc();
 *     void doSomethingWithImage(void *imageData, size_t imageLength);
 * ...
 * };
 *
 * ...
 *
 * extern "C" int my_apc_func(int image, struct fg_apc_data *apcdata)
 * {
 *     void *data = Fg_getImagePtrEx(apcdata->fg, image, apcdata->dma, apcdata->mem);
 *     size_t len;
 *
 *     Fg_getParameterEx(apcdata->fg, FG_TRANSFER_LEN, &len, apcdata->dma, apcdata->mem, image);
 *
 *     apcdata->my->doSomethingWithImage(data, len);
 *
 *     return 0;
 * }
 *
 * my_class::my_class(Fg_Struct *fg, unsigned int dma)
 * {
 *     apcdata.version = 0;
 *     apcdata.fg = fg;
 *     apcdata.dma = dma;
 *     apcdata.my = this;
 *     // apcdata.mem is later initialized by some other method of my_class
 * }
 *
 * my_class::~my_class()
 * {
 *     Fg_registerApcHandler(apcdata.fg, apcdata.dma, NULL, FG_APC_CONTROL_BASIC);
 * }
 *
 * int my_class::registerApc()
 * {
 *     struct FgApcControl ctrl;
 *
 *     ctrl.func = my_apc_func;
 *     ctrl.data = &apcdata;
 *     ctrl.flags = FG_APC_DEFAULTS;
 *     ctrl.timeout = 10;
 *
 *     return Fg_registerApcHandler(apcdata.fg, apcdata.dma, &ctrl, FG_APC_CONTROL_BASIC);
 * }
 * \endcode
 */
int Fg_registerApcHandler(Fg_Struct *Fg, const unsigned int DmaIndex, const struct FgApcControl *control, enum FgApcControlFlags flags);
/*@}*/

/**
 * \brief	Getting the last error code.
 * \anchor  Fg_getLastErrorNumber
 * 
 * The function Fg_getLastErrorNumber() displays the error code of last occurred fault.
 *
 * \param	Fg	frame grabber to use
 *
 * \returns	The number of the last error that occurred.
 */

int Fg_getLastErrorNumber(Fg_Struct *Fg);

/**
 * \brief	Description of error message.
 * \anchor  getErrorDescription
 * 
 * This returns a description string for the given error code.
 *
 * \param	ErrorNumber	The error code to translate.
 *
 * \returns	The message of the last error that occurred.
 */
const char *const getErrorDescription(int ErrorNumber);

/**
 * \brief	Description of last error message.
 * \anchor  Fg_getLastErrorDescription
 * 
 * With the function Fg_getLastErrorDescription() the last occurred error will be returned as error message.
 *
 * \param	Fg	frame grabber to use
 *
 * \returns	The message of the last error that occurred.
 *
 * This function is equivalent to getErrorDescription(Fg_getLastErrorNumber(Fg)).
 */
const char *const Fg_getLastErrorDescription(Fg_Struct *Fg);

/**
 * \brief	Description of error message to an error code.
 * \anchor  Fg_getErrorDescription
 * 
 * The function Fg_getErrorDescription() returns the error message, when receiving the error code.
 *
 * \param	Fg		frame grabber to use
 * \param	ErrorNumber	Identification number of error code.
 *
 * \returns	Error message of the error code specified
 */

const char *const Fg_getErrorDescription(Fg_Struct *Fg, int ErrorNumber);

/** @name Device and Applet Information
 *
 * These functions will return static information of the given device or applet
 * or combination thereof. Calling these functions again on the same input will
 * usually return the same information again.
 */
/*@{*/
/**
 * \brief	Board type.
 * \anchor  Fg_getBoardType
 * 
 * Fg_getBoardType() returns the type of board: If it is a microEnable III / -XXL or microEnable
 * IV-Full x1 / -Full x4. PixelPlant is not able to be detected at the moment.
 *
 * \param	BoardIndex	index of the board
 *
 * \retval	FG_ERROR	there is no board with the given index
 *
 * See sisoboards.h for the board type definitions.
 *
 * If you already have initialized the board use
 *
 * \code
 * uint32_t boardType = BINFO_BOARDTYPE;
 * Fg_getParameterWithType(fg, FG_BOARD_INFORMATION, &boardType, 0, FG_PARAM_TYPE_UINT32_T);
 * \endcode
 */
int Fg_getBoardType(int BoardIndex);

/**
 * \brief	Board name from type.
 * \anchor  Fg_getBoardNameByType
 * 
 * Fg_getBoardNameByType() returns the name for the type of board.
 *
 * \param	BoardType	type of the board
 * \param	UseShortName	get the short name for the board
 *
 * \retval	NULL	there is no board with the given type
 *
 * See sisoboards.h for the board type definitions.
 */
const char * Fg_getBoardNameByType(const int BoardType, const int UseShortName);

/**
 * \brief	Serial number.
 * \anchor  Fg_getSerialNumber
 * 
 * The function Fg_getSerialNumber() reads the serial number of the frame grabber.
 *
 * \param	Fg	frame grabber to use
 *
 * \returns	The serial number of the board. To get the same representation as seen on the board it should be written out in hexadecimal.
 */
unsigned int Fg_getSerialNumber(Fg_Struct *Fg);

/**
 * \brief	Software version number.
 * \anchor  Fg_getSWVersion
 * 
 * The function Fg_getSWVersion() returns the runtime version of the SDK running.
 *
 * \returns	Runtime version of the SDK running, i. e. "5.2.0"
 */
const char *const Fg_getSWVersion();

/**
 * \brief	Get the version of the currently running applet.
 * \anchor  Fg_getAppletVersion
 * 
 * \param	Fg		frame grabber to use
 * \param	AppletId	The applet ID returned by Fg_getappletId()
 *
 * \returns	the applet version of the design if it is not a design created using VisualApplets.
 *
 * See fgrab_define.h for the list of ids.
 */
const char *Fg_getAppletVersion(Fg_Struct *Fg, int AppletId);

/**
 * \brief	Get the ID of the currently running applet.
 * \anchor  Fg_getAppletId
 * 
 * \param	Fg		frame grabber to use
 * \param	ignored		should be NULL
 *
 * \returns	the applet ID of the design if it is not a design created using VisualApplets.
 *
 * See fgrab_define.h for the list of ids.
 *
 * The second parameter is only there to be compatible with earlier software interfaces. It is
 * ignored and should be NULL.
 */
int Fg_getAppletId(Fg_Struct *Fg, const char *ignored);




/**
 * \brief	query the properties of a given parameter
 * \anchor  Fg_getParameterProperty
 * 
 * \param	Fg		frame grabber to use
 * \param	parameterId	ID of parameter
 * \param	propertyId	property to be queried
 * \param	buffer	pointer to memory for return value
 * \param	bufLen	[in,out] length of the buffer for the return value, returns the length of the needed buffer
 *
 * \returns	state of success
 * \retval FG_OK success
 * \retval <> FG_OK : call fg_getErrorDescription to for details
 *
 * This function allows to query additional information for a certain parameter like datatype,...
 * The result is basically returned as a string value (null terminated string)
 * In other cases, the type of data buffer has to be declared and a type check will be done internally
 * When passing an null-pointer as buffer, the function will return the length of the needed buffersize 
 * (in bytes) to place the contents to.
 * \note PROP_ID_MIN_ID, PROP_ID_MAX_ID, PROP_ID_ACCESS_ID are returning virtual parameter IDs. The values 
 *       therefore can be retrieved by a call to Fg_GetParameter functions
 *       
 *
 * \since 5.2.0
 */
int Fg_getParameterProperty(Fg_Struct *Fg, 
							const int parameterId, 
							const enum FgProperty propertyId, 
							void* buffer, int* bufLen);

/**
 * \brief	query the properties of a given parameter
 * \anchor  Fg_getParameterPropertyEx
 * 
 * \param	Fg		frame grabber to use
 * \param	parameterId	ID of parameter
 * \param	propertyId	property to be queried
 * \param	DmaIndex	the DMA index for the parameter to be queried
 * \param	buffer	pointer to memory for return value
 * \param	bufLen	[in,out] length of the buffer for the return value, returns the length of the needed buffer
 *
 * \returns	state of success
 * \retval FG_OK success
 * \retval <> FG_OK : call fg_getErrorDescription to for details
 *
 * This function allows to query additional information for a certain parameter like datatype,...
 * The result is basically returned as a string value (null terminated string)
 * In other cases, the type of data buffer has to be declared and a type check will be done internally
 * When passing an null-pointer as buffer, the function will return the length of the needed buffersize 
 * (in bytes) to place the contents to.
 * \note PROP_ID_MIN_ID, PROP_ID_MAX_ID, PROP_ID_ACCESS_ID are returning virtual parameter IDs. The values 
 *       therefore can be retrieved by a call to Fg_GetParameter functions
 *       
 *
 * \since 5.5.1
 */
int Fg_getParameterPropertyEx(Fg_Struct *Fg, 
							const int parameterId, 
							const enum FgProperty propertyId, 
							const int DmaIndex, 
							void* buffer, int* bufLen);

/**
 * \brief	query information about the overall framegrabber setup
 * \anchor  Fg_getSystemInformation
 * 
 * returns various information about the currently present framegrabber system in form of a null-terminated string.
 * For additional control / addressing the information to be queried, param1 can be used, depending on the value to e queried. 
 * Example: when querying information about a certain board in a multiboard system, the board has to be addressed. Param1 might be used
 * for addressing the board
 * When passing an null-pointer as buffer, the function will return the length of the needed buffersize (in bytes) to place the contents to.
 *
 * \param	Fg		frame grabber to use, might set to 0 for global information, must be set at certain parameters
 * \param	selector	value to query
 * \param	propertyId	determine which information to get, corresponding to FgProperty
 * \param	param1	1st parameter for addressing the information, depends on the selected info (selector) to be queried
 *                  If there is a conflict to the passed. Fg parameter, this parameter gets ignored 
 * \param	buffer	buffer for result value; for some queries the buffer is used both for input and output, on output the buffer will be overwritten
 * \param	bufLen	length of the allocated buffer [in], number of bytes filled into the buffer [out]
 *
 * \returns	#FG_OK for success and valid data inside the buffer. In this case the parameter buflen is filled accordingly
 * \returns	<> #FG_OK in case of errros, call fg_getLastErrorDescription() for Details
 *
 *
 * \since 5.2.0
 */
int Fg_getSystemInformation(Fg_Struct *Fg, const enum Fg_Info_Selector selector, const enum FgProperty  propertyId, int param1, void* buffer, unsigned int* bufLen);

#ifdef __cplusplus
}

/**
 * \overload
 */
static INLINE int Fg_getIntSystemInformationForFgHandle(Fg_Struct *Fg, const enum Fg_Info_Selector selector, const enum FgProperty propertyId, int * val)
{
    char * buffer = 0;
    unsigned int len = 0;

    int result = Fg_getSystemInformation(Fg, selector, propertyId, -1, NULL, &len);
    if (result) return result;

    buffer = new(std::nothrow) char[len];
    if (!buffer) return FG_NOT_ENOUGH_MEMORY;

    result = Fg_getSystemInformation(Fg, selector, propertyId, -1, buffer, &len);
    if (!result) {
        std::istringstream is(buffer);
        is >> (*val);
        if (is.fail()) result = FG_INVALID_TYPE;
    }

    delete[] buffer;
    return result;
}

/**
 * \overload
 */
static INLINE int Fg_getIntSystemInformationForBoardIndex(unsigned int BoardIndex, const enum Fg_Info_Selector selector, const enum FgProperty propertyId, int * val)
{
    char * buffer = 0;
    unsigned int len = 0;

    int result = Fg_getSystemInformation(NULL, selector, propertyId, static_cast<int>(BoardIndex), NULL, &len);
    if (result) return result;

    buffer = new(std::nothrow) char[len];
    if (!buffer) return FG_NOT_ENOUGH_MEMORY;

    result = Fg_getSystemInformation(NULL, selector, propertyId, static_cast<int>(BoardIndex), buffer, &len);
    if (!result) {
        std::istringstream is(buffer);
        is >> (*val);
        if (is.fail()) result = FG_INVALID_TYPE;
    }

    delete[] buffer;
    return result;
}

/**
 * \overload
 */
static INLINE int Fg_getStringSystemInformationForFgHandle(Fg_Struct *Fg, const enum Fg_Info_Selector selector, const enum FgProperty propertyId, std::string & val, const std::string & arg = "")
{
    char * buffer = 0;
    unsigned int len = 0;

    if (arg.length()) {
        len = static_cast<unsigned int>(arg.length()) + 1;
        buffer = new(std::nothrow) char[len];
        if (!buffer) return FG_NOT_ENOUGH_MEMORY;

#ifdef _WIN32
        strncpy_s(buffer, len, arg.c_str(), len - 1);
#else
        strncpy(buffer, arg.c_str(), len - 1);
#endif
    }

    int result = Fg_getSystemInformation(Fg, selector, propertyId, -1, buffer, &len);
    if (!result) {
        if (!arg.empty() && len <= static_cast<unsigned int>(arg.length()) + 1) {
            val = buffer;
            delete[] buffer;
            return 0;
        }
    } else {
        delete[] buffer;
        return result;
    }

    delete[] buffer;
    buffer = new(std::nothrow) char[len];
    if (!buffer) return FG_NOT_ENOUGH_MEMORY;

    if (!arg.empty()) {
#ifdef _WIN32
        strncpy_s(buffer, len, arg.c_str(), arg.length());
#else
        strncpy(buffer, arg.c_str(), arg.length());
#endif
    }

    result = Fg_getSystemInformation(Fg, selector, propertyId, -1, buffer, &len);
    if (!result) {
        val = buffer;
    }

    delete[] buffer;
    return result;
}

/**
 * \overload
 */
static INLINE int Fg_getStringSystemInformationForBoardIndex(unsigned int BoardIndex, const enum Fg_Info_Selector selector, const enum FgProperty propertyId, std::string & val, const std::string & arg = "")
{
    char * buffer = 0;
    unsigned int len = 0;

    if (!arg.empty()) {
        len = static_cast<unsigned int>(arg.length()) + 1;
        buffer = new(std::nothrow) char[len];
        if (!buffer) return FG_NOT_ENOUGH_MEMORY;

#ifdef _WIN32
        strncpy_s(buffer, len, arg.c_str(), arg.length());
#else
        strncpy(buffer, arg.c_str(), arg.length());
#endif
    }

    int result = Fg_getSystemInformation(NULL, selector, propertyId, static_cast<int>(BoardIndex), buffer, &len);
    if (!result) {
        if (!arg.empty() && len <= static_cast<unsigned int>(arg.length()) + 1) {
            val = buffer;
            delete[] buffer;
            return 0;
        }
    } else {
        delete[] buffer;
        return result;
    }

    delete[] buffer;
    buffer = new(std::nothrow) char[len];
    if (!buffer) return FG_NOT_ENOUGH_MEMORY;

    if (!arg.empty()) {
#ifdef _WIN32
        strncpy_s(buffer, len, arg.c_str(), arg.length());
#else
        strncpy(buffer, arg.c_str(), arg.length());
#endif
    }

    result = Fg_getSystemInformation(NULL, selector, propertyId, static_cast<int>(BoardIndex), buffer, &len);
    if (!result) {
        val = buffer;
    }

    delete[] buffer;
    return result;
}

extern "C" {
#endif

/**
 * \brief	read user data area
 * \anchor  Fg_readUserDataArea
 * 
 * Using Fg_readUserDataArea() you can read a block of memory from reserved space in the configuration memory of the frame grabber.
 * Please consult the manual of your frame grabber about support for this feature.
 * Also note that the memory might be very limited, slow and implemented on a device that only provides a limited number of write cycles.
 *
 * \param	Fg		frame grabber to use, if set to 0 you must provide a board number
 * \param	boardId	board number, can be set to (-1) if you supply a frame grabber handle
 * \param	offs	offset to the start of the user memory area
 * \param	size	number of bytes to read
 * \param	buffer	buffer for the memory transfer
 *
 * \returns	#FG_OK for success and valid data inside the buffer.
 * \returns	<> #FG_OK in case of errros, call Fg_getLastErrorDescription() for Details.
 * \returns #FG_NOT_IMPLEMENTED if the feature is not supported on the hardware.
 * \returns #FG_INVALID_BOARD_NUMBER if no handle was supplied and the board number could not be found (or the handle and board number don't match).
 * \returns #FG_RANGE_ERR if the requested offset is not within the size of the user data area.
 * \returns #FG_SIZE_ERROR if the requested size is too large to fit in the user data area (from the given offset).
 * \returns #FG_PTR_INVALID if the buffer address is invalid (or the size is 0)
 *
 * \since 5.3
 */
int Fg_readUserDataArea(Fg_Struct *Fg, const int boardId, const unsigned int offs, const unsigned int size, void * buffer);

/**
 * \brief	write user data area
 * \anchor  Fg_writeUserDataArea
 * 
 * Using Fg_writeUserDataArea() you can write a block of memory to reserved space in the configuration memory of the frame grabber.
 * Please consult the manual of your frame grabber about support for this feature.
 * Also note that the memory might be very limited, slow and implemented on a device that only provides a limited number of write cycles.
 *
 * \param	Fg		frame grabber to use, if set to 0 you must provide a board number
 * \param	boardId	board number, can be set to (-1) if you supply a frame grabber handle
 * \param	offs	offset to the start of the user memory area
 * \param	size	number of bytes to write
 * \param	buffer	buffer for the memory transfer
 *
 * \returns	#FG_OK for success and valid data inside the buffer.
 * \returns	<> #FG_OK in case of errros, call Fg_getLastErrorDescription() for Details.
 * \returns #FG_NOT_IMPLEMENTED if the feature is not supported on the hardware.
 * \returns #FG_INVALID_BOARD_NUMBER if no handle was supplied and the board number could not be found (or the handle and board number don't match).
 * \returns #FG_RANGE_ERR if the requested offset is not within the size of the user data area.
 * \returns #FG_SIZE_ERROR if the requested size is too large to fit in the user data area (from the given offset).
 * \returns #FG_PTR_INVALID if the buffer address is invalid (or the size is 0)
 *
 * \since 5.3
 */
int Fg_writeUserDataArea(Fg_Struct *Fg, const int boardId, const unsigned int offs, const unsigned int size, const void * buffer);

/**
 * \brief	Reading the status of frame buffer.
 * \anchor  Fg_getStatus
 * 
 * With the function Fg_getStatus() you are getting information about the acquisition status. The parameter
 * data depends on the chosen flag. For details, please see tables below.
 *
 * By the help of Fg_setStatus() buffers can be blocked or unblocked.
 *
 * <TABLE>
 * <TR><TD><B>Parameter</B></TD><TD><B>Meaning</B></TD></TR>
 * <TR><TD>NUMBER_OF_GRABBED_IMAGES</TD><TD>Sum of all acquired images. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_LOST_IMAGES</TD><TD>Sum of lost images. The parameter data requires the DMA no.</TD></TR>
 * <TR><TD>NUMBER_OF_BLOCK_LOST_IMAGES</TD><TD>Sum of all images, which are lost by blocking of the frame buffer. The parameter data requires the DMA no.</TD></TR>
 * <TR><TD>NUMBER_OF_BLOCKED_IMAGES</TD><TD>Sum of blocked images. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_ACT_IMAGE</TD><TD>Number of last acquired image. The parameter data requires the timeout value.</TD></TR>
 * <TR><TD>NUMBER_OF_LAST_IMAGE</TD><TD>Number of last get image. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_NEXT_IMAGE</TD><TD>Number of next image after last get image. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_IMAGES_IN_PROGRESS</TD><TD>Sum of all images which aren't gotten, yet. The parameter data will be ignored.</TD></TR>
 * <TR><TD>BUFFER_STATUS</TD><TD>1 if the buffer is locked, otherwise 0. The parameter data will be ignored.</TD></TR>
 * <TR><TD>GRAB_ACTIVE_X</TD><TD>Tells whether the commad is active (1) at the moment (otherwise 0). The parameter data will be ignored.</TD></TR>
 * </TABLE>
 *
 * \param	Fg		frame grabber to use
 * \param	Param		See table below ...
 * \param	Data		Optional data.
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 *
 * \returns	Return depends on given flag, see table above ...
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

frameindex_t Fg_getStatus(Fg_Struct *Fg, const int Param, const frameindex_t Data, const unsigned int DmaIndex);

/**
 * \brief	Reading the status of frame buffer.
 * \anchor Fg_getStatusEx
 * 
 * With the function Fg_getStatusEx() you are getting information about the acquisition status. The parameter
 * data depends on the chosen flag. For details, please see tables below.
 *
 * By the help of Fg_setStatusEx() buffers can be blocked or unblocked.
 *
 * <TABLE>
 * <TR><TD><B>Parameter</B></TD><TD><B>Meaning</B></TD></TR>
 * <TR><TD>NUMBER_OF_GRABBED_IMAGES</TD><TD>Sum of all acquired images. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_LOST_IMAGES</TD><TD>Sum of lost images. The parameter data requires the DMA no.</TD></TR>
 * <TR><TD>NUMBER_OF_BLOCK_LOST_IMAGES</TD><TD>Sum of all images, which are lost by blocking of the frame buffer. The parameter data requires the DMA no.</TD></TR>
 * <TR><TD>NUMBER_OF_BLOCKED_IMAGES</TD><TD>Sum of blocked images. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_ACT_IMAGE</TD><TD>Number of last acquired image. The parameter data requires the timeout value.</TD></TR>
 * <TR><TD>NUMBER_OF_LAST_IMAGE</TD><TD>Number of last get image. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_NEXT_IMAGE</TD><TD>Number of next image after last get image. The parameter data will be ignored.</TD></TR>
 * <TR><TD>NUMBER_OF_IMAGES_IN_PROGRESS</TD><TD>Sum of all images which aren't gotten, yet. The parameter data will be ignored.</TD></TR>
 * <TR><TD>BUFFER_STATUS</TD><TD>1 if the buffer is locked, otherwise 0. The parameter data will be ignored.</TD></TR>
 * <TR><TD>GRAB_ACTIVE_X</TD><TD>Tells whether the commad is active (1) at the moment (otherwise 0). The parameter data will be ignored.</TD></TR>
 * </TABLE>
 *
 * \param	Fg	frame grabber to use
 * \param	Param	See table below ...
 * \param	Data	Optional data, details see parameter "Param".
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	pMem	Pointer to variable buffer.
 *
 * \returns	Return depends on given flag, see table above ...
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

frameindex_t Fg_getStatusEx(Fg_Struct *Fg, const int Param, const frameindex_t Data, const unsigned int DmaIndex, dma_mem *pMem);

/**
 * \brief	Setting the status of frame buffer.
 * \anchor  Fg_setStatus
 * 
 * The function Fg_setStatus() explicitely allows, blocking or releasing of a blocked buffer (see table).
 *
 * By the help of Fg_getStatus() the acquisition status can be gotten.
 *
 * <TABLE>
 * <TR><TD><B>Parameter</B></TD><TD><B>Meaning</B></TD></TR>
 * <TR><TD>FG_UNBLOCK_ALL</TD><TD>Explicit release of all buffers.</TD></TR>
 * <TR><TD>FG_UNBLOCK</TD><TD>Explicit release of one buffer.</TD></TR>
 * </TABLE>
 *
 * \param	Fg		frame grabber to use
 * \param	Param		See table below ...
 * \param	Data		Optional data.
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 *
 * \returns	Pointer to the corresponding buffer.
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

int Fg_setStatus(Fg_Struct *Fg, const int Param, const frameindex_t Data, const unsigned int DmaIndex);

/**
 * \brief	Setting the status of frame buffer.
 * \anchor  Fg_setStatus
 * 
 * The function Fg_setStatus() explicitely allows, blocking or releasing of a blocked buffer (see table).
 *
 * By the help of Fg_getStatus() the acquisition status can be gotten.
 *
 * <TABLE>
 * <TR><TD><B>Parameter</B></TD><TD><B>Meaning</B></TD></TR>
 * <TR><TD>FG_UNBLOCK_ALL</TD><TD>Explicit release of all buffers.</TD></TR>
 * <TR><TD>FG_UNBLOCK</TD><TD>Explicit release of one buffer.</TD></TR>
 * </TABLE>
 *
 * \param	Fg		frame grabber to use
 * \param	Param		See table below ...
 * \param	Data		Optional data.
 * \param	DmaIndex	Logical number of used camera port. camera port A = 0 and camera port B = 1
 * \param	pMem		Pointer to variable buffer.
 *
 * \returns	Pointer to the corresponding buffer.
 *
 * \retval	#FG_INVALID_PARAMETER An invalid parameter has been given to the function.
 */

int Fg_setStatusEx(Fg_Struct *Fg, const int Param, const frameindex_t Data, const unsigned int DmaIndex, dma_mem *pMem);

/**
 * \brief   Gets an applet iterator.
 * \anchor  Fg_getAppletIterator
 * 
 * This function gets an applet iterator for the applets on either the
 * board or the filesystem
 * Once not needed any more, the function Fg_freeAppletIterator() must be 
 * called to free the iterator.
 * 
 * \param boardIndex    Board index
 * \param src           Applets' source.
 * \param iter          A pointer to return the iterator
 * \param flags         Can be used to filter the applets according to \ref AppletFlags "Applet Flags".
 * 
 * <TABLE>
 * <TR><TD><B>Flag</B></TD><TD><B>Meaning</B></TD></TR>
 * <TR><TD>FG_AF_IS_AVAILABLE</TD><TD>Applet is available on the filesystem.</TD></TR>
 * <TR><TD>FG_AF_IS_CORRECT_PLATFORM</TD><TD>Applet has the correct platform (E.g. Win 32/64 or Linux 32/64 Bit).</TD></TR>
 * <TR><TD>FG_AF_IS_VALID_LICENSE</TD><TD>Applet has a valid license.</TD></TR>
 * <TR><TD>FG_AF_IS_ACTIVE</TD><TD>Applet is currently active on the board.</TD></TR>
 * <TR><TD>FG_AF_IS_LOADABLE</TD><TD>Applet can be loaded via Fg_Init.</TD></TR>
 * <TR><TD>FG_AF_IS_POWERUP_APPLET</TD><TD>Applet is loaded when the system powers up.</TD></TR>
 * <TR><TD>FG_AF_IS_COMPATIBLE</TD><TD>This file is compatible with the flashed design (same FPGA UUID).</TD></TR>
 * <TR><TD>FG_AF_IS_FLASHED</TD><TD>This exact file is flashed (same FPGA & APPLET UUID).</TD></TR>
 * </TABLE>
 * 
 * \returns the number of items found, or a negative value in case of an error
 * 
 * \see Fg_freeAppletIterator
 * \see Fg_getAppletIteratorItem
 */
int Fg_getAppletIterator(int boardIndex, const enum FgAppletIteratorSource src, Fg_AppletIteratorType * iter, int flags);

/**
 * \brief   This function releases an applet iterator
 * \anchor  Fg_freeAppletIterator
 * 
 * \param iter  The iterator to be released
 * 
 * \see Fg_getAppletIterator
 */
int Fg_freeAppletIterator(Fg_AppletIteratorType iter);

/**
 * \brief   Gets an item from applet iterator by index.
 * \anchor  Fg_getAppletIteratorItem
 * 
 * This function gets an identifier for the n-th applet in the iterator
 * Identifier can then be used to get different applet properties.
 * 
 * \param iter  applet iterator
 * \param index index of the applet
 * 
 * \returns the applet iterator item, or NULL if index is negative or larger
 * than or equal to the iterator size.
 * 
 * \see Fg_getAppletIntProperty
 * \see Fg_getAppletStringProperty
 */
Fg_AppletIteratorItem Fg_getAppletIteratorItem(Fg_AppletIteratorType iter, int index);

/**
 * \brief   Looks for an item from applet iterator matching the applet path, file, or name.
 * \anchor  Fg_getAppletIteratorItem
 * 
 * This function gets an identifier for the n-th applet in the iterator
 * Identifier can then be used to get different applet properties.
 * 
 * \param iter  applet iterator
 * \param path  path, file or name of the applet
 * 
 * \returns the applet iterator item, or NULL if no matching item was found
 * 
 * \see Fg_getAppletIntProperty
 * \see Fg_getAppletStringProperty
 */
Fg_AppletIteratorItem Fg_findAppletIteratorItem(Fg_AppletIteratorType iter, const char * path);

/**
 * \brief   Add an applet to applet iterator
 * 
 * This function add an applet to applet iterator
 * 
 * \param iter  Applet iterator
 * \param path  Path, file or name of the applet
 * \param numItems Number of all applet in iterator
 *
 * \returns Applet iterator item or NUll in case of an error          
 *
 * 
 */
Fg_AppletIteratorItem Fg_addAppletIteratorItem(Fg_AppletIteratorType iter, const char * path, int * numItems);

/**
 * \brief   Gets an integer applet properties
 * \anchor  Fg_getAppletIntProperty
 * 
 * This function gets an integer property of an applet.  Different properties
 * are available within enum FgAppletIntProperty.
 * Before getting any property, FG_AP_INT_INFO property must be read to
 * make sure that other properties are valid, using FG_AI_IS_VALID mask.
 * If FG_AI_IS_VALID is not set, then all other properties are not valid.
 * The applet item should be retrieved from an Fg_AppletIterator using
 * function Fg_getAppletIteratorItem.
 * 
 * \param item      Applet item that is retrieved from Fg_getAppletIteratorItem
 * \param property  Selected property
 * 
 * \returns integer value corresponding to the passed property
 */
int64_t Fg_getAppletIntProperty(Fg_AppletIteratorItem item, const enum FgAppletIntProperty property);

/**
 * \brief Gets string applet properties
 * \anchor  Fg_getAppletStringProperty
 * 
 * This function gets a string property of an applet.  Different properties
 * are available within enum FgAppletStringProperty.
 * Before getting any property, FG_AP_INT_INFO property must be read (using
 * Fg_getAppletIntProperty) to make sure that other properties are valid,
 * using FG_AI_IS_VALID mask.  If FG_AI_IS_VALID is not set, then all other
 * properties are not valid.
 * The applet item should be retrieved from an Fg_AppletIterator using
 * function Fg_getAppletIteratorItem.
 * 
 * \param item applet item that is retrieved from Fg_getAppletIteratorItem
 * \param property selected property
 * 
 * \returns string value corresponding to the passed property
 */
const char * Fg_getAppletStringProperty(Fg_AppletIteratorItem item, const enum FgAppletStringProperty property);

#ifdef __cplusplus
} /* extern "C" */

/**
 * overload
 */
static INLINE int Fg_findApplet(const unsigned int BoardIndex, std::string & Path) {

	// ensure a defined state of the output argument
	Path.clear();

	Fg_AppletIteratorType iter;
	int ret = Fg_getAppletIterator(BoardIndex, FG_AIS_BOARD, &iter, FG_AF_IS_ACTIVE | FG_AF_IS_AVAILABLE);

	if (ret < 0) {
		return ret;
	} 

	const int numItems = ret;
	ret = FG_FILE_NOT_FOUND;

	// Iterate over found applets in case there are more than one (althought this should not be the case).
	for (int i = 0; i < numItems && ret != FG_OK; ++i) {
		Fg_AppletIteratorItem item = Fg_getAppletIteratorItem(iter, i);
		if (item != NULL) {
			int64_t info = Fg_getAppletIntProperty(item, FG_AP_INT_INFO);
			if (info & FG_AI_IS_VALID) {
				Path = Fg_getAppletStringProperty(item, FG_AP_STRING_APPLET_PATH);
				ret = FG_OK;
			}
		}
	}

    Fg_freeAppletIterator(iter);
	return ret;
}

extern "C" {
#endif /* __cplusplus */

/** @name Events
 *
 * These functions offer notifications about certain conditions
 * like level change of the CameraLink FVAL signal. The available
 * events depend on applet and board type.
 */
/*@{*/
/**

 * \brief get the event mask for the named event
 * \anchor  Fg_getEventMask
 * 
 * \param	Fg    frame grabber to use
 * \param	name  name of the requested event
 * \returns     event mask of the event
 * \retval      0 no event of the given name was found
 *
 * \since 5.1.0
 */
uint64_t Fg_getEventMask(Fg_Struct *Fg, const char *name);

/**
 * \brief get the payload size for the event represented by the event mask
 * \anchor Fg_getEventPayload
 * 
 * \param	Fg     frame grabber to use
 * \param	mask   event mask
 * \returns	size of payload of event if any, FG_INVALID_EVENTMASK or FG_NO_EVENTS_FOUND on error
 *
 * Only one bit in the event mask may be set.
 *
 * \since 5.5.1
 */
int Fg_getEventPayload(Fg_Struct *Fg, uint64_t mask);

/**
 * \brief get the name for the event represented by the event mask
 * \anchor Fg_getEventName
 * 
 * \param	Fg     frame grabber to use
 * \param	mask   event mask
 * \returns	     name of event if any, NULL pointer if no event is associated with that name
 *
 * Only one bit in the event mask may be set.
 *
 * \since 5.1.0
 */
const char * const Fg_getEventName(Fg_Struct *Fg, uint64_t mask);

/**
 * \brief get the number of available event types
 * \anchor Fg_getEventCount
 * 
 * \param	Fg  frame grabber to use
 * \returns   number of available event types.
 *
 * This function returns the number of available event types depending on
 * the currently loaded applet. It may be used for querying purposes
 *
 * \since 5.2
 */
int Fg_getEventCount(Fg_Struct *Fg);

/**
 * \brief activates/disable event generation
 * \anchor Fg_activateEvents
 *
 * \param Fg frame grabber to use
 * \param mask event mask to work on
 * \param enable 0 to disable, 1 to enable
 * \retval #FG_NOT_INIT Fg is NULL
 * \retval #FG_INVALID_PARAMETER enable is not 0 or 1 or mask is not a valid event mask
 *
 * \since 5.1.2
 *
 * mask may only contain valid event bits. As an exception it is allowed to
 * pass a value where all bits are set (i.e. (uint64_t)-1) and enable set to 0 to disable
 * all events which is explicitly allowed even if there are less than 64 events.
 */
int Fg_activateEvents(Fg_Struct *Fg, uint64_t mask, const unsigned int enable);

/**

 * \brief	drop all already queued events
 * \anchor Fg_clearEvents
 *
 * \param Fg frame grabber to use
 * \param mask which events to clear
 * \retval #FG_NOT_INIT Fg is NULL
 * \retval #FG_INVALID_PARAMETER mask is not a valid event mask
 *
 * mask may only contain valid event bits. As an exception it is allowed to
 * pass a value where all bits are set (i.e. (uint64_t)-1) to clear all
 * events which is explicitly allowed even if there are less than 64 events.
 *
 * \since 5.1.2
 */
int Fg_clearEvents(Fg_Struct *Fg, uint64_t mask);

/**

 * \brief wait for events
 * \anchor Fg_eventWait
 *
 * \param	Fg		frame grabber to use
 * \param	mask		event mask
 * \param	timeout		timeout in seconds
 * \param	flags		control flags. Or'ed combination of FgEventControlFlags. In doubt use FG_EVENT_DEFAULT_FLAGS.
 * \param	info		event info structure to fill with additional data, in doubt pass NULL
 * \returns	mask of triggered events or 0 on error
 * \retval	0 an error occurred
 *
 * You must not call this function and Fg_registerEventCallback() on the same event.
 *
 * If 0 is returned Fg_getLastErrorNumber() will return the information about the error
 * type. Usually this will be #FG_TIMEOUT_ERR.
 *
 * The event mask can be obtained by calling Fg_getEventMask(). Several masks may be
 * or'ed together to wait for the first of those events to happen. The return value
 * states which events actually happened. If an info structure is passed the corresponding
 * entries for those events will have the requested data filled in. The entries of all
 * other events are left untouched.
 *
 * You are strongly encouraged to call FG_EVENT_INFO_INIT(info) before every call to
 * Fg_eventWait() if you are using the info parameter to make sure you don't see old data
 * in the structure.
 *
 * \since 5.1.0
 */
uint64_t Fg_eventWait(Fg_Struct *Fg, uint64_t mask, unsigned int timeout, const unsigned int flags, struct fg_event_info *info);

/**

 * \brief register callback for events
 * \anchor Fg_registerEventCallback
 *
 * \param	Fg		frame grabber to use
 * \param	mask		event mask
 * \param	handler		event handler function to call
 * \param	data		context data passed to handler on event
 * \param	flags		control flags. Or'ed combination of FgEventControlFlags. In doubt use FG_EVENT_DEFAULT_FLAGS.
 * \param	info		event info structure to fill with additional data, in doubt pass NULL
 * \returns	FG_OK on success, error code otherwise
 *
 * You must not call this function and Fg_eventWait() on the same event.
 *
 * Use this function to register a callback function for any combination of event
 * masks. This will overwrite the handler for any events in mask. Use Fg_getEventMask()
 * to obtain the event mask corresponding to a specific event.
 *
 * To unregister a callback pass NULL as handler to this function. You must pass the
 * same event mask you passed in when registering the callback. The flags argument must
 * be set to FG_EVENT_DEFAULT_FLAGS.
 *
 * \code
 * Fg_registerEventCallback(Fg, mask, NULL, NULL, FG_EVENT_DEFAULT_FLAGS, NULL);
 * \endcode
 *
 * All callbacks of one board will be handled in the same thread. If your callback
 * handler takes a long time to complete it will delay all further events until it
 * has finished. If you need both a long running event handler and short reaction time
 * you must create your own worker thread and start it's execution from the callback.
 *
 * You may register multiple callbacks with different event masks at the same time. You
 * may also register the same event handler with different data arguments at the same time.
 * \verbatim
  If your event handler returns a value other than 0 it is automatically unregistered from
  all events it was registered to with the same data argument.\endverbatim
 *
 * If you pass a value different from NULL as info these structure will be filled with
 * the requested information before handler is called. You should never use the data
 * outside your handler as it will be overwritten before handler is called. You must not
 * change the contents of the struct inside the handler.
 *
 * \since 5.1.0
 */
int Fg_registerEventCallback(Fg_Struct *Fg, uint64_t mask, Fg_EventFunc_t handler, struct fg_event_data *data, const unsigned int flags, struct fg_event_info *info);
/*@}*/

/**
 * \brief register callback for asynchronous notifications
 * \anchor Fg_registerAsyncNotifyCallback
 *
 * \param	Fg			frame grabber to use
 * \param	handler		notification handler function to call
 * \param	context		context data passed to handler on notification
 * \returns	FG_OK on success, error code otherwise
 *
 * Use this function to register a callback function for asynchronous
 * notifications sent by the driver.
 * This will install a new callback function, but not override any
 * callbacks that were registered before.
 * The same callback function can be registered more than once, only if you
 * provide a unique, non-NULL context pointer for each registration.
 * You cannot register the same callback, if it was already registered with
 * NULL as context.
 *
 * To unregister a callback, call Fg_unregisterAsyncNotifyCallback().
 *
 * All callbacks of one board will be handled in the same thread. If your callback
 * handler takes a long time to complete it will delay all further notifications until it
 * has finished. If you need both a long running notification handler and short reaction time
 * you must create your own worker thread and start it's execution from the callback.
 *
 * \verbatim
  If your event handler returns a value other than 0 it is automatically unregistered.\endverbatim
 *
 * \since 5.3
 */
int Fg_registerAsyncNotifyCallback(Fg_Struct *Fg, Fg_AsyncNotifyFunc_t handler, void * context);

/**
 * \brief unregister callback for asynchronous notifications
 * \anchor Fg_registerAsyncNotifyCallback
 *
 * \param	Fg			frame grabber to use
 * \param	handler		notification handler function to call
 * \param	context		context data passed to handler on notification
 * \returns	FG_OK on success, error code otherwise
 *
 * Use this function to unregister a callback function for asynchronous
 * notifications that was registered before with
 * Fg_registerAsyncNotifyCallback().
 * You must provide the same parameters that you used for registering the
 * callback. You can, however, provide NULL for the context, in which case
 * all callbacks using the same function with different context pointers
 * will be unregistered.
 *
 * \since 5.3
 */
int Fg_unregisterAsyncNotifyCallback(Fg_Struct *Fg, Fg_AsyncNotifyFunc_t handler, void * context);

/**
 * \brief acknowledge an asynchronous notifications
 * \anchor Fg_resetAsyncNotify
 *
 * \param	Fg			frame grabber to use
 * \param	notification	the notification code
 * \param	pl			parameter 1 (notification dependent)
 * \param	ph			parameter 2 (notification dependent)
 * \returns	FG_OK on success, error code otherwise
 *
 * Use this function to acknowledge an asynchronous notification that was
 * received through one of your registered handlers.
 *
 * \since 5.3
 */
int Fg_resetAsyncNotify(Fg_Struct *Fg, unsigned long notification, unsigned long pl, unsigned long ph);

/**
 * \brief	set Exsync camera signal
 * \anchor  Fg_setExsync
 *
 * \param	Fg	frame grabber to use
 * \param	Flag	Exsync value to set (#FG_ON or #FG_OFF).
 * \param	CamPort	Camera port to set.
 * \returns	Status code.
 * \retval	#FG_OK	Exsync value was set.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. nonexistent camera port).
 *
 * The function int Fg_setExsync() explicitly allows a switch to on and off of the exsync signal to the camera.
 *
 * \deprecated
 * This function internally calls Fg_setParameter() with parameter #FG_EXSYNCON and passes Flag as
 * value. You should use that interface as this has slightly less overhead.
 *
 \verbatim
 This function can only be used with acquisition applets. VisualApplets generated applets
 do not use this interface. Please refer to the VisualApplets operator manual for documentation
 for trigger operators.\endverbatim
 */
int Fg_setExsync(Fg_Struct *Fg, int Flag, const unsigned int CamPort);

/**
 * \brief	set strobe camera signal
 * \anchor  Fg_setFlash
 *
 * \param	Fg	frame grabber to use
 * \param	Flag	Strobe value to set (#FG_ON or #FG_OFF).
 * \param	CamPort	Camera port to set.
 * \returns	Status code.
 * \retval	#FG_OK	Exsync value was set.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. nonexistent camera port).
 *
 * The function int Fg_setFlash() explicitly allows a switch to on and off of the strobe signal.
 *
 * \deprecated
 * This function internally calls Fg_setParameter() with parameter #FG_FLASHON and passes Flag as
 * value. You should use that interface as this has slightly less overhead.
 *
 \verbatim
 This function can only be used with acquisition applets. VisualApplets generated applets
 do not use this interface. Please refer to the VisualApplets operator manual for documentation
 for trigger operators.\endverbatim
 */
int Fg_setFlash(Fg_Struct *Fg, int Flag, const unsigned int CamPort);

/**
* \brief	send trigger signal to the camera
* \anchor   Fg_sendSoftwareTrigger
 *
* \param	Fg	frame grabber to use
* \param	CamPort	Camera port to set.
* \returns	Status code.
* \retval	#FG_OK	Trigger was accepted (softwaretrigger mode). Queue empty and trigger accepted (softwaretrigger queue mode)
* \retval	#FG_INVALID_PARAMETER	An invalid argument was passed (e.g. nonexistent camera port).
* \retval	#FG_SOFTWARE_TRIGGER_PENDING	Trigger is accepted but can't be performed immediately since another trigger is currently processed. The trigger is added to the queue and will be processed delayed as soon as possible (only in software trigger queue mode)
* \retval	#FG_SOFTWARE_TRIGGER_BUSY	The trigger command cannot be accepted and gets lost due to on of the following reasons: Trigger pulse is already active (in softwaretrigger mode). Trigger queue is too full to accept the trigger. (softwaretrigger queue mode)
*
* This function can be used to send immediately a single trigger pulse to the camera in softwaretrigger mode or to add one trigger to the queue in softwaretrigger queue mode.
* The length of the signals is determined by the exposure time and is configured by the function Fg_setParameter(). During the duration
* of the camera trigger, a new trigger signal can't be sent in software trigger mode. In software trigger queue mode a new trigger signal to the camera are created if the previous triggered image was fully transfered onto the grabber.
*
* Besides the camera trigger, the signal StrobePulse is activated, which can be used as control for
* flash lights. The delay of the StrobePulse relative to the camera trigger is set by the function
* Fg_getParameter() (see flash light control).
*
* This function internally calls Fg_setParameter() with parameter #FG_SENDSOFTWARETRIGGER with Triggers as value. 
* You should use that interface as this has slightly less overhead.
*
\verbatim
The microEnable IV-GigE x4 currently does not support the trigger system.\endverbatim
*
\verbatim
This function can only be used with acquisition applets. VisualApplets generated applets
do not use this interface. Please refer to the VisualApplets operator manual for documentation
for trigger operators.\endverbatim
*/
int Fg_sendSoftwareTrigger(Fg_Struct *Fg, const unsigned int CamPort);

/**
* \brief	send multiple trigger signals to the camera
* \anchor   Fg_sendSoftwareTriggerEx
 *
* \param	Fg	frame grabber to use
* \param	CamPort	Camera port to set.
* \param	Triggers Number of triggers to be sent to the camera.
* \returns	Status code.
* \retval	#FG_OK	Trigger was accepted (softwaretrigger mode). Trigger will be processed immediately
* \retval	#FG_INVALID_PARAMETER	An invalid argument was passed (e.g. nonexistent camera port).
* \retval	#FG_SOFTWARE_TRIGGER_PENDING	Trigger is accepted but can't be performed immediately since another trigger is currently processed. The trigger is added to the queue and will be processed delayed as soon as possible (only in software trigger queue mode)
* \retval	#FG_SOFTWARE_TRIGGER_BUSY	Trigger pulse is already active (softwaretrigger mode). Trigger queue is too full to accept the value. (softwaretrigger queue mode)
* \retval	#FG_VALUE_OUT_OF_RANGE	The entered value is outside of the valid range.
*
* This function can be used either to send immediately a single trigger pulse to the camera in softwaretrigger mode or to add (and send) multiple triggers to the queue in softwaretrigger queue mode.
* The length of the signals is determined by the exposure time and is configured by the function Fg_setParameter(). During the duration
* of the camera trigger, a new trigger signal can't be sent in software trigger mode. In software trigger queue mode next trigger signal to the camera is created if the previous triggered image was fully transfered onto the grabber.
*
* Besides the camera trigger, the signal StrobePulse is activated, which can be used as control for
* flash lights. The delay of the StrobePulse relative to the camera trigger is set by the function
* Fg_getParameter() (see flash light control).
*
* This function internally calls Fg_setParameter() with parameter #FG_SENDSOFTWARETRIGGER with Triggers as value. 
* You should use that interface as this has slightly less overhead.
*
\verbatim
The microEnable IV-GigE x4 currently does not support the trigger system.\endverbatim
*
\verbatim
This function can only be used with acquisition applets. VisualApplets generated applets
do not use this interface. Please refer to the VisualApplets operator manual for documentation
for trigger operators.\endverbatim
 *
 * \since 5.1.2
 */
int Fg_sendSoftwareTriggerEx(Fg_Struct *Fg, const unsigned int CamPort, const unsigned int Triggers);

/** @name Shading Correction
 *
 * \verbatim
These functions can only be used with acquisition applets that offer shading functionality.
VisualApplets generated applets with shading functionality do not use this interface. Please refer
to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
/*@{*/
/**
 * \brief	allocate shading control object
 * \anchor  Fg_AllocShading
 *
 * \param	Fg	frame grabber to use
 * \param	set	Shading data set to use.
 * \param	CamPort	Camera port to associate this shading buffer.
 * \returns	Pointer to shading control object
 * \retval	NULL	Shading control object was not created as an error occured.
 *
 * Before using the shading functionality, it is neccessary to allocate some buffer for shading calculation.
 * Fg_AllocShading() is the first function to be used for complete shading feature.
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
ShadingMaster *Fg_AllocShading(Fg_Struct *Fg, int set, const unsigned int CamPort);

/**
 * \brief	release shading control object
 * \anchor  Fg_FreeShading
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to release.
 * \returns	Status code.
 * \retval	#FG_OK	Shading control object was released.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Fg_FreeShading(Fg_Struct *Fg, ShadingMaster *sh);

/**
 * \brief	activate shading object
 * \anchor  Shad_GetAccess
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to activate.
 * \returns	Status code.
 * \retval	#FG_OK	Shading control object was activated.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 * Before using the shading interface, it is neccessary to activate the access with Shad_GetAccess().
 * In detail, the registers on your grabber will be prepared for shading. Afterwards, the interface
 * can be closed with Shad_FreeAccess().
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Shad_GetAccess(Fg_Struct *Fg, ShadingMaster *sh);

/**
 * \brief	deactivate shading object
 * \anchor  Shad_FreeAccess
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to deactivate.
 * \returns	Status code.
 * \retval	#FG_OK	Shading control object was deactivated.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 * After opening the access to the shading interface and after working with the shading feature,
 * the shading interface can be closed with Shad_FreeAccess().
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Shad_FreeAccess(Fg_Struct *Fg, ShadingMaster *sh);

/**
 * \brief	query current line number for shading correction
 * \anchor  Shad_GetMaxLine
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to use.
 * \returns	line number or negative error code
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 * Before shading calculation for the current image can be done, the line number has to be
 * known. Shad_GetMaxLine() returns with that information.
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Shad_GetMaxLine(Fg_Struct *Fg, ShadingMaster *sh);

/**
 * \brief	set substractive correction value for shading correction
 * \anchor  Shad_SetSubValueLine
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to use.
 * \param	x	Position in the line.
 * \param	channel	Color channel to use (0 for gray applets, #FG_RED, #FG_GREEN, or #FG_BLUE for color applets).
 * \param	sub	Subtractive correction value (range 0 to 1).
 * \returns	Status code.
 * \retval	#FG_OK	Shading value was set.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 * Before shading calculation can be done, some parameters have to be set.
 * Shad_SetSubValueLine() sets parameters for the offset (subtractive correction).
 *
 * You can either manually set values for shading with this function or you can otherwise set a path to a file for
 * subtraction calculation. This can be done with Fg_setParameter() using FG_SHADING_SUBFILE0 parameter.
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Shad_SetSubValueLine(Fg_Struct *Fg, ShadingMaster *sh, int x, int channel, float sub);

/**
 * \brief	set multiplicative correction value for shading correction
 * \anchor   Shad_SetMultValueLine
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to use.
 * \param	x	Position in the line.
 * \param	channel	Color channel to use (0 for gray applets, #FG_RED, #FG_GREEN, or #FG_BLUE for color applets).
 * \param	mult	Multiplicative correction value (range 0 to 8).
 * \returns	Status code.
 * \retval	#FG_OK	Shading value was set.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 * Before shading calculation can be done, some parameters have to be set.
 * Shad_SetMultValueLine() sets parameters for the gain (multiplicative correction).
 *
 * You can either manually set values for shading with this function or you can otherwise set a path to a file for
 * multiplication calculation. This can be done with Fg_setParameter() using FG_SHADING_MULTFILE0 parameter.
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Shad_SetMultValueLine(Fg_Struct *Fg, ShadingMaster *sh, int x, int channel, float mult);

/**
 * \brief	set defective pixel interpolation value for shading correction
 * \anchor  Shad_SetFixedPatternNoiseLine 
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to use.
 * \param	x	Position in the line.
 * \param	channel	Color channel to use (0 for gray applets, #FG_RED, #FG_GREEN, or #FG_BLUE for color applets).
 * \param	on	enable or disable interpolation for this pixel (values 0 or 1).
 * \returns	Status code.
 * \retval	#FG_OK	Shading value was set.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 * Before shading calculation can be done, some parameters have to be set.
 * Shad_SetFixedPatternNoiseLine() sets parameters for the defective pixel interpolation.
 *
 * You can either manually set values for shading with this function or you can otherwise set a path to a file for
 * defective pixel interpolation. This can be done with Fg_setParameter() using FG_SHADING_FPNFILE0 parameter.
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Shad_SetFixedPatternNoiseLine(Fg_Struct *Fg, ShadingMaster *sh, int x, int channel, int on);

/**
 * \brief	write shading correction values for given line
 * \anchor  Shad_WriteActLine 
 *
 * \param	Fg	frame grabber to use
 * \param	sh	Shading control object to use.
 * \param	Line	Line number which has to be corrected.
 * \returns	Status code.
 * \retval	#FG_OK	Shading value was set.
 * \retval	#FG_INVALID_PARAMETER	an invalid argument was passed (e.g. a NULL pointer).
 *
 * The currently corrected line can be written with Shad_WriteActLine().
 *
 \verbatim
 This function can only be used with acquisition applets that offer shading functionality.
 VisualApplets generated applets with shading functionality do not use this interface. Please refer
 to the VisualApplets operator manual for documentation of that shading interface.\endverbatim
 */
int Shad_WriteActLine(Fg_Struct *Fg, ShadingMaster *sh, int Line);
/*@}*/

#ifdef __cplusplus

struct VAdevice;
class fpga_design;
struct RegisterInfo;

/**
 * \brief	internal initialization function
 * Do not try to use this function, this is for internal purpose only.
 *
 * \param appletName
 * \param hwhandle
 * \param deshandle
 */
Fg_Struct* Fg_InitHandle(const char *appletName, struct VAdevice *hwhandle, fpga_design *deshandle);

/**
 * \brief	internal initialization function
 * Do not try to use this function, this is for internal purpose only.
 *
 * \param appletName
 * \param hwhandle
 * \param deshandle
 * \param isSlave
 */
Fg_Struct* Fg_InitHandleSlaveAware(const char *appletName, struct VAdevice *hwhandle, fpga_design *deshandle, int isSlave);

/**
 * \brief	internal query function
 * Do not try to use this function, this is for internal purpose only.
 *
 * \param Fg
 * \param CamPort
 */
struct RegisterInfo *Fg_getParameterInfo(Fg_Struct *Fg, const unsigned int CamPort);

#endif /* __cplusplus */

/* Deprecated functions, do not use as they will eventually go away */

/**
 * \brief stub to maintain interface compatibility
 * \returns The last error code of the framegrabber library
 * \deprecated This function is only here to maintain interface compatibility. It will only call
 * Fg_getLastErrorNumber(NULL) internally. Do not use it as it will eventually go away.
 */
int getLastErrorNumber();

#ifdef __cplusplus
} /* extern "C" */

#endif

#endif /* __FGRAB_PROTOTYP_H */

/**
 * clser.h
 *
 * Copyright (c) 2002-2017 Silicon Software GmbH, All Rights Reserved.
 *
 *
 * \file clser.h
 *
 * \brief Silicon Software clser API
 *
 * \author Silicon Software GmbH
 */

 #ifndef _SISOCLSER
#define _SISOCLSER

#define CL_OK                           0
#define CL_ERR_NO_ERR                   0      /**< Function returned successfully. */
#define CL_ERR_BUFFER_TOO_SMALL         -10001 /**< User buffer not large enough to hold data. */
#define CL_ERR_MANU_DOES_NOT_EXIST      -10002 /**< The requested manufacturer's DLL does not exist on your system. */
#define CL_ERR_PORT_IN_USE              -10003 /**< Port is valid but cannot be opened because it is in use. */
#define CL_ERR_TIMEOUT                  -10004 /**< Operation not completed within specified timeout period. */
#define CL_ERR_INVALID_INDEX            -10005 /**< Not a valid index. */
#define CL_ERR_INVALID_REFERENCE        -10006 /**< The serial reference is not valid. */
#define CL_ERR_ERROR_NOT_FOUND          -10007 /**< Could not find the error description for this error code. */
#define CL_ERR_BAUD_RATE_NOT_SUPPORTED  -10008 /**< Requested baud rate not supported by this interface. */
#define CL_ERR_OUT_OF_MEMORY            -10009 /**< System is out of memory and could not perform required actions. */
#define CL_ERR_INVALID_ARG              -10097 /**< The function was called by using an invalid argument. */
#define CL_ERR_UNABLE_TO_LOAD_DLL       -10098 /**< The DLL was unable to load due to a lack of memory or because it does not export all required functions. */
#define CL_ERR_FUNCTION_NOT_FOUND       -10099 /**< Function does not exist in the manufacturer's library. */

#define CL_BAUDRATE_9600        1
#define CL_BAUDRATE_19200       2
#define CL_BAUDRATE_38400       4
#define CL_BAUDRATE_57600       8
#define CL_BAUDRATE_115200     16
#define CL_BAUDRATE_230400     32
#define CL_BAUDRATE_460800     64
#define CL_BAUDRATE_921600    128

#define CL_DLL_VERSION_NO_VERSION  1 /**< This library is not a valid Camera Link library */
#define CL_DLL_VERSION_1_0         2 /**< This Camera Link library conforms to the October 2000 version of the Camera Link Specifications */
#define CL_DLL_VERSION_1_1         3 /**< This Camera Link library conforms to the November 2002 version of the Camera Link Specifications */

#define SISO_CL_EXT_PORT_FEATURE_CONFIG                     1 /**< Set port configuration. The value is a combination of ORed SISO_CL_EXT_PORT_CONFIG_ flags */
#define SISO_CL_EXT_PORT_CONFIG_INVERT_TX              0x0001 /**< Invert the transmitted signal */
#define SISO_CL_EXT_PORT_CONFIG_INVERT_RX              0x0002 /**< Invert the received signal */
#define SISO_CL_EXT_PORT_CONFIG_PARITY_ON              0x0004 /**< Set parity on. If you set the parity with clSetParity, you don't need to include this flag. */
#define SISO_CL_EXT_PORT_CONFIG_LOOPBACK_MODE          0x0008 /**< Enable loopback mode */
#define SISO_CL_EXT_PORT_CONFIG_SETUP_TX_RISING_EDGE   0x0010 /**< Setup transmitted signal with the rising edge of the clock (USART mode) */
#define SISO_CL_EXT_PORT_CONFIG_SAMPLE_RX_FALLING_EDGE 0x0020 /**< Sample received signal with the falling edge of the clock (USART mode) */

#define SISO_CL_EXT_PORT_FEATURE_FIFO_DEPTH                 2 /**< Set the serial port FIFO depth in characters (default 0) */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief This function initializes the device referred to by serialIndex and returns a pointer to an internal serial reference structure.
 * \anchor clSerialInit
 *
 * The serial port will be initialized with the default settings.
 * The baudrate will be set to 9600, no parity and any advanced port
 * features will be set to the hardware defaults. If the serial port
 * supports both UART and USART mode, it will be set to UART mode.
 *
 * \param serialIndex A zero-based index value. For n serial devices in the system supported by this library, serialIndex has a range of 0 to (n-1).
 * \param serialRefPtr On a successful call, points to a value that contains a pointer to the vendor-specific reference to the current session.
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_PORT_IN_USE
 * \retval #CL_ERR_INVALID_INDEX
 * \retval #CL_ERR_INVALID_REFERENCE
 */
int clSerialInit(unsigned int serialIndex, void **serialRefPtr);

/**
 * \brief This function reads numBytes from the serial device referred to by serialRef.
 * \anchor clSerialRead
 *
 * clSerialRead() will return when numBytes are available at the serial port or when the
 * serialTimeout period has passed. Upon success, numBytes are copied into buffer. In the
 * case of any error, including #CL_ERR_TIMEOUT, no data is copied into buffer.
 * 
 * \param serialRef The value obtained from the clSerialInit() function.
 * \param buffer Points to a user-allocated buffer. Upon a successful call, buffer contains the
 *               data read from the serial device. Upon failure, this buffer is not affected.
 *               Caller should ensure that buffer is at least numBytes in size.
 * \param numBytes The number of bytes requested by the caller.
 * \param serialTimeout Indicates the timeout in milliseconds.
 * 
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_INVALID_REFERENCE
 * \retval #CL_ERR_TIMEOUT
 */
int clSerialRead(void *serialRef, char *buffer, unsigned int *numBytes, unsigned int serialTimeout);

/**
 * \brief This function writes the data in the buffer to the serial device referenced by serialRef.
 * \anchor clSerialWrite
 *
 * \param serialRef The value obtained from the clSerialInit() function.
 * \param buffer Contains data to write to the serial port (see note below).
 * \param bufferSize Contains the buffer size indicating the maximum number of bytes to be
 *                   written. Upon a successful call, bufferSize contains the number of
 *                   bytes written to the serial device.
 * \param serialTimeout Indicates the timeout in milliseconds.
 * 
 * \note The type of the input buffer is defined to be "char *" by the CameraLink specification.
 *       The Silicon Software implementation guarantees to never modify this data, in fact it
 *       will cast the input pointer to a "const char *" immediately. So it is fine to pass in
 *       arbitrary constant data that is casted to "char *" to satisfy this interface.
 * 
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_INVALID_REFERENCE
 * \retval #CL_ERR_TIMEOUT
 */
int clSerialWrite(void *serialRef, char *buffer, unsigned int *bufferSize, unsigned int serialTimeout);

/**
 * \brief This function closes the serial device and cleans up the resources associated with serialRef.
 * \anchor clSerialClose
 *
 * Upon return, serialRef is no longer usable.
 * 
 * \param serialRef The value obtained from the clSerialInit() function for clean-up.
 */
void clSerialClose(void *serialRef);

/**
 * \brief This function returns the name of the frame grabber manufacturer who created
 *        the DLL and the version of the Camera Link specifications with which the DLL complies.
 * \anchor clGetManufacturerInfo
 *
 * \param manufacturerName A pointer to a user-allocated buffer into which the function
 *                         copies the manufacturer name. The returned name is NULL-terminated.
 * \param bufferSize As an input, this value should be the size of the buffer that is passed.
 *                   On successful return, this parameter contains the number of bytes written
 *                   into the buffer, including the NULL termination character. On
 *                   #CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size of the buffer
 *                   needed to write the data text.
 * \param version A constant stating the version of the Camera Link specifications with which this DLL complies.
 * \retval #CL_ERR_BUFFER_TOO_SMALL manufacturerName is too small to contain the manufacturer name,
 *                                  it needs to be scaled to the value written into bufferSize.
 * \retval #CL_ERR_NO_ERR
 */
int clGetManufacturerInfo(char *manufacturerName, unsigned int *bufferSize, unsigned int *version);

/**
 * \brief This function returns the number of serial ports in your system from a specific manufacturer.
 * \anchor clGetNumSerialPorts
 *
 * \param numSerialPorts The number of serial ports in your system that you can access with the current DLL.
 * \retval CL_ERR_NO_ERR
 */
int clGetNumSerialPorts(unsigned int *numSerialPorts);

/**
 * \brief This function returns a manufacturer-specific identifier for each serial port in your system.
 * \anchor clGetSerialPortIdentifier
 *
 * This returns a string which contains the board and port indexes that are used by
 * Silicon Software framegrabber SDK (e.g. function Fg_InitEx()).
 *
 * \param serialIndex A zero-based index value. The valid range for serialIndex is 0 to (n–1),
 *                    where n is the value of numSerialPorts, as returned by clGetNumSerialPorts().
 * \param portID Manufacturer-specific identifier for the serial port
 * \param bufferSize As an input, this value should be the size of the buffer that is passed.
 *                   On successful return, this parameter contains the number of bytes written
 *                   into the buffer, including the NULL termination character.
 *                   On #CL_ERR_BUFFER_TOO_SMALL, this parameter contains the size of the buffer needed
 *                   to write the data text.
 *
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_INVALID_INDEX
 * \retval #CL_ERR_BUFFER_TOO_SMALL portID is too small to contain the port identification string,
 *                                  it needs to be scaled to the value written into bufferSize.
 * 
 * Example code to print out the port identifier of port with index "i":
 * \code
 * char *buf = NULL;
 * unsigned int buflen = 0;
 * if (clGetSerialPortIdentifier(i, buf, &buflen) != CL_ERR_BUFFER_TOO_SMALL)
 *     return;
 * buf = malloc(buflen);
 * if (clGetSerialPortIdentifier(i, buf, &buflen) == CL_ERR_NO_ERR)
 *     printf("port %i is identified as %s\n", i, buf);
 * free(buf);
 * \endcode
 */
int clGetSerialPortIdentifier(unsigned int serialIndex,char* portID,unsigned int* bufferSize);

/**
 * \brief This function outputs the number of bytes that are received at the port specified by serialRef but are not yet read out.
 * \anchor clGetNumBytesAvail
 *
 * \param serialRef The value obtained by the clSerialInit() function.
 * \param numBytes The number of bytes currently available to be read from the port.
 *
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_INVALID_REFERENCE
 */
int clGetNumBytesAvail(void *serialRef, unsigned int *numBytes);

/**
 * \brief This function discards any bytes that are available in the input buffer.
 * \anchor clFlushPort
 *
 * \param serialRef The value obtained by the clSerialInit() function that describes the port to be flushed.
 */
int clFlushPort(void* serialRef);

/**
 * \brief This function returns the valid baud rates of the current interface.
 * \anchor clGetSupportedBaudRates
 *
 * \param serialRef The value obtained from the clSerialInit() function, which describes the port
 *                  being queried for baud rates.
 * \param baudRates Bitfield that describes all supported baud rates of the serial port as described
 *                  by serialRefPtr.
 *
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_INVALID_REFERENCE
 */
int clGetSupportedBaudRates(void *serialRef, unsigned int *baudRates);

/**
 * \brief This function sets the baud rate for the serial port of the selected device.
 * \anchor clSetBaudRate
 *
 * Use clGetSupportedBaudRates() to determine supported baud rates.
 * The serial port will be initialized with the default settings.
 * The parity will be disabled and any advanced port features will be set
 * to the hardware defaults.
 * 
 * \param serialRef The value obtained from the clSerialInit() function.
 * \param baudRate The baud rate you want to use. This parameter expects the values represented by the
 *                 CL_BAUDRATE constants.
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_INVALID_REFERENCE
 * \retval #CL_ERR_BAUD_RATE_NOT_SUPPORTED
 */
int clSetBaudRate(void *serialRef, unsigned int baudRate);

/**
 * \brief This function sets the parity  for the serial port of the selected device.
 * \anchor clSetParity
 * 
 * Any advanced port settings will be reset to the hardware defaults.
 *
 * \param serialRef The value obtained from the clSerialInit() function.
 * \param parityOn The parity that shall be used ( 0: parity off, 1: even parity)
 *
 * \retval #CL_ERR_NO_ERR
 * \retval #CL_ERR_INVALID_REFERENCE
 */
int clSetParity(void *serialRef, unsigned int parityOn);


/**
 * \brief This function allows to activate different enhanced settings.
 * \anchor clSetPortFeature
 * 
 * The features are specific to certain hardware and frame grabber types.
 * The function is reserved for future releases of Silicon Software 
 * hardware. Each bit at the bit mask represents a certain switch. 
 * 
 * \param serialRef The value obtained from the clSerialInit() function.
 * \param feature identifier for a certain feature
 * \param value value to be set for the feature
 *
 * \retval #CL_ERR_NO_ERR successful function call
 * \retval #CL_ERR_INVALID_REFERENCE invalid serial handle
 * \retval #CL_ERR_FUNCTION_NOT_FOUND invalid port feature or feature not supported
 * \retval #CL_ERR_INVALID_ARG invalid value for port feature
 */
int clSetPortFeature(void *serialRef, unsigned int feature, unsigned int value);


/**
 * \brief This function allows to enable asymmetric RTS/CTS hardware flow control.
 * \anchor clSetFlowControlMode
 * 
 * Some serial ports support standard asymmetric RTS/CTS hardware flow control.
 * Since RTS and CTS signals require additional connections, two UART modules
 * have to be combined. The primary port is used for communication, while
 * the secondary port will be disabled when hardware flow control is used.
 * Please see device documentation for support of hardware flow control,
 * and details which UART modules must be combined.
 * 
 * \param serialRef Primary port handle obtained from the clSerialInit() function.
 * \param secondRef Secondary port handle obtained from the clSerialInit() function.
 * \param flowControl 0: turn flow control off, 0x01 use hardware flow control
 *
 * \retval #CL_ERR_NO_ERR successful function call
 * \retval #CL_ERR_INVALID_REFERENCE invalid serial handle
 * \retval #CL_ERR_FUNCTION_NOT_FOUND primary port does not support USART mode
 * \retval #CL_ERR_INVALID_INDEX secondary port does not match primary port
 */
int clSetFlowControlMode(void *serialRef, void *secondRef, unsigned int flowControl);


/**
 * \brief This function converts an error code to error text for display in a dialog box or in a standard I/O window.
 * \anchor clGetErrorText
 *
 * \param errorCode The error code used to find the appropriate error text. An error code is returned by every
 *                  function in this library.
 * \param errorText A caller-allocated buffer which contains the NULL-terminated error text on function return.
 * \param errorTextSize On success, contains the number of bytes written into the buffer, including the
 *                      NULL-termination character. This value should be the size in bytes of the error text
 *                      buffer passed in. On #CL_ERR_BUFFER_TOO_SMALL, contains the size of the buffer needed to
 *                      write the error text.
 * \retval #CL_ERR_NO_ERR the error text has been written into errorText
 * \retval #CL_ERR_BUFFER_TOO_SMALL errorText is too small to contain the error message, it needs to be scaled
 *                                  to the value written into errorTextSize
 * \retval #CL_ERR_ERROR_NOT_FOUND
 */
int clGetErrorText(int errorCode,char *errorText,unsigned int* errorTextSize);

#ifdef __cplusplus
}
#endif

#endif

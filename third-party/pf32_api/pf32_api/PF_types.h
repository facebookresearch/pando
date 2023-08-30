/*! \file
*  \brief Header file defining types and constants used in the Photon Force C API.
*/
#pragma once

typedef void* PF32_HANDLE;

enum LOGLEVEL
{
   LOGLEVEL_TRACE = 1,  //!< Low level execution tracing information.
   LOGLEVEL_DEBUG = 2,  //!< Debugging information
   LOGLEVEL_INFO  = 3,  //!< Status information.
   LOGLEVEL_WARNING  = 4,  //!< Warnings.
   LOGLEVEL_ERROR = 5,  //!< Critical errors.
   LOGLEVEL_OFF   = 6   //!< Error logging disabled.
};


/** \brief PF32 camera connection status.
 */
typedef enum
{
   disconnected = 0,         /**< \brief   No camera connected.
                               *  \details When a camera is physically powered off or disconnected, the connection status will automatically move to the disconnected state.
                               */
   connected_pre_init = 1,   /**< \brief   Camera connected, but not yet initialised.
                               *  \details When a camera is physically powered on and connected, the connection status will automatically move to the connected_pre_init state.
                               *  \note Note that in this case, firmware and PLL settings may not yet have been loaded.
                               *  All comms are expected to fail.
                               *  To move to the ready state, call \ref PhotonForce::loadCustomFirmware "loadCustomFirmware()".
                               */
   ready = 2,                /**< \brief   Camera connected, initialised, and ready for operation.
                               */
   error = 3                 /**< \brief   Cannot continue without human intervention, e.g. invalid firmware path provided. See error log for details.
                               */
} PF32_conn_status;

/** \brief PF32 camera data acquisition source - specifies test data or real sensor data.
 */
typedef enum
{
   sensor_data = 0,          //!< Real sensor image data.
   test_data = 1             //!< Test data for software debugging.
} PF32_data_source;

/** \brief PF32 camera operating mode
 */
typedef enum
{
   photon_counting = 0,      //!< Photon counting mode
   TCSPC_laser_master = 1,   //!< Camera in time-resolved mode, configured to accept external laser SYNC input as TDC stop signal.
   TCSPC_sys_master = 2,     //!< Camera in time-resolved mode, configured to generate TRIG output and TDC stop signal.
   raw_SPAD = 10,            /**< \brief   Raw SPAD output mode.
                               *  \details One row of pixel SPAD signals are directly connected to each column outputs. 
                               *  \warning Consult manual before using.
                               */
   test_pulse_counting = 11, //!< Counting electrical test pulses (TESTSTART signal)
   test_data_1 = 20,         //!< Readout test pattern #1.
   test_data_2 = 21          //!< Readout test pattern #2. Stats at pixel 0,0 in the top left, increasing horizontally along the row and then vertically downwards. With each frame, all values increment by 1, wrapping around at 1023.
} PF32_mode;


/*! \brief Whether the bit file is 1, 2, 4, 8 or 10 bit.
 *
 *
 */
typedef enum
{
   OneBit = 0,
   TwoBit = 1,
   FourBit = 2,
   EightBit = 3,
   TenBit = 4
} BitFile_type;
 

/*! \brief Readout mode.
 *
 * 
 */
typedef enum
{
   raw_frames = 0,
   histogramming_only = 1
} readout_type;

/*! \brief Used for DAC comms methods.
 * Coupled with pf32\PF32_API.java so assigned values have to match.
 */
typedef enum
{
   VBD_DAC = 0,              //!< Breakdown voltage DAC.
   VEB_DAC = 1,              //!< Excess bias voltage DAC.
   VQ_DAC = 2,               //!< Quench voltage DAC.
   VNBL_DAC = 3,             //!< TDC resolution control voltage DAC.
   VIO_DAC = 4,              //!< 
   VTDC_DAC = 5              //!< 
} PF32_DAC_type;


const unsigned int MAX_SERIAL_NUMBER_LENGTH = 32;
const unsigned int MAX_MODEL_NUMBER_LENGTH = 32;
const unsigned int NO_OF_BYTES_PER_POSITIONAL_FOOTER = 16;
const int LOWEST_OK_ERROR_CODE = -20;

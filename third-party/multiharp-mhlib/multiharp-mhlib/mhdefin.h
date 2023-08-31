
/* 
    MHLib programming library for MultiHarp 150
    PicoQuant GmbH 

    Ver. 2.0.0.0     May 2020
*/


#define LIB_VERSION "2.0"	// library version

#define MAXDEVNUM   8       // max number of USB devices
 
#define MAXINPCHAN  16      // max number of physicl input channels

#define BINSTEPSMAX 23      // max number of binning steps, 
                            // get actual number via MH_GetBaseResolution()

#define MAXHISTLEN  65536   // max number of histogram bins

#define TTREADMAX  1048576  // number of event records that can be read by MH_ReadFiFo
                            // buffer must provide space for this number of dwords

//symbolic constants for MH_Initialize
#define REFSRC_INTERNAL			 0		 // use internal clock
#define REFSRC_EXTERNAL_10MHZ    1       // use 10MHz external clock
#define REFSRC_WR_MASTER_GENERIC 2       // White Rabbit master with generic partner
#define REFSRC_WR_SLAVE_GENERIC  3       // White Rabbit slave with generic partner
#define REFSRC_WR_GRANDM_GENERIC 4       // White Rabbit grand master with generic partner
#define REFSRC_EXTN_GPS_PPS      5       // use 10 MHz + PPS from GPS
#define REFSRC_EXTN_GPS_PPS_UART 6       // use 10 MHz + PPS + time via UART from GPS
#define REFSRC_WR_MASTER_MHARP   7       // White Rabbit master with MultiHarp as partner
#define REFSRC_WR_SLAVE_MHARP    8       // White Rabbit slave with MultiHarp as partner
#define REFSRC_WR_GRANDM_MHARP   9       // White Rabbit grand master with MultiHarp as partner

//symbolic constants for MH_Initialize
#define MODE_HIST       0
#define MODE_T2         2
#define MODE_T3         3

//symbolic constants for MH_SetMeasControl
#define MEASCTRL_SINGLESHOT_CTC            0 //default
#define MEASCTRL_C1_GATED                  1
#define MEASCTRL_C1_START_CTC_STOP         2
#define MEASCTRL_C1_START_C2_STOP          3
#define MEASCTRL_WR_M2S                    4
#define MEASCTRL_WR_S2M                    5

//symb. const. for MH_SetMeasControl, MH_SetSyncEdgeTrg and MH_SetInputEdgeTrg
#define EDGE_RISING   1
#define EDGE_FALLING  0

//bitmasks for results from MH_GetFeatures
#define FEATURE_DLL       0x0001  // DLL License available
#define FEATURE_TTTR      0x0002  // TTTR mode available
#define FEATURE_MARKERS   0x0004  // Markers available
#define FEATURE_LOWRES    0x0008  // Long range mode available 
#define FEATURE_TRIGOUT   0x0010  // Trigger output available
#define FEATURE_PROG_TD   0x0020  // Programmable deadtime available

//bitmasks for results from MH_GetFlags
#define FLAG_OVERFLOW     0x0001  // histo mode only
#define FLAG_FIFOFULL     0x0002  // TTTR mode only
#define FLAG_SYNC_LOST    0x0004  
#define FLAG_REF_LOST     0x0008  
#define FLAG_SYSERROR     0x0010  // hardware error, must contact support
#define FLAG_ACTIVE       0x0020  // measurement is running
#define FLAG_CNTS_DROPPED 0x0040  // counts were dropped

//limits for MH_SetHistoLen
//note: length codes 0 and 1 will not work with MH_GetHistogram
//if you need these short lengths then use MH_GetAllHistograms
#define MINLENCODE  0	
#define MAXLENCODE  6		//default

//limits for MH_SetSyncDiv
#define SYNCDIVMIN          1
#define SYNCDIVMAX         16

//limits for MH_SetSyncEdgeTrg and MH_SetInputEdgeTrg
#define TRGLVLMIN       -1200     // mV
#define TRGLVLMAX        1200     // mV

//limits for MH_SetSyncChannelOffset and MH_SetInputChannelOffset
#define CHANOFFSMIN    -99999     // ps
#define CHANOFFSMAX     99999     // ps

//limits for MH_SetSyncDeadTime and MH_SetInputDeadTime
#define EXTDEADMIN        800     // ps
#define EXTDEADMAX     160000     // ps

//limits for MH_SetOffset
#define OFFSETMIN           0     // ns
#define OFFSETMAX   100000000     // ns

//limits for MH_StartMeas
#define ACQTMIN             1     // ms
#define ACQTMAX     360000000     // ms  (100*60*60*1000ms = 100h)

//limits for MH_SetStopOverflow
#define STOPCNTMIN          1
#define STOPCNTMAX 4294967295     // 32 bit is mem max

//limits for MH_SetTriggerOutput
#define TRIGOUTMIN          0	  // 0=off
#define TRIGOUTMAX   16777215     // in units of 100ns

//limits for MH_SetMarkerHoldoffTime
#define HOLDOFFMIN          0     // ns
#define HOLDOFFMAX      25500     // ns


//The following are bitmasks for results from GetWarnings()

#define WARNING_SYNC_RATE_ZERO				0x0001
#define WARNING_SYNC_RATE_VERY_LOW			0x0002
#define WARNING_SYNC_RATE_TOO_HIGH			0x0004
#define WARNING_INPT_RATE_ZERO				0x0010
#define WARNING_INPT_RATE_TOO_HIGH			0x0040
#define WARNING_INPT_RATE_RATIO				0x0100
#define WARNING_DIVIDER_GREATER_ONE			0x0200
#define WARNING_TIME_SPAN_TOO_SMALL			0x0400
#define WARNING_OFFSET_UNNECESSARY			0x0800
#define WARNING_DIVIDER_TOO_SMALL			0x1000
#define WARNING_COUNTS_DROPPED				0x2000

//The following is only for use with White Rabbit

#define WR_STATUS_LINK_ON               0x00000001  // WR link is switched on
#define WR_STATUS_LINK_UP               0x00000002  // WR link is established

#define WR_STATUS_MODE_BITMASK          0x0000000C  // mask for the mode bits
#define WR_STATUS_MODE_OFF              0x00000000  // mode is "off"
#define WR_STATUS_MODE_SLAVE            0x00000004  // mode is "slave"
#define WR_STATUS_MODE_MASTER           0x00000008  // mode is "master" 
#define WR_STATUS_MODE_GMASTER          0x0000000C  // mode is "grandmaster"

#define WR_STATUS_LOCKED_CALIBD         0x00000010  // locked and calibrated

#define WR_STATUS_PTP_BITMASK           0x000000E0  // mask for the PTP bits
#define WR_STATUS_PTP_LISTENING         0x00000020
#define WR_STATUS_PTP_UNCLWRSLCK        0x00000040
#define WR_STATUS_PTP_SLAVE             0x00000060
#define WR_STATUS_PTP_MSTRWRMLCK        0x00000080
#define WR_STATUS_PTP_MASTER            0x000000A0

#define WR_STATUS_SERVO_BITMASK         0x00000700  // mask for the servo bits
#define WR_STATUS_SERVO_UNINITLZD       0x00000100  //
#define WR_STATUS_SERVO_SYNC_SEC        0x00000200  //
#define WR_STATUS_SERVO_SYNC_NSEC       0x00000300  //
#define WR_STATUS_SERVO_SYNC_PHASE      0x00000400  //
#define WR_STATUS_SERVO_WAIT_OFFST      0x00000500  //
#define WR_STATUS_SERVO_TRCK_PHASE      0x00000600  //

#define WR_STATUS_MAC_SET               0x00000800  // user defined mac address is set
#define WR_STATUS_IS_NEW                0x80000000  // status updated since last check


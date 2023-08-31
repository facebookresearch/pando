/* 
    MHLib programming library for MultiHarp 150
    PicoQuant GmbH 

    Ver. 2.0.0.0     May 2020
*/

#define MH_ERROR_NONE                                      0 
									
#define MH_ERROR_DEVICE_OPEN_FAIL                         -1 
#define MH_ERROR_DEVICE_BUSY                              -2 
#define MH_ERROR_DEVICE_HEVENT_FAIL                       -3 
#define MH_ERROR_DEVICE_CALLBSET_FAIL                     -4 
#define MH_ERROR_DEVICE_BARMAP_FAIL                       -5 
#define MH_ERROR_DEVICE_CLOSE_FAIL                        -6 
#define MH_ERROR_DEVICE_RESET_FAIL                        -7 
#define MH_ERROR_DEVICE_GETVERSION_FAIL                   -8 
#define MH_ERROR_DEVICE_VERSION_MISMATCH                  -9 
#define MH_ERROR_DEVICE_NOT_OPEN                         -10
#define MH_ERROR_DEVICE_LOCKED                           -11
#define MH_ERROR_DEVICE_DRIVERVER_MISMATCH               -12
									
#define MH_ERROR_INSTANCE_RUNNING                        -16 
#define MH_ERROR_INVALID_ARGUMENT                        -17 
#define MH_ERROR_INVALID_MODE                            -18 
#define MH_ERROR_INVALID_OPTION                          -19 
#define MH_ERROR_INVALID_MEMORY                          -20 
#define MH_ERROR_INVALID_RDATA                           -21 
#define MH_ERROR_NOT_INITIALIZED                         -22 
#define MH_ERROR_NOT_CALIBRATED                          -23 
#define MH_ERROR_DMA_FAIL                                -24 
#define MH_ERROR_XTDEVICE_FAIL                           -25 
#define MH_ERROR_FPGACONF_FAIL                           -26 
#define MH_ERROR_IFCONF_FAIL                             -27 
#define MH_ERROR_FIFORESET_FAIL                          -28 
#define MH_ERROR_THREADSTATE_FAIL                        -29 
#define MH_ERROR_THREADLOCK_FAIL                         -30 
									
#define MH_ERROR_USB_GETDRIVERVER_FAIL                   -32 
#define MH_ERROR_USB_DRIVERVER_MISMATCH                  -33 
#define MH_ERROR_USB_GETIFINFO_FAIL                      -34 
#define MH_ERROR_USB_HISPEED_FAIL                        -35 
#define MH_ERROR_USB_VCMD_FAIL                           -36 
#define MH_ERROR_USB_BULKRD_FAIL                         -37 
#define MH_ERROR_USB_RESET_FAIL                          -38 

#define MH_ERROR_LANEUP_TIMEOUT                          -40 
#define MH_ERROR_DONEALL_TIMEOUT                         -41 
#define MH_ERROR_MB_ACK_TIMEOUT                          -42 
#define MH_ERROR_MACTIVE_TIMEOUT                         -43 
#define MH_ERROR_MEMCLEAR_FAIL                           -44 
#define MH_ERROR_MEMTEST_FAIL                            -45 
#define MH_ERROR_CALIB_FAIL                              -46 
#define MH_ERROR_REFSEL_FAIL                             -47 
#define MH_ERROR_STATUS_FAIL                             -48 	
#define MH_ERROR_MODNUM_FAIL                             -49 
#define MH_ERROR_DIGMUX_FAIL                             -50	
#define MH_ERROR_MODMUX_FAIL                             -51 
#define MH_ERROR_MODFWPCB_MISMATCH                       -52 	
#define MH_ERROR_MODFWVER_MISMATCH                       -53 
#define MH_ERROR_MODPROPERTY_MISMATCH                    -54 	
#define MH_ERROR_INVALID_MAGIC                           -55  
#define MH_ERROR_INVALID_LENGTH                          -56	
#define MH_ERROR_RATE_FAIL                               -57  	
#define MH_ERROR_MODFWVER_TOO_LOW                        -58
#define MH_ERROR_MODFWVER_TOO_HIGH                       -59
#define MH_ERROR_MB_ACK_FAIL                             -60

#define MH_ERROR_EEPROM_F01                              -64 
#define MH_ERROR_EEPROM_F02                              -65 
#define MH_ERROR_EEPROM_F03                              -66 
#define MH_ERROR_EEPROM_F04                              -67 
#define MH_ERROR_EEPROM_F05                              -68 
#define MH_ERROR_EEPROM_F06                              -69 
#define MH_ERROR_EEPROM_F07                              -70 
#define MH_ERROR_EEPROM_F08                              -71 
#define MH_ERROR_EEPROM_F09                              -72 
#define MH_ERROR_EEPROM_F10                              -73 
#define MH_ERROR_EEPROM_F11                              -74 
#define MH_ERROR_EEPROM_F12                              -75 
#define MH_ERROR_EEPROM_F13                              -76
#define MH_ERROR_EEPROM_F14                              -77
#define MH_ERROR_EEPROM_F15                              -78

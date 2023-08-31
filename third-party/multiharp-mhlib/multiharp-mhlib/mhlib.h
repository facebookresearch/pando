/* 
    MHLib programming library for MultiHarp 150
    PicoQuant GmbH 

    Ver. 2.0.0.0     May 2020
*/

#ifndef _WIN32
#define _stdcall
#endif

extern int _stdcall MH_GetLibraryVersion(char* vers);
extern int _stdcall MH_GetErrorString(char* errstring, int errcode);

extern int _stdcall MH_OpenDevice(int devidx, char* serial); 
extern int _stdcall MH_CloseDevice(int devidx);  
extern int _stdcall MH_Initialize(int devidx, int mode, int refsource);

//all functions below can only be used after MH_Initialize

extern int _stdcall MH_GetHardwareInfo(int devidx, char* model, char* partno, char* version); 
extern int _stdcall MH_GetSerialNumber(int devidx, char* serial);
extern int _stdcall MH_GetFeatures(int devidx, int* features);                                
extern int _stdcall MH_GetBaseResolution(int devidx, double* resolution, int* binsteps);
extern int _stdcall MH_GetNumOfInputChannels(int devidx, int* nchannels);

extern int _stdcall MH_SetSyncDiv(int devidx, int div);
extern int _stdcall MH_SetSyncEdgeTrg(int devidx, int level, int edge);
extern int _stdcall MH_SetSyncChannelOffset(int devidx, int value);
extern int _stdcall MH_SetSyncDeadTime(int devidx, int on, int deadtime);  //new since v1.1

extern int _stdcall MH_SetInputEdgeTrg(int devidx, int channel, int level, int edge);
extern int _stdcall MH_SetInputChannelOffset(int devidx, int channel, int value);
extern int _stdcall MH_SetInputDeadTime(int devidx, int channel, int on, int deadtime);  //new since v1.1
extern int _stdcall MH_SetInputChannelEnable(int devidx, int channel, int enable);

extern int _stdcall MH_SetStopOverflow(int devidx, int stop_ovfl, unsigned int stopcount);	
extern int _stdcall MH_SetBinning(int devidx, int binning);
extern int _stdcall MH_SetOffset(int devidx, int offset);
extern int _stdcall MH_SetHistoLen(int devidx, int lencode, int* actuallen); 
extern int _stdcall MH_SetMeasControl(int devidx, int control, int startedge, int stopedge);
extern int _stdcall MH_SetTriggerOutput(int devidx, int period);

extern int _stdcall MH_ClearHistMem(int devidx);
extern int _stdcall MH_StartMeas(int devidx, int tacq);
extern int _stdcall MH_StopMeas(int devidx);
extern int _stdcall MH_CTCStatus(int devidx, int* ctcstatus);

extern int _stdcall MH_GetHistogram(int devidx, unsigned int *chcount, int channel);
extern int _stdcall MH_GetAllHistograms(int devidx, unsigned int *chcount);
extern int _stdcall MH_GetResolution(int devidx, double* resolution); 
extern int _stdcall MH_GetSyncPeriod(int devidx, double* period);
extern int _stdcall MH_GetSyncRate(int devidx, int* syncrate);
extern int _stdcall MH_GetCountRate(int devidx, int channel, int* cntrate);
extern int _stdcall MH_GetAllCountRates(int devidx, int* syncrate, int* cntrates); 
extern int _stdcall MH_GetFlags(int devidx, int* flags);
extern int _stdcall MH_GetElapsedMeasTime(int devidx, double* elapsed);
extern int _stdcall MH_GetStartTime(int devidx, unsigned int* timedw2, unsigned int* timedw1, unsigned int* timedw0);

extern int _stdcall MH_GetWarnings(int devidx, int* warnings);
extern int _stdcall MH_GetWarningsText(int devidx, char* text, int warnings);

//for the time tagging modes only
extern int _stdcall MH_SetMarkerHoldoffTime(int devidx, int holdofftime);
extern int _stdcall MH_SetMarkerEdges(int devidx, int me1, int me2, int me3, int me4);
extern int _stdcall MH_SetMarkerEnable(int devidx, int en1, int en2, int en3, int en4);
extern int _stdcall MH_ReadFiFo(int devidx, unsigned int* buffer, int* nactual);

//for debugging only
extern int _stdcall MH_GetDebugInfo(int devidx, char *debuginfo);                   
extern int _stdcall MH_GetNumOfModules(int devidx, int* nummod);
extern int _stdcall MH_GetModuleInfo(int devidx, int modidx, int* modelcode, int* versioncode);

//for White Rabbit only
extern int _stdcall MH_WRabbitGetMAC(int devidx, char* mac_addr);
extern int _stdcall MH_WRabbitSetMAC(int devidx, char* mac_addr);
extern int _stdcall MH_WRabbitGetInitScript(int devidx, char* initscript);
extern int _stdcall MH_WRabbitSetInitScript(int devidx, char* initscript);
extern int _stdcall MH_WRabbitGetSFPData(int devidx, char* sfpnames, int* dTxs, int* dRxs, int* alphas);
extern int _stdcall MH_WRabbitSetSFPData(int devidx, char* sfpnames, int* dTxs, int* dRxs, int* alphas);
extern int _stdcall MH_WRabbitInitLink(int devidx, int link_on);
extern int _stdcall MH_WRabbitSetMode(int devidx, int bootfromscript, int reinit_with_mode, int mode);
extern int _stdcall MH_WRabbitSetTime(int devidx, unsigned int timehidw, unsigned int timelodw);
extern int _stdcall MH_WRabbitGetTime(int devidx, unsigned int* timehidw, unsigned int* timelodw, unsigned int* subsec16ns);
extern int _stdcall MH_WRabbitGetStatus(int devidx, int* wrstatus);
extern int _stdcall MH_WRabbitGetTermOutput(int devidx, char* buffer, int* nchar);


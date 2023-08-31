/*! \file
*  \brief Header file for Photon Force correlator C API.
*/

#pragma once


#ifdef _WIN32
#ifdef PF32_API_LOCAL
#define MS_DECL_SPEC
#else
#ifdef PF32_API_EXPORTS
#define MS_DECL_SPEC __declspec(dllexport)
#else
#define MS_DECL_SPEC __declspec(dllimport)    
#endif    
#endif
#else
#define MS_DECL_SPEC
#endif

#include "PF32_API.h"
#include "PF_types.h"


#ifdef __cplusplus
extern "C" 
{
#endif
   const unsigned int SIZE_OF_CORRELATOR_FOOTER_BYTES = 1024;
   const unsigned int SIZE_OF_CORRELATOR_INTENSITY_MAP_BYTES = 4096;

   MS_DECL_SPEC void setTintFrames(PF32_HANDLE hnd, unsigned long tintFrames);
   MS_DECL_SPEC void enableCorrelator(PF32_HANDLE hnd, bool enable, bool testMode);
   MS_DECL_SPEC bool readFromCorrelator(PF32_HANDLE hnd, uint8_t * raw, uint8_t * footer, uint8_t * intensityMap);
   MS_DECL_SPEC unsigned int getSizeOfCorrelatorData(PF32_HANDLE hnd);
   MS_DECL_SPEC unsigned int getNoOfTauValuesPerPixel(PF32_HANDLE hnd);
   MS_DECL_SPEC void convertCorrelatorOutput(PF32_HANDLE hnd, uint8_t * raw, double * converted, unsigned int enabledHeight, unsigned int width);
   MS_DECL_SPEC const char * outputCorrelatorStatus(PF32_HANDLE hnd);
   MS_DECL_SPEC bool getCorrelatorUnlocked(PF32_HANDLE hnd);
   MS_DECL_SPEC void setSpacing(PF32_HANDLE hnd, unsigned int spacing, int rebinningPoint); // rebinningPoint starts from 0
   MS_DECL_SPEC unsigned int * getSpacing(PF32_HANDLE hnd); // Client is responsible for delete [] returned pointer afterwards.
   MS_DECL_SPEC void enableIntensityMap(PF32_HANDLE hnd, bool enable);
   MS_DECL_SPEC bool getWhetherIntensityMapIsEnabled(PF32_HANDLE hnd);
   MS_DECL_SPEC bool getWhetherSimplifiedNormalisation(PF32_HANDLE hnd);
   MS_DECL_SPEC void resetCorrelator(PF32_HANDLE hnd);
   MS_DECL_SPEC double getFPGATemp(PF32_HANDLE hnd);

#ifdef __cplusplus
} /* end extern "C" */
#endif

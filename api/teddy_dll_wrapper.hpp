/* DLL wrapper for teddy
 * 
 * Copyright (C) u-blox Melbourn Ltd
 * u-blox Melbourn Ltd, Melbourn, UK
 * 
 * All rights reserved.
 *
 * This source file is the sole property of u-blox Melbourn Ltd.
 * Reproduction or utilization of this source in whole or part is
 * forbidden without the written consent of u-blox Melbourn Ltd.
 */

#ifndef TEDDY_DLL_WRAPPER_HPP
#define TEDDY_DLL_WRAPPER_HPP

/**
 * @file teddy_wrapper.h
 * This file defines the encapsulation necessary to
 * call the CPP Message Handling functions from C#.
 */
 
#ifdef __cplusplus
extern "C" {
#endif

    #define DLL __declspec(dllexport)

    DLL uint32_t __cdecl maxDatagramSizeRaw (void);
    DLL uint32_t __cdecl maxDebugStringSize (void);
    DLL uint32_t __cdecl revisionLevel (void);
    DLL uint32_t __cdecl encodeRebootReqDlMsg (char * pBuffer,
                                               bool devModeOnNotOff);
    DLL uint32_t __cdecl encodeIntervalsGetReqDlMsg (char * pBuffer);
    DLL uint32_t __cdecl encodeReportingIntervalSetReqDlMsg (char * pBuffer,
                                                             uint32_t reportingIntervalMinutes);
    DLL uint32_t __cdecl encodeHeartbeatSetReqDlMsg (char * pBuffer,
                                                           uint32_t heartbeatSeconds);
    DLL uint32_t __cdecl encodeSensorsReportGetReqDlMsg (char * pBuffer);
    DLL uint32_t __cdecl decodeUlMsgType (const char * pInBuffer,
                                          uint32_t sizeInBuffer);
    DLL bool __cdecl decodeUlMsgInitInd (const char ** ppInBuffer,
                                         uint32_t sizeInBuffer,
                                         uint32_t * pWakeUpCode,
                                         uint32_t * pRevision);
    DLL bool __cdecl decodeUlMsgIntervalsGetCnf (const char ** ppInBuffer,
                                                 uint32_t sizeInBuffer,
                                                 uint32_t * pReportingIntervalMinutes,
                                                 uint32_t * pHeartbeatSeconds);
    DLL bool __cdecl decodeUlMsgReportingIntervalSetCnf (const char ** ppInBuffer,
                                                         uint32_t sizeInBuffer,
                                                         uint32_t * pReportingIntervalMinutes);
    DLL bool __cdecl decodeUlMsgHeartbeatSetCnf (const char ** ppInBuffer,
                                                       uint32_t sizeInBuffer,
                                                       uint32_t * pHeartbeatSeconds);
    DLL bool __cdecl decodeUlMsgPollInd (const char ** ppInBuffer,
                                         uint32_t sizeInBuffer);
    DLL bool __cdecl decodeUlMsgSensorsReportxxx (const char ** ppInBuffer,
                                                  uint32_t sizeInBuffer,
                                                  uint32_t * pTime,
                                                  bool * pGpsPositionPresent,
                                                  int32_t * pGpsPositionLatitude,
                                                  int32_t * pGpsPositionLongitude,
                                                  int32_t * pGpsPositionElevation,
                                                  int32_t * pGpsPositionSpeed,
                                                  bool * pLclPositionPresent,
                                                  uint32_t * pLclPositionOrientation,
                                                  uint32_t * pLclPositionHugsThisPeriod,
                                                  uint32_t * pLclPositionSlapsThisPeriod,
                                                  uint32_t * pLclPositionDropsThisPeriod,
                                                  uint32_t * pLclPositionNudgesThisPeriod,
                                                  bool * pSoundLevelPresent,
                                                  uint32_t * pSoundLevel,
                                                  bool * pLuminosityPresent,
                                                  uint32_t * pLuminosity,
                                                  bool * pTemperaturePresent,
                                                  int32_t * pTemperature,
                                                  bool * pRssiPresent,
                                                  uint32_t *pRssi,
                                                  bool * pPowerStatePresent,
                                                  uint32_t *pPowerStateChargeState,
                                                  uint32_t *pPowerStateBatteryMV,
                                                  int32_t *pPowerStateEnergyUAH);
    DLL bool __cdecl decodeUlMsgDebugInd (const char ** ppInBuffer,
                                          uint32_t sizeInBuffer,
                                          uint32_t * pSizeOfString,
                                          char * pString);

    DLL void  initDll (void (*guiPrintToConsole) (const char *)); 

#ifdef __cplusplus
}
#endif

#endif

// End Of File

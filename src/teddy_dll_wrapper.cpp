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

/**
 * @file teddy_dll_wrapper.cpp
 * This file implements the encapsulation necessary to
 * call the CPP Message Handling functions from C#.
 */
#include <stdint.h>
#include <teddy_api.hpp>
#include <teddy_dll_wrapper.hpp>
#include "windows.h"

#ifdef __cplusplus
extern "C" {
#endif

    // Instantiate a messageHandler 
    MessageCodec gMessageCodec;

    // ----------------------------------------------------------------
    // CONSTANTS
    // ----------------------------------------------------------------

    uint32_t __cdecl maxDatagramSizeRaw (void)
    {
        return MAX_DATAGRAM_SIZE_RAW;
    }
       
    uint32_t __cdecl maxDebugStringSize (void)
    {
        return MAX_DEBUG_STRING_SIZE;
    }

    uint32_t __cdecl revisionLevel (void)
    {
        return REVISION_LEVEL;
    }
       
    // ----------------------------------------------------------------
    // PRIVATE FUNCTIONS
    // ----------------------------------------------------------------

    // ----------------------------------------------------------------
    // MESSAGE ENCODE WRAPPER FUNCTIONS
    // ----------------------------------------------------------------

    // Wrap encodeRebootReqDlMsg 
    uint32_t __cdecl encodeRebootReqDlMsg (char * pBuffer,
                                           bool devModeOnNotOff)
    {
	    RebootReqDlMsg_t msg;
	    msg.devModeOnNotOff = devModeOnNotOff;

        return gMessageCodec.encodeRebootReqDlMsg (pBuffer, &msg);
    }

    // Wrap encodeIntervalsGetReqDlMsg 
    uint32_t __cdecl encodeIntervalsGetReqDlMsg (char * pBuffer)
    {
        return gMessageCodec.encodeIntervalsGetReqDlMsg (pBuffer);
    }

    // Wrap encodeReportingIntervalSetReqDlMsg 
    uint32_t __cdecl encodeReportingIntervalSetReqDlMsg (char * pBuffer,
                                                         uint32_t reportingIntervalMinutes)
    {
	    ReportingIntervalSetReqDlMsg_t msg;
	    msg.reportingIntervalMinutes = reportingIntervalMinutes;

        return gMessageCodec.encodeReportingIntervalSetReqDlMsg (pBuffer, &msg);
    }

    // Wrap encodeHeartbeatSetReqDlMsg 
    uint32_t __cdecl encodeHeartbeatSetReqDlMsg (char * pBuffer,
                                                 uint32_t heartbeatSeconds)
    {
	    HeartbeatSetReqDlMsg_t msg;
	    msg.heartbeatSeconds = heartbeatSeconds;

        return gMessageCodec.encodeHeartbeatSetReqDlMsg (pBuffer, &msg);
    }

    // Wrap encodeSensorsReportGetReqDlMsg 
    uint32_t __cdecl encodeSensorsReportGetReqDlMsg (char * pBuffer)
    {
        return gMessageCodec.encodeSensorsReportGetReqDlMsg (pBuffer);
    }

    // ----------------------------------------------------------------
    // MESSAGE DECODE WRAPPER FUNCTIONS
    // ----------------------------------------------------------------

    // Wrap decodeUlMsg but just returning the message type
    uint32_t __cdecl decodeUlMsgType (const char * pInBuffer,
                                      uint32_t sizeInBuffer)
    {
	    MessageCodec::DecodeResult_t decodeResult;

	    decodeResult = gMessageCodec.decodeUlMsg (&pInBuffer,
                                                  sizeInBuffer,
                                                  NULL);

        return decodeResult;
    }

    // Wrap decodeUlMsg for an InitInd 
    bool __cdecl decodeUlMsgInitInd (const char ** ppInBuffer,
                                     uint32_t sizeInBuffer,
                                     uint32_t * pWakeUpCode,
                                     uint32_t * pRevisionLevel)
    {
	    bool success = false;
        MessageCodec::DecodeResult_t decodeResult;
        UlMsgUnion_t outBuffer;
	    decodeResult = gMessageCodec.decodeUlMsg (ppInBuffer,
                                                  sizeInBuffer,
                                                  &outBuffer);
        if (decodeResult == MessageCodec::DECODE_RESULT_INIT_IND_UL_MSG)
        {
            *pWakeUpCode = (uint32_t) outBuffer.initIndUlMsg.wakeUpCode;
            *pRevisionLevel = (uint32_t) outBuffer.initIndUlMsg.revisionLevel;
            success = true;
        }
        
        return success;
    }

    // Wrap decodeUlMsg for IntervalsGetCnf 
    bool __cdecl decodeUlMsgIntervalsGetCnf (const char ** ppInBuffer,
                                             uint32_t sizeInBuffer,
                                             uint32_t * pReportingIntervalMinutes,
                                             uint32_t * pHeartbeatSeconds)
    {
	    bool success = false;
        MessageCodec::DecodeResult_t decodeResult;
        UlMsgUnion_t outBuffer;
        
	    decodeResult = gMessageCodec.decodeUlMsg (ppInBuffer,
                                                  sizeInBuffer,
                                                  &outBuffer);
                                                  
        if (decodeResult == MessageCodec::DECODE_RESULT_INTERVALS_GET_CNF_UL_MSG)
        {
            *pReportingIntervalMinutes = (uint32_t) outBuffer.intervalsGetCnfUlMsg.reportingIntervalMinutes;
            *pHeartbeatSeconds = (uint32_t) outBuffer.intervalsGetCnfUlMsg.heartbeatSeconds;
            success = true;
        }
        
        return success;
    }

    // Wrap decodeUlMsg for ReportingIntervalSetCnf 
    bool __cdecl decodeUlMsgReportingIntervalSetCnf (const char ** ppInBuffer,
                                                     uint32_t sizeInBuffer,
                                                     uint32_t * pReportingIntervalMinutes)
    {
	    bool success = false;
        MessageCodec::DecodeResult_t decodeResult;
        UlMsgUnion_t outBuffer;
        
	    decodeResult = gMessageCodec.decodeUlMsg (ppInBuffer,
                                                  sizeInBuffer,
                                                  &outBuffer);
                                                  
        if (decodeResult == MessageCodec::DECODE_RESULT_REPORTING_INTERVAL_SET_CNF_UL_MSG)
        {
            *pReportingIntervalMinutes = (uint32_t) outBuffer.reportingIntervalSetCnfUlMsg.reportingIntervalMinutes;
            success = true;
        }

        return success;
    }

    // Wrap decodeUlMsg for HeartbeatSetCnf 
    bool __cdecl decodeUlMsgHeartbeatSetCnf (const char ** ppInBuffer,
                                             uint32_t sizeInBuffer,
                                             uint32_t * pHeartbeatSeconds)
    {
	    bool success = false;
        MessageCodec::DecodeResult_t decodeResult;
        UlMsgUnion_t outBuffer;
        
	    decodeResult = gMessageCodec.decodeUlMsg (ppInBuffer,
                                                  sizeInBuffer,
                                                  &outBuffer);
                                                  
        if (decodeResult == MessageCodec::DECODE_RESULT_HEARTBEAT_SET_CNF_UL_MSG)
        {
            *pHeartbeatSeconds = (uint32_t) outBuffer.heartbeatSetCnfUlMsg.heartbeatSeconds;
            success = true;
        }

        return success;
    }

    // Wrap decodeUlMsg for an PollInd 
    bool __cdecl decodeUlMsgPollInd (const char ** ppInBuffer,
                                     uint32_t sizeInBuffer)
    {
	    bool success = false;
        MessageCodec::DecodeResult_t decodeResult;
        UlMsgUnion_t outBuffer;
	    decodeResult = gMessageCodec.decodeUlMsg (ppInBuffer,
                                                  sizeInBuffer,
                                                  &outBuffer);
        if (decodeResult == MessageCodec::DECODE_RESULT_POLL_IND_UL_MSG)
        {
            success = true;
        }
        
        return success;
    }

    // Wrap decodeUlMsg for SensorsReportGetCnf or SensorsReportInd
    bool __cdecl decodeUlMsgSensorsReportxxx (const char ** ppInBuffer,
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
                                             int32_t *pPowerStateEnergyUAH)
    {
	    bool success = false;
        MessageCodec::DecodeResult_t decodeResult;
        UlMsgUnion_t outBuffer;
        
	    decodeResult = gMessageCodec.decodeUlMsg (ppInBuffer,
                                                  sizeInBuffer,
                                                  &outBuffer);
                                                  
        if ((decodeResult == MessageCodec::DECODE_RESULT_SENSORS_REPORT_GET_CNF_UL_MSG) ||
            (decodeResult == MessageCodec::DECODE_RESULT_SENSORS_REPORT_IND_UL_MSG))
        {
            SensorReadings_t * pSensorReadings = &(outBuffer.sensorsReportIndUlMsg.sensorReadings);
            *pTime = (uint32_t) pSensorReadings->time;
            
            *pGpsPositionPresent = false;
            *pLclPositionPresent = false;
            *pSoundLevelPresent  = false;
            *pLuminosityPresent = false;
            *pTemperaturePresent = false;
            *pRssiPresent = false;
            *pPowerStatePresent = false;

            if (pSensorReadings->gpsPositionPresent)
            {
                *pGpsPositionPresent = true;
                *pGpsPositionLatitude = (int32_t) pSensorReadings->gpsPosition.latitude;
                *pGpsPositionLongitude = (int32_t) pSensorReadings->gpsPosition.longitude;
                *pGpsPositionElevation = (int32_t) pSensorReadings->gpsPosition.elevation;
                *pGpsPositionSpeed = (int32_t) pSensorReadings->gpsPosition.speed;
            }

            if (pSensorReadings->lclPositionPresent)
            {
                *pLclPositionPresent = true;
                *pLclPositionOrientation = (uint32_t) pSensorReadings->lclPosition.orientation;
                *pLclPositionHugsThisPeriod = (uint32_t) pSensorReadings->lclPosition.hugsThisPeriod;
                *pLclPositionSlapsThisPeriod = (uint32_t) pSensorReadings->lclPosition.slapsThisPeriod;
                *pLclPositionDropsThisPeriod = (uint32_t) pSensorReadings->lclPosition.dropsThisPeriod;
                *pLclPositionNudgesThisPeriod = (uint32_t) pSensorReadings->lclPosition.nudgesThisPeriod;
            }

            if (pSensorReadings->soundLevelPresent)
            {
                *pSoundLevelPresent = true;
                *pSoundLevel = (uint32_t) pSensorReadings->soundLevel;
            }

            if (pSensorReadings->luminosityPresent)
            {
                *pLuminosityPresent = true;
                *pLuminosity = (uint32_t) pSensorReadings->luminosity;
            }

            if (pSensorReadings->temperaturePresent)
            {
                *pTemperaturePresent = true;
                *pTemperature = (uint32_t) pSensorReadings->temperature;
            }

            if (pSensorReadings->rssiPresent)
            {
                *pRssiPresent = true;
                *pRssi = (uint32_t) pSensorReadings->rssi;
            }

            if (pSensorReadings->powerStatePresent)
            {
                *pPowerStatePresent = true;
                *pPowerStateChargeState = (uint32_t) pSensorReadings->powerState.chargeState;
                *pPowerStateBatteryMV = (uint32_t) pSensorReadings->powerState.batteryMV;
                *pPowerStateEnergyUAH = (int32_t) pSensorReadings->powerState.energyUAH;
            }

            success = true;
        }

        return success;
    }

    // Wrap decodeUlMsg for an DebugInd 
    bool __cdecl decodeUlMsgDebugInd (const char ** ppInBuffer,
                                      uint32_t sizeInBuffer,
                                      uint32_t * pSizeOfString,
                                      char * pString)
    {
	    bool success = false;
        MessageCodec::DecodeResult_t decodeResult;
        UlMsgUnion_t outBuffer;
	    decodeResult = gMessageCodec.decodeUlMsg (ppInBuffer,
                                                  sizeInBuffer,
                                                  &outBuffer);
        if (decodeResult == MessageCodec::DECODE_RESULT_DEBUG_IND_UL_MSG)
        {
            *pSizeOfString = (uint32_t) outBuffer.debugIndUlMsg.sizeOfString;
            memcpy (pString, &(outBuffer.debugIndUlMsg.string[0]), *pSizeOfString);
            success = true;
        }
        
        return success;
    }

    // ----------------------------------------------------------------
    // MISC FUNCTIONS
    // ----------------------------------------------------------------

    void initDll (void (*printToConsole) (const char *))
    {
        gMessageCodec.initDll (printToConsole);
    } 

#ifdef __cplusplus
}  // extern "C"
#endif

// End Of File

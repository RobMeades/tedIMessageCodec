/* teddy interface
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
 * @file teddy_msg_handler.cpp
 * This file implements the API to the teddy.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h> // for va_...
#include <string.h> // for memcpy()
#include <teddy_api.hpp>

//#define DEBUG

#ifdef DEBUG
#define MESSAGE_CODEC_LOGMSG(...)    MessageCodec::logMsg(__VA_ARGS__)
#else
#define MESSAGE_CODEC_LOGMSG(...)
#endif

// ----------------------------------------------------------------
// GENERAL COMPILE-TIME CONSTANTS
// ----------------------------------------------------------------

/// The max size of a debug message (including terminator)
#define MAX_DEBUG_MESSAGE_LEN 128

/// The maximum number of bitmap bytes expected
#define MAX_BITMAP_BYTES 2

// ----------------------------------------------------------------
// PRIVATE VARIABLES
// ----------------------------------------------------------------

void (*MessageCodec::mp_guiPrintToConsole) (const char*) = NULL;

// ----------------------------------------------------------------
// ON-AIR MESSAGE IDs
// ----------------------------------------------------------------

/// The message IDs in the downlink direction (i.e. to the teddy)
typedef enum MsgIdDlTag_t
{
  REBOOT_REQ_DL_MSG,                 //!< Reboot the C027N/water-meter
                                     //! device.
  INTERVALS_GET_REQ_DL_MSG,          //!< Get the rate at which sensor
                                     //! readings and reports are made.
  REPORTING_INTERVAL_SET_REQ_DL_MSG, //!< Set the rate at which sensor
                                     //! reports are returned by the teddy.
  HEARTBEAT_SET_REQ_DL_MSG,   //!< Set the rate at which sensor
                                     //! readings are taken by the teddy.
  SENSORS_REPORT_GET_REQ_DL_MSG,     //!< Get a report from the teddy.
  TRAFFIC_REPORT_GET_REQ_DL_MSG,     //!< Get a traffic report from the teddy.
  MAX_NUM_DL_MSGS                    //!< The maximum number of downlink
                                     //! messages.
} MsgIdDl_t;

/// The message IDs in the uplink direction (i.e. from the teddy)
typedef enum MsgIdUlTag_t
{
  INIT_IND_UL_MSG,                   //!< Power on of the teddy has completed.
  INTERVALS_GET_CNF_UL_MSG,          //!< The rate at which sensor
                                     //! readings and reports are made.
  REPORTING_INTERVAL_SET_CNF_UL_MSG, //!< The rate at which sensor readings are
                                     //! reported by the teddy.
  HEARTBEAT_SET_CNF_UL_MSG,   //!< The rate at which sensor readings are
                                     //! taken by the teddy.
  POLL_IND_UL_MSG,                   //!< A poll message, sent on expiry of a
                                     // reporting period.
  SENSORS_REPORT_GET_CNF_UL_MSG,     //!< Response to a sensor report request.
  SENSORS_REPORT_IND_UL_MSG,         //!< A periodic sensor report.
  DEBUG_IND_UL_MSG,                  //!< A debug string.
  TRAFFIC_REPORT_GET_CNF_UL_MSG,     //!< Response to a traffic report request.
  TRAFFIC_REPORT_IND_UL_MSG,         //!< A periodic traffic report.
  MAX_NUM_UL_MSGS                    //!< The maximum number of uplink messages.
} MsgIdUl_t;

// ----------------------------------------------------------------
// GENERIC PRIVATE FUNCTIONS
// ----------------------------------------------------------------

/// Encode a boolean value.
uint32_t MessageCodec::encodeBool (char * pBuffer, bool value)
{
    *pBuffer = value;
    return 1;
}

/// Decode a boolean value.
bool MessageCodec::decodeBool (const char ** ppBuffer)
{
    bool boolValue = false;

    if (**ppBuffer)
    {
        boolValue = true;
    }

    (*ppBuffer)++;

    return boolValue;
}

/// Encode a uint32_t
uint32_t MessageCodec::encodeUint32 (char * pBuffer, uint32_t value)
{
    uint32_t numBytesEncoded = 4;

    pBuffer[0] = 0xff & (value >> 24);
    pBuffer[1] = 0xff & (value >> 16);
    pBuffer[2] = 0xff & (value >> 8);
    pBuffer[3] = 0xff & value;

    return numBytesEncoded;
}

/// Decode a uint32_t
uint32_t MessageCodec::decodeUint32 (const char ** ppBuffer)
{
    uint32_t value = 0;

    value += ((**ppBuffer) & 0xFF) << 24;
    (*ppBuffer)++;
    value += ((**ppBuffer) & 0xFF) << 16;
    (*ppBuffer)++;
    value += ((**ppBuffer) & 0xFF) << 8;
    (*ppBuffer)++;
    value += ((**ppBuffer) & 0xFF);
    (*ppBuffer)++;

    return value;
}

/// Encode a uint16_t
uint32_t MessageCodec::encodeUint16 (char * pBuffer, uint16_t value)
{
    uint32_t numBytesEncoded = 2;

    pBuffer[0] = 0xff & (value >> 8);
    pBuffer[1] = 0xff & value;

    return numBytesEncoded;
}

/// Decode a uint16_t
uint32_t MessageCodec::decodeUint16 (const char ** ppBuffer)
{
    uint32_t value = 0;

    value += ((**ppBuffer) & 0xFF) << 8;
    (*ppBuffer)++;
    value += ((**ppBuffer) & 0xFF);
    (*ppBuffer)++;

    return value;
}

/// Encode a SensorReadings_t
//The sensor readings message is coded as follows:
//
// uint32_t    time             UTC seconds
// uint8_t     bytesToFollow    so that some checking can be done
// uint8_t     itemsBitmap0     see below
// [uint8_t    itemsBitmap1]... may repeat, see below
// [itemsUnion item0]...        zero or more items.
//
// The generic format of the itemsBitmap is:
//  bit   7 6 5 4 3 2 1 0
//  value y x x x x x x x
//
// ...where x is set to 1 if the associated logical item is
// present, otherwise 0, and y is set to 1 if another
// itemsBitmap follows, otherwise 0.
//
// There is no generic format for an item, each is defined
// explicitly and must be present in the order of the
// itemsBitmap (where the item at bit 0 of itemsBitmap0
// comes first).  The following item formats are defined:
//
// x  Name              Size      Format
// 0: GPSPosition,      12 bytes: 32 bits lat, 32 bits long, 32 bits elevation
// 1: LclPosition,       3 bytes: bits 0-3: orientation
//                                bits 4-7: number of hugs
//                                bits 8-12: number of slaps
//                                bits 13-15: number of drops
//                                bits 16-23: number if nudges
// 2: SoundLevel,        2 bytes: 0 nothing, 65535 maximum
// 3: Luminosity,        2 bytes: 0 nothing, 65535 maximum
// 4: Temperature,       1 byte:  -128 to +127 C
// 5: RSSI,              1 byte:  as defined for AT+CSQ
// 6: Power state,       3 bytes: bits 0-5: battery voltage, 0 == 0 volts, 63 = 10 volts
//                                bits 6-7: charger state
//                                bits 8-23: energy in uAH (16 bits, signed)
// 7: Smoke/fire sensor, 2 bytes: 0 nothing, 65535 maximum
// 8: Alcohol sensor,    2 bytes: 0 nothing, 65535 maximum

uint32_t MessageCodec::encodeSensorReadings (char * pBuffer, SensorReadings_t * pSensorReadings)
{
    char *pBufferAtStart;
    char *pBytesToFollow;
    uint8_t bitMap = 0;

    pBufferAtStart = pBuffer;

    // Encode time
    pBuffer += encodeUint32 (pBuffer, pSensorReadings->time);

    // Set things up so that bytesToFollow can be filled in later
    pBytesToFollow = pBuffer;
    pBuffer++;

    // Now fill in the bit-map, determining which items are present
    // and filling in the bitmap according to the order of the items
    // in the structure
    if (pSensorReadings->gpsPositionPresent)
    {
        bitMap |= 0x01;
    }
    if (pSensorReadings->lclPositionPresent)
    {
        bitMap |= 0x02;
    }
    if (pSensorReadings->soundLevelPresent)
    {
        bitMap |= 0x04;
    }
    if (pSensorReadings->luminosityPresent)
    {
        bitMap |= 0x08;
    }
    if (pSensorReadings->temperaturePresent)
    {
        bitMap |= 0x10;
    }
    if (pSensorReadings->rssiPresent)
    {
        bitMap |= 0x20;
    }
    if (pSensorReadings->powerStatePresent)
    {
        bitMap |= 0x40;
    }
    // That's 7 things coded up so write this bitmap value
    // but don't advance the pointer as it may be necessary
    // to set the extension bit
    *pBuffer = bitMap;

    // That's all we have but if we have more it would go as follows
    // if (pSensorReadings->blahPresent || pSensorReadings->blahBlahPresent)
    // {
    //     // Set the extension bit, re-write the last bitmap
    //     // value and start on the next one
    //     bitMap |= 0x80;
    //     *pBuffer = bitMap;
    //     pBuffer++;
    //     bitMap = 0;
    //
    //     if (pSensorReadings->blahPresent)
    //     {
    //         bitMap |= 0x01;
    //     }
    //     if (pSensorReadings->blahBlahPresent)
    //     {
    //         bitMap |= 0x02;
    //     }
    //
    //     // This is the end of the structure, so now write this
    //     // bitmap value
    //     *pBuffer = bitMap;
    // }

    // Advance the pointer
    pBuffer++;

    // Now fill in the actual values
    if (pSensorReadings->gpsPositionPresent)
    {
        pBuffer += encodeUint32 (pBuffer, (uint32_t) pSensorReadings->gpsPosition.latitude);
        pBuffer += encodeUint32 (pBuffer, (uint32_t) pSensorReadings->gpsPosition.longitude);
        pBuffer += encodeUint32 (pBuffer, (uint32_t) pSensorReadings->gpsPosition.elevation);
        pBuffer += encodeUint32 (pBuffer, (uint32_t) pSensorReadings->gpsPosition.speed);
    }
    if (pSensorReadings->lclPositionPresent)
    {
        uint8_t x = 0;

        x |= pSensorReadings->lclPosition.orientation & 0x0F;
        if (pSensorReadings->lclPosition.hugsThisPeriod > MAX_HUGS_THIS_PERIOD)
        {
            pSensorReadings->lclPosition.hugsThisPeriod = MAX_HUGS_THIS_PERIOD;
        }
        x |= ((pSensorReadings->lclPosition.hugsThisPeriod << 4) & 0xF0);
        *pBuffer = x;
        pBuffer++;
        if (pSensorReadings->lclPosition.slapsThisPeriod > MAX_SLAPS_THIS_PERIOD)
        {
            pSensorReadings->lclPosition.slapsThisPeriod = MAX_SLAPS_THIS_PERIOD;
        }
        x = pSensorReadings->lclPosition.slapsThisPeriod & 0x0F;
        if (pSensorReadings->lclPosition.dropsThisPeriod > MAX_DROPS_THIS_PERIOD)
        {
            pSensorReadings->lclPosition.dropsThisPeriod = MAX_DROPS_THIS_PERIOD;
        }
        x |= ((pSensorReadings->lclPosition.dropsThisPeriod << 4) & 0xF0);
        *pBuffer = x;
        pBuffer++;
        x = pSensorReadings->lclPosition.nudgesThisPeriod;
        *pBuffer = x;
        pBuffer++;
    }
    if (pSensorReadings->soundLevelPresent)
    {
        pBuffer += encodeUint16 (pBuffer, pSensorReadings->soundLevel);
    }
    if (pSensorReadings->luminosityPresent)
    {
        pBuffer += encodeUint16 (pBuffer, pSensorReadings->luminosity);
    }
    if (pSensorReadings->temperaturePresent)
    {
        *pBuffer = (uint8_t) pSensorReadings->temperature;
        pBuffer++;
    }
    if (pSensorReadings->rssiPresent)
    {
        *pBuffer = pSensorReadings->rssi;
        pBuffer++;
    }
    if (pSensorReadings->powerStatePresent)
    {
        uint8_t x = 0;

        if (pSensorReadings->powerState.batteryMV > MAX_BATTERY_VOLTAGE_MV)
        {
            pSensorReadings->powerState.batteryMV = MAX_BATTERY_VOLTAGE_MV;
        }
        x |= (((uint32_t) pSensorReadings->powerState.batteryMV * 0x3F / 10000) & 0x3F);
        x |= ((pSensorReadings->powerState.chargeState << 6) & 0xC0);
        *pBuffer = x;
        pBuffer++;
        pBuffer += encodeUint16 (pBuffer, pSensorReadings->powerState.energyUAH);
    }

    // This is as many as we currently have.  If we have more it goes
    // as follows
    // if (pSensorReadings->blahPresent)
    // {
    //     Encode blah and advance pBuffer, e.g.
    //     pBuffer += encodeUint16 (pBuffer, pSensorReadings->blah);
    // }
    // if (pSensorReadings->blahBlahPresent)
    // {
    //     Encode blahBlah and advance pBuffer, e.g.
    //     pBuffer += encodeUint16 (pBuffer, pSensorReadings->blahBlah);
    // }

    /* Now fill in the value for bytesToFollow */
    *pBytesToFollow = pBuffer - (pBufferAtStart + 5); // 5 for a UInt32 and bytesToFollow itself

    return (pBuffer - pBufferAtStart);
}

// Decode a SensorReading_t
bool MessageCodec::decodeSensorReadings (const char ** ppBuffer, SensorReadings_t * pSensorReadings)
{
    bool success = false;
    uint8_t x;
    const char *pBufferAfterEnd;
    uint8_t bitMapBytes[MAX_BITMAP_BYTES];
    bool moreBitmapBytes = true;

    memset (&(bitMapBytes[0]), 0, sizeof (bitMapBytes));
    memset (pSensorReadings, 0, sizeof (*pSensorReadings));

    // Decode time
    pSensorReadings->time = decodeUint32 (ppBuffer);

    // Decode bytesToFollow and add it to the current pointer to find the end
    pBufferAfterEnd = *ppBuffer + **ppBuffer + 1; //+1 because this is before the increment
    (*ppBuffer)++;

    // Decode the bitmap byte(s)
    for (x = 0; moreBitmapBytes && (*ppBuffer < pBufferAfterEnd); x++)
    {
        uint8_t y;

        y = (uint8_t) **ppBuffer;
        (*ppBuffer)++;

        if (x < MAX_BITMAP_BYTES)
        {
            bitMapBytes[x] = y;
        }

        if ((y & 0x80) == 0)
        {
            moreBitmapBytes = false;
        }
    }

    // moreBitmapBytes should be false by now and,
    // if so, decode the values, in order
    if (!moreBitmapBytes)
    {
        // GPS Position
        if (bitMapBytes[0] & 0x01)
        {
            pSensorReadings->gpsPositionPresent = true;
            pSensorReadings->gpsPosition.latitude = (int32_t) decodeUint32 (ppBuffer);
            pSensorReadings->gpsPosition.longitude = (int32_t) decodeUint32 (ppBuffer);
            pSensorReadings->gpsPosition.elevation = (int32_t) decodeUint32 (ppBuffer);
            pSensorReadings->gpsPosition.speed = (int32_t) decodeUint32 (ppBuffer);
        }
        // Lcl Position
        if (bitMapBytes[0] & 0x02)
        {
            pSensorReadings->lclPositionPresent = true;

            x = (uint8_t) **ppBuffer;
            (*ppBuffer)++;
            pSensorReadings->lclPosition.orientation = (Orientation_t) (x & 0x0F);
            pSensorReadings->lclPosition.hugsThisPeriod = (x & 0xF0) >> 4;
            x = (uint8_t) **ppBuffer;
            (*ppBuffer)++;
            pSensorReadings->lclPosition.slapsThisPeriod = x & 0x0F;
            pSensorReadings->lclPosition.dropsThisPeriod = (x & 0xF0) >> 4;
            x = (uint8_t) **ppBuffer;
            (*ppBuffer)++;
            pSensorReadings->lclPosition.nudgesThisPeriod = x;
        }
        // Sound Level
        if (bitMapBytes[0] & 0x04)
        {
            pSensorReadings->soundLevelPresent = true;
            pSensorReadings->soundLevel = decodeUint16 (ppBuffer);
        }
        // Luminosity
        if (bitMapBytes[0] & 0x08)
        {
            pSensorReadings->luminosityPresent = true;
            pSensorReadings->luminosity = decodeUint16 (ppBuffer);
        }
        // Temperature
        if (bitMapBytes[0] & 0x10)
        {
            pSensorReadings->temperaturePresent = true;
            pSensorReadings->temperature = (int8_t) **ppBuffer;
            (*ppBuffer)++;
        }
        // RSSI
        if (bitMapBytes[0] & 0x20)
        {
            pSensorReadings->rssiPresent = true;
            pSensorReadings->rssi = (uint8_t) **ppBuffer;
            (*ppBuffer)++;
        }
        // PowerState
        if (bitMapBytes[0] & 0x40)
        {
            pSensorReadings->powerStatePresent = true;
            x = (uint8_t) **ppBuffer;
            (*ppBuffer)++;

            pSensorReadings->powerState.batteryMV = (uint32_t) ((uint32_t) x & 0x3F) * 10000 / 0x3F;
            pSensorReadings->powerState.chargeState = (ChargeState_t) ((x & 0xC0) >> 6);
            pSensorReadings->powerState.energyUAH = (int16_t) decodeUint16 (ppBuffer);
        }

        // That's all we have but if we have more it would go as follows
        // // Next bitmap byte
        // // Blah Sensor
        // if (bitMapBytes[1] & 0x01)
        // {
        //     pSensorReadings->blahPresent = true;
        //     pSensorReadings->blah = decodeUint16 (ppBuffer);
        // }
        // // BlahBlah Sensor
        // if (bitMapBytes[1] & 0x02)
        // {
        //     pSensorReadings->blahBlahPresent = true;
        //     pSensorReadings->blahBlah = decodeUint16 (ppBuffer);
        // }

        // Having done all that, the pointer must now be at
        // the end point that we established above
        if (*ppBuffer == pBufferAfterEnd)
        {
            success = true;
        }
        else
        {
            // If it isn't, the safest thing is to use the bytesToFollow
            // value from the message to get us to the next thing as
            // we've probably misinterpreted something in the middle
            *ppBuffer = pBufferAfterEnd;
        }
    }

    return success;
}

// ----------------------------------------------------------------
// MESSAGE ENCODING FUNCTIONS
// ----------------------------------------------------------------

uint32_t MessageCodec::encodeInitIndUlMsg (char * pBuffer,
                                           InitIndUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding InitIndUlMsg, ID 0x%.2x, ", INIT_IND_UL_MSG);
    pBuffer[numBytesEncoded] = INIT_IND_UL_MSG;
    numBytesEncoded++;
    pBuffer[numBytesEncoded] = (uint8_t) pMsg->wakeUpCode;
    numBytesEncoded++;
    numBytesEncoded += encodeUint16 (&(pBuffer[numBytesEncoded]), REVISION_LEVEL);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeRebootReqDlMsg (char * pBuffer,
											 RebootReqDlMsg_t *pMsg)
{
    uint32_t numBytesEncoded = 0;

    pBuffer[numBytesEncoded] = REBOOT_REQ_DL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeBool (&(pBuffer[numBytesEncoded]), pMsg->devModeOnNotOff);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeIntervalsGetReqDlMsg (char * pBuffer)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding IntervalsGetReqDlMsg, ID 0x%.2x, ", INTERVALS_GET_REQ_DL_MSG);
    pBuffer[numBytesEncoded] = INTERVALS_GET_REQ_DL_MSG;
    numBytesEncoded++;
    // Empty body
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeIntervalsGetCnfUlMsg (char * pBuffer,
                                                   IntervalsGetCnfUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding IntervalsGetCnfUlMsg, ID 0x%.2x, ", INTERVALS_GET_CNF_UL_MSG);
    pBuffer[numBytesEncoded] = INTERVALS_GET_CNF_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->reportingIntervalMinutes);
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->heartbeatSeconds);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeReportingIntervalSetReqDlMsg (char * pBuffer,
                                                           ReportingIntervalSetReqDlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding ReportingIntervalSetReqDlMsg, ID 0x%.2x, ", REPORTING_INTERVAL_SET_REQ_DL_MSG);
    pBuffer[numBytesEncoded] = REPORTING_INTERVAL_SET_REQ_DL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->reportingIntervalMinutes);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeReportingIntervalSetCnfUlMsg (char * pBuffer,
                                                           ReportingIntervalSetCnfUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding ReportingIntervalSetCnfUlMsg, ID 0x%.2x, ", REPORTING_INTERVAL_SET_CNF_UL_MSG);
    pBuffer[numBytesEncoded] = REPORTING_INTERVAL_SET_CNF_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->reportingIntervalMinutes);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeHeartbeatSetReqDlMsg (char * pBuffer,
                                                         HeartbeatSetReqDlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding HeartbeatSetReqDlMsg, ID 0x%.2x, ", HEARTBEAT_SET_REQ_DL_MSG);
    pBuffer[numBytesEncoded] = HEARTBEAT_SET_REQ_DL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->heartbeatSeconds);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeHeartbeatSetCnfUlMsg (char * pBuffer,
                                                         HeartbeatSetCnfUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding HeartbeatSetCnfUlMsg, ID 0x%.2x, ", HEARTBEAT_SET_CNF_UL_MSG);
    pBuffer[numBytesEncoded] = HEARTBEAT_SET_CNF_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->heartbeatSeconds);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodePollIndUlMsg (char * pBuffer)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding PollIndMsg, ID 0x%.2x, ", POLL_IND_UL_MSG);
    pBuffer[numBytesEncoded] = POLL_IND_UL_MSG;
    numBytesEncoded++;
    // Empty body
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeSensorsReportGetReqDlMsg (char * pBuffer)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding SensorsReportGetReqDlMsg, ID 0x%.2x, ", SENSORS_REPORT_GET_REQ_DL_MSG);
    pBuffer[numBytesEncoded] = SENSORS_REPORT_GET_REQ_DL_MSG;
    numBytesEncoded++;
    // Empty body
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeSensorsReportGetCnfUlMsg (char * pBuffer,
                                                       SensorsReportGetCnfUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding SensorsReportGetCnfUlMsg, ID 0x%.2x, ", SENSORS_REPORT_GET_CNF_UL_MSG);
    pBuffer[numBytesEncoded] = SENSORS_REPORT_GET_CNF_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeSensorReadings (&(pBuffer[numBytesEncoded]), &(pMsg->sensorReadings));
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeSensorsReportIndUlMsg (char * pBuffer,
                                                    SensorsReportIndUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding SensorsReportIndUlMsg, ID 0x%.2x, ", SENSORS_REPORT_IND_UL_MSG);
    pBuffer[numBytesEncoded] = SENSORS_REPORT_IND_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeSensorReadings (&(pBuffer[numBytesEncoded]), &(pMsg->sensorReadings));
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeTrafficReportGetReqDlMsg (char * pBuffer)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding TrafficReportGetReqDlMsg, ID 0x%.2x, ", TRAFFIC_REPOR_GET_REQ_DL_MSG);
    pBuffer[numBytesEncoded] = TRAFFIC_REPORT_GET_REQ_DL_MSG;
    numBytesEncoded++;
    // Empty body
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeTrafficReportGetCnfUlMsg (char * pBuffer,
                                                       TrafficReportGetCnfUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding TrafficReportGetCnfUlMsg, ID 0x%.2x, ", TRAFFIC_REPORT_GET_CNF_UL_MSG);
    pBuffer[numBytesEncoded] = TRAFFIC_REPORT_GET_CNF_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numDatagramsSent);
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numBytesSent);
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numDatagramsReceived);
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numBytesReceived);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeTrafficReportIndUlMsg (char * pBuffer,
                                                    TrafficReportIndUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;

    MESSAGE_CODEC_LOGMSG ("Encoding TrafficReportIndUlMsg, ID 0x%.2x, ", TRAFFIC_REPORT_IND_UL_MSG);
    pBuffer[numBytesEncoded] = TRAFFIC_REPORT_IND_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numDatagramsSent);
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numBytesSent);
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numDatagramsReceived);
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), pMsg->numBytesReceived);
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

uint32_t MessageCodec::encodeDebugIndUlMsg (char * pBuffer,
                                            DebugIndUlMsg_t * pMsg)
{
    uint32_t numBytesEncoded = 0;
    uint32_t sizeOfString = pMsg->sizeOfString;

    MESSAGE_CODEC_LOGMSG ("Encoding DebugIndUlMsg, ID 0x%.2x, ", DEBUG_IND_UL_MSG);
    if (sizeOfString > MAX_DEBUG_STRING_SIZE)
    {
        sizeOfString = MAX_DEBUG_STRING_SIZE;
    }
    pBuffer[numBytesEncoded] = DEBUG_IND_UL_MSG;
    numBytesEncoded++;
    numBytesEncoded += encodeUint32 (&(pBuffer[numBytesEncoded]), (uint32_t) pMsg->sizeOfString);
    memcpy (&(pBuffer[numBytesEncoded]), &(pMsg->string[0]), sizeOfString);
    numBytesEncoded += sizeOfString;
    MESSAGE_CODEC_LOGMSG ("%d bytes encoded.\n", numBytesEncoded);

    return numBytesEncoded;
}

// ----------------------------------------------------------------
// MESSAGE DECODING FUNCTIONS
// ----------------------------------------------------------------
MessageCodec::DecodeResult_t MessageCodec::decodeDlMsg (const char ** ppInBuffer,
                                                        uint32_t sizeInBuffer,
                                                        DlMsgUnion_t * pOutBuffer)
{
    MsgIdDl_t msgId;
    DecodeResult_t decodeResult = DECODE_RESULT_FAILURE;

    if (sizeInBuffer <  MIN_MESSAGE_SIZE)
    {
        decodeResult = DECODE_RESULT_INPUT_TOO_SHORT;
    }
    else
    {
        decodeResult = DECODE_RESULT_UNKNOWN_MSG_ID;
        // First byte should be a valid DL message ID
        msgId = (MsgIdDl_t) **ppInBuffer;
        (*ppInBuffer)++;
        if (msgId < MAX_NUM_DL_MSGS)
        {
            switch (msgId)
            {
                case REBOOT_REQ_DL_MSG:
                {
                    decodeResult = DECODE_RESULT_REBOOT_REQ_DL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->rebootReqDlMsg.devModeOnNotOff = decodeBool (ppInBuffer);
                    }
                }
                break;
                case INTERVALS_GET_REQ_DL_MSG:
                {
                    decodeResult = DECODE_RESULT_INTERVALS_GET_REQ_DL_MSG;
                    // Empty message
                }
                break;
                case REPORTING_INTERVAL_SET_REQ_DL_MSG:
                {
                    decodeResult = DECODE_RESULT_REPORTING_INTERVAL_SET_REQ_DL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->reportingIntervalSetReqDlMsg.reportingIntervalMinutes = decodeUint32 (ppInBuffer);
                    }
                }
                break;
                case HEARTBEAT_SET_REQ_DL_MSG:
                {
                    decodeResult = DECODE_RESULT_HEARTBEAT_SET_REQ_DL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->heartbeatSetReqDlMsg.heartbeatSeconds = decodeUint32 (ppInBuffer);
                    }
                }
                break;
                case SENSORS_REPORT_GET_REQ_DL_MSG:
                {
                    decodeResult = DECODE_RESULT_SENSORS_REPORT_GET_REQ_DL_MSG;
                    // Empty message
                }
                break;
                case TRAFFIC_REPORT_GET_REQ_DL_MSG:
                {
                    decodeResult = DECODE_RESULT_TRAFFIC_REPORT_GET_REQ_DL_MSG;
                    // Empty message
                }
                break;
                default:
                // The decodeResult will be left as Unknown message
                break;
            }
        }
    }

    return decodeResult;
}

MessageCodec::DecodeResult_t MessageCodec::decodeUlMsg (const char ** ppInBuffer,
                                                        uint32_t sizeInBuffer,
                                                        UlMsgUnion_t * pOutBuffer)
{
    MsgIdUl_t msgId;
    DecodeResult_t decodeResult = DECODE_RESULT_FAILURE;

    if (sizeInBuffer < MIN_MESSAGE_SIZE)
    {
        decodeResult = DECODE_RESULT_INPUT_TOO_SHORT;
    }
    else
    {
        decodeResult = DECODE_RESULT_UNKNOWN_MSG_ID;
        // First byte should be a valid UL message ID
        msgId = (MsgIdUl_t) **ppInBuffer;
        (*ppInBuffer)++;
        if (msgId < MAX_NUM_UL_MSGS)
        {
            switch (msgId)
            {
                case INIT_IND_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_INIT_IND_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->initIndUlMsg.wakeUpCode = (WakeUpCode_t) **ppInBuffer;
                        (*ppInBuffer)++;
                        pOutBuffer->initIndUlMsg.revisionLevel  = decodeUint16 (ppInBuffer);
                    }
                }
                break;
                case INTERVALS_GET_CNF_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_INTERVALS_GET_CNF_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->intervalsGetCnfUlMsg.reportingIntervalMinutes = decodeUint32 (ppInBuffer);
                        pOutBuffer->intervalsGetCnfUlMsg.heartbeatSeconds = decodeUint32 (ppInBuffer);
                    }
                }
                break;
                case REPORTING_INTERVAL_SET_CNF_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_REPORTING_INTERVAL_SET_CNF_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->reportingIntervalSetCnfUlMsg.reportingIntervalMinutes = decodeUint32 (ppInBuffer);
                    }
                }
                break;
                case HEARTBEAT_SET_CNF_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_HEARTBEAT_SET_CNF_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->heartbeatSetCnfUlMsg.heartbeatSeconds = decodeUint32 (ppInBuffer);
                    }
                }
                break;
                case POLL_IND_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_POLL_IND_UL_MSG;
                    // Empty message
                }
                break;
                case SENSORS_REPORT_GET_CNF_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_SENSORS_REPORT_GET_CNF_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        if (!decodeSensorReadings (ppInBuffer, &(pOutBuffer->sensorsReportGetCnfUlMsg.sensorReadings)))
                        {
                            decodeResult = DECODE_RESULT_BAD_MSG_FORMAT;
                        }
                    }
                }
                break;
                case SENSORS_REPORT_IND_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_SENSORS_REPORT_IND_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        if (!decodeSensorReadings (ppInBuffer, &(pOutBuffer->sensorsReportIndUlMsg.sensorReadings)))
                        {
                            decodeResult = DECODE_RESULT_BAD_MSG_FORMAT;
                        }
                    }
                }
                break;
                case TRAFFIC_REPORT_GET_CNF_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_TRAFFIC_REPORT_GET_CNF_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->trafficReportGetCnfUlMsg.numDatagramsSent = decodeUint32 (ppInBuffer);
                        pOutBuffer->trafficReportGetCnfUlMsg.numBytesSent = decodeUint32 (ppInBuffer);
                        pOutBuffer->trafficReportGetCnfUlMsg.numDatagramsReceived = decodeUint32 (ppInBuffer);
                        pOutBuffer->trafficReportGetCnfUlMsg.numBytesReceived = decodeUint32 (ppInBuffer);
                    }
                }
                break;
                case TRAFFIC_REPORT_IND_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_TRAFFIC_REPORT_IND_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->trafficReportIndUlMsg.numDatagramsSent = decodeUint32 (ppInBuffer);
                        pOutBuffer->trafficReportIndUlMsg.numBytesSent = decodeUint32 (ppInBuffer);
                        pOutBuffer->trafficReportIndUlMsg.numDatagramsReceived = decodeUint32 (ppInBuffer);
                        pOutBuffer->trafficReportIndUlMsg.numBytesReceived = decodeUint32 (ppInBuffer);
                    }
                }
                break;
                case DEBUG_IND_UL_MSG:
                {
                    decodeResult = DECODE_RESULT_DEBUG_IND_UL_MSG;
                    if (pOutBuffer != NULL)
                    {
                        pOutBuffer->debugIndUlMsg.sizeOfString = (uint32_t) decodeUint32 (ppInBuffer);
                        if (pOutBuffer->debugIndUlMsg.sizeOfString > MAX_DEBUG_STRING_SIZE)
                        {
                            pOutBuffer->debugIndUlMsg.sizeOfString = MAX_DEBUG_STRING_SIZE;
                        }
                        memcpy (&(pOutBuffer->debugIndUlMsg.string[0]), *(ppInBuffer), pOutBuffer->debugIndUlMsg.sizeOfString);
                        *(ppInBuffer) += pOutBuffer->debugIndUlMsg.sizeOfString;
                    }
                }
                break;
                default:
                // The decodeResult will be left as Unknown message
                break;
            }
        }
    }

    return decodeResult;
}

// ----------------------------------------------------------------
// MISC FUNCTIONS
// ----------------------------------------------------------------

// Log debug messages
void MessageCodec::logMsg (const char * pFormat, ...)
{
    char buffer[MAX_DEBUG_MESSAGE_LEN];

    va_list args;
    va_start (args, pFormat);
    vsnprintf (buffer, sizeof (buffer), pFormat, args);
    va_end (args);
#ifdef WIN32
    if (MessageCodec::mp_guiPrintToConsole)
    {
        (*MessageCodec::mp_guiPrintToConsole) (buffer);
    }
#else
    // Must be on ARM
    printf (buffer);
#endif
}

void  MessageCodec::initDll (void (*guiPrintToConsole) (const char *))
{
#ifdef WIN32
    mp_guiPrintToConsole = guiPrintToConsole; 
    // This is the signal to the GUI that we're done with initialisation
    logMsg ("MessageCodec::ready.\n");
#endif
}

// End Of File

/* Teddy interface definition
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

#ifndef TEDDY_API_HPP
#define TEDDY_API_HPP

/**
 * @file teddy_api.h
 * This file defines the API to the teddy device
 */

#include <teddy_msgs.hpp>

// ----------------------------------------------------------------
// GENERAL COMPILE-TIME CONSTANTS
// ----------------------------------------------------------------

/// The revision level of this API
#define REVISION_LEVEL 0

/// The maximum length of a raw datagram in bytes
#define MAX_DATAGRAM_SIZE_RAW 122

/// How often a sensor report is sent to the network
#define DEFAULT_REPORTING_INTERVAL_MINUTES 1

/// How often the sensors are read
#define DEFAULT_HEARTBEAT_SECONDS   10
// ----------------------------------------------------------------
// CLASSES
// ----------------------------------------------------------------

class MessageCodec {
public:

    // ----------------------------------------------------------------
    // MESSAGE ENCODING FUNCTIONS
    // ----------------------------------------------------------------

    /// Encode an uplink message that is sent at power-on of the
    // teddy.  Indicates that the device has been initialised.  After
    // transmission of this message sensor readings will be taken
    // and reported at the indicated rates.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeInitIndUlMsg (char * pBuffer,
                                 InitIndUlMsg_t * pMsg);

    /// Encode a downlink message that reboots the device.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeRebootReqDlMsg (char * pBuffer,
    		                       RebootReqDlMsg_t *pMsg);

    /// Encode an uplink message that contains a debug string.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeDebugIndUlMsg (char * pBuffer,
                                  DebugIndUlMsg_t * pMsg);

    /// Encode a downlink message that retrieves the reading and
    // reporting intervals.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeIntervalsGetReqDlMsg (char * pBuffer);

    /// Encode an uplink message that is sent as a response to a
    // IntervalsGetReqDlMsg.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeIntervalsGetCnfUlMsg (char * pBuffer,
                                         IntervalsGetCnfUlMsg_t * pMsg);

    /// Encode a downlink message that sets the sensor reporting interval.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \return  The number of bytes encoded.
    uint32_t encodeReportingIntervalSetReqDlMsg (char * pBuffer,
                                                 ReportingIntervalSetReqDlMsg_t * pMsg);

    /// Encode an uplink message that is sent as a response to a
    // ReportingIntervalSetReqDlMsg.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeReportingIntervalSetCnfUlMsg (char * pBuffer,
                                                 ReportingIntervalSetCnfUlMsg_t * pMsg);

    /// Encode a downlink message that sets the heartbeat.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \return  The number of bytes encoded.
    uint32_t encodeHeartbeatSetReqDlMsg (char * pBuffer,
                                         HeartbeatSetReqDlMsg_t * pMsg);

    /// Encode an uplink message that is sent as a response to a
    // HeartbeatSetReqDlMsg.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeHeartbeatSetCnfUlMsg (char * pBuffer,
                                         HeartbeatSetCnfUlMsg_t * pMsg);

    /// Encode an uplink message that is sent at the expiry of a
    // reporting period.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    uint32_t encodePollIndUlMsg (char * pBuffer);

    /// Encode a downlink message that retrieves the sensor readings.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeSensorsReportGetReqDlMsg (char * pBuffer);

    /// Encode an uplink message that is sent as a response to a
    // SensorReportGetReqDlMsg.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeSensorsReportGetCnfUlMsg (char * pBuffer,
                                             SensorsReportGetCnfUlMsg_t * pMsg);

    /// Encode an uplink message containing the sensor readings.
    // \param pBuffer  A pointer to the buffer to encode into.  The
    // buffer length must be at least MAX_MESSAGE_SIZE long
    // \param pMsg  A pointer to the message to send.
    // \return  The number of bytes encoded.
    uint32_t encodeSensorsReportIndUlMsg (char * pBuffer,
                                          SensorsReportIndUlMsg_t * pMsg);

    // ----------------------------------------------------------------
    // MESSAGE DECODING FUNCTIONS
    // ----------------------------------------------------------------

    /// The outcome of message decoding.
    //
    // !!! When you add anything to the generic of UL sections
    // here (but not the DL bits as C# never decodes a DL thing),
    // align it with the DLL exported version in the dll wrapper files
    // so that the C# application can decode it.
    typedef enum DecodeResultTag_t
    {
      DECODE_RESULT_FAILURE = 0,         //!< Generic failed decode.
      DECODE_RESULT_INPUT_TOO_SHORT,     //!< Not enough input bytes.
      DECODE_RESULT_OUTPUT_TOO_SHORT,    //!< Not enough room in the
                                         //! output.
      DECODE_RESULT_UNKNOWN_MSG_ID,      //!< Rogue message ID.
      DECODE_RESULT_BAD_MSG_FORMAT,      //!< A problem with the format of a message.
      DECODE_RESULT_DL_MSG_BASE = 0x40,  //!< From here on are the
                                         //! downlink messages.
                                         // !!! If you add one here
                                         // update the next line !!!
      DECODE_RESULT_REBOOT_REQ_DL_MSG = DECODE_RESULT_DL_MSG_BASE,
      DECODE_RESULT_INTERVALS_GET_REQ_DL_MSG,
      DECODE_RESULT_REPORTING_INTERVAL_SET_REQ_DL_MSG,
      DECODE_RESULT_HEARTBEAT_SET_REQ_DL_MSG,
      DECODE_RESULT_SENSORS_REPORT_GET_REQ_DL_MSG, // !!! If you add one here
                                                   // update the next line !!!
      MAX_DL_REQ_MSG = DECODE_RESULT_SENSORS_REPORT_GET_REQ_DL_MSG,
      DECODE_RESULT_UL_MSG_BASE = 0x80,    //!< From here on are the
                                           //! uplink messages.
      DECODE_RESULT_INIT_IND_UL_MSG = DECODE_RESULT_UL_MSG_BASE,
      DECODE_RESULT_INTERVALS_GET_CNF_UL_MSG,
      DECODE_RESULT_REPORTING_INTERVAL_SET_CNF_UL_MSG,
      DECODE_RESULT_HEARTBEAT_SET_CNF_UL_MSG,
      DECODE_RESULT_POLL_IND_UL_MSG,
      DECODE_RESULT_SENSORS_REPORT_GET_CNF_UL_MSG,
      DECODE_RESULT_SENSORS_REPORT_IND_UL_MSG,
      DECODE_RESULT_DEBUG_IND_UL_MSG,  // !!! If you add one here update
                                      // the next line !!!
      MAX_UL_REQ_MSG = DECODE_RESULT_DEBUG_IND_UL_MSG,
      MAX_NUM_DECODE_RESULTS             //!< The maximum number of
                                         //! decode results.
    } DecodeResult_t;

    /// Decode a downlink message. When a datagram has been received
    // this function should be called iteratively to decode all the
    // messages contained within it.  The result, in pOutputBuffer,
    // should be cast by the calling function to DlMsgUnion_t and
    // the relevant member selected according to the
    // DecodeResult_t code.
    // \param ppInBuffer  A pointer to the pointer to decode from.
    // On completion this is pointing to the next byte that
    // could be decoded, after the currently decoded message,
    // in the buffer.
    // \param sizeInBuffer  The number of bytes left to decode.
    // \param pOutBuffer  A pointer to the buffer to write the
    // result into.
    // \param pBytesDecoded A pointer to a place to write the number
    // of bytes decoded.
    // \return  The result of the decoding, which hopefully says
    // what message has been decoded.
    DecodeResult_t decodeDlMsg (const char ** ppInBuffer,
                                uint32_t sizeInBuffer,
                                DlMsgUnion_t * pOutBuffer);

    /// Decode an uplink message. When a datagram has been received
    // this function should be called iteratively to decode all the
    // messages contained within it.  The result, in pOutputBuffer,
    // should be cast by the calling function to UlMsgUnion_t and
    // the relevant member selected according to the
    // DecodeResult_t code.
    // \param ppInBuffer  A pointer to the pointer to decode from.
    // On completion this is pointing to the next byte that
    // could be decoded, after the currently decoded message,
    // in the buffer.
    // \param sizeInBuffer  The number of bytes left to decode.
    // \param pOutBuffer  A pointer to the buffer to write the
    // result into.
    // \return  The result of the decoding, which hopefully says
    // what message has been decoded.
    DecodeResult_t decodeUlMsg (const char ** ppInBuffer,
                                uint32_t sizeInBuffer,
                                UlMsgUnion_t * pOutBuffer);

    // ----------------------------------------------------------------
    // MISC FUNCTIONS
    // ----------------------------------------------------------------

    /// Only used in the DLL form, sets up the "printf()" function
    // for logging.
    // \param guiPrintToConsole  the printf function.
    void initDll (void (*guiPrintToConsole) (const char *));

    /// User callback function for "printf()" logging.  
    static void (*mp_guiPrintToConsole) (const char *);

private:
    /// Encode a boolean value.
    // \param pBuffer  A pointer to where the encoded
    // value should be placed.
    // \param value The Boolean value.
    // \return  The number of bytes encoded.
    uint32_t encodeBool (char * pBuffer, bool value);
    /// Decode a Boolean value.
    // \param ppBuffer  A pointer to the pointer to decode.
    // On completion this points to the location after the
    // bool in the input buffer.
    // \return  The decoded value.
    bool decodeBool (const char ** ppBuffer);
    /// Encode a uint32_t value.
    // \param pBuffer  A pointer to the value to decode.
    // \param value The value.
    // \return  The number of bytes encoded.
    uint32_t encodeUint32 (char * pBuffer, uint32_t value);
    /// Decode a uint32_t value.
    // \param ppBuffer  A pointer to the pointer to decode.
    // On completion this points to the location after the
    // uint32_t in the input buffer.
    uint32_t decodeUint32 (const char ** ppBuffer);
    /// Encode a uint16_t value.
    // \param pBuffer  A pointer to the value to decode.
    // \param value The value.
    // \return  The number of bytes encoded.
    uint32_t encodeUint16 (char * pBuffer, uint16_t value);
    /// Decode a uint16_t value.
    // \param ppBuffer  A pointer to the pointer to decode.
    // On completion this points to the location after the
    // uint16_t in the input buffer.
    uint32_t decodeUint16 (const char ** ppBuffer);
    /// Encode the sensor readings.
    // \param pBuffer         A pointer to the sensor readings to decode.
    // \param pSensorReadings A pointer to the sensor readings.
    // \return  The number of bytes encoded.
    uint32_t encodeSensorReadings (char * pBuffer, SensorReadings_t * pSensorReadings);
    /// Decode the sensor readings into pValue.
    // \param ppBuffer        A pointer to the pointer to decode.
    // On completion this points to the location after the
    // SensorReading_t in the input buffer.
    // \param pSensorReadings A place to put the sensor readings.
    // \return true if the decode is successful, otherwise false.
    bool decodeSensorReadings (const char ** ppBuffer, SensorReadings_t * pSensorReadings);
    /// Log a message for debugging, "printf()" style.
    // \param pFormat The printf() stle parameters.
    void logMsg (const char * pFormat, ...);
};

#endif

// End Of File

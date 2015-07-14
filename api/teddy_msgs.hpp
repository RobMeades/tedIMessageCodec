/* Teddy message definitions.
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

#ifndef TEDDY_MSGS_HPP
#define TEDDY_MSGS_HPP

/**
 * @file teddy_msgs.h
 * This file defines the messages sent between the
 * teddy and a web server.
 *
 * The message format is a 1 byte ID followed by a message
 * of length determined by the specific message type in question.
 */
 
// ----------------------------------------------------------------
// GENERAL COMPILE-TIME CONSTANTS
// ----------------------------------------------------------------

/// The maximum length of a messages in bytes
#define MAX_MESSAGE_SIZE 35

/// The minimum length of a messages in bytes
#define MIN_MESSAGE_SIZE 1

/// The maximum debug string size
#define MAX_DEBUG_STRING_SIZE MAX_MESSAGE_SIZE - sizeof (uint32_t)

/// The lower limits for reading and reporting intervals
// There is an upper limit also but it is set by the
// memory capacity of the target (to store readings before
// the are reported) which is not known here
#define MIN_HEARTBEAT_SECONDS 10
#define MIN_REPORTING_INTERVAL_MINUTES 1

/// Other limits
#define MAX_BATTERY_VOLTAGE_MV 10000
#define MAX_ENERGY_UWH 0xFFFFFF
#define MAX_HUGS_THIS_PERIOD 0x0F
#define MAX_SLAPS_THIS_PERIOD 0x0F
#define MAX_DROPS_THIS_PERIOD 0x0F
#define MAX_NUDGES_THIS_PERIOD 0xFF
#define MAX_RSSI 100
#define MAX_LUMINOSITY 0xFFFF
#define MAX_SOUND_LEVEL 0xFFFF

// ----------------------------------------------------------------
// TYPES
// ----------------------------------------------------------------

/// The wake up code sent from the device.
typedef enum WakeUpCodeTag_t
{
	WAKE_UP_CODE_OK,                          //!< A good wake-up, no problems.
	WAKE_UP_CODE_WATCHDOG,                    //!< Wake-up due to the watchdog
									          //! firing.
	WAKE_UP_CODE_AT_COMMAND_PROBLEM,          //!< Wake-up after assert due to
									          //! problems with AT commands.
	WAKE_UP_CODE_NETWORK_SEND_PROBLEM,        //!< Wake-up after assert due to
								       	      //! problems sending to the network.
	WAKE_UP_CODE_MEMORY_ALLOC_PROBLEM,        //!< Wake-up after assert due to
						       			      //! memory allocation issues.
	WAKE_UP_CODE_PROTOCOL_PROBLEM,            //!< Wake-up after assert due to
							                  //! a protocol problem.
	WAKE_UP_CODE_GENERIC_FAILURE,             //!< Wake-up after a generic failure.
	WAKE_UP_CODE_REBOOT,                      //!< Waking up after a commanded reboot.
	MAX_NUM_WAKE_UP_CODES                     //!< The maximum number of
									          //! decode results.
} WakeUpCode_t;

// ----------------------------------------------------------------
// TYPES FOR SENSOR REPORTING
// ----------------------------------------------------------------

/// The sensor types.
typedef enum SensorTypeTag_t
{
    SENSOR_GPS_POSITION,
    SENSOR_LCL_POSITION,
    SENSOR_SOUND_LEVEL,
    SENSOR_LUMINOSITY,
    SENSOR_TEMPERATURE,
    SENSOR_RSSI,
    SENSOR_POWER_STATE,
    MAX_NUM_SENSORS
} SensorType_t;

/// Structure to hold the GPS Position.
typedef struct GpsPositionTag_t
{
    int32_t latitude;  //!< In thousandths of a minute of arc (so divide by 60,000 to get degrees)
    int32_t longitude; //!< In thousandths of a minute of arc (so divide by 60,000 to get degrees)
    int32_t elevation; //!< In metres
    int32_t speed;     //!< In km/h
} GpsPosition_t;

/// Enum to define the possible orientations (most be codeable into four bits).
typedef enum OrientationTag_t
{
    ORIENTATION_UNCERTAIN = 0x00,
    ORIENTATION_FACE_UP = 0x01,
    ORIENTATION_FACE_DOWN = 0x02,
    ORIENTATION_UPRIGHT = 0x03,
    ORIENTATION_UPSIDE_DOWN = 0x04,
    ORIENTATION_ON_LEFT_SIDE = 0x05,
    ORIENTATION_ON_RIGHT_SIDE = 0x06,
    MAX_NUM_ORIENTATION
} Orientation_t;

/// Structure to hold the local position.
typedef struct LclPositionTag_t
{
    Orientation_t orientation;
    uint8_t hugsThisPeriod;   //!< Only values from 0x00 to 0x0F permitted.
    uint8_t slapsThisPeriod;  //!< Only values from 0x00 to 0x0F permitted.
    uint8_t dropsThisPeriod;  //!< Only values from 0x00 to 0x0F permitted.
    uint8_t nudgesThisPeriod;
} LclPosition_t;

/// Sounds level: 0 for none, 0xFFFF for maximum.
typedef uint16_t SoundLevel_t;

/// Luminosity: 0 for none, 0xFFFF, in lux.
typedef uint16_t Luminosity_t;

/// Temperature -128 to +127, in Centigrade.
typedef int8_t Temperature_t;

/// RSSI, values as defined for AT+CSQ.
typedef uint8_t Rssi_t;

/// Enum to hold the power state (must be codeable into two bits).
typedef enum ChargeStateTag_t
{
    CHARGING_UNKNOWN = 0x00,
    CHARGING_OFF = 0x01,
    CHARGING_ON = 0x02,
    CHARGING_FAULT = 0x03,
    MAX_NUM_CHARGING
} ChargeState_t;

/// Structure to hold the power state.
typedef struct PowerStateTag_t
{
    ChargeState_t chargeState;
    uint16_t batteryMV; //!< battery voltage in mV, max 10,000 mV, will be coded to a resolution of 159 mV (i.e. in 5 bits).
    uint32_t energyUWH;  //!< uWh consumed since the last power state reading, maximum value 0xFFFFFF (24 bits).
} PowerState_t;

/// The overall sensor readings structure.
typedef struct SensorReadingsTag_t
{
    uint32_t  time;              //!< Time in UTC seconds.
    bool gpsPositionPresent;
    GpsPosition_t gpsPosition;
    bool lclPositionPresent;
    LclPosition_t lclPosition;
    bool soundLevelPresent;
    SoundLevel_t soundLevel;
    bool luminosityPresent;
    Luminosity_t luminosity;
    bool temperaturePresent;
    Temperature_t temperature;
    bool rssiPresent;
    Rssi_t rssi;
    bool powerStatePresent;
    PowerState_t powerState;
} SensorReadings_t;

/// Generic control structure for any sensor.
typedef struct SensorControlGenericTag_t
{
    uint32_t readingInterval;         //!< How often, in heartbeats, to take a reading.
    bool useHysteresis;               //!< If true, only report if the value changes by
    uint32_t hysteresisValue;         //! +/-hysteresisValue.
    bool onlyRecordIfPresent;         //!< If true, hysteresis is ignored and reports
    bool onlyRecordIfAboveNotBelow;   //! are sent based on onlyRecordIfValue
    int32_t onlyRecordIfValue;        //! which can be when above or below the value.
    bool onlyRecordIfAtTransitionOnly;//!< If true, only record when the value crosses
                                      //! onlyRecordIfValue.
    bool onlyRecordIfIsOneShot;       //!< If true, return to normal mode after one
                                      //! onlyRecordIfValue.
    bool reportImmediately;           //!< If true, don't wait for the reporting period
                                      //! to expire when there's a reading that meets
                                      //! the hysteresis or onlyRecordIf triggers.
} SensorControlGeneric_t;

/// The sensor control structure specifically for GPS.
typedef struct SensorControlGpsTag_t
{
    bool gpsLatLongPresent;
    SensorControlGeneric_t gpsLatLong;       //!< Values are in arc seconds.
    bool gpsElevPresent;
    SensorControlGeneric_t gpsElev;          //!< Values are in metres.
} SensorControlGps_t;

/// The sensor control structure specifically for LCL
typedef struct SensorControlLclTag_t
{
    bool lclStressSensorOnReportImmediately;
    bool lclOrientationReportImmediately;
    SensorControlGeneric_t lclMovement;      //!< Value applies to any of the three movement directions.
} SensorControlLcl_t;

/// The sensor control structure specifically for PowerState.
typedef struct SensorControlPowerStateTag_t
{
    bool powerStateChargeStateReportImmediately;
    SensorControlGeneric_t powerStateBatteryVoltage;
    SensorControlGeneric_t powerStateBatteryCurrent;
} SensorControlPowerState_t;

/// The union of all sensor control structures.
typedef union SensorControlUnionTag_t
{
    SensorControlGps_t gps;
    SensorControlLcl_t lcl;
    SensorControlGeneric_t soundLevel;
    SensorControlGeneric_t luminosity;
    SensorControlGeneric_t temperature;
    SensorControlGeneric_t rssi;
    SensorControlPowerState_t powerState;
} SensorControlUnion_t;

/// The overall sensors control structure.
typedef struct SensorContolTag_t
{
    SensorType_t sensorType;
    SensorControlUnion_t sensorControl;
} SensorControl_t;

// ----------------------------------------------------------------
// MESSAGE STRUCTURES
// ----------------------------------------------------------------

/// InitIndUlMsg_t.  Sent at power on of the teddy, indicating that it
// has initialised.  The revision level field should will be populated
// automatically by the message codec.
typedef struct InitIndUlMsgTag_t
{
    WakeUpCode_t wakeUpCode;      //!< A wake-up code from the teddy.
    uint16_t     revisionLevel;   //!< Revision level of this messaging protocol.
} InitIndUlMsg_t;

/// RebootReqDlMsg_t. Sent to reboot the teddy and set the development
// mode on or off.  By default development mode is OFF.
typedef struct RebootReqDlMsgTag_t
{
    bool devModeOnNotOff; //!< If true development mode is on, else it is off.
} RebootReqDlMsg_t;

/// IntervalsGetReqDlMsg_t. Get the intervals at which the teddy reads and
// reports sensor readings.
// No structure for this, it's an empty message.

/// IntervalsGetCnfUlMsg_t. The intervals at which the teddy reads and
// reports sensor readings.  Sent in response to IntervalGetReqDlMsg_t.
typedef struct IntervalsGetCnfUlMsgTag_t
{
  uint32_t reportingIntervalMinutes; //!< The interval at which the teddy
                                     //! sends sensor reports.
  uint32_t heartbeatSeconds;         //!< The interval at which the teddy
                                     //! makes sensor readings.
} IntervalsGetCnfUlMsg_t;

/// ReportingIntervalSetReqDlMsg_t. Set the interval at which the teddy sends sensor
// reports.
typedef struct ReportingIntervalSetReqDlMsgTag_t
{
  uint32_t reportingIntervalMinutes; //!< The interval at which the teddy
                                     //! should send sensor reports.
} ReportingIntervalSetReqDlMsg_t;

/// ReportingIntervalSetCnfUlMsg_t. The interval at which the teddy sends sensor
// reports. Sent in response to ReportingIntervalSetReqDlMsg_t.
typedef struct ReportingIntervalSetCnfUlMsgTag_t
{
  uint32_t reportingIntervalMinutes; //!< The interval at which the teddy
                                     //! sends sensor reports.
} ReportingIntervalSetCnfUlMsg_t;

/// HeartbeatSetReqDlMsg_t. Set the interval at which the teddy makes sensor
// measurements.
typedef struct HeartbeatSetReqDlMsgTag_t
{
  uint32_t heartbeatSeconds; //!< The interval at which the teddy
                             //! should make sensor readings.
} HeartbeatSetReqDlMsg_t;

/// HeartbeatSetCnfUlMsg_t. The interval at which the teddy makes sensor
// readings. Sent in response to HeartbeatSetReqDlMsg_t.
typedef struct HeartbeatSetCnfUlMsgTag_t
{
  uint32_t heartbeatSeconds; //!< The interval at which the teddy
                             //! makes sensor readings.
} HeartbeatSetCnfUlMsg_t;

/// PollIndUlMsg_t.  Sent every reporting period in case there are
// no sensor readings to report.
// No structure for this, it's an empty message.

/// SensorsReportIndUlMsg_t.  A set of sensor readings, sent either periodically
// or as a result of some local trigger on the teddy.
typedef struct SensorsReportIndUlMsgTag_t
{
    SensorReadings_t sensorReadings; //!< All the sensor readings.
} SensorsReportIndUlMsg_t;

/// SensorsReportReqDlMsg_t.  Request a set of sensor readings.
// No structure for this, it's an empty message.

/// SensorReportGetCnfUlMsg_t.  A set of sensor readings, sent in response to a
// SensorReportGetReqDlMsg_t.
typedef struct SensorsReportGetCnfUlMsgTag_t
{
    SensorReadings_t sensorReadings; //!< All the sensor readings.
} SensorsReportGetCnfUlMsg_t;

/// TrafficReportIndUlMsg_t.  A report of the traffic data that has occurred
// since the last InitIndUlMsg_t.
typedef struct TrafficReportIndUlMsgTag_t
{
    uint32_t numDatagramsSent;
    uint32_t numBytesSent;
    uint32_t numDatagramsReceived;
    uint32_t numBytesReceived;
} TrafficReportIndUlMsg_t;

/// TrafficReportReqDlMsg_t.  Request a traffic report.
// No structure for this, it's an empty message.

/// TrafficReportGetCnfUlMsg_t.  A report of the traffic data that has occurred
// since InitIndUlMsg_t, sent in response to a TrafficReportGetReqDlMsg_t.
typedef struct TrafficReportGetCnfUlMsgTag_t
{
    uint32_t numDatagramsSent;
    uint32_t numBytesSent;
    uint32_t numDatagramsReceived;
    uint32_t numBytesReceived;
} TrafficReportGetCnfUlMsg_t;

/// DebugIndUlMsg_t.  A generic message containing a debug string.
typedef struct DebugIndUlMsgTag_t
{
    uint32_t sizeOfString;              //!< String size in bytes
    char string[MAX_DEBUG_STRING_SIZE]; //!< The string (not NULL terminated).
} DebugIndUlMsg_t;

// ----------------------------------------------------------------
// MESSAGE UNIONS
// ----------------------------------------------------------------

/// Union of all downlink messages.
typedef union DlMsgUnionTag_t
{
    RebootReqDlMsg_t rebootReqDlMsg;
    ReportingIntervalSetReqDlMsg_t reportingIntervalSetReqDlMsg;
    HeartbeatSetReqDlMsg_t heartbeatSetReqDlMsg;
} DlMsgUnion_t;

/// Union of all uplink messages.
typedef union UlMsgUnionTag_t
{
    InitIndUlMsg_t initIndUlMsg;
    IntervalsGetCnfUlMsg_t intervalsGetCnfUlMsg;
    ReportingIntervalSetCnfUlMsg_t reportingIntervalSetCnfUlMsg;
    HeartbeatSetCnfUlMsg_t heartbeatSetCnfUlMsg;
    SensorsReportGetCnfUlMsg_t sensorsReportGetCnfUlMsg;
    SensorsReportIndUlMsg_t sensorsReportIndUlMsg;
    TrafficReportGetCnfUlMsg_t trafficReportGetCnfUlMsg;
    TrafficReportIndUlMsg_t trafficReportIndUlMsg;
    DebugIndUlMsg_t debugIndUlMsg;
} UlMsgUnion_t;

#endif

// End Of File

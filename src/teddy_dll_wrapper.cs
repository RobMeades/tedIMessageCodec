using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;
using Microsoft.Win32.SafeHandles;
using System.Threading;

namespace MessageCodec
{
    /// <summary>
    /// This is a wrapper around the native dll for MessageCodec calls
    /// </summary>
    public class MessageCodec_dll
    {

        [DllImport ("Kernel32.dll")]
        extern static SafeFileHandle GetStdHandle(Int32 nStdHandle);

        /// <summary>
        /// Windows specific calls to load a library at runtime
        /// </summary>

        [DllImport ("kernel32.dll")]
        internal static extern IntPtr LoadLibrary(String dllname);

        /// <summary>
        /// Windows specific call to get the address of a entry in a dll
        /// </summary>

        [DllImport ("kernel32.dll")]
        internal static extern IntPtr GetProcAddress(IntPtr hModule, String procname);

        /// The outcome of message decoding.
        // !!! ORDER IS IMPORTANT AND THIS MUST align with the generic
        // and UL (but not DL as the C# DLL is not intended to decode
        // a DL message) portions of the C typedef DecodeResult_t.
        public enum CSDecodeResult
        {
          DECODE_RESULT_FAILURE = 0,         //!< Generic failed decode.
          DECODE_RESULT_INPUT_TOO_SHORT,     //!< Not enough input bytes.
          DECODE_RESULT_OUTPUT_TOO_SHORT,    //!< Not enough room in the
                                             //! output.
          DECODE_RESULT_UNKNOWN_MSG_ID,      //!< Rogue message ID.
          DECODE_RESULT_BAD_MSG_FORMAT,      //!< A problem with the format of a message.
          DECODE_RESULT_UL_MSG_BASE = 0x80,  //!< From here on are the
                                             //! uplink messages.
          DECODE_RESULT_INIT_IND_UL_MSG = DECODE_RESULT_UL_MSG_BASE,
          DECODE_RESULT_INTERVALS_GET_CNF_UL_MSG,
          DECODE_RESULT_REPORTING_INTERVAL_SET_CNF_UL_MSG,
          DECODE_RESULT_HEARTBEAT_SET_CNF_UL_MSG,
          DECODE_RESULT_POLL_IND_UL_MSG,
          DECODE_RESULT_SENSORS_REPORT_GET_CNF_UL_MSG,
          DECODE_RESULT_SENSORS_REPORT_IND_UL_MSG,
          DECODE_RESULT_DEBUG_IND_UL_MSG,
          DECODE_RESULT_TRAFFIC_REPORT_GET_CNF_UL_MSG,
          DECODE_RESULT_TRAFFIC_REPORT_IND_UL_MSG, // !!! If you add one here update
                                                   // the next line !!!
          MAX_UL_REQ_MSG = DECODE_RESULT_TRAFFIC_REPORT_IND_UL_MSG,
          MAX_NUM_DECODE_RESULTS             //!< The maximum number of
                                             //! decode results.
        };

        // uint32_t __cdecl maxDatagramSizeRaw (void);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _maxDatagramSizeRaw ();
        public _maxDatagramSizeRaw maxDatagramSizeRaw;

        // uint32_t __cdecl maxDebugStringSize (void);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _maxDebugStringSize();
        public _maxDebugStringSize maxDebugStringSize;

        // uint32_t __cdecl revisionLevel (void);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _revisionLevel();
        public _revisionLevel revisionLevel;

        // uint32_t __cdecl encodeRebootReqDlMsg (char * pBuffer,
        //                                        bool devModeOnNotOff);
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _encodeRebootReqDlMsg(byte* pBuffer,
                                                            Boolean devModeOnNotOff);
        public _encodeRebootReqDlMsg encodeRebootReqDlMsg;

        // uint32_t __cdecl encodeIntervalsGetReqDlMsg (char * pBuffer);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _encodeIntervalsGetReqDlMsg(byte* pBuffer);
        public _encodeIntervalsGetReqDlMsg encodeIntervalsGetReqDlMsg;

        // uint32_t __cdecl encodeReportingIntervalSetReqDlMsg (char * pBuffer,
        //                                                      UInt32 reportingIntervalMinutes);
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _encodeReportingIntervalSetReqDlMsg(byte* pBuffer,
                                                                          UInt32 reportingIntervalMinutes);
        public _encodeReportingIntervalSetReqDlMsg encodeReportingIntervalSetReqDlMsg;

        // uint32_t __cdecl encodeHeartbeatSetReqDlMsg (char * pBuffer,
        //                                              UInt32 heartbeatSeconds);
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _encodeHeartbeatSetReqDlMsg(byte* pBuffer,
                                                                  UInt32 heartbeatSeconds);
        public _encodeHeartbeatSetReqDlMsg encodeHeartbeatSetReqDlMsg;

        // uint32_t __cdecl encodeSensorsReportGetReqDlMsg (char * pBuffer);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _encodeSensorsReportGetReqDlMsg(byte* pBuffer);
        public _encodeSensorsReportGetReqDlMsg encodeSensorsReportGetReqDlMsg;

        // uint32_t __cdecl encodeTrafficReportGetReqDlMsg (char * pBuffer);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate UInt32 _encodeTrafficReportGetReqDlMsg(byte* pBuffer);
        public _encodeTrafficReportGetReqDlMsg encodeTrafficReportGetReqDlMsg;

        // CsDecodeResult_t __cdecl decodeUlMsgType (const char * pInBuffer,
        //                                           uint32_t sizeInBuffer);
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgType(byte* pInBuffer,
                                                               UInt32 sizeInBuffer);
        public _decodeUlMsgType decodeUlMsgType;

        // Boolean __cdecl decodeUlMsgInitInd (const char ** ppInBuffer,
        //                                     uint32_t sizeInBuffer,
        //                                     uint32_t *pWakeUpCode,
        //                                     uint32_t *pRevision);
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgInitInd(byte** ppInBuffer,
                                                                  UInt32 sizeInBuffer,
                                                                  UInt32 *pWakeUpCode,
                                                                  UInt32 *pRevision);
        public _decodeUlMsgInitInd decodeUlMsgInitInd;

        // bool __cdecl decodeUlMsgIntervalsGetCnf (const char ** ppInBuffer,
        //                                          uint32_t sizeInBuffer,
        //                                          uint32_t * pReportingIntervalMinutes,
        //                                          uint32_t * pHeartbeatSeconds)
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgIntervalsGetCnf(byte** ppInBuffer,
                                                                          UInt32 sizeInBuffer,
                                                                          UInt32 *pReportingIntervalMinutes,
                                                                          UInt32 *pHeartbeatSeconds);
        public _decodeUlMsgIntervalsGetCnf decodeUlMsgIntervalsGetCnf;

        // bool __cdecl decodeUlMsgReportingIntervalSetCnf (const char ** ppInBuffer,
        //                                                  uint32_t sizeInBuffer,
        //                                                  uint32_t * pReportingIntervalMinutes)
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgReportingIntervalSetCnf(byte** ppInBuffer,
                                                                                  UInt32 sizeInBuffer,
                                                                                  UInt32 *pReportingIntervalMinutes);
        public _decodeUlMsgReportingIntervalSetCnf decodeUlMsgReportingIntervalSetCnf;

        // bool __cdecl decodeUlMsgHeartbeatSetCnf (const char ** ppInBuffer,
        //                                          uint32_t sizeInBuffer,
        //                                          uint32_t * pHeartbeatSeconds)
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgHeartbeatSetCnf(byte** ppInBuffer,
                                                                          UInt32 sizeInBuffer,
                                                                          UInt32 *pHeartbeatSeconds);
        public _decodeUlMsgHeartbeatSetCnf decodeUlMsgHeartbeatSetCnf;

        // Boolean __cdecl decodeUlMsgPollInd (const char ** ppInBuffer,
        //                                     uint32_t sizeInBuffer);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgPollInd(byte** ppInBuffer,
                                                                  UInt32 sizeInBuffer);
        public _decodeUlMsgPollInd decodeUlMsgPollInd;

        // bool __cdecl decodeUlMsgSensorsReportxxx (const char ** ppInBuffer,
        //                                           uint32_t sizeInBuffer,
        //                                           uint32_t * pTime,
        //                                           bool * pGpsPositionPresent,
        //                                           int32_t * pGpsPositionLatitude,
        //                                           int32_t * pGpsPositionLongitude,
        //                                           int32_t * pGpsPositionElevation,
        //                                           int32_t * pGpsPositionSpeed,
        //                                           bool * pLclPositionPresent,
        //                                           uint32_t * pLclPositionOrientation,
        //                                           uint32_t * pLclPositionHugsThisPeriod,
        //                                           uint32_t * pLclPositionSlapsThisPeriod,
        //                                           uint32_t * pLclPositionDropsThisPeriod,
        //                                           uint32_t * pLclPositionNudgesThisPeriod,
        //                                           bool * pSoundLevelPresent,
        //                                           uint32_t * pSoundLevel,
        //                                           bool * pLuminosityPresent,
        //                                           uint32_t * pLuminosity,
        //                                           bool * pTemperaturePresent,
        //                                           int32_t * pTemperature,
        //                                           bool * pRssiPresent,
        //                                           uint32_t *pRssi,
        //                                           bool * pPowerStatePresent,
        //                                           uint32_t *pPowerStateChargeState,
        //                                           uint32_t *pPowerStateBatteryMV,
        //                                           int32_t *pPowerStateEnergyUAH)
        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgSensorsReportxxx(byte** ppInBuffer,
                                                                           UInt32 sizeInBuffer,
                                                                           UInt32 * pTime,
                                                                           Boolean * pGpsPositionPresent,
                                                                           Int32 * pGpsPositionLatitude,
                                                                           Int32 * pGpsPositionLongitude,
                                                                           Int32 * pGpsPositionElevation,
                                                                           Int32* pGpsPositionSpeed,
                                                                           Boolean* pLclPositionPresent,
                                                                           UInt32 * pLclPositionOrientation,
                                                                           UInt32 * pLclPositionHugsThisPeriod,
                                                                           UInt32 * pLclPositionSlapsThisPeriod,
                                                                           UInt32 * pLclPositionDropsThisPeriod,
                                                                           UInt32 * pLclPositionNudgesThisPeriod,
                                                                           Boolean* pSoundLevelPresent,
                                                                           UInt32 * pSoundLevel,
                                                                           Boolean * pLuminosityPresent,
                                                                           UInt32 * pLuminosity,
                                                                           Boolean * pTemperaturePresent,
                                                                           Int32 * pTemperature,
                                                                           Boolean * pRssiPresent,
                                                                           UInt32 *pRssi,
                                                                           Boolean * pPowerStatePresent,
                                                                           UInt32 *pPowerStateChargeState,
                                                                           UInt32 * pPowerStateBatteryMV,
                                                                           Int32* pPowerStateEnergyUAH);
        public _decodeUlMsgSensorsReportxxx decodeUlMsgSensorsReportxxx;

        // bool __cdecl decodeUlMsgTrafficReportGetCnf (const char ** ppInBuffer,
        //                                              uint32_t sizeInBuffer,
        //                                              uint32_t * pNumDatagramsSent,
        //                                              uint32_t * pNumBytesSent,
        //                                              uint32_t * pNumDatagramsReceived,
        //                                              uint32_t * pNumBytesReceived)
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgTrafficReportGetCnf(byte** ppInBuffer,
                                                                              UInt32 sizeInBuffer,
                                                                              UInt32* pNumDatagramsSent,
                                                                              UInt32* pNumBytesSent,
                                                                              UInt32* pNumDatagramsReceived,
                                                                              UInt32* pNumBytesReceived);
        public _decodeUlMsgTrafficReportGetCnf decodeUlMsgTrafficReportGetCnf;

        // bool __cdecl decodeUlMsgTrafficReportInd (const char ** ppInBuffer,
        //                                           uint32_t sizeInBuffer,
        //                                           uint32_t * pNumDatagramsSent,
        //                                           uint32_t * pNumBytesSent,
        //                                           uint32_t * pNumDatagramsReceived,
        //                                           uint32_t * pNumBytesReceived)
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgTrafficReportInd(byte** ppInBuffer,
                                                                           UInt32 sizeInBuffer,                                                                              UInt32* pNumDatagramsSent,
                                                                           UInt32* pNumBytesSent,
                                                                           UInt32* pNumDatagramsReceived,
                                                                           UInt32* pNumBytesReceived);
        public _decodeUlMsgTrafficReportInd decodeUlMsgTrafficReportInd;

        // Boolean __cdecl decodeUlMsgDebugInd (const char ** ppInBuffer,
        //                                      uint32_t sizeInBuffer
        //                                      uint32_t *pSizeOfString
        //                                      char *pString);
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public unsafe delegate CSDecodeResult _decodeUlMsgDebugInd(byte** ppInBuffer,
                                                                   UInt32 sizeInBuffer,
                                                                   UInt32* pSizeOfString,
                                                                   byte *pString);
        public _decodeUlMsgDebugInd decodeUlMsgDebugInd;

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void guiPrintToConsoleCallback(StringBuilder data);

        [UnmanagedFunctionPointer (CallingConvention.Cdecl)]
        public delegate void _initDll([MarshalAs (UnmanagedType.FunctionPtr)] guiPrintToConsoleCallback callbackPointer);
        public _initDll initDll;

        void guiPrintToConsole(StringBuilder data)
        {
            if (onConsoleTrace != null)
            {
                onConsoleTrace (data.ToString ());
            }
        }

        /// <summary>
        /// Load the dll and do the bindings
        /// </summary>
        /// <param name="dllLocation">location of dll</param>
        public void bindDll(string dllLocation)
        {
            IntPtr ptrDll = LoadLibrary (dllLocation);

            if (ptrDll == IntPtr.Zero) throw new Exception (String.Format ("Cannot find {0}", dllLocation));

            maxDatagramSizeRaw = (_maxDatagramSizeRaw)bindItem(ptrDll, "maxDatagramSizeRaw", typeof(_maxDatagramSizeRaw));
            maxDebugStringSize = (_maxDebugStringSize)bindItem(ptrDll, "maxDebugStringSize", typeof(_maxDebugStringSize));
            revisionLevel = (_revisionLevel)bindItem(ptrDll, "revisionLevel", typeof(_revisionLevel));
            encodeRebootReqDlMsg = (_encodeRebootReqDlMsg)bindItem(ptrDll, "encodeRebootReqDlMsg", typeof(_encodeRebootReqDlMsg));
            encodeIntervalsGetReqDlMsg = (_encodeIntervalsGetReqDlMsg)bindItem(ptrDll, "encodeIntervalsGetReqDlMsg", typeof(_encodeIntervalsGetReqDlMsg));
            encodeReportingIntervalSetReqDlMsg = (_encodeReportingIntervalSetReqDlMsg)bindItem(ptrDll, "encodeReportingIntervalSetReqDlMsg", typeof(_encodeReportingIntervalSetReqDlMsg));
            encodeHeartbeatSetReqDlMsg = (_encodeHeartbeatSetReqDlMsg)bindItem(ptrDll, "encodeHeartbeatSetReqDlMsg", typeof(_encodeHeartbeatSetReqDlMsg));
            encodeSensorsReportGetReqDlMsg = (_encodeSensorsReportGetReqDlMsg)bindItem(ptrDll, "encodeSensorsReportGetReqDlMsg", typeof(_encodeSensorsReportGetReqDlMsg));
            encodeTrafficReportGetReqDlMsg = (_encodeTrafficReportGetReqDlMsg)bindItem(ptrDll, "encodeTrafficReportGetReqDlMsg", typeof(_encodeTrafficReportGetReqDlMsg));
            decodeUlMsgType = (_decodeUlMsgType)bindItem(ptrDll, "decodeUlMsgType", typeof(_decodeUlMsgType));
            decodeUlMsgInitInd = (_decodeUlMsgInitInd)bindItem(ptrDll, "decodeUlMsgInitInd", typeof(_decodeUlMsgInitInd));
            decodeUlMsgIntervalsGetCnf = (_decodeUlMsgIntervalsGetCnf)bindItem(ptrDll, "decodeUlMsgIntervalsGetCnf", typeof(_decodeUlMsgIntervalsGetCnf));
            decodeUlMsgReportingIntervalSetCnf = (_decodeUlMsgReportingIntervalSetCnf)bindItem(ptrDll, "decodeUlMsgReportingIntervalSetCnf", typeof(_decodeUlMsgReportingIntervalSetCnf));
            decodeUlMsgHeartbeatSetCnf = (_decodeUlMsgHeartbeatSetCnf)bindItem(ptrDll, "decodeUlMsgHeartbeatSetCnf", typeof(_decodeUlMsgHeartbeatSetCnf));
            decodeUlMsgPollInd = (_decodeUlMsgPollInd)bindItem(ptrDll, "decodeUlMsgPollInd", typeof(_decodeUlMsgPollInd));
            decodeUlMsgSensorsReportxxx = (_decodeUlMsgSensorsReportxxx)bindItem(ptrDll, "decodeUlMsgSensorsReportxxx", typeof(_decodeUlMsgSensorsReportxxx));
            decodeUlMsgTrafficReportGetCnf = (_decodeUlMsgTrafficReportGetCnf)bindItem(ptrDll, "decodeUlMsgTrafficReportGetCnf", typeof(_decodeUlMsgTrafficReportGetCnf));
            decodeUlMsgTrafficReportInd = (_decodeUlMsgTrafficReportInd)bindItem(ptrDll, "decodeUlMsgTrafficReportInd", typeof(_decodeUlMsgTrafficReportInd));
            decodeUlMsgDebugInd = (_decodeUlMsgDebugInd)bindItem(ptrDll, "decodeUlMsgDebugInd", typeof(_decodeUlMsgDebugInd));
            initDll = (_initDll)bindItem(ptrDll, "initDll", typeof(_initDll));
            initDll (guiPrintToConsole);
        }

        public object bindItem(IntPtr ptrDll, string dllFuncName, Type type)
        {
            // Get pointer to dllexport function
            IntPtr procaddr = GetProcAddress (ptrDll, dllFuncName);
            if (procaddr == IntPtr.Zero) throw new Exception (String.Format ("Cannot find {0}", dllFuncName));

            // Bind it to the function
            Object result = Marshal.GetDelegateForFunctionPointer (procaddr, type);
            if (result == null) throw new Exception (String.Format ("Cannot bind to {0}", dllFuncName));

            return result;
        }

        public delegate void ConsoleTrace(string data);
        public event ConsoleTrace onConsoleTrace;
    }
}
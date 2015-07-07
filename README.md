# tedIMessageCodec
Message codec used for communicating with tedI.

A simple MSC of the most important messages would be as follows, where the items in brackest are optional and the downlink Req's may be sent at any time:

```
                   tedI                    Server
Power On        InitInd           ->                            Indicates WakeUp code (normally 0) and revisionLevel of API.
                SensorsReportInd  ->                            Initial report from all the sensors.
               [                  <-   IntervalsGetReq          Get the Heartbeat and Reporting intervals.]
               [                  ->   IntervalsGetCnf          The Heartbeat and Reporting intervals.]
               [                  <-   ReportingIntervalSetReq  Set the Reporting interval.]
               [                  <-   ReportingIntervalSetCnf  Confirmation of the above.]
               [                  <-   HeartbeatSetReq          Set the Heartbeat.]
               [                  <-   HeartbeatSetCnf          Confirmation of the above.]
                                 ....
End of          PollInd           ->                            Indicates "reporting in", at the end of a reporting period.
Reporting      [SensorsReportInd  ->                            Report from sensors for heartbeat n.]
Interval  x    [SensorsReportInd  ->                            Report from sensors for heartbeat n + 1.]
               [SensorsReportInd  ->                            Report from sensors for heartbeat n + m.]
                                 ....
End of          PollInd           ->                            Indicates "reporting in", at the end of a reporting period.
Reporting      [SensorsReportInd  ->                            Report from sensors for heartbeat n + m + 1.]
Interval x + 1 [SensorsReportInd  ->                            Report from sensors for heartbeat n + m + 2.]
               [SensorsReportInd  ->                            Report from sensors for heartbeat n + m + 3.]
```            
Note that the above refer to messages and that multiple messages may be packed into a single datagram in order to optimise transmission time/power.


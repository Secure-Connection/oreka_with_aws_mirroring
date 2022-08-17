//
// Created by arik halperin on 17/08/2022.
//

#ifndef OREKA_WITH_AWS_MIRRORING_QMETRICS_H
#define OREKA_WITH_AWS_MIRRORING_QMETRICS_H

#include "QMetricsProxy.h"
#include "PacketHeaderDefs.h"

#define QMETRICS_CALL_TIMEOUT 60 * 120

struct QMetricsCall {
    int64_t call_time;
    CStdString number;
    QMetricsCall(CStdString &number) : call_time(time(NULL)), number(number) {}
};

typedef oreka::shared_ptr<QMetricsCall> QMetricsCallRef;

class QMetrics : public QMetricsFunctor {
    static QMetrics * s_instance;
    std::mutex m_mutex;
    std::map<CStdString, QMetricsCallRef> qmetrics_calls;
public:
    QMetrics();
    static QMetrics * instance();
    virtual CStdString FinishCall(CStdString local_party, CStdString remote_party);
    void HandlePacket(EthernetHeaderStruct* ethernetHeader, IpHeaderStruct* ipHeader, int ipHeaderLength, u_char* ipPacketEnd);
    void HandleNewQmetricsCall(char *number_agent, char *number_ext);
    void clear_stale_calls();
    void set_call_time(CStdString key, int64_t call_time);
    void dump_calls();
};


#endif //OREKA_WITH_AWS_MIRRORING_QMETRICS_H

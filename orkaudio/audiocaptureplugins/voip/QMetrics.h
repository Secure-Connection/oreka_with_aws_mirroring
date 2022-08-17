//
// Created by arik halperin on 17/08/2022.
//

#ifndef OREKA_WITH_AWS_MIRRORING_QMETRICS_H
#define OREKA_WITH_AWS_MIRRORING_QMETRICS_H

#include "QMetricsProxy.h"

QMetricsFunctor a;

#include "PacketHeaderDefs.h"

class QMetrics : public QMetricsFunctor {
    static QMetrics * s_instance;

public:
    static QMetrics * instance();
    CStdString FinishCall(CStdString & local_party, CStdString & remote_party);
    void HandlePacket(EthernetHeaderStruct* ethernetHeader, IpHeaderStruct* ipHeader, int ipHeaderLength, u_char* ipPacketEnd);
};


#endif //OREKA_WITH_AWS_MIRRORING_QMETRICS_H

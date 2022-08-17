//
// Created by arik halperin on 17/08/2022.
//

#ifndef OREKA_WITH_AWS_MIRRORING_QMETRICS_H
#define OREKA_WITH_AWS_MIRRORING_QMETRICS_H


class QMetrics {

public:
    static void HandlePacket(EthernetHeaderStruct* ethernetHeader, IpHeaderStruct* ipHeader, int ipHeaderLength, u_char* ipPacketEnd);
    static CStdString FinishCall(CStdString & local_party, CStdString & remote_party);
};


#endif //OREKA_WITH_AWS_MIRRORING_QMETRICS_H

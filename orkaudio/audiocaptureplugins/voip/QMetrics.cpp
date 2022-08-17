//
// Created by arik halperin on 17/08/2022.
//

#include "QMetrics.h"

QMetrics * QMetrics::s_instance = NULL;

QMetrics *QMetrics::instance() {
    if(s_instance==NULL) {
        s_instance = new QMetrics;
    }
    return s_instance;
}


QMetrics::QMetrics() {
    QMetricsProxy::SetFinishCallFunctor(this);
}


extern void log_to_packet_log(CStdString msg);

CStdString QMetrics::FinishCall(CStdString local_party, CStdString remote_party) {
    CStdString msg;
    msg.Format("Finished Call:%s %s", local_party, remote_party);
    log_to_packet_log(msg);
    return local_party;
}

void QMetrics::HandlePacket(EthernetHeaderStruct* ethernetHeader, IpHeaderStruct* ipHeader, int ipHeaderLength, u_char* ipPacketEnd) {

}
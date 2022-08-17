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


CStdString QMetrics::FinishCall(CStdString local_party, CStdString remote_party) {
    return "";
}

void QMetrics::HandlePacket(EthernetHeaderStruct* ethernetHeader, IpHeaderStruct* ipHeader, int ipHeaderLength, u_char* ipPacketEnd) {

}
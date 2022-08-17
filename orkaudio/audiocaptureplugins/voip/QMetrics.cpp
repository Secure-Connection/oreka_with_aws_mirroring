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

    CStdString result = local_party;

    MutexSentinel mutexSentinel(m_mutex); // serialize access for competing pcap threads and tape finish
    const auto &it = qmetrics_calls.find(remote_party);
    if (it != qmetrics_calls.end()) {
        log_to_packet_log("Found QMetrics Call");
        if local_party=="anonymous":
            msg.Format("Replacing anonymous with %s",it->second->number);
            log_to_packet_log(msg);
            result = it->second->number;
        qmetrics_calls.erase(it);
    }
    return result;
}


inline char *clone(char *src, std::size_t len) {
    char *res = new char[len + 1];
    memcpy(res, src, len);
    res[len] = 0;
    return res;
}

void QMetrics::HandlePacket(EthernetHeaderStruct* ethernetHeader, IpHeaderStruct* ipHeader, int ipHeaderLength, u_char* ipPacketEnd) {
    TcpHeaderStruct* tcpHeader = (TcpHeaderStruct*)((char *)ipHeader + ipHeaderLength);
    char* startTcpPayload = (char*)tcpHeader + (tcpHeader->off * 4);
    int payloadLength = ntohs(ipHeader->ip_len) - (ipHeader->ip_hl*4) - TCP_HEADER_LENGTH;
    if (strncmp("Action: Originate\r", startTcpPayload, 18)) {
        return;
    }

    log_to_packet_log("Got new Qmetrics message");

    char  *number_agent = nullptr, *number_ext = nullptr;
    for (char *token = strtok(startTcpPayload, "\r\n"); token; token = strtok(nullptr, "\r\n")) {
        // get agent number
        if (!strncmp("Variable: AGENTCODE=", token, 20)) {
            if (!number_agent) {
                number_agent = clone(token + 20, strlen(token + 20));
            }
        }
         // get ext number
        else if (!strncmp("Variable: EXTTODIAL=", token, 20)) {
            if (!number_ext) {
                number_ext = clone(token + 20, strlen(token + 20));
            }
        }
    }

    // if we got both numbers, use its data
    if (number_agent && number_ext) {
        HandleNewQmetricsCall(number_agent, number_ext);
    }


    delete[] number_agent;
    delete[] number_ext;
}


void QMetrics::HandleNewQmetricsCall(char *number_agent, char *number_ext) {
    CStdString msg;
    msg.Format("Qmetrics message:%s %s", number_agent, number_ext);
    log_to_packet_log(msg);


    MutexSentinel mutexSentinel(m_mutex); // serialize access for competing pcap threads and tape finish

    CStdString key(number_ext), number(number_agent);

    // if the call is already active, update the info
    const auto &it = qmetrics_calls.find(key);
    if (it != active_calls.end()) {
        qmetrics_calls.erase(it);
    }
    qmetrics_calls[key] = QMetricsCallRef(new QMetricsCall(number));
    clear_stale_calls();
}


void QMetrics::clear_stale_calls() {
    std::vector<CStdString> remove;
    int64_t now = time(NULL);
    for (const auto &call : qmetrics_calls) {
        if (now - call.second->call_time > QMETRICS_CALL_TIMEOUT) {
            CStdString msg;
            msg.Format("Gave up on QMETRICS call:%s", call.second->number);
            log_to_packet_log(msg);
            remove.push_back(call.first);
        }
    }
    for (const auto &to_remove : remove) {
        qmetrics_calls.erase(to_remove);
    }
}
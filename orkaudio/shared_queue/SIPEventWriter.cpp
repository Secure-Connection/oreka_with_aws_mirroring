//
// Created by arik halperin on 05/11/2020.
//

#include "SIPEventWriter.h"

SIPEventWriter *SIPEventWriter::s_instance = nullptr;

SIPEventWriter *SIPEventWriter::instance() {
    if (s_instance == nullptr) {
        s_instance = new SIPEventWriter();
    }
    return s_instance;
}


bool SIPEventWriter::write_sip_event(std::string caller, std::string callee, std::string session_id, uint64_t eventTime, SipEventType eventType) {
    SIPEvent event{};
    strncpy(event.caller, caller.c_str(), sizeof(event.caller));
    strncpy(event.callee, callee.c_str(), sizeof(event.callee));
    strncpy(event.session_id, session_id.c_str(), sizeof(event.session_id));
    event.time_from_epoch_micros = eventTime;
    event.eventType = eventType;

    if(is_full()) {
	    printf("Queue is full!!!!");
	    exit(0);
        return false;
    }

    write_element((unsigned char *)&event);

    return true;
}
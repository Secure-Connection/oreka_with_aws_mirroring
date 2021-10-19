//
// Created by arik halperin on 05/11/2020.
//

#include "SIPEventWriter.h"

SIPEventWriter *SIPEventWriter::s_instance = nullptr;

SIPEventWriter * SIPEventWriter::instance() {
    if(s_instance==NULL) {
        s_instance = new SIPEventWriter();
    }
    return s_instance;
}


bool SIPEventWriter::write_sip_event(std::string caller, std::string callee, std::string session_id, uint64_t eventTime, SipEventType eventType ){
    SIPEvent event;
    strncpy(event.caller,caller.c_str(),32);
    strncpy(event.callee,callee.c_str(),32);
    strncpy(event.session_id,session_id.c_str(),64);
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

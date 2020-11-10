//
// Created by arik halperin on 05/11/2020.
//

#include "SIPEventWriter.h"

int SIPEventWriter::get_queue_size() {
    return 512;
}
int SIPEventWriter::get_element_size() {
    return sizeof(SIPEvent);
}
virtual int SIPEventWriter::get_queue_identifier() {
    return 0x1790;
}



SIPEventWriter * SIPEventWriter::instance() {
    if(s_instance==NULL) {
        s_instance = new SIPEventWriter();
    }
    return s_instance;
}


bool SIPEventWriter::write_sip_event(CStdString caller, CStdString callee, CStdString session_id, uint64_t eventTime, SipEventType eventType ){
    SIPEvent event;

    strncpy(event.caller,caller.c_str(),32);
    strncpy(event.callee,callee.c_str(),32);
    strncpy(event.session_id,session_id.c_str(),64);
    event.time_from_epoch = eventTime;
    event.eventType = eventType;

    if(is_full()) {
        return false;
    }

    write_element(&event);

    return true;
}
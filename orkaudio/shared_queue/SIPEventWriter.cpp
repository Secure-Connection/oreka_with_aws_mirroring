//
// Created by arik halperin on 05/11/2020.
//

#include "SIPEventWriter.h"
static LoggerPtr s_parsersLog = Logger::getLogger("parsers.sip");


SIPEventWriter * SIPEventWriter::s_instance= NULL;

SIPEventWriter * SIPEventWriter::instance() {
    if(s_instance==NULL) {
        s_instance = new SIPEventWriter();
    }
    return s_instance;
}


bool SIPEventWriter::write_sip_event(CStdString caller, CStdString callee, CStdString session_id, uint64_t eventTime, SipEventType eventType ){
    SIPEvent event;
    LOG4CXX_INFO(s_parsersLog, "write_sip_event entry");
    /*strncpy(event.caller,caller.c_str(),32);
    strncpy(event.callee,callee.c_str(),32);
    strncpy(event.session_id,session_id.c_str(),64);
    event.time_from_epoch_micros = eventTime;
    event.eventType = eventType;

    if(is_full()) {
        return false;
    }

    write_element((unsigned char *)&event);*/

    return true;
}
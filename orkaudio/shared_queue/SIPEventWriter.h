//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_SIPEVENTWRITER_H
#define OREKA_SIPEVENTWRITER_H

#include "SharedMemoryQueueWriter.h"

enum SipEventType { SIP_START, SIP_STOP };

typedef struct {
    char caller[32];
    char callee[32];
    char session_id[64];
    uint64_t time_from_epoch_micros;
    SipEventType eventType;
}SIPEvent;

class SIPEventWriter: public SharedMemoryQueueWriter {
    static SIPEventWriter * s_instance;

    virtual int get_queue_size();
    virtual int get_element_size();
    virtual int get_queue_identifier();
public:
    static SIPEventWriter * instance();
    virtual bool write_sip_event(CStdString caller, CStdString callee, CStdString session_id, uint64_t time, SipEventType eventType );
};


#endif //OREKA_SIPEVENTWRITER_H

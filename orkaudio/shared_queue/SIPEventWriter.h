//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_SIPEVENTWRITER_H
#define OREKA_SIPEVENTWRITER_H

#include "SharedMemoryQueueWriter.h"
#include <string>

enum SipEventType : uint8_t { SIP_START, SIP_START_INVITE, SIP_STOP };

typedef struct {
    char caller[32]{};
    char callee[32]{};
    char session_id[64]{};
    uint64_t time_from_epoch_micros;
    SipEventType eventType;
}SIPEvent;

class SIPEventWriter: public SharedMemoryQueueWriter {
    static SIPEventWriter * s_instance;

public:
    SIPEventWriter():SharedMemoryQueueWriter(  0x90,(int) sizeof(SIPEvent), 512) {}
    static SIPEventWriter * instance();
    virtual bool write_sip_event(std::string caller, std::string callee, std::string session_id, uint64_t time, SipEventType eventType );
};


#endif //OREKA_SIPEVENTWRITER_H

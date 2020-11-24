//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_AUDIODATAWRITER_H
#define OREKA_AUDIOATAWRITER_H

#include "SharedMemoryQueueWriter.h"

typedef struct {
    char session_id[64];
    unsigned char channel;
    uint64_t timestamp;
    uint16_t data_length;
    unsigned char payload_type;
    unsigned char pcmdata[512];
}PCMData;

class AudioDataWriter: public SharedMemoryQueueWriter {
    static AudioDataWriter * s_instance;
public:
    AudioDataWriter(): SharedMemoryQueueWriter(0x89, (int)(sizeof(PCMData)), 2048) {}

    static AudioDataWriter * instance();
    bool write_pcm_data(std::string session_id, int channel, int payload_type, int data_length, unsigned char * audio_data, uint64_t timestamp);
};


#endif //OREKA_AUDIODATAWRITER_H

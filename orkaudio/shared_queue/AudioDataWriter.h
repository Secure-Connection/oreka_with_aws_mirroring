//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_AUDIODATAWRITER_H
#define OREKA_AUDIOATAWRITER_H

#include <queue>
#include "SharedMemoryQueueWriter.h"

struct PCMData {
    char session_id[37]{};
    unsigned char channel;
    int sequence_number;
    uint32_t timestamp;
    uint16_t data_length;
    unsigned char payload_type;
    unsigned char pcmdata[512]{};
};

class AudioDataWriter : public SharedMemoryQueueWriter {
    static AudioDataWriter *s_instance;
    static std::queue<PCMData> s_backlog;

public:
    AudioDataWriter() : SharedMemoryQueueWriter(0x89, (int) (sizeof(PCMData)), 2048) {}

    static AudioDataWriter *instance();

    bool write_pcm_data(std::string session_id, int channel, int payload_type, int sequence_number, int data_length,
                        unsigned char *audio_data, uint64_t timestamp);
};


#endif //OREKA_AUDIODATAWRITER_H

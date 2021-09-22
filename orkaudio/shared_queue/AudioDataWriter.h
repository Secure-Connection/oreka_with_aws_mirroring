//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_AUDIODATAWRITER_H
#define OREKA_AUDIOATAWRITER_H

#include <queue>
#include "SharedMemoryQueueWriter.h"

struct PCMData {
    static constexpr uint8_t SESSION_ID_LENGTH = 64;// FIXME: move to where it actually belongs
    static constexpr uint16_t MAX_DATA_LENGTH = 512;

    char session_id[PCMData::SESSION_ID_LENGTH];
    unsigned char channel;
    int sequence_number;
    uint32_t timestamp;
    uint16_t data_length;
    unsigned char payload_type;
    unsigned char pcmdata[PCMData::MAX_DATA_LENGTH];
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

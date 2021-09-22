//
// Created by arik halperin on 05/11/2020.
//

#include "AudioDataWriter.h"

AudioDataWriter *AudioDataWriter::s_instance = nullptr;
std::queue<PCMData> AudioDataWriter::s_backlog;

AudioDataWriter *AudioDataWriter::instance() {
    if (s_instance == NULL) {
        s_instance = new AudioDataWriter();
    }
    return s_instance;
}

// FIXME: when SHMEM frees up it should auto write the backlog
bool AudioDataWriter::write_pcm_data(std::string session_id, int channel, int payload_type, int sequence_number,
                                     int data_length, uint8_t *audio_data, uint64_t timestamp) {

    // before sending the supplied data, flush whatever we can from the backlog
    while (!is_full() && !s_backlog.empty()) {
        write_element((uint8_t *) &s_backlog.front());
        s_backlog.pop();
    }

    // ensure we don't overflow the queue, memcpy_s doesn't exist in UNIX
    assert(data_length <= PCMData::MAX_DATA_LENGTH);

    // create the struct to retain the PCM data
    PCMData element_memory;
    session_id.copy(element_memory.session_id, PCMData::SESSION_ID_LENGTH);
    element_memory.channel = channel;
    element_memory.timestamp = timestamp;
    element_memory.payload_type = payload_type;
    element_memory.data_length = data_length;
    element_memory.sequence_number = sequence_number;
    memcpy(element_memory.pcmdata, audio_data, data_length);

    // if SHMEM is full, push the supplied data unto the backlog
    if (is_full()) {
        s_backlog.push(element_memory);
        return false;
    }

    // SHMEM isn't full, so write the data
    write_element((uint8_t *) &element_memory);
    return true;
}
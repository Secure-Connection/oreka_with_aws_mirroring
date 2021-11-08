//
// Created by arik halperin on 05/11/2020.
//

#include "AudioDataWriter.h"
#include <cassert>

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
    assert(data_length < sizeof(element.session_id));

    // create the struct to retain the PCM data
    PCMData element{};
    strncpy(element.session_id, session_id.c_str(), sizeof(element.session_id));
    element.session_id[sizeof(element.session_id) - 1] = 0;
    element.channel = channel;
    element.timestamp = timestamp;
    element.payload_type = payload_type;
    element.data_length = data_length;
    element.sequence_number = sequence_number;
    memcpy(element.pcmdata, audio_data, data_length);

    // if SHMEM is full, push the supplied data unto the backlog
    if (is_full()) {
        s_backlog.push(element);
        return false;
    }

    // SHMEM isn't full, so write the data
    write_element((uint8_t *) &element);
    return true;
}
//
// Created by arik halperin on 05/11/2020.
//

#include "AudioDataWriter.h"

AudioDataWriter * AudioDataWriter::s_instance = new AudioDataWriter();
AudioDataWriter * AudioDataWriter::instance() {
    if(s_instance==NULL) {
        s_instance = new AudioDataWriter();
    }
    return s_instance;
}


bool AudioDataWriter::write_pcm_data(std::string session_id, int channel, int payload_type, int data_length, unsigned char * audio_data, uint64_t timestamp) {
    if(is_full()) {
        return false;
    }

    unsigned char *element_memory = (unsigned char*)malloc(sizeof(PCMData));

    strcpy(((PCMData *)element_memory)->session_id,session_id.c_str());
    ((PCMData *)element_memory)->channel = channel;
    ((PCMData *)element_memory)->timestamp = timestamp;
    ((PCMData *)element_memory)->payload_type = payload_type;
    ((PCMData *)element_memory)->data_length = data_length;
    memcpy(((PCMData *)element_memory)->pcmdata, audio_data, data_length);

    write_element(element_memory);

    free(element_memory);
}
//
// Created by arik halperin on 05/11/2020.
//

#include "AudioDataWriter.h"

AudioDataWriter * AudioDataWriter::s_instance = NULL;

AudioDataWriter * AudioDataWriter::instance() {
    if(s_instance==NULL) {
        s_instance = new AudioDataWriter();
    }
    return s_instance;
}


bool AudioDataWriter::write_pcm_data(CStdString session_id, int channel, int payload_type, int data_length, unsigned char * audio_data) {
    if(is_full()) {
        return false;
    }

    unsigned char *element_memory = (unsigned char*)malloc(sizeof(PCMData)+512);

    write_element(element_memory);

    free(element_memory);

}
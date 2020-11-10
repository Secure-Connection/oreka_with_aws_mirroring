//
// Created by arik halperin on 05/11/2020.
//

#include "AuidioDataWriter.h"

int AuidioDataWriter::get_queue_size() {
    return 2048;
}
int AuidioDataWriter::get_element_size() {
    return sizeof(PCMData)+512;
}
virtual int AuidioDataWriter::get_queue_identifier() {
    return 0x1789;
}


bool AuidioDataWriter::write_pcm_data(CStdString session_id, int channel, int payload_type, int data_length, unsigned char * audio_data) {
    if(is_full()) {
        return false;
    }

    unsigned char *element_memory = malloc(get_element_size());

    write_element(element_memory);

    free(element_memory);

}
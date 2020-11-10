//
// Created by arik halperin on 05/11/2020.
//

#include "SharedMemoryQueueWriter.h"


SharedMemoryQueueWriter::SharedMemoryQueueWriter() {
    key = ftok("memory",get_queue_identifier());
    shmid = shmget(key, 2*sizeof(int) + get_element_size() * get_queue_size() ,0666|IPC_CREAT);
    if(shmid == -1){
        //printf("Unable to create the Shared Memory Segment.\n");
    }
    shared_memory = (unsigned char *)shmat(shmid,(void*)0,0);
    write_pointer = (int *)shared_memory;
    read_pointer = (int *)(shared_memory+sizeof(int));
    *write_pointer = 0;
    *read_pointer = 0;
}

bool SharedMemoryQueueWriter::is_full() {
    return get_next_value(*write_pointer)==*read_pointer();
}

bool SharedMemoryQueueWriter::is_empty() {
    return *write_pointer==*read_pointer;
}

bool SharedMemoryQueueWriter::write_element(unsigned char * element) {
    MutexSentinel mutexSentinel(queue_mutex);
    if(isFull()) {
        return false;
    }
    memcpy(shared_memory+get_element_size()*(*write_pointer),element,get_element_size());
    advance_pointer(write_pointer);
}

SharedMemoryQueueWriter::~SharedMemoryQueueWriter() {
    shmdt(shared_memory);
}
//
// Created by arik halperin on 05/11/2020.
//

#include "SharedMemoryQueueWriter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/shm.h>
#include <sys/ipc.h>

SharedMemoryQueueWriter::SharedMemoryQueueWriter(int _queue_identifier, int _element_size, int _queue_size) {
    queue_identifier = _queue_identifier;
    element_size = _element_size;
    queue_size = _queue_size;
    std::string logMsg;
    std::string keyval = "/root/"+std::to_string(queue_identifier);
    std::ofstream shared_memory_file;
    shared_memory_file.open(keyval,std::ios::out | std::ios::app | std::ios::binary);
    shared_memory_file.close();

    std::cout<<"Creating shared memory\n";

    key = ftok(keyval.c_str(), queue_identifier);
    if(key==-1) {
        logMsg= "Could not generate key:" +  std::to_string(errno)  + "\n";
        std::cout<<logMsg;
    }

    shmid = shmget(key, 2*sizeof(int) + element_size * queue_size ,0666|IPC_CREAT);


    if(shmid == -1){
        logMsg= "Shared Memory For Queue:" +  std::to_string(errno) + " " + std::to_string(element_size)+ " " +std::to_string(queue_size)  + "\n";
        std::cout<<logMsg;
    }

    void * memory = shmat(shmid,NULL,0);

    if (memory == (char *)(-1)) {
        logMsg= "Error mapping shared memory: "+std::to_string(errno) + "\n";
        std::cout<<logMsg;
        return;
    } else {
        logMsg = "shared memory: "+ std::to_string((unsigned long)(unsigned char *)memory) + "\n";
        shared_memory = (unsigned char *)memory;
        std::cout<<logMsg;
    }

    write_pointer = (int *)shared_memory;
    read_pointer = (int *)(shared_memory+sizeof(int));
    *write_pointer = 0;
    *read_pointer = 0;
}

bool SharedMemoryQueueWriter::is_full() {
    return get_next_value(*write_pointer)==*read_pointer;
}

bool SharedMemoryQueueWriter::is_empty() {
    return *write_pointer==*read_pointer;
}

bool SharedMemoryQueueWriter::write_element(unsigned char * element) {
    queue_mutex.lock();
    if(is_full()) {
        queue_mutex.unlock();
        return false;
    }
    memcpy(shared_memory+element_size*(*write_pointer)+2*sizeof(int),element,element_size);
    advance_pointer(write_pointer);
    queue_mutex.unlock();
}

SharedMemoryQueueWriter::~SharedMemoryQueueWriter() {
    shmdt(shared_memory);
}

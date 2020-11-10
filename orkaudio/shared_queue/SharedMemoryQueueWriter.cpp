//
// Created by arik halperin on 05/11/2020.
//

#include "SharedMemoryQueueWriter.h"
#include <StdString.h>
#include "LogManager.h"

static LoggerPtr s_parsersLog = Logger::getLogger("parsers.sip");

SharedMemoryQueueWriter::SharedMemoryQueueWriter(int _queue_identifier, int _element_size, int _queue_size) {
    queue_identifier = _queue_identifier;
    element_size = _element_size;
    queue_size = queue_size;

    key = ftok("memory",queue_identifier);
    shmid = shmget(key, 2*sizeof(int) + element_size * queue_size ,0666|IPC_CREAT);
    if(shmid == -1){
        LOG4CXX_ERROR(s_parsersLog, "Unable to create the Sha\"Shared Memory For Queue:%d", errno);
    }

    shared_memory = (unsigned char *)shmat(shmid,(void*)0,0);
    CStdString logMsg;
    logMsg.Format("Shared Memory For Queue:%x", (unsigned long) shared_memory);

    LOG4CXX_INFO(s_parsersLog, logMsg);
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
    memcpy(shared_memory+element_size*(*write_pointer),element,element_size);
    advance_pointer(write_pointer);
    queue_mutex.unlock();
}

SharedMemoryQueueWriter::~SharedMemoryQueueWriter() {
    shmdt(shared_memory);
}
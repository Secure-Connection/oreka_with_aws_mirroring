//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_SHAREDMEMORYQUEUE_H
#define OREKA_SHAREDMEMORYQUEUE_H
#include <sys/ipc.h>
#include <sys/shm.h>
#include <memory.h>
#include <mutex>
#include <StdString.h>

class SharedMemoryQueueWriter {
private:
    int * write_pointer;
    int * read_pointer;
    key_t key;
    int shmid;
    unsigned char * shared_memory;
    std::mutex queue_mutex;

    inline int get_next_value(int value) {
        int new_value = value + 1;
        if(new_value==get_queue_size()) {
            new_value = 0;
        }
        return new_value;
    }

    inline void advance_pointer(int *ptr) {
        *ptr = get_next_value(*ptr);
    }

 protected:
    virtual int get_queue_size() = 0;
    virtual int get_element_size() = 0;
    virtual int get_queue_identifier()=0;

  public:
     SharedMemoryQueueWriter();
     ~SharedMemoryQueueWriter();
     bool is_full();

     bool is_empty();

     bool write_element(unsigned char * element);
};


#endif //OREKA_SHAREDMEMORYQUEUE_H

//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_SHAREDMEMORYQUEUE_H
#define OREKA_SHAREDMEMORYQUEUE_H
#include <sys/ipc.h>
#include <sys/shm.h>
#include <memory.h>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

using namespace boost::interprocess;
class SharedMemoryQueueWriter {
private:
    int * write_pointer;
    int * read_pointer;
    key_t key;
    int shmid;
    int element_size;
    int queue_size;
    int queue_identifier;
    unsigned char * shared_memory;
    named_mutex * queue_mutex;

    inline int get_next_value(int value) {
        int new_value = value + 1;
        if(new_value==queue_size) {
            new_value = 0;
        }
        return new_value;
    }

    inline void advance_pointer(int *ptr) {
        *ptr = get_next_value(*ptr);
    }

 protected:

  public:
     SharedMemoryQueueWriter(int queue_identifier, int element_size, int queue_size);
     ~SharedMemoryQueueWriter();
     bool is_full();

     bool is_empty();

     bool write_element(unsigned char * element);
};


#endif //OREKA_SHAREDMEMORYQUEUE_H

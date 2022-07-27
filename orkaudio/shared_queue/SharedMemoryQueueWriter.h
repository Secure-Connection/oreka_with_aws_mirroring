//
// Created by arik halperin on 05/11/2020.
//

#ifndef OREKA_SHAREDMEMORYQUEUE_H
#define OREKA_SHAREDMEMORYQUEUE_H
#include <sys/ipc.h>
#include <sys/shm.h>
#include <memory.h>
#include <string>
#include <mutex>

using namespace std;
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
    mutex m_mutex;

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

    key_t get_key(std::string keyval, int queue_identifier);

 protected:

  public:
     SharedMemoryQueueWriter(int queue_identifier, int element_size, int queue_size);
     ~SharedMemoryQueueWriter();
     bool is_full();

     bool is_empty();

     bool write_element(unsigned char * element);
};


#endif //OREKA_SHAREDMEMORYQUEUE_H

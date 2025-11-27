#pragma once 

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstring>
#include <iomanip>
#include <cstddef>

#include "utils/constants.hpp"

class Memory {
private:
    uint64_t mem_size  = 0;
    uint64_t curr_size = 0;
    std::byte* mem = nullptr;

public:
    Memory (int size = DEFAULT_MEM_SIZE): mem_size (size) {
        mem = new std::byte [mem_size];
    }
    ~Memory () {
        delete [] mem;
    }

    inline uint64_t get_mem_size () { return mem_size; }
    inline uint64_t get_curr_size () { return curr_size; }

    void mem_store (uint64_t offset, void* ptr, int ptr_size);
    void mem_load (uint64_t offset, void* ptr, int ptr_size);
    void dump ();
    void dump_stack (uint64_t sp);
};
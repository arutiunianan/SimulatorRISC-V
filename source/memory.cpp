#include "memory/memory.hpp"

void Memory::mem_load (uint64_t offset, void* ptr, int ptr_size) {
    std::memcpy (ptr, mem + offset, ptr_size);
}

void Memory::mem_store (uint64_t offset, void* ptr, int ptr_size) {
    // TODO: make correct assertion
    // assert (curr_size + ptr_size <= mem_size 
    //     && "there is no more space in the memory");

    assert ((offset + ptr_size <= mem_size) && (offset + ptr_size >= 0) 
        && "going beyond the boundaries of the addr space");
    curr_size += ptr_size;
    std::memcpy (mem + offset, ptr, ptr_size);
}

void Memory::dump () {
    int zero_counter = 0;

    for (uint64_t i = 0; i < mem_size; ++i) {
        if (zero_counter == 32) {
            if (std::to_integer<int>(mem[i]) != 0) {
                std::cout << std::endl << "..." << std::endl;
                zero_counter = 0;
            }
            else
                continue;
        }

        std::cout << std::hex << std::setw (2) << std::setfill('0')
                  << static_cast<int>(mem[i]) << " ";

        if (std::to_integer<int>(mem[i]) == 0)
            zero_counter++;
    }
    std::cout << std::dec << std::endl;
}

void Memory::dump_stack (uint64_t sp_offset) {
    for (uint64_t i = sp_offset; i < mem_size; ++i) {
        std::cout << std::hex << std::setw (2) << std::setfill('0')
                  << static_cast<int>(mem[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}


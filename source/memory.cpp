#include "memory/memory.hpp"

void Memory::mem_load (uint64_t offset, void* ptr, int ptr_size) {
    std::memcpy (ptr, mem + offset, ptr_size);
}

void Memory::mem_store (uint64_t offset, void* ptr, int ptr_size) {
    assert ((offset + ptr_size <= mem_size) && (offset + ptr_size >= 0) 
        && "going beyond the boundaries of the addr space");
    curr_size += ptr_size;
    std::memcpy (mem + offset, ptr, ptr_size);
}

void Memory::dump (std::ostream& ostr) {
    int zero_counter = 0;

    for (uint64_t i = 0; i < mem_size; ++i) {
        if (zero_counter == 32) {
            if (std::to_integer<int>(mem[i]) != 0) {
                ostr << std::endl << "..." << std::endl;
                zero_counter = 0;
            }
            else {
                continue;
            }
        }

        ostr << std::hex << std::setw (2) << std::setfill('0')
                  << static_cast<int>(mem[i]) << " ";

        if (std::to_integer<int>(mem[i]) == 0) {
            zero_counter++;
        }
    }
    ostr << std::dec << std::endl;
}

void Memory::dump_stack (uint64_t sp_offset, std::ostream& ostr) {
    for (uint64_t i = sp_offset; i < mem_size; ++i) {
        ostr << std::hex << std::setw (2) << std::setfill('0')
                  << static_cast<int>(mem[i]) << " ";
    }
    ostr << std::dec << std::endl;
}


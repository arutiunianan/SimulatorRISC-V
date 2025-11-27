#pragma once

#include <iostream>
#include <chrono>

#include "core/regfile.hpp"
#include "core/segment.hpp"
#include "core/inst.hpp"

#include "utils/constants.hpp"

#include "memory/memory.hpp"
#include "memory/mmu.hpp"
#include "stages/decoder.hpp"

//--------------------------------------------------------------------------
// Cells
//--------------------------------------------------------------------------
// Cell connecting fetch and decode stages
struct fd_cell {
    uint32_t inst = 0;
    uint64_t addr = 0;
};

// Cell connecting decode and execute stages
struct de_cell {
    Inst* inst = nullptr;
};

//--------------------------------------------------------------------------
// Hart
//--------------------------------------------------------------------------
class Hart final {
private:
    bool stop = false;

    uint64_t start_addr;

    Memory memory;
    MMU mmu;
    Regfile regfile;
    Reg pc;
    Decoder  decoder;

// Pipeline stages
    void fetch ();
    fd_cell fd;
    void decode ();
    de_cell de;
    void execute (bool trace);

// Auxiliary functions for inserting bubbles
    friend void set_nop_fd_cell (Hart& hart);
    friend void set_nop_de_cell (Hart& hart);

public:
    Hart(): mmu(*this) {}

// Function that terminates running of pipeline
    void finish () { stop = true; }

// Interaction with memory
    void map_seg_to_VAS (Segment& segment);
    inline void set_start_addr (uint64_t vaddr) { start_addr = vaddr; }
    inline void set_sp () { regfile.set_reg_val (2, DEFAULT_MEM_SIZE + start_addr); }
    bool read_phys_u64 (uint64_t phys_addr, uint64_t &out);
    inline void write_satp(uint64_t val) { mmu.satp.raw = val; mmu.flush_tlb(); }
    inline uint64_t read_satp() const { return mmu.satp.raw; }
    void load_from_memory (uint64_t vaddr, void* load_ptr, int load_size);
    void store_in_memory (uint64_t vaddr, uint64_t val, int store_size);
    void memory_dump () { memory.dump (); }
    
// Interaction with regfile
    inline void set_reg_val (uint8_t reg, uint64_t v) { regfile.set_reg_val (reg, v); }
    inline uint64_t get_reg_val (uint8_t reg) { return regfile.get_reg_val (reg); }
    inline void set_pc_val (uint64_t new_pc) { pc.set_val (new_pc); }
    inline uint64_t get_pc_val () { return pc.get_val (); }

// Main pipeline cycle
    void run_pipeline (bool trace);

    void dump ();
};
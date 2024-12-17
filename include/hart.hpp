#pragma once

#include <iostream>
#include <chrono>

#include "core/regfile.hpp"
#include "core/segment.hpp"
#include "core/inst.hpp"
#include "core/bb_cache.hpp"

#include "utils/constants.hpp"

#include "memory.hpp"
#include "stages/decoder.hpp"

//--------------------------------------------------------------------------
// Cells
//--------------------------------------------------------------------------
// Cell connecting fetch and decode stages
struct fetch_cell {
    uint32_t inst = 0;
    uint64_t addr = 0;
};

//--------------------------------------------------------------------------
// Hart
//--------------------------------------------------------------------------
class Hart final {
private:
    bool flag_new_bb = true;
    bool flag_execute_from_inst_cache = false;
    bool stop = false;

    uint64_t num_of_executed_inst = 0;

    uint64_t start_addr;
    uint64_t bb_start_pc;

    Memory memory;
    BBCache bb_cache;
    Decoder  decoder;
    Regfile regfile;
    Reg pc;

    fetch_cell f_cell;

// Pipeline stages
    void fetch ();
    void decode ();
    void execute ();

public:
// Function that terminates running of pipeline
    inline void finish () { stop = true; }

// If we executed jump or branch we need to set flag_new_bb
    inline void set_flag_new_bb () { flag_new_bb = true; }

// Interaction with memory
    void map_seg_to_VAS (Segment& segment);
    inline void set_start_addr (uint64_t vaddr) { start_addr = vaddr; }
    inline void set_sp () { regfile.set_reg_val (2, DEFAULT_MEM_SIZE + start_addr); }
    void load_from_memory (uint64_t vaddr, void* load_ptr, int load_size);
    void store_in_memory (uint64_t vaddr, uint64_t val, int store_size);
    void memory_dump () { memory.dump (); }
    
// Interaction with regfile
    inline void set_reg_val (uint8_t reg, uint64_t v) { regfile.set_reg_val (reg, v); }
    inline uint64_t get_reg_val (uint8_t reg) { return regfile.get_reg_val (reg); }
    inline void set_pc_val (uint64_t new_pc) { pc.set_val (new_pc); }
    inline uint64_t get_pc_val () { return pc.get_val (); }

// Main pipeline cycle
    void run_pipeline ();

    void dump ();
};
#include "stages/executor.hpp"
#include "hart.hpp"

//--------------------------------------------------------------------------
// Interaction with memory
//--------------------------------------------------------------------------
void Hart::map_seg_to_VAS (Segment& segment) {
    //how many bytes are needed for alignment
    uint64_t vaddr = segment.get_vaddr();
    uint64_t vp_alignment = (VPAGE_SIZE - vaddr % VPAGE_SIZE) % VPAGE_SIZE;
    if (vp_alignment != 0) {
        //check if the page is readable or writable
        if (segment.get_flag() & (PF_W || PF_R)) {
            return;
        }

        memory.mem_store (vaddr - start_addr, segment.get_data(), vp_alignment);
    }

    //copy the remaining pages
    for (uint64_t vpage_offset = 0; vpage_offset < segment.get_size() - vp_alignment; vpage_offset += VPAGE_SIZE) {
        uint64_t paddr = vaddr + vp_alignment + vpage_offset - start_addr;

        //determine the size for the record
        size_t store_size = VPAGE_SIZE;
        if ((segment.get_size() - vp_alignment - vpage_offset) < VPAGE_SIZE)
            store_size = segment.get_size() - vp_alignment - vpage_offset;
            
        memory.mem_store (paddr, (char*)segment.get_data() + vp_alignment + vpage_offset, store_size);
    }
}

void Hart::load_from_memory (uint64_t vaddr, void* load_ptr, int load_size) {
    assert ((load_size == BYTE_SIZE) || (load_size == HWORD_SIZE) ||
            (load_size == WORD_SIZE) || (load_size == DWORD_SIZE) &&
            "incorrect load size (only 1, 2, 4, 8 b)");
    assert (((vaddr % load_size) == 0) && "incorrect alignment");
    
    memory.mem_load (vaddr - start_addr, load_ptr, load_size);
}

void Hart::store_in_memory (uint64_t vaddr, uint64_t val, int store_size) {
    assert ((store_size == BYTE_SIZE) || (store_size == HWORD_SIZE) ||
            (store_size == WORD_SIZE) || (store_size == DWORD_SIZE) &&
            "incorrect load size (only 1, 2, 4, 8 b)");
    assert (((vaddr % store_size) == 0) && "incorrect alignment");
    
    memory.mem_store (vaddr - start_addr, &val, store_size);
}

//--------------------------------------------------------------------------
// Pipeline stages
//--------------------------------------------------------------------------
void Hart::fetch () {
    uint64_t cur_pc_val = pc.get_val();

    load_from_memory (cur_pc_val, &f_cell.inst, WORD_SIZE);

    f_cell.addr = cur_pc_val;

    if (flag_new_bb) {
        bb_start_pc = f_cell.addr;
    
        if (bb_cache.in_cache_p (bb_start_pc))
            flag_execute_from_inst_cache = true;
        
        else
            flag_execute_from_inst_cache = false;
        
        flag_new_bb = false;
    }
}

void Hart::decode () {
    if (flag_execute_from_inst_cache)
        return;

    decoder.d_cell.inst_type = decoder.decode_inst (f_cell.inst);
}

void Hart::execute () {
    uint64_t cur_pc_val = pc.get_val();

    if (flag_execute_from_inst_cache) {
        int bb_size = bb_cache.get_bb_size(bb_start_pc);
        
        pc.set_val (cur_pc_val + WORD_SIZE * bb_size);
        bb_cache.execute_bb (bb_start_pc, *this);

        num_of_executed_inst += bb_cache.get_bb_size(bb_start_pc);
    }
    
    else {
        pc.set_val (cur_pc_val + WORD_SIZE);

        Inst* new_inst_in_bb;

        switch (decoder.d_cell.inst_type) {
            case InstType::R:
                decoder.d_cell.tmp_inst_R.addr = f_cell.addr;
                decoder.d_cell.tmp_inst_R.execute_func(&decoder.d_cell.tmp_inst_R, *this);
                new_inst_in_bb = new Inst_R;
                *static_cast<Inst_R*>(new_inst_in_bb) = decoder.d_cell.tmp_inst_R; 
                break;
            
            case InstType::I:
                decoder.d_cell.tmp_inst_I.addr = f_cell.addr;
                decoder.d_cell.tmp_inst_I.execute_func(&decoder.d_cell.tmp_inst_I, *this);
                new_inst_in_bb = new Inst_I;
                *static_cast<Inst_I*>(new_inst_in_bb) = decoder.d_cell.tmp_inst_I;  
                break;
            
            case InstType::S:
                decoder.d_cell.tmp_inst_S.addr = f_cell.addr;
                decoder.d_cell.tmp_inst_S.execute_func(&decoder.d_cell.tmp_inst_S, *this);
                new_inst_in_bb = new Inst_S;
                *static_cast<Inst_S*>(new_inst_in_bb) = decoder.d_cell.tmp_inst_S; 
                break;
            
            case InstType::B:
                decoder.d_cell.tmp_inst_B.addr = f_cell.addr;
                decoder.d_cell.tmp_inst_B.execute_func(&decoder.d_cell.tmp_inst_B, *this);
                new_inst_in_bb = new Inst_B;
                *static_cast<Inst_B*>(new_inst_in_bb) = decoder.d_cell.tmp_inst_B; 
                break;
            
            case InstType::U:
                decoder.d_cell.tmp_inst_U.addr = f_cell.addr;
                decoder.d_cell.tmp_inst_U.execute_func(&decoder.d_cell.tmp_inst_U, *this);
                new_inst_in_bb = new Inst_U;
                *static_cast<Inst_U*>(new_inst_in_bb) = decoder.d_cell.tmp_inst_U; 
                break;
            
            case InstType::J:
                decoder.d_cell.tmp_inst_J.addr = f_cell.addr;
                decoder.d_cell.tmp_inst_J.execute_func(&decoder.d_cell.tmp_inst_J, *this);
                new_inst_in_bb = new Inst_J;
                *static_cast<Inst_J*>(new_inst_in_bb) = decoder.d_cell.tmp_inst_J; 
                break;
        }

        bb_cache.add_inst (bb_start_pc, new_inst_in_bb);

        num_of_executed_inst++;
    }
}

//--------------------------------------------------------------------------
// Main pipeline cycle
//--------------------------------------------------------------------------
void Hart::run_pipeline () {
    auto t_start = std::chrono::high_resolution_clock::now();

    do {
        set_reg_val (0, 0);

        fetch();
        decode();
        execute();
    } while (!stop);

    const auto t_end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(t_end - t_start).count();
    double perfomance = num_of_executed_inst / duration;

    std::cout << "Time passed: " << duration  << " s" << std::endl 
              << "Number of executed instructions: " << num_of_executed_inst << std::endl
              << "Performance: " << perfomance / 1000000 << " MIPS" << std::endl;
}

void Hart::dump () {
    std::cout << "----------------------- Hart -------------------" << std::endl;
    std::cout << "pc = " << std::setfill ('0') << "\033[32m0x" << std::setw(16) 
              << std::hex << pc.get_val() << "\033[0m" << std::endl;
    std::cout << "----------------------- Regfile ----------------" << std::endl;
    regfile.dump();
    std::cout << "----------------------- Memory -----------------" << std::endl;
    memory.dump();
    std::cout << "----------------------- Stack ------------------" << std::endl;
    memory.dump_stack (get_reg_val(2) - start_addr);
    std::cout << "------------------------------------------------" << std::endl;
    std::cout << std::endl;
}
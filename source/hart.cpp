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

    pc.set_val (cur_pc_val + WORD_SIZE);
}

void Hart::decode () {
    if (!f_cell.inst)
        return;

    d_cell.inst_type = decoder.decode_inst (f_cell.inst);
}

void Hart::execute () {
    // // Dump regfile eachtime we enter into or return out of function
    // if (d_cell.inst_type == InstType::I && decoder.tmp_inst_I.inst_name == InstName::JALR)) {
    //     regfile.spike_type_dump();
    //     std::cout << std::endl;
    // }

    switch (d_cell.inst_type) {
        case InstType::R:
            decoder.tmp_inst_R.addr = f_cell.addr;
            decoder.tmp_inst_R.execute_func(&decoder.tmp_inst_R, *this);
            break;
        
        case InstType::I:
            decoder.tmp_inst_I.addr = f_cell.addr;
            decoder.tmp_inst_I.execute_func(&decoder.tmp_inst_I, *this);
            break;
        
        case InstType::S:
            decoder.tmp_inst_S.addr = f_cell.addr;
            decoder.tmp_inst_S.execute_func(&decoder.tmp_inst_S, *this);
            break;
        
        case InstType::B:
            decoder.tmp_inst_B.addr = f_cell.addr;
            decoder.tmp_inst_B.execute_func(&decoder.tmp_inst_B, *this);
            break;
        
        case InstType::U:
            decoder.tmp_inst_U.addr = f_cell.addr;
            decoder.tmp_inst_U.execute_func(&decoder.tmp_inst_U, *this);
            break;
        
        case InstType::J:
            decoder.tmp_inst_J.addr = f_cell.addr;
            decoder.tmp_inst_J.execute_func(&decoder.tmp_inst_J, *this);
            break;
    }
}

//--------------------------------------------------------------------------
// Main pipeline cycle
//--------------------------------------------------------------------------
void Hart::run_pipeline () {
    uint64_t num_of_executed_inst = 0;

    auto t_start = std::chrono::high_resolution_clock::now();

    do {
        set_reg_val (0, 0);
        // // Fetch
        // load_from_memory (pc.get_val(), &cur_fetch_inst, WORD_SIZE);
        // pc.set_val (pc.get_val() + WORD_SIZE);

        // // Decode
        // cur_dec_inst = decoder.decode_inst (cur_fetch_inst);
        // cur_dec_inst->addr = pc.get_val() - WORD_SIZE;

        // // Execute
        // if (!cur_dec_inst)
        //     exit(-1);
        // cur_dec_inst->execute_func (cur_dec_inst, *this);
        // delete cur_dec_inst;

        fetch();
        decode();
        execute();

        num_of_executed_inst++;
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
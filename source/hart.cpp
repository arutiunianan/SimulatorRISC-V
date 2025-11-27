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
        if (segment.get_flag() & (PF_W | PF_R)) {
            return;
        }

        memory.mem_store (vaddr - start_addr, segment.get_data(), vp_alignment);
    }

    //copy the remaining pages
    for (uint64_t vpage_offset = 0; vpage_offset < segment.get_size() - vp_alignment;
         vpage_offset += VPAGE_SIZE) {
        uint64_t paddr = vaddr + vp_alignment + vpage_offset - start_addr;

        //determine the size for the record
        size_t store_size = VPAGE_SIZE;
        if ((segment.get_size() - vp_alignment - vpage_offset) < VPAGE_SIZE) {
            store_size = segment.get_size() - vp_alignment - vpage_offset;
        }

        memory.mem_store (paddr, (char*)segment.get_data() + vp_alignment + vpage_offset, store_size);
    }
}

bool Hart::read_phys_u64 (uint64_t phys_addr, uint64_t &out) {
    // check physical region inside mapped memory:
    if (phys_addr < start_addr) {
        return false;
    }
    uint64_t offset = phys_addr - start_addr;
    if ((offset + sizeof(uint64_t)) > memory.get_mem_size()) {
        return false;
    }
    memory.mem_load(offset, &out, sizeof(uint64_t));
    return true;
}

void Hart::load_from_memory (uint64_t vaddr, void* load_ptr, int load_size) {
    assert ((load_size == BYTE_SIZE) || (load_size == HWORD_SIZE) ||
            (load_size == WORD_SIZE) || (load_size == DWORD_SIZE) &&
            "incorrect load size (only 1, 2, 4, 8 b)");
    assert (((vaddr % load_size) == 0) && "incorrect alignment");

    uint64_t paddr = 0;
    if (!mmu.translate (vaddr, paddr, AccessType::LOAD)) {
        std::cerr << "Memory translate failed (load) VA=0x"
                  << std::hex << vaddr << std::dec << std::endl;
        finish ();
        return;
    }

    uint64_t offset = paddr - start_addr;
    memory.mem_load (offset, load_ptr, load_size);
}

void Hart::store_in_memory (uint64_t vaddr, uint64_t val, int store_size) {
    assert ((store_size == BYTE_SIZE) || (store_size == HWORD_SIZE) ||
            (store_size == WORD_SIZE) || (store_size == DWORD_SIZE) &&
            "incorrect load size (only 1, 2, 4, 8 b)");
    assert (((vaddr % store_size) == 0) && "incorrect alignment");

    uint64_t paddr = 0;
    if (!mmu.translate (vaddr, paddr, AccessType::STORE)) {
        std::cerr << "Memory translate failed (store) VA=0x"
                  << std::hex << vaddr << std::dec << std::endl;
        finish ();
        return;
    }

    uint64_t offset = paddr - start_addr;
    memory.mem_store (offset, &val, store_size);
}

//--------------------------------------------------------------------------
// Pipeline stages
//--------------------------------------------------------------------------
void Hart::fetch () {
    uint64_t cur_pc_val = pc.get_val();
    uint32_t cur_inst;

    uint64_t paddr = 0;
    if (!mmu.translate (pc.get_val(), paddr, AccessType::IFETCH)) {
        std::cerr << "Instruction fetch translate failed PC=0x"
                  << std::hex << pc.get_val() << std::dec << std::endl;
        finish();
        return;
    }

    memory.mem_load(paddr - start_addr, &cur_inst, WORD_SIZE);
    fd.inst = cur_inst;
    fd.addr = pc.get_val();
    pc.set_val (cur_pc_val + WORD_SIZE);
}

void Hart::decode () {
    uint32_t cur_fd_inst = fd.inst;

    if (!cur_fd_inst)
        return;

    Inst* cur_de_inst = decoder.decode_inst (cur_fd_inst);
    cur_de_inst->addr = fd.addr;

    de.inst = cur_de_inst;
}

void Hart::execute (bool trace) {
    Inst* cur_de_inst = de.inst;

    if (!cur_de_inst)
        return;
    
    // Dump regfile eachtime we enter into or return out of function
    if (trace && (cur_de_inst->name == InstName::JALR)) {
        regfile.spike_type_dump();
        std::cout << std::endl;
    }

    cur_de_inst->execute_func (cur_de_inst, *this);

    delete cur_de_inst;
}

//--------------------------------------------------------------------------
// Main pipeline cycle
//--------------------------------------------------------------------------
void Hart::run_pipeline (bool trace) {
    uint64_t num_of_executed_inst = 0;

    auto t_start = std::chrono::high_resolution_clock::now();

    do {
        set_reg_val (0, 0);
        
        fetch();
        decode();
        execute (trace);

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
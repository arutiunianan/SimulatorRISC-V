#include "stages/executor.hpp"
#include "hart.hpp"

//--------------------------------------------------------------------------
// Auxiliary functions
//--------------------------------------------------------------------------
inline uint64_t sext (uint64_t val, uint8_t size) {
    uint64_t msb = 1 << (size - 1);

    if (!(val & msb))
        return val;
    
    uint64_t ext_mask = 1;
    for (uint8_t i = 0; i < size - 1; i++) {
        ext_mask <<= 1;
        ext_mask |= 1;
    }
    ext_mask = ~ext_mask;

    return val | ext_mask;
}

//--------------------------------------------------------------------------
// RV32I Base Instruction Set
//--------------------------------------------------------------------------
void Executor::execute_LUI (Inst* inst, Hart& hart) {
    Inst_U* inst_U = static_cast<Inst_U*> (inst);

    uint64_t imm_val = inst_U->get_imm() << 12;

    hart.set_reg_val (inst_U->get_rd(), sext(imm_val, 32));
}

void Executor::execute_AUIPC (Inst* inst, Hart& hart) {
    Inst_U* inst_U = static_cast<Inst_U*> (inst);

    uint64_t imm_val = inst_U->get_imm() << 12;

    hart.set_reg_val (inst_U->get_rd(), inst_U->get_addr() + sext(imm_val, 32));
}

// NOTE: If an instruction access-fault or instruction 
// page-fault exception occurs on the target of a jump or
// taken branch, the exception is reported on the target 
// instruction, not on the jump or branch instruction.

// TODO: The JAL and JALR instructions will generate an 
// instruction-address-misaligned exception if the target
// address is not aligned to a four-byte boundary.
void Executor::execute_JAL (Inst* inst, Hart& hart) {
    Inst_J* inst_J = static_cast<Inst_J*> (inst);

    uint64_t offset = sext (inst_J->get_imm(), 21);
    uint64_t target_addr = inst_J->get_addr() + offset;

    // TODO: instruction-address-misaligned exception
    if ((target_addr % WORD_SIZE) != 0)
        return;

    hart.set_pc_val (target_addr);
    hart.set_reg_val (inst_J->get_rd(), inst_J->get_addr() + WORD_SIZE);
    hart.set_flag_new_bb ();
}

void Executor::execute_JALR (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t offset = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());
    uint64_t target_addr = (rs1_val + offset) & ~1;

    // TODO: instruction-address-misaligned exception
    if ((target_addr % WORD_SIZE) != 0)
        return;
    
    if (target_addr == 0)
        hart.finish();

    hart.set_pc_val (target_addr);
    hart.set_reg_val (inst_I->get_rd(), inst_I->get_addr() + WORD_SIZE);
    hart.set_flag_new_bb ();
}

// TODO: The conditional branch instructions will generate an 
// instruction-address-misaligned exception if the target address 
// is not aligned to a four-byte boundary and the branch condition 
// evaluates to true. If the branch condition evaluates to false, 
// the instruction-address-misaligned exception will not be raised.
void Executor::execute_BEQ (Inst* inst, Hart& hart) {
    Inst_B* inst_B = static_cast<Inst_B*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_B->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_B->get_rs1());

    uint64_t offset = sext (inst_B->get_imm(), 13);
    uint64_t target_addr = inst_B->get_addr() + offset;

    if (rs1_val == rs2_val) {
        // TODO: instruction-address-misaligned exception
        if ((target_addr % WORD_SIZE) != 0)
            return;

        hart.set_pc_val (target_addr);
    }   

    hart.set_flag_new_bb ();  
}

void Executor::execute_BNE (Inst* inst, Hart& hart) {
    Inst_B* inst_B = static_cast<Inst_B*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_B->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_B->get_rs1());

    uint64_t offset = sext (inst_B->get_imm(), 13);
    uint64_t target_addr = inst_B->get_addr() + offset;

    if (rs1_val != rs2_val) {
        // TODO: instruction-address-misaligned exception
        if ((target_addr % WORD_SIZE) != 0)
            return;

        hart.set_pc_val (target_addr);
    }

    hart.set_flag_new_bb ();
}

void Executor::execute_BLT (Inst* inst, Hart& hart) {
    Inst_B* inst_B = static_cast<Inst_B*> (inst);

    int64_t rs2_val = hart.get_reg_val (inst_B->get_rs2());
    int64_t rs1_val = hart.get_reg_val (inst_B->get_rs1());

    uint64_t offset = sext (inst_B->get_imm(), 13);
    uint64_t target_addr = inst_B->get_addr() + offset;

    if (rs1_val < rs2_val) {
        // TODO: instruction-address-misaligned exception
        if ((target_addr % WORD_SIZE) != 0)
            return;

        hart.set_pc_val (target_addr);
    }

    hart.set_flag_new_bb ();
}

void Executor::execute_BGE (Inst* inst, Hart& hart) {
    Inst_B* inst_B = static_cast<Inst_B*> (inst);

    int64_t rs2_val = hart.get_reg_val (inst_B->get_rs2());
    int64_t rs1_val = hart.get_reg_val (inst_B->get_rs1());

    uint64_t offset = sext (inst_B->get_imm(), 13);
    uint64_t target_addr = inst_B->get_addr() + offset;

    if (rs1_val >= rs2_val) {
        // TODO: instruction-address-misaligned exception
        if ((target_addr % WORD_SIZE) != 0)
            return;

        hart.set_pc_val (target_addr);
    }

    hart.set_flag_new_bb ();
}

void Executor::execute_BLTU (Inst* inst, Hart& hart) {
    Inst_B* inst_B = static_cast<Inst_B*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_B->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_B->get_rs1());

    uint64_t offset = sext (inst_B->get_imm(), 13);
    uint64_t target_addr = inst_B->get_addr() + offset;

    if (rs1_val < rs2_val) {
        // TODO: instruction-address-misaligned exception
        if ((target_addr % WORD_SIZE) != 0)
            return;

        hart.set_pc_val (target_addr);
    }

    hart.set_flag_new_bb ();
}

void Executor::execute_BGEU (Inst* inst, Hart& hart) {
    Inst_B* inst_B = static_cast<Inst_B*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_B->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_B->get_rs1());

    uint64_t offset = sext (inst_B->get_imm(), 13);
    uint64_t target_addr = inst_B->get_addr() + offset;

    if (rs1_val >= rs2_val) {
        // TODO: instruction-address-misaligned exception
        if ((target_addr % WORD_SIZE) != 0)
            return;

        hart.set_pc_val (target_addr);
    }

    hart.set_flag_new_bb ();
}

void Executor::execute_LB (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;
    uint8_t val;
    hart.load_from_memory (effective_addr, &val, BYTE_SIZE);

    hart.set_reg_val (inst_I->get_rd(), sext (val, 8));
}

void Executor::execute_LH (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;
    uint16_t val;
    hart.load_from_memory (effective_addr, &val, HWORD_SIZE);

    hart.set_reg_val (inst_I->get_rd(), sext (val, 16));
}

void Executor::execute_LW (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;
    uint32_t val;
    hart.load_from_memory (effective_addr, &val, WORD_SIZE);

    hart.set_reg_val (inst_I->get_rd(), sext (val, 32));
}

void Executor::execute_LBU (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;
    uint8_t val;
    hart.load_from_memory (effective_addr, &val, BYTE_SIZE);

    hart.set_reg_val (inst_I->get_rd(), val);
}

void Executor::execute_LHU (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;
    uint16_t val;
    hart.load_from_memory (effective_addr, &val, HWORD_SIZE);

    hart.set_reg_val (inst_I->get_rd(), val);
}

void Executor::execute_SB (Inst* inst, Hart& hart) {
    Inst_S* inst_S = static_cast<Inst_S*> (inst);

    uint64_t imm_val = sext (inst_S->get_imm(), 12);
    uint64_t rs2_val = hart.get_reg_val (inst_S->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_S->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;

    hart.store_in_memory (effective_addr, rs2_val, BYTE_SIZE);
}

void Executor::execute_SH (Inst* inst, Hart& hart) {
    Inst_S* inst_S = static_cast<Inst_S*> (inst);

    uint64_t imm_val = sext (inst_S->get_imm(), 12);
    uint64_t rs2_val = hart.get_reg_val (inst_S->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_S->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;

    hart.store_in_memory (effective_addr, rs2_val, HWORD_SIZE);
}

void Executor::execute_SW (Inst* inst, Hart& hart) {
    Inst_S* inst_S = static_cast<Inst_S*> (inst);

    uint64_t imm_val = sext (inst_S->get_imm(), 12);
    uint64_t rs2_val = hart.get_reg_val (inst_S->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_S->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;

    hart.store_in_memory (effective_addr, rs2_val, WORD_SIZE);
}

void Executor::execute_ADDI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    hart.set_reg_val (inst_I->get_rd(), rs1_val + imm_val);
}

void Executor::execute_SLTI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    int64_t imm_val = sext (inst_I->get_imm(), 12);
    int64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    if (rs1_val < imm_val)
        hart.set_reg_val (inst_I->get_rd(), 1);
    
    else 
        hart.set_reg_val (inst_I->get_rd(), 0);
}

void Executor::execute_SLTIU (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    if (rs1_val < imm_val)
        hart.set_reg_val (inst_I->get_rd(), 1);
    
    else 
        hart.set_reg_val (inst_I->get_rd(), 0);
}

void Executor::execute_XORI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    hart.set_reg_val (inst_I->get_rd(), rs1_val ^ imm_val);
}

void Executor::execute_ORI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    hart.set_reg_val (inst_I->get_rd(), rs1_val | imm_val);
}

void Executor::execute_ANDI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    hart.set_reg_val (inst_I->get_rd(), rs1_val & imm_val);
}

void Executor::execute_ADD (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    hart.set_reg_val (inst_R->get_rd(), rs1_val + rs2_val);
}

void Executor::execute_SUB (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    hart.set_reg_val (inst_R->get_rd(), rs1_val - rs2_val);
}

void Executor::execute_SLL (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint8_t shamt = hart.get_reg_val (inst_R->get_rs2()) & 0b111111;
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    hart.set_reg_val (inst_R->get_rd(), rs1_val << shamt);
}

void Executor::execute_SLT (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    if (rs1_val < rs2_val)
        hart.set_reg_val (inst_R->get_rd(), 1);
    
    else 
        hart.set_reg_val (inst_R->get_rd(), 0);
}

void Executor::execute_SLTU (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    int64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    int64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    if (rs1_val < rs2_val)
        hart.set_reg_val (inst_R->get_rd(), 1);
    
    else 
        hart.set_reg_val (inst_R->get_rd(), 0);
}

void Executor::execute_XOR (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    hart.set_reg_val (inst_R->get_rd(), rs1_val ^ rs2_val);
}

void Executor::execute_SRL (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint8_t shamt = hart.get_reg_val (inst_R->get_rs2()) & 0b111111;
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    hart.set_reg_val (inst_R->get_rd(), rs1_val >> shamt);
}

void Executor::execute_SRA (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint8_t shamt = hart.get_reg_val (inst_R->get_rs2()) & 0b111111;
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    // As right shift may fill "empty" bits with the original MSB 
    // (i.e. perform sign extension) or it may shift in zeroes, 
    // depending on platform and/or compiler, we made sign extension
    // for SRA explicitly. 
    uint64_t rd_val = rs1_val >> shamt;
    hart.set_reg_val (inst_R->get_rd(), sext (rd_val, 64 - shamt));
}

void Executor::execute_OR (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    hart.set_reg_val (inst_R->get_rd(), rs1_val | rs2_val);
}

void Executor::execute_AND (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    hart.set_reg_val (inst_R->get_rd(), rs1_val & rs2_val);
}

// RV32I contains 40 unique instructions, though a simple 
// implementation might cover the ECALL/EBREAK instructions 
// with a single SYSTEM hardware instruction that always traps 
// and might be able to implement the FENCE instruction as a 
// NOP, reducing base instruction count to 38 total.
void Executor::execute_FENCE (Inst* inst, Hart& hart) {
    // NOP
    return;
}

void Executor::execute_FENCE_TSO (Inst* inst, Hart& hart) {
    // NOP
    return;
}

// We assume SYSTEM instructions just wait for something 
// from keyboard.
void Executor::execute_PAUSE (Inst* inst, Hart& hart) {
    int tmp;
    std::cin >> tmp;
    return;
}

void Executor::execute_ECALL (Inst* inst, Hart& hart) {
    int tmp;
    std::cin >> tmp;
    return;
}

void Executor::execute_EBREAK (Inst* inst, Hart& hart) {
    int tmp;
    std::cin >> tmp;
    return;
}

//--------------------------------------------------------------------------
// RV64I Base Instruction Set (in addition to RV32I)
//--------------------------------------------------------------------------
void Executor::execute_LWU (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;
    uint32_t val;
    hart.load_from_memory (effective_addr, &val, WORD_SIZE);

    hart.set_reg_val (inst_I->get_rd(), val);
}

void Executor::execute_LD (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;
    uint64_t val;
    hart.load_from_memory (effective_addr, &val, DWORD_SIZE);

    hart.set_reg_val (inst_I->get_rd(), val);
}

void Executor::execute_SD (Inst* inst, Hart& hart) {
    Inst_S* inst_S = static_cast<Inst_S*> (inst);

    uint64_t imm_val = sext (inst_S->get_imm(), 12);
    uint64_t rs2_val = hart.get_reg_val (inst_S->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_S->get_rs1());

    uint64_t effective_addr = rs1_val + imm_val;

    hart.store_in_memory (effective_addr, rs2_val, DWORD_SIZE);
}

void Executor::execute_SLLI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint8_t shamt = inst_I->get_imm() & 0b111111;
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    hart.set_reg_val (inst_I->get_rd(), rs1_val << shamt);
}

void Executor::execute_SRLI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint8_t shamt = inst_I->get_imm() & 0b111111;
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    hart.set_reg_val (inst_I->get_rd(), rs1_val >> shamt);
}

void Executor::execute_SRAI (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint8_t shamt = inst_I->get_imm() & 0b111111;
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    // As right shift may fill "empty" bits with the original MSB 
    // (i.e. perform sign extension) or it may shift in zeroes, 
    // depending on platform and/or compiler, we made sign extension
    // for SRAI explicitly. 
    uint64_t rd_val = rs1_val >> shamt;
    hart.set_reg_val (inst_I->get_rd(), sext (rd_val, 64 - shamt));
}

void Executor::execute_ADDIW (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint64_t imm_val = sext (inst_I->get_imm(), 12);
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());
    uint32_t rd_val = rs1_val + imm_val;

    hart.set_reg_val (inst_I->get_rd(), sext (rd_val, 32));
}

void Executor::execute_SLLIW (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint8_t shamt = inst_I->get_imm() & 0b11111;
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());
    uint32_t rd_val = rs1_val << shamt;

    hart.set_reg_val (inst_I->get_rd(), sext (rd_val, 32));
}

void Executor::execute_SRLIW (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint8_t shamt = inst_I->get_imm() & 0b11111;
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());
    uint32_t rd_val = rs1_val >> shamt;

    hart.set_reg_val (inst_I->get_rd(), sext (rd_val, 32));
}

void Executor::execute_SRAIW (Inst* inst, Hart& hart) {
    Inst_I* inst_I = static_cast<Inst_I*> (inst);

    uint8_t shamt = inst_I->get_imm() & 0b11111;
    uint64_t rs1_val = hart.get_reg_val (inst_I->get_rs1());

    // As right shift may fill "empty" bits with the original MSB 
    // (i.e. perform sign extension) or it may shift in zeroes, 
    // depending on platform and/or compiler, we made sign extension
    // for SRAI explicitly. 
    uint32_t rd_val = rs1_val >> shamt;
    hart.set_reg_val (inst_I->get_rd(), sext (rd_val, 32 - shamt));
}

void Executor::execute_ADDW (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());
    uint64_t rd_val = rs1_val + rs2_val;

    hart.set_reg_val (inst_R->get_rd(), sext (rd_val, 32));
}

void Executor::execute_SUBW (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint64_t rs2_val = hart.get_reg_val (inst_R->get_rs2());
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());
    uint64_t rd_val = rs1_val - rs2_val;

    hart.set_reg_val (inst_R->get_rd(), sext (rd_val, 32));
}

void Executor::execute_SLLW (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint8_t shamt = hart.get_reg_val (inst_R->get_rs2()) & 0b11111;
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());
    uint64_t rd_val = rs1_val << shamt;

    hart.set_reg_val (inst_R->get_rd(), sext (rd_val, 32));
}

void Executor::execute_SRLW (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint8_t shamt = hart.get_reg_val (inst_R->get_rs2()) & 0b11111;
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());
    uint64_t rd_val = rs1_val >> shamt;

    hart.set_reg_val (inst_R->get_rd(), sext (rd_val, 32));
}

void Executor::execute_SRAW (Inst* inst, Hart& hart) {
    Inst_R* inst_R = static_cast<Inst_R*> (inst);

    uint8_t shamt = hart.get_reg_val (inst_R->get_rs2()) & 0b111111;
    uint64_t rs1_val = hart.get_reg_val (inst_R->get_rs1());

    // As right shift may fill "empty" bits with the original MSB 
    // (i.e. perform sign extension) or it may shift in zeroes, 
    // depending on platform and/or compiler, we made sign extension
    // for SRA explicitly. 
    uint64_t rd_val = rs1_val >> shamt;
    hart.set_reg_val (inst_R->get_rd(), sext (rd_val, 32 - shamt));
}
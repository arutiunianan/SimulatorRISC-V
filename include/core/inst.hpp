#pragma once

#include <cstdint>

enum class Opcode : uint8_t {
    NONE = 0,

    LUI       = 0b0110111,
    AUIPC     = 0b0010111,
    JAL       = 0b1101111,
    JALR      = 0b1100111,
    BRANCH    = 0b1100011,
    LOAD      = 0b0000011,
    STORE     = 0b0100011,
    OP_IMM    = 0b0010011,
    OP        = 0b0110011,
    MISC_MEM  = 0b0001111,
    SYSTEM    = 0b1110011,
    OP_IMM_32 = 0b0011011, 
    OP_32     = 0b0111011
};

enum class InstType {
    NONE = 0,

    R,
    I,
    S,
    B,
    U,
    J
};

enum class InstName {
    NONE = 0,

    // RV32I Base Instruction Set
    LUI,
    AUIPC,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    // SLLI,  <- in RV64I Base Instruction Set
    // SRLI,  <- in RV64I Base Instruction Set
    // SRAI,  <- in RV64I Base Instruction Set
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    FENCE,
    FENCE_TSO,
    PAUSE,
    ECALL,
    EBREAK,

    // RV64I Base Instruction Set (in addition to RV32I)
    LWU,
    LD,
    SD,
    SLLI,
    SRLI,
    SRAI,
    ADDIW,
    SLLIW,
    SRLIW,
    SRAIW,
    ADDW,
    SUBW,
    SLLW,
    SRLW,
    SRAW
};

//--------------------------------------------------------------------------
// Inst
//--------------------------------------------------------------------------
class Decoder;
class Hart;
class Inst_I;

class Inst {
    friend class Decoder;
    friend class Hart;

private:
    InstType type = InstType::NONE;
    InstName name = InstName::NONE;
    uint64_t addr = 0;
    
    void (*execute_func) (Inst*, Hart&) = nullptr;

public:
    inline uint64_t get_addr () { return addr; }

    virtual ~Inst() {};
}; 

//--------------------------------------------------------------------------
// Inst_R
//--------------------------------------------------------------------------
class Inst_R final : public Inst {
    friend class Decoder;

private:
    uint8_t rs2;
    uint8_t rs1;
    uint8_t rd;

public:
    inline uint8_t get_rs2 () { return rs2; }
    inline uint8_t get_rs1 () { return rs1; }
    inline uint8_t get_rd  () { return rd;  }
};

//--------------------------------------------------------------------------
// Inst_I
//--------------------------------------------------------------------------
class Inst_I final : public Inst {
    friend class Decoder;

private:
    uint32_t imm;
    uint8_t  rs1;
    uint8_t  rd;

public:
    inline uint32_t get_imm () { return imm; }
    inline uint8_t  get_rs1 () { return rs1; }
    inline uint8_t  get_rd  () { return rd;  }
}; 

//--------------------------------------------------------------------------
// Inst_S
//--------------------------------------------------------------------------
class Inst_S final : public Inst {
    friend class Decoder;

private:
    uint32_t imm;
    uint8_t  rs2;
    uint8_t  rs1;

public:
    inline uint32_t get_imm () { return imm; }
    inline uint8_t  get_rs2 () { return rs2; }
    inline uint8_t  get_rs1 () { return rs1; }
};

//--------------------------------------------------------------------------
// Inst_B
//--------------------------------------------------------------------------
class Inst_B final : public Inst {
    friend class Decoder;

private:
    uint32_t imm;
    uint8_t  rs2;
    uint8_t  rs1;

public:
    inline uint32_t get_imm () { return imm; }
    inline uint8_t  get_rs2 () { return rs2; }
    inline uint8_t  get_rs1 () { return rs1; }  
}; 

//--------------------------------------------------------------------------
// Inst_U
//--------------------------------------------------------------------------
class Inst_U final : public Inst {
    friend class Decoder;

private:
    uint32_t imm;
    uint8_t  rd;      

public:
    inline uint32_t get_imm () { return imm; }
    inline uint8_t  get_rd  () { return rd;  }
};

//--------------------------------------------------------------------------
// Inst_J
//--------------------------------------------------------------------------
class Inst_J final : public Inst {
    friend class Decoder;

private:
    uint32_t imm;
    uint8_t  rd;

public:
    inline uint32_t get_imm () { return imm; }
    inline uint8_t  get_rd  () { return rd;  }
}; 
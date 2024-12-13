#include <unordered_map>

#include "core/inst.hpp"
#include "stages/executor.hpp"


//------------------------------------
// All decode maps declaratons
//------------------------------------
struct funct7_map {
    bool valid;
    InstName inst_name;
    std::unordered_map <uint8_t, InstName> map;
};

struct funct3_map {
    bool valid;
    InstName inst_name;
    std::unordered_map <uint8_t, funct7_map> map;
};

using opcode_map = std::unordered_map <uint8_t, funct3_map>;

//------------------------------------
// Funct7 maps
//------------------------------------
funct7_map JALR_000_f7_m = {false, InstName::JALR};

funct7_map BRANCH_000_f7_m = {false, InstName::BEQ};
funct7_map BRANCH_001_f7_m = {false, InstName::BNE};
funct7_map BRANCH_100_f7_m = {false, InstName::BLT};
funct7_map BRANCH_101_f7_m = {false, InstName::BGE};
funct7_map BRANCH_110_f7_m = {false, InstName::BLTU};
funct7_map BRANCH_111_f7_m = {false, InstName::BGEU};

funct7_map LOAD_000_f7_m = {false, InstName::LB};
funct7_map LOAD_001_f7_m = {false, InstName::LH};
funct7_map LOAD_010_f7_m = {false, InstName::LW};
funct7_map LOAD_100_f7_m = {false, InstName::LBU};
funct7_map LOAD_101_f7_m = {false, InstName::LHU};
funct7_map LOAD_110_f7_m = {false, InstName::LWU};
funct7_map LOAD_011_f7_m = {false, InstName::LD};

funct7_map STORE_000_f7_m = {false, InstName::SB};
funct7_map STORE_001_f7_m = {false, InstName::SH};
funct7_map STORE_010_f7_m = {false, InstName::SW};
funct7_map STORE_011_f7_m = {false, InstName::SD};

funct7_map OP_IMM_000_f7_m = {false, InstName::ADDI};
funct7_map OP_IMM_010_f7_m = {false, InstName::SLTI};
funct7_map OP_IMM_011_f7_m = {false, InstName::SLTIU};
funct7_map OP_IMM_100_f7_m = {false, InstName::XORI};
funct7_map OP_IMM_110_f7_m = {false, InstName::ORI};
funct7_map OP_IMM_111_f7_m = {false, InstName::ANDI};
funct7_map OP_IMM_001_f7_m = {true, InstName::NONE,
    {{0b0000000, InstName::SLLI},
     {0b0000001, InstName::SLLI}}};
funct7_map OP_IMM_101_f7_m = {true, InstName::NONE,
    {{0b0000000, InstName::SRLI},
     {0b0000001, InstName::SRLI},
     {0b0100000, InstName::SRAI},
     {0b0100001, InstName::SRAI}}};

funct7_map OP_000_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::ADD},
     {0b0100000, InstName::SUB}}};
funct7_map OP_001_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::SLL}}};
funct7_map OP_010_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::SLT}}};
funct7_map OP_011_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::SLTU}}};
funct7_map OP_100_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::XOR}}};
funct7_map OP_101_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::SRL},
     {0b0100000, InstName::SRA}}};
funct7_map OP_110_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::OR}}};
funct7_map OP_111_f7_m = {true, InstName::NONE, 
    {{0b0000000, InstName::AND}}};

funct7_map OP_IMM_32_000_f7_m = {false, InstName::ADDIW};
funct7_map OP_IMM_32_001_f7_m = {true, InstName::NONE,
    {{0b0000000, InstName::SLLIW}}};
funct7_map OP_IMM_32_101_f7_m = {true, InstName::NONE,
    {{0b0000000, InstName::SRLIW},
     {0b0100000, InstName::SRAIW}}};

funct7_map OP_32_000_f7_m = {true, InstName::NONE,
    {{0b0000000, InstName::ADDW},
     {0b0100000, InstName::SUBW}}};
funct7_map OP_32_001_f7_m = {true, InstName::NONE,
    {{0b0000000, InstName::SLLW}}};
funct7_map OP_32_101_f7_m = {true, InstName::NONE,
    {{0b0000000, InstName::SRLW},
     {0b0100000, InstName::SRAW}}};

//------------------------------------
// Funct3 maps
//------------------------------------
funct3_map LUI_f3_m {false, InstName::LUI};
funct3_map AUIPC_f3_m {false, InstName::AUIPC};
funct3_map JAL_f3_m {false, InstName::JAL};

funct3_map JALR_f3_m {true, InstName::NONE, 
    {{0b000, JALR_000_f7_m}}};

funct3_map BRANCH_f3_m {true, InstName::NONE,
    {{0b000, BRANCH_000_f7_m},
     {0b001, BRANCH_001_f7_m},
     {0b100, BRANCH_100_f7_m},
     {0b101, BRANCH_101_f7_m},
     {0b110, BRANCH_110_f7_m},
     {0b111, BRANCH_111_f7_m}}};

funct3_map LOAD_f3_m {true, InstName::NONE,
    {{0b000, LOAD_000_f7_m},
     {0b001, LOAD_001_f7_m},
     {0b010, LOAD_010_f7_m},
     {0b100, LOAD_100_f7_m},
     {0b101, LOAD_101_f7_m},
     {0b110, LOAD_110_f7_m},
     {0b011, LOAD_011_f7_m}}};

funct3_map STORE_f3_m {true, InstName::NONE,
    {{0b000, STORE_000_f7_m},
     {0b001, STORE_001_f7_m},
     {0b010, STORE_010_f7_m},
     {0b011, STORE_011_f7_m}}};

funct3_map OP_IMM_f3_m {true, InstName::NONE,
    {{0b000, OP_IMM_000_f7_m},
     {0b010, OP_IMM_010_f7_m},
     {0b011, OP_IMM_011_f7_m},
     {0b100, OP_IMM_100_f7_m},
     {0b110, OP_IMM_110_f7_m},
     {0b111, OP_IMM_111_f7_m},
     {0b001, OP_IMM_001_f7_m},
     {0b101, OP_IMM_101_f7_m}}};

funct3_map OP_f3_m {true, InstName::NONE,
    {{0b000, OP_000_f7_m}, 
     {0b001, OP_001_f7_m}, 
     {0b010, OP_010_f7_m}, 
     {0b011, OP_011_f7_m}, 
     {0b100, OP_100_f7_m}, 
     {0b101, OP_101_f7_m}, 
     {0b110, OP_110_f7_m}, 
     {0b111, OP_111_f7_m}}
};

// TODO: fix decode for MISC_MEM instructions
funct3_map MISC_MEM_f3_m {false, InstName::PAUSE};

// TODO: fix decode for SYSTEM instructions
funct3_map SYSTEM_f3_m {false, InstName::ECALL};

funct3_map OP_IMM_32_f3_m {true, InstName::NONE,
    {{0b000, OP_IMM_32_000_f7_m},
     {0b001, OP_IMM_32_001_f7_m},
     {0b101, OP_IMM_32_101_f7_m}}};

funct3_map OP_32_f3_m {true, InstName::NONE,
    {{0b000, OP_32_000_f7_m},
     {0b001, OP_32_001_f7_m},
     {0b101, OP_32_101_f7_m}}};

//------------------------------------
// Opcode map
//------------------------------------
opcode_map opc_m = {
    {0b0110111, LUI_f3_m},
    {0b0010111, AUIPC_f3_m},
    {0b1101111, JAL_f3_m},
    {0b1100111, JALR_f3_m},      
    {0b1100011, BRANCH_f3_m},    
    {0b0000011, LOAD_f3_m},      
    {0b0100011, STORE_f3_m},     
    {0b0010011, OP_IMM_f3_m},    
    {0b0110011, OP_f3_m},
    {0b0001111, MISC_MEM_f3_m},  
    {0b1110011, SYSTEM_f3_m},    
    {0b0011011, OP_IMM_32_f3_m},  
    {0b0111011, OP_32_f3_m}    
};

//------------------------------------
// InstName <-> InstType map
//------------------------------------
std::unordered_map<uint8_t, InstType> opcode_type_map {
    {0b0110111, InstType::U},
    {0b0010111, InstType::U},
    {0b1101111, InstType::J},
    {0b1100111, InstType::I},
    {0b1100011, InstType::B},
    {0b0000011, InstType::I},
    {0b0100011, InstType::S},
    {0b0010011, InstType::I},
    {0b0110011, InstType::R},
    {0b0001111, InstType::I},
    {0b1110011, InstType::I},
    {0b0011011, InstType::I},
    {0b0111011, InstType::R}
};

//------------------------------------
// InstName <-> Executor map
//------------------------------------
std::unordered_map<InstName, void (*) (Inst*, Hart&)> name_executor_map {
    {InstName::NONE, nullptr},
    {InstName::LUI, Executor::execute_LUI},
    {InstName::AUIPC, Executor::execute_AUIPC},
    {InstName::JAL, Executor::execute_JAL},
    {InstName::JALR, Executor::execute_JALR},
    {InstName::BEQ, Executor::execute_BEQ},
    {InstName::BNE, Executor::execute_BNE},
    {InstName::BLT, Executor::execute_BLT},
    {InstName::BGE, Executor::execute_BGE},
    {InstName::BLTU, Executor::execute_BLTU},
    {InstName::BGEU, Executor::execute_BGEU},
    {InstName::LB, Executor::execute_LB},
    {InstName::LH, Executor::execute_LH},
    {InstName::LW, Executor::execute_LW},
    {InstName::LBU, Executor::execute_LBU},
    {InstName::LHU, Executor::execute_LHU},
    {InstName::LWU, Executor::execute_LWU},
    {InstName::LD, Executor::execute_LD},
    {InstName::SB, Executor::execute_SB},
    {InstName::SH, Executor::execute_SH},
    {InstName::SW, Executor::execute_SW},
    {InstName::SD, Executor::execute_SD},
    {InstName::ADDI, Executor::execute_ADDI},
    {InstName::SLTI, Executor::execute_SLTI},
    {InstName::SLTIU, Executor::execute_SLTIU},
    {InstName::XORI, Executor::execute_XORI},
    {InstName::ORI, Executor::execute_ORI},
    {InstName::ANDI, Executor::execute_ANDI},
    {InstName::SLLI, Executor::execute_SLLI},
    {InstName::SRLI, Executor::execute_SRLI},
    {InstName::SRAI, Executor::execute_SRAI},
    {InstName::ADD, Executor::execute_ADD},
    {InstName::SUB, Executor::execute_SUB},
    {InstName::SLL, Executor::execute_SLL},
    {InstName::SLT, Executor::execute_SLT},
    {InstName::SLTU, Executor::execute_SLTU},
    {InstName::XOR, Executor::execute_XOR},
    {InstName::SRL, Executor::execute_SRL},
    {InstName::SRA, Executor::execute_SRA},
    {InstName::OR, Executor::execute_OR},
    {InstName::AND, Executor::execute_AND},
    {InstName::FENCE, Executor::execute_FENCE},
    {InstName::FENCE_TSO, Executor::execute_FENCE_TSO},
    {InstName::PAUSE, Executor::execute_PAUSE},
    {InstName::ECALL, Executor::execute_ECALL},
    {InstName::EBREAK, Executor::execute_EBREAK},
    {InstName::ADDIW, Executor::execute_ADDIW},
    {InstName::SLLIW, Executor::execute_SLLIW},
    {InstName::SRLIW, Executor::execute_SRLIW},
    {InstName::SRAIW, Executor::execute_SRAIW},
    {InstName::ADDW, Executor::execute_ADDW},
    {InstName::SUBW, Executor::execute_SUBW},
    {InstName::SLLW, Executor::execute_SLLW},
    {InstName::SRLW, Executor::execute_SRLW},
    {InstName::SRAW, Executor::execute_SRAW}
};
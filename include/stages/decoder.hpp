#pragma once

#include <array>

#include "core/inst.hpp"

//--------------------------------------------------------------------------
// Executor
//--------------------------------------------------------------------------
class Decoder {
private:
// Auxiliary functions
    uint32_t decode_imm_I  (uint32_t inst);
    uint32_t decode_imm_S  (uint32_t inst);
    uint32_t decode_imm_B  (uint32_t inst);
    uint32_t decode_imm_U  (uint32_t inst);
    uint32_t decode_imm_J  (uint32_t inst);
    uint8_t  decode_funct7 (uint32_t inst);
    uint8_t  decode_rs2    (uint32_t inst);
    uint8_t  decode_rs1    (uint32_t inst);
    uint8_t  decode_funct3 (uint32_t inst);
    uint8_t  decode_rd     (uint32_t inst);
    uint8_t  decode_opcode (uint32_t inst);

public:
// Tmp insts, that are returned by decode_inst()
    Inst_R tmp_inst_R;
    Inst_I tmp_inst_I;
    Inst_S tmp_inst_S;
    Inst_B tmp_inst_B;
    Inst_U tmp_inst_U;
    Inst_J tmp_inst_J;

    InstType decode_inst (uint32_t inst);
    InstType decode_inst_map (uint32_t inst);
};
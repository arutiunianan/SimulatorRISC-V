#pragma once

#include "core/inst.hpp"

#include "utils/constants.hpp"

//--------------------------------------------------------------------------
// Executor
//--------------------------------------------------------------------------
class Hart;

namespace Executor 
{
// RV32I Base Instruction Set
    void execute_LUI       (Inst* inst, Hart& hart);
    void execute_AUIPC     (Inst* inst, Hart& hart);
    void execute_JAL       (Inst* inst, Hart& hart);
    void execute_JALR      (Inst* inst, Hart& hart);
    void execute_BEQ       (Inst* inst, Hart& hart);
    void execute_BNE       (Inst* inst, Hart& hart);
    void execute_BLT       (Inst* inst, Hart& hart);
    void execute_BGE       (Inst* inst, Hart& hart);
    void execute_BLTU      (Inst* inst, Hart& hart);
    void execute_BGEU      (Inst* inst, Hart& hart);
    void execute_LB        (Inst* inst, Hart& hart);
    void execute_LH        (Inst* inst, Hart& hart);
    void execute_LW        (Inst* inst, Hart& hart);
    void execute_LBU       (Inst* inst, Hart& hart);
    void execute_LHU       (Inst* inst, Hart& hart);
    void execute_SB        (Inst* inst, Hart& hart);
    void execute_SH        (Inst* inst, Hart& hart);
    void execute_SW        (Inst* inst, Hart& hart);
    void execute_ADDI      (Inst* inst, Hart& hart);
    void execute_SLTI      (Inst* inst, Hart& hart);
    void execute_SLTIU     (Inst* inst, Hart& hart);
    void execute_XORI      (Inst* inst, Hart& hart);
    void execute_ORI       (Inst* inst, Hart& hart);
    void execute_ANDI      (Inst* inst, Hart& hart);
    void execute_ADD       (Inst* inst, Hart& hart);
    void execute_SUB       (Inst* inst, Hart& hart);
    void execute_SLL       (Inst* inst, Hart& hart);
    void execute_SLT       (Inst* inst, Hart& hart);
    void execute_SLTU      (Inst* inst, Hart& hart);
    void execute_XOR       (Inst* inst, Hart& hart);
    void execute_SRL       (Inst* inst, Hart& hart);
    void execute_SRA       (Inst* inst, Hart& hart);
    void execute_OR        (Inst* inst, Hart& hart);
    void execute_AND       (Inst* inst, Hart& hart);
    void execute_FENCE     (Inst* inst, Hart& hart);
    void execute_FENCE_TSO (Inst* inst, Hart& hart);
    void execute_PAUSE     (Inst* inst, Hart& hart);
    void execute_ECALL     (Inst* inst, Hart& hart);
    void execute_EBREAK    (Inst* inst, Hart& hart);

// RV64I Base Instruction Set (in addition to RV32I)
    void execute_LWU   (Inst* inst, Hart& hart);
    void execute_LD    (Inst* inst, Hart& hart);
    void execute_SD    (Inst* inst, Hart& hart);
    void execute_SLLI  (Inst* inst, Hart& hart);
    void execute_SRLI  (Inst* inst, Hart& hart);
    void execute_SRAI  (Inst* inst, Hart& hart);
    void execute_ADDIW (Inst* inst, Hart& hart);
    void execute_SLLIW (Inst* inst, Hart& hart);
    void execute_SRLIW (Inst* inst, Hart& hart);
    void execute_SRAIW (Inst* inst, Hart& hart);
    void execute_ADDW  (Inst* inst, Hart& hart);
    void execute_SUBW  (Inst* inst, Hart& hart);
    void execute_SLLW  (Inst* inst, Hart& hart);
    void execute_SRLW  (Inst* inst, Hart& hart);
    void execute_SRAW  (Inst* inst, Hart& hart);
};
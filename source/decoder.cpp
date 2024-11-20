#include "stages/decoder.hpp"

//--------------------------------------------------------------------------
// Auxiliary functions
//--------------------------------------------------------------------------
inline uint32_t Decoder::decode_imm_I (uint32_t inst) {
    return inst >> 20;                              // imm [11:0]
}

inline uint32_t Decoder::decode_imm_S (uint32_t inst) {
    return ((inst >>  7) & 0b11111)        |        // imm [4:0]
           ((inst >> 25) & 0b1111111) << 5 ;        // imm [11:5] 
}

inline uint32_t Decoder::decode_imm_B (uint32_t inst) {
    return ((inst >> 8)  & 0b1111)   << 1  |        // imm [4:1]
           ((inst >> 25) & 0b111111) << 5  |        // imm [5:10]
           ((inst >> 7)  & 0b1)      << 11 |        // imm [11]
           ((inst >> 31) & 0b1)      << 12 ;        // imm [12]
}

inline uint32_t Decoder::decode_imm_U (uint32_t inst) {
    return inst >> 12;                              // imm [31:12]
}

inline uint32_t Decoder::decode_imm_J (uint32_t inst) {
    return ((inst >> 21) & 0b1111111111) << 1  |    // imm [10:1]
           ((inst >> 20) & 0b1)          << 11 |    // imm [11]
           ((inst >> 12) & 0b11111111)   << 12 |    // imm [19:12]
           ((inst >> 31) & 0b1)          << 20 ;    // imm [20]
}

inline uint8_t Decoder::decode_funct7 (uint32_t inst) {
    return inst >> 25 & 0b1111111;                  // inst [31:25]
}

inline uint8_t Decoder::decode_rs2 (uint32_t inst) {
    return (inst >> 20) & 0b11111;                  // inst [24:20]
}

inline uint8_t Decoder::decode_rs1 (uint32_t inst) {
    return (inst >> 15) & 0b11111;                  // inst [19:15]
}

inline uint8_t Decoder::decode_funct3 (uint32_t inst) {
    return (inst >> 12) & 0b111;                    // inst [14:12]
}

inline uint8_t Decoder::decode_rd (uint32_t inst) {
    return (inst >> 7) & 0b11111;                   // inst [11:7]
}

inline uint8_t Decoder::decode_opcode (uint32_t inst) {
    return inst & 0b1111111;                        // inst [6:0]
}

//--------------------------------------------------------------------------
// Core functions
//--------------------------------------------------------------------------
// Build object of class Inst from incoming inst encoded in uint32_t
Inst* Decoder::decode_inst (uint32_t inst) {
    Inst* result_inst = nullptr;

    InstType inst_type = recognize_inst (inst);
    uint8_t opcode = decode_opcode (inst);

    switch (inst_type) {
        case InstType::R:
            tmp_inst_R.type   = InstType::R;
            tmp_inst_R.opcode = static_cast<Opcode>(opcode);
            tmp_inst_R.funct7 = decode_funct7 (inst);
            tmp_inst_R.rs2    = decode_rs2 (inst);
            tmp_inst_R.rs1    = decode_rs1 (inst);
            tmp_inst_R.funct3 = decode_funct3 (inst);
            tmp_inst_R.rd     = decode_rd (inst);
            result_inst       = new Inst_R;
            *(static_cast<Inst_R*>(result_inst)) = tmp_inst_R;
            break;
        
        case InstType::I:
            tmp_inst_I.type   = InstType::I;
            tmp_inst_I.opcode = static_cast<Opcode>(opcode);
            tmp_inst_I.imm    = decode_imm_I (inst);
            tmp_inst_I.rs1    = decode_rs1 (inst);
            tmp_inst_I.funct3 = decode_funct3 (inst);
            tmp_inst_I.rd     = decode_rd (inst);
            result_inst       = new Inst_I;
            *(static_cast<Inst_I*>(result_inst)) = tmp_inst_I;
            break;
        
        case InstType::S:
            tmp_inst_S.type   = InstType::S;
            tmp_inst_S.opcode = static_cast<Opcode>(opcode);
            tmp_inst_S.imm    = decode_imm_S (inst);
            tmp_inst_S.rs2    = decode_rs2 (inst);
            tmp_inst_S.rs1    = decode_rs1 (inst);
            tmp_inst_S.funct3 = decode_funct3 (inst);
            result_inst       = new Inst_S;
            *(static_cast<Inst_S*>(result_inst)) = tmp_inst_S;
            break;
        
        case InstType::B:
            tmp_inst_B.type   = InstType::B;
            tmp_inst_B.opcode = static_cast<Opcode>(opcode);
            tmp_inst_B.imm    = decode_imm_B (inst);
            tmp_inst_B.rs2    = decode_rs2 (inst);
            tmp_inst_B.rs1    = decode_rs1 (inst);
            tmp_inst_B.funct3 = decode_funct3 (inst);
            result_inst       = new Inst_B;
            *(static_cast<Inst_B*>(result_inst)) = tmp_inst_B;
            break;
        
        case InstType::U:
            tmp_inst_U.type   = InstType::U;
            tmp_inst_U.opcode = static_cast<Opcode>(opcode);
            tmp_inst_U.imm    = decode_imm_U (inst);
            tmp_inst_U.rd     = decode_rd (inst);
            result_inst       = new Inst_U;
            *(static_cast<Inst_U*>(result_inst)) = tmp_inst_U;
            break;
        
        case InstType::J:
            tmp_inst_J.type   = InstType::J;
            tmp_inst_J.opcode = static_cast<Opcode>(opcode);
            tmp_inst_J.imm    = decode_imm_J (inst);
            tmp_inst_J.rd     = decode_rd (inst);
            result_inst       = new Inst_J;
            *(static_cast<Inst_J*>(result_inst)) = tmp_inst_J;
            break;
        
        case InstType::NONE:
            // ERROR
            return nullptr;
    }

    return result_inst;
}

// Recognize instruction relying on opcode, funct3 and funct7 and set appropriate execute_func
InstType Decoder::recognize_inst (uint32_t inst) {
    uint8_t funct7 = decode_funct7 (inst);
    uint8_t funct3 = decode_funct3 (inst);
    Opcode  opcode = static_cast<Opcode>(decode_opcode (inst));

    switch (opcode) {
        case Opcode::LUI:
            tmp_inst_U.name = InstName::LUI;
            tmp_inst_U.execute_func = execute_LUI;
            return InstType::U;

        case Opcode::AUIPC:
            tmp_inst_U.name = InstName::AUIPC;
            tmp_inst_U.execute_func = execute_AUIPC;
            return InstType::U;

        case Opcode::JAL:
            tmp_inst_J.name = InstName::JAL;
            tmp_inst_J.execute_func = execute_JAL;
            return InstType::J;
            
        case Opcode::JALR:
            if (funct3 != 0b000)
                return InstType::NONE;
            tmp_inst_I.name = InstName::JALR;
            tmp_inst_I.execute_func = execute_JALR;
            return InstType::I;

        case Opcode::BRANCH:
            switch (funct3) {
                case 0b000:
                    tmp_inst_B.name = InstName::BEQ;
                    tmp_inst_B.execute_func = execute_BEQ;
                    break;

                case 0b001:
                    tmp_inst_B.name = InstName::BNE;
                    tmp_inst_B.execute_func = execute_BNE;
                    break;

                case 0b100:
                    tmp_inst_B.name = InstName::BLT;
                    tmp_inst_B.execute_func = execute_BLT;
                    break;

                case 0b101:
                    tmp_inst_B.name = InstName::BGE;
                    tmp_inst_B.execute_func = execute_BGE;
                    break;

                case 0b110:
                    tmp_inst_B.name = InstName::BLTU;
                    tmp_inst_B.execute_func = execute_BLTU;
                    break;

                case 0b111:
                    tmp_inst_B.name = InstName::BGEU;
                    tmp_inst_B.execute_func = execute_BGEU;
                    break;

                default:
                    return InstType::NONE;
            }
            return InstType::B;
    
        case Opcode::LOAD:
            switch (funct3) {
                case 0b000:
                    tmp_inst_I.name = InstName::LB;
                    tmp_inst_I.execute_func = execute_LB;
                    break;

                case 0b001:
                    tmp_inst_I.name = InstName::LH;
                    tmp_inst_I.execute_func = execute_LH;
                    break;

                case 0b010:
                    tmp_inst_I.name = InstName::LW;
                    tmp_inst_I.execute_func = execute_LW;
                    break;

                case 0b100:
                    tmp_inst_I.name = InstName::LBU;
                    tmp_inst_I.execute_func = execute_LBU;
                    break;

                case 0b101:
                    tmp_inst_I.name = InstName::LHU;
                    tmp_inst_I.execute_func = execute_LHU;
                    break;

                case 0b110:
                    tmp_inst_I.name = InstName::LWU;
                    tmp_inst_I.execute_func = execute_LWU;
                    break;

                case 0b011:
                    tmp_inst_I.name = InstName::LD;
                    tmp_inst_I.execute_func = execute_LD;
                    break;

                default:
                    return InstType::NONE;
            }
            return InstType::I;

        case Opcode::STORE:
            switch (funct3) {
                case 0b000:
                    tmp_inst_S.name = InstName::SB;
                    tmp_inst_S.execute_func = execute_SB;
                    break;

                case 0b001:
                    tmp_inst_S.name = InstName::SH;
                    tmp_inst_S.execute_func = execute_SH;
                    break;

                case 0b010:
                    tmp_inst_S.name = InstName::SW;
                    tmp_inst_S.execute_func = execute_SW;
                    break;
                
                case 0b011:
                    tmp_inst_S.name = InstName::SD;
                    tmp_inst_S.execute_func = execute_SD;
                    break;

                default:
                    return InstType::NONE;
            }
            return InstType::S;
             
        case Opcode::OP_IMM:
            switch (funct3) {
                case 0b000:
                    tmp_inst_I.name = InstName::ADDI;
                    tmp_inst_I.execute_func = execute_ADDI;
                    break;

                case 0b010:
                    tmp_inst_I.name = InstName::SLTI;
                    tmp_inst_I.execute_func = execute_SLTI;
                    break;

                case 0b011:
                    tmp_inst_I.name = InstName::SLTIU;
                    tmp_inst_I.execute_func = execute_SLTIU;
                    break;

                case 0b100:
                    tmp_inst_I.name = InstName::XORI;
                    tmp_inst_I.execute_func = execute_XORI;
                    break;

                case 0b110:
                    tmp_inst_I.name = InstName::ORI;
                    tmp_inst_I.execute_func = execute_ORI;
                    break;

                case 0b111:
                    tmp_inst_I.name = InstName::ANDI;
                    tmp_inst_I.execute_func = execute_ANDI;
                    break;
                
                case 0b001:
                    if (funct7 >> 1 != 0b000000)
                        return InstType::NONE;
                    tmp_inst_I.name = InstName::SLLI;
                    tmp_inst_I.execute_func = execute_SLLI;
                    break;
                
                case 0b101:
                    switch (funct7 >> 1) {
                        case 0b000000:
                            tmp_inst_I.name = InstName::SRLI;
                            tmp_inst_I.execute_func = execute_SRLI;
                            break;

                        case 0b010000:
                            tmp_inst_I.name = InstName::SRAI;
                            tmp_inst_I.execute_func = execute_SRAI;
                            break;

                        default:
                            return InstType::NONE;
                    }
                    break;

                default:
                    return InstType::NONE;
            }
            return InstType::I;  

        case Opcode::OP:
            switch (funct3) {
                case 0b000:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::ADD;
                            tmp_inst_R.execute_func = execute_ADD;
                            break;

                        case 0b0100000:
                            tmp_inst_R.name = InstName::SUB;
                            tmp_inst_R.execute_func = execute_SUB;
                            break;

                        case 0b0000001:
                            tmp_inst_R.name = InstName::MUL;
                            tmp_inst_R.execute_func = execute_MUL;
                            break;

                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b001:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::SLL;
                            tmp_inst_R.execute_func = execute_SLL;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::MULH;
                            tmp_inst_R.execute_func = execute_MULH;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b010:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::SLT;
                            tmp_inst_R.execute_func = execute_SLT;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::MULHSU;
                            tmp_inst_R.execute_func = execute_MULHSU;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;

                case 0b011:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::SLTU;
                            tmp_inst_R.execute_func = execute_SLTU;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::MULHU;
                            tmp_inst_R.execute_func = execute_MULHU;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b100:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::XOR;
                            tmp_inst_R.execute_func = execute_XOR;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::DIV;
                            tmp_inst_R.execute_func = execute_DIV;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b101:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::SRL;
                            tmp_inst_R.execute_func = execute_SRL;
                            break;

                        case 0b0100000:
                            tmp_inst_R.name = InstName::SRA;
                            tmp_inst_R.execute_func = execute_SRA;
                            break;

                        case 0b0000001:
                            tmp_inst_R.name = InstName::DIVU;
                            tmp_inst_R.execute_func = execute_DIVU;
                            break;

                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b110:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::OR;
                            tmp_inst_R.execute_func = execute_OR;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::REM;
                            tmp_inst_R.execute_func = execute_REM;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b111:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::AND;
                            tmp_inst_R.execute_func = execute_AND;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::REMU;
                            tmp_inst_R.execute_func = execute_REMU;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;

                default:
                    return InstType::NONE;
            }
            return InstType::R;

        case Opcode::MISC_MEM:
            switch (funct3) {
                case 0b000:
                    switch (inst) {
                        case 0b10000011001100000000000000001111:
                            tmp_inst_I.name = InstName::FENCE_TSO;
                            tmp_inst_I.execute_func = execute_FENCE_TSO;
                            break;
                        
                        case 0b00000001000000000000000000001111:
                            tmp_inst_I.name = InstName::PAUSE;
                            tmp_inst_I.execute_func = execute_PAUSE;  
                            break;        

                        default:
                            tmp_inst_I.name = InstName::FENCE;
                            tmp_inst_I.execute_func = execute_FENCE;
                    }
                    break;
                
                case 0b001:
                    tmp_inst_I.name = InstName::FENCE_I;
                    tmp_inst_I.execute_func = execute_FENCE_I;
                    break;
                
                default:
                    return InstType::NONE;
            }
            return InstType::I;

        case Opcode::SYSTEM:
            switch (funct3) {
                case 0b000:
                    switch (inst) {
                        case 0b00000000000000000000000001110011:
                            tmp_inst_I.name = InstName::ECALL;
                            tmp_inst_I.execute_func = execute_ECALL;
                            break;
                        
                        case 0b00000000000100000000000001110011:
                            tmp_inst_I.name = InstName::EBREAK;
                            tmp_inst_I.execute_func = execute_EBREAK;  
                            break;     

                        default:
                            return InstType::NONE;
                    }
                    break;

                case 0b001:
                    tmp_inst_I.name = InstName::CSRRW;
                    tmp_inst_I.execute_func = execute_CSRRW;
                    break;

                case 0b010:
                    tmp_inst_I.name = InstName::CSRRS;
                    tmp_inst_I.execute_func = execute_CSRRS;
                    break;

                case 0b011:
                    tmp_inst_I.name = InstName::CSRRC;
                    tmp_inst_I.execute_func = execute_CSRRC;
                    break;

                case 0b101:
                    tmp_inst_I.name = InstName::CSRRWI;
                    tmp_inst_I.execute_func = execute_CSRRWI;
                    break;

                case 0b110:
                    tmp_inst_I.name = InstName::CSRRSI;
                    tmp_inst_I.execute_func = execute_CSRRSI;
                    break;

                case 0b111:
                    tmp_inst_I.name = InstName::CSRRCI;
                    tmp_inst_I.execute_func = execute_CSRRCI;
                    break;
            }
            return InstType::I;

        case Opcode::OP_IMM_32:
            switch (funct3) {
                case 0b000:
                    tmp_inst_I.name = InstName::ADDIW;
                    tmp_inst_I.execute_func = execute_ADDIW;
                    break;
                
                case 0b001:
                    if (funct7 != 0b0000000)
                        return InstType::NONE;
                    tmp_inst_I.name = InstName::SLLIW;
                    tmp_inst_I.execute_func = execute_SLLIW;
                    break;
                
                case 0b101:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_I.name = InstName::SRLIW;
                            tmp_inst_I.execute_func = execute_SRLIW;
                            break;
                        
                        case 0b0100000:
                            tmp_inst_I.name = InstName::SRAIW;
                            tmp_inst_I.execute_func = execute_SRAIW;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;
                
                default:
                    return InstType::NONE;
            }
            return InstType::I;

        case Opcode::OP_32:
            switch (funct3) {
                case 0b000:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::ADDW;
                            tmp_inst_R.execute_func = execute_ADDW;
                            break;
                        
                        case 0b0100000:
                            tmp_inst_R.name = InstName::SUBW;
                            tmp_inst_R.execute_func = execute_SUBW;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::MULW;
                            tmp_inst_R.execute_func = execute_MULW;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b001:
                    if (funct7 != 0b0000000)
                        return InstType::NONE;
                    tmp_inst_R.name = InstName::SLLW;
                    tmp_inst_R.execute_func = execute_SLLW;
                    break;
                
                case 0b101:
                    switch (funct7) {
                        case 0b0000000:
                            tmp_inst_R.name = InstName::SRLW;
                            tmp_inst_R.execute_func = execute_SRLW;
                            break;
                        
                        case 0b0100000:
                            tmp_inst_R.name = InstName::SRAW;
                            tmp_inst_R.execute_func = execute_SRAW;
                            break;
                        
                        case 0b0000001:
                            tmp_inst_R.name = InstName::DIVUW;
                            tmp_inst_R.execute_func = execute_DIVUW;
                            break;
                        
                        default:
                            return InstType::NONE;
                    }
                    break;
                
                case 0b100:
                    if (funct7 != 0b0000001)
                        return InstType::NONE;
                    tmp_inst_R.name = InstName::DIVW;
                    tmp_inst_R.execute_func = execute_DIVW;
                    break;
                
                case 0b110:
                    if (funct7 != 0b0000001)
                        return InstType::NONE;
                    tmp_inst_R.name = InstName::REMW;
                    tmp_inst_R.execute_func = execute_REMW;
                    break;
                
                case 0b111:
                    if (funct7 != 0b0000001)
                        return InstType::NONE;
                    tmp_inst_R.name = InstName::REMUW;
                    tmp_inst_R.execute_func = execute_REMUW;
                    break;
                
                default:
                    return InstType::NONE;
            }
            return InstType::R;

        default:
            return InstType::NONE;
    }
}
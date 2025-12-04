#pragma once

#include <iostream>
#include <iomanip>
#include <array>
#include <cstdint>
#include <unordered_map>

const std::unordered_map<int, std::string> reg_names = {
    {0,  "zero"}, {1,  "ra"}, {2,  "sp" }, {3,  "gp" },
    {4,  "tp"  }, {5,  "t0"}, {6,  "t1" }, {7,  "t2" },
    {8,  "s0"  }, {9,  "s1"}, {10, "a0" }, {11, "a1" },
    {12, "a2"  }, {13, "a3"}, {14, "a4" }, {15, "a5" },
    {16, "a6"  }, {17, "a7"}, {18, "s2" }, {19, "s3" },
    {20, "s4"  }, {21, "s5"}, {22, "s6" }, {23, "s7" },
    {24, "s8"  }, {25, "s9"}, {26, "s10"}, {27, "s11"},
    {28, "t3"  }, {29, "t4"}, {30, "t5" }, {31, "t6" }
};

//--------------------------------------------------------------------------
// Reg
//--------------------------------------------------------------------------
class Reg final {
private:
    uint64_t val = 0;

public:
    inline uint64_t get_val () { return val; }
    inline void set_val (uint64_t v) { val = v; }
};

//--------------------------------------------------------------------------
// Regfile
//--------------------------------------------------------------------------
class Regfile final {
private:
    friend class Hart;
    std::array<Reg, 32> regs;

public:
    inline uint64_t get_reg_val (uint8_t reg) { return regs[reg].get_val(); }
    inline void set_reg_val (uint8_t reg, uint64_t v) { regs[reg].set_val (v); }

    void dump (std::ostream& ostr = std::cout) {
        for (int i = 0; i < 32; i++) {
            uint64_t val = get_reg_val (i);

            ostr << std::dec << "x_" << std::setfill ('0') << std::setw(2) << i
                 << " [";

            if (val != 0) {
                ostr << "\033[32m";
            }

            ostr << std::setw(20) << val << "\033[0m] " << "(";

            if (val != 0) {
                ostr << "\033[32m";
            }

            ostr << "0x" << std::setw(16) << std::hex << val << "\033[0m)" << std::endl;
        }
    }

    void spike_type_dump (std::ostream& ostr = std::cout) {
        for (int i = 0; i < 32; i += 4) {
            for (int j = 0; j < 4; j++) {
                uint64_t val = get_reg_val (i+j);

                ostr << std::setw(4) << reg_names.at(i+j) << ": 0x" 
                     << std::setw(16) << std::setfill ('0') << std::hex 
                     << val << std::setfill (' ');
            }
            ostr << std::endl;
        }
    }
};
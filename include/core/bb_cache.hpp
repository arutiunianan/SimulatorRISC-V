#pragma once

#include <unordered_map>
#include <vector>
#include <cassert>
#include <iostream>

#include "core/inst.hpp"
#include "utils/constants.hpp"

class BBCache {
private:
    std::unordered_map <uint64_t, std::vector<Inst*>> map;

public:
    inline bool in_cache_p (uint64_t bb_start_pc) {
        return map.find (bb_start_pc) != map.end();
    }

    inline void add_inst (uint64_t bb_start_pc, Inst* inst) {
                map[bb_start_pc].push_back (inst);
    }

    inline int get_bb_size (uint64_t bb_start_pc) {
        return map[bb_start_pc].size();
    } 

    inline Inst* get_next_inst (uint64_t bb_start_pc, uint64_t inst_addr) {
        return map[bb_start_pc][(inst_addr - bb_start_pc) / WORD_SIZE];
    }

    void execute_bb (uint64_t bb_start_pc, Hart& hart) {
        for (auto inst : map[bb_start_pc])
            inst->execute_func (inst, hart);
    }

    ~BBCache () {
        for (auto& pair : map) {
            for (auto& inst : pair.second) {
                delete inst;
            }
        }
    }
};
#pragma once

#include <cstdint>
#include <vector>
#include <optional>

class TLB {
public:
    struct Entry {
        uint64_t vpn = 0;
        uint64_t ppn = 0;
        uint8_t  perm = 0; // bits: bit0 = V, bit1 = R, bit2 = W, bit3 = X
        bool     valid = false;
    };

    TLB (size_t sz = 16): size (sz), entries (sz) {}

    std::optional<Entry> lookup (uint64_t vpn) {
        for (size_t i = 0; i < size; ++i) {
            if (entries[i].valid && entries[i].vpn == vpn) {
                return entries[i];
            }
        }
        return std::nullopt;
    }

    //(FIFO)
    void insert (uint64_t vpn, uint64_t ppn, uint8_t perm) {
        entries[next_insert].vpn   = vpn;
        entries[next_insert].ppn   = ppn;
        entries[next_insert].perm  = perm;
        entries[next_insert].valid = true;
        next_insert = (next_insert + 1) % size;
    }

    void flush () {
        for (auto &e: entries) {
            e.valid = false;
        }
        next_insert = 0;
    }

private:
    size_t size;
    std::vector<Entry> entries;
    size_t next_insert = 0;
};
#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <optional>

#include "tlb.hpp"
#include "utils/constants.hpp"

class Hart;

enum class AccessType {
    IFETCH,
    LOAD,
    STORE
};

struct SATP {
    uint64_t raw = 0;
    static const uint64_t SV39MODE = 1ULL << 60;

    inline uint64_t mode () const { return (raw >> 60) & 0xF; }
    inline uint64_t asid () const { return (raw >> 44) & 0xFFFF; }
    inline uint64_t ppn ()  const { return raw & ((1ULL << 44) - 1); }
};

class MMU {
public:
    SATP satp;

public:
    MMU (Hart& hart, size_t tlb_size = 16);

    // Translate virtual address -> physical address.
    bool translate (uint64_t vaddr, uint64_t &paddr, AccessType atype);

    void flush_tlb () { tlb.flush(); }

private:
    Hart& hart;
    TLB tlb;

private:
    bool page_walk (uint64_t vaddr, uint64_t &paddr, uint8_t &out_perm, AccessType atype);

    // Helpers
    uint64_t vpn_from_va (uint64_t va) { return va >> 12; }
    uint64_t page_offset (uint64_t va) { return va & 0xFFF; }
};
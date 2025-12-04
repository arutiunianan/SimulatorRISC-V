#include "memory/mmu.hpp"
#include "hart.hpp"
#include "utils/constants.hpp"
#include <cstring>

MMU::MMU (Hart& h, size_t tlb_size): hart (h), tlb (tlb_size) {
    satp.raw = SATP::SV39MODE | (DEFAULT_MEM_SIZE / 2);
}

void set_perm_flag (uint8_t &perm_bits, uint8_t perm, const uint64_t mask) {
    if (perm & mask) {
        perm_bits |= mask >> 0;
    }
}

bool MMU::translate (uint64_t vaddr, uint64_t &paddr, AccessType atype) {
    if (satp.mode () == 0) {
        paddr = vaddr - hart.get_start_addr();
        return true;
    }

    // TLB lookup
    uint64_t vpn = vpn_from_va (vaddr);
    auto e = tlb.lookup (vpn);
    if (e.has_value ()) {
        uint8_t perm = e->perm;
        if (atype == AccessType::LOAD && !(perm & (PTE_R_MASK >> 0))) {
            return false;
        }
        if (atype == AccessType::STORE && !(perm & (PTE_W_MASK >> 0))) {
            return false;
        }
        if (atype == AccessType::IFETCH && !(perm & (PTE_X_MASK >> 0))) {
            return false;
        }

        paddr = (e->ppn << 12) | page_offset (vaddr);
        return true;
    }

    // TLB miss -> page walk
    uint8_t perm = 0;
    if (!page_walk (vaddr, paddr, perm, atype)) {
        return false;
    }

    // insert to TLB
    uint64_t found_vpn = vpn;
    uint64_t found_ppn = vpn_from_va (paddr);
    uint8_t  perm_bits = 0;
    set_perm_flag (perm_bits, perm, PTE_V_MASK);
    set_perm_flag (perm_bits, perm, PTE_R_MASK);
    set_perm_flag (perm_bits, perm, PTE_W_MASK);
    set_perm_flag (perm_bits, perm, PTE_X_MASK);

    tlb.insert (found_vpn, found_ppn, perm_bits);

    if ((atype == AccessType::LOAD   && !(perm & PTE_R_MASK)) ||
        (atype == AccessType::STORE  && !(perm & PTE_W_MASK)) ||
        (atype == AccessType::IFETCH && !(perm & PTE_X_MASK))) {
        return false;
    }

    return true;
}

bool MMU::page_walk (uint64_t vaddr, uint64_t &paddr, uint8_t &out_perm, AccessType atype) {
    // Simplified Sv39-like walk: 3 levels, page size 4KB
    // const int levels = 3;
    uint64_t vpn[3];
    vpn[0] = (vaddr >> 12) & 0x1FF;
    vpn[1] = (vaddr >> 21) & 0x1FF;
    vpn[2] = (vaddr >> 30) & 0x1FF;

    uint64_t base_addr = satp.ppn () << 12;

    if (atype == AccessType::STORE ) {
        uint64_t pte_addr = base_addr + (vpn[0] * 8);
        uint64_t new_ppn = vpn_from_va(vaddr - hart.get_start_addr ());
        uint64_t pte = (new_ppn << PTE_PPN_SHIFT) | PTE_V_MASK | PTE_R_MASK | PTE_W_MASK | PTE_X_MASK;
        hart.write_phys_u64 (pte_addr, pte);
        paddr = (new_ppn << 12) | (vaddr & 0xFFF);
        out_perm = PTE_V_MASK | PTE_R_MASK | PTE_W_MASK | PTE_X_MASK;
        return true;
    }

    for (int level = 2; level >= 0; --level) {
        uint64_t pte_addr = base_addr + (vpn[level] * 8); // each PTE is 8 bytes

        uint64_t pte = 0;
        if (!hart.read_phys_u64 (pte_addr, pte)) {
            return false;
        }

        bool v_bit = (pte & PTE_V_MASK);
        bool r_bit = (pte & PTE_R_MASK);
        bool w_bit = (pte & PTE_W_MASK);
        bool x_bit = (pte & PTE_X_MASK);

        if (!v_bit) {
            return false;
        }

        if (r_bit || w_bit || x_bit) {
            uint64_t ppn = (pte >> PTE_PPN_SHIFT);
            paddr = (ppn << 12) | (vaddr & 0xFFF);
            out_perm = (uint8_t)(pte & 0xFF);
            return true;
        } else {
            uint64_t next_ppn = (pte >> PTE_PPN_SHIFT);
            base_addr = next_ppn << 12;
        }
    }

    return false;
}
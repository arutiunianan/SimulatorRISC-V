#pragma once

static const int BYTE_SIZE  = sizeof(uint8_t);
static const int HWORD_SIZE = sizeof(uint16_t);
static const int WORD_SIZE  = sizeof(uint32_t);
static const int DWORD_SIZE = sizeof(uint64_t);

static const int VPAGE_SIZE        = 1 << 12;
static const int VPAGE_OFFSET_MASK = VPAGE_SIZE - 1;
static const int DEFAULT_MEM_SIZE  = 1 << 30;

//--------------------FOR PTE--------------------
static const uint64_t PTE_V_MASK = 1ull << 0;
static const uint64_t PTE_R_MASK = 1ull << 1;
static const uint64_t PTE_W_MASK = 1ull << 2;
static const uint64_t PTE_X_MASK = 1ull << 3;
static const uint64_t PTE_PPN_SHIFT = 10;

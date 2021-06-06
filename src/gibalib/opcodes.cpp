//
// Created by daholland on 5/4/21.
//

#include "opcodes.h"

using namespace opcodes;

//constexpr OpCycleTime::OpCycleTime(uint8_t nCycles)
//    : trueBranchTime{nCycles}, falseBranchTime{nCycles} {}
//
//constexpr OpCycleTime::OpCycleTime(uint8_t trueCycles, uint8_t falseCycles)
//    : trueBranchTime{trueCycles}, falseBranchTime{falseCycles} {}
//
//constexpr FlagAffections::FlagAffections(FlagEffect z, FlagEffect n, FlagEffect h, FlagEffect c)
//    : Z{z}, N{n}, H{h}, C{c} {}
//
//constexpr Opcode::Opcode(uint8_t hex, std::string_view mnemonic,
//        size_t bytes, OpCycleTime cycles,
//        Operands_t operands, FlagAffections flagAffects)
//        : Hex{hex}, Mnemonic{mnemonic},
//        Bytes{bytes}, Cycles{cycles},
//        Operands{operands}, FlagAffects{flagAffects} {}
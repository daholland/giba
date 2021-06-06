//
// Created by daholland on 5/4/21.
//

#ifndef GIBA_OPCODES_H
#define GIBA_OPCODES_H

#include <variant>
#include <array>
#include <cstdint>
#include <string>

namespace opcodes {

    struct Operand {
        uint8_t val;
        bool immediate;
        std::string_view name;
        constexpr explicit Operand(uint8_t val, bool immediate = false, std::string_view name = "") : val{val}, immediate{immediate}, name{name} {}
    };

    struct OpCycleTime {
        uint8_t trueBranchTime;
        uint8_t falseBranchTime;

        constexpr explicit OpCycleTime(uint8_t nCycles) : trueBranchTime{nCycles}, falseBranchTime{nCycles} {}
        constexpr OpCycleTime(uint8_t trueCycles, uint8_t falseCycles) : trueBranchTime{trueCycles}, falseBranchTime{falseCycles} {}
    };

    enum struct FlagEffect {
        Reset, //'0'
        Set,  //'1'
        Noop, //'-'
        OpDependant //'Z'
    };

    struct FlagAffections {
        FlagEffect Z;
        FlagEffect N;
        FlagEffect H;
        FlagEffect C;

        constexpr FlagAffections(FlagEffect z, FlagEffect n, FlagEffect h, FlagEffect c)
                    : Z{z}, N{n}, H{h}, C{c} {}
    };

    struct Opcode {
        using Operands_t = std::variant<std::array<Operand,0>, std::array<Operand,1>,std::array<Operand,2>>;
        uint8_t Hex;
        std::string_view Mnemonic;
        size_t Bytes; //1 2 or 3 based on operands.size()
        OpCycleTime Cycles;
        Operands_t Operands; //0, 1 u8 or 2 u8(1 u16)
        FlagAffections FlagAffects;

        constexpr Opcode(uint8_t hex, std::string_view mnemonic,
                      size_t bytes, OpCycleTime cycles,
                      Operands_t operands, FlagAffections flagAffects) :
                      Hex{hex}, Mnemonic{mnemonic},
                      Bytes{bytes}, Cycles{cycles},
                      Operands{operands}, FlagAffects{flagAffects} {}
        //constructor = default
        //copy assign = default
        //copy constructor = default
        //move assign = delete
        //move const = delete
    };

    constexpr std::array<Opcode, 0x100> OpCodeTable{
        Opcode{0x00, "NOP", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x01, "LD", 3, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "BC"},Operand {0, true, "d16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x02, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "BC"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x03, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "BC"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x04, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x05, "DEC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x06, "LD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x07, "RLCA", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Reset, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x08, "LD", 3, OpCycleTime{20}, std::array<Operand, 2>{Operand {0, true, "a16"},Operand {0, true, "SP"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x09, "ADD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "HL"},Operand {0, true, "BC"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x0A, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "BC"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x0B, "DEC", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "BC"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x0C, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x0D, "DEC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x0E, "LD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x0F, "RRCA", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Reset, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x10, "STOP", 2, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "d8"}},    //0x10
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x11, "LD", 3, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "DE"},Operand {0, true, "d16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x12, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "DE"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x13, "INC", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "DE"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x14, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x15, "DEC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x16, "LD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x17, "RLA", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Reset, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x18, "JR", 2, OpCycleTime{12}, std::array<Operand, 1>{Operand {0, true, "r8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x19, "ADD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "HL"},Operand {0, true, "DE"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x1A, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "DE"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x1B, "DEC", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "DE"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x1C, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x1D, "DEC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x1E, "LD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x1F, "RRA", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Reset, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x20, "JR", 2, OpCycleTime{12,8}, std::array<Operand, 2>{Operand {0, true, "NZ"},Operand {0, true, "r8"}},    //0x20
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x21, "LD", 3, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "HL"},Operand {0, true, "d16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x22, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL+"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x23, "INC", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x24, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x25, "DEC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x26, "LD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x27, "DAA", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Noop,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x28, "JR", 2, OpCycleTime{12,8}, std::array<Operand, 2>{Operand {0, true, "Z"},Operand {0, true, "r8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x29, "ADD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "HL"},Operand {0, true, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x2A, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "HL+"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x2B, "DEC", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x2C, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x2D, "DEC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x2E, "LD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x2F, "CPL", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Set,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x30, "JR", 2, OpCycleTime{12,8}, std::array<Operand, 2>{Operand {0, true, "NC"},Operand {0, true, "r8"}},  //0x30
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x31, "LD", 3, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "SP"},Operand {0, true, "d16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x32, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL-"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x33, "INC", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "SP"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x34, "INC", 1, OpCycleTime{12}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x35, "DEC", 1, OpCycleTime{12}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x36, "LD", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x37, "SCF", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Set}},

        Opcode{0x38, "JR", 2, OpCycleTime{12,8}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "r8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x39, "ADD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "HL"},Operand {0, true, "SP"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x3A, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "HL-"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x3B, "DEC", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "SP"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x3C, "INC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x3D, "DEC", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::Noop}},

        Opcode{0x3E, "LD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x3F, "CCF", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x40, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x41, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x42, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x43, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x44, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x45, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x46, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x47, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "B"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x48, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x49, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x4A, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x4B, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x4C, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x4D, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x4E, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x4F, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x50, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x51, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x52, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x53, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x54, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x55, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x56, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x57, "NOP", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "D"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x58, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x59, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x5A, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x5B, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x5C, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x5D, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x5E, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x5F, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "E"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x60, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x61, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x62, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x63, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x64, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x65, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x66, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x67, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "H"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x68, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x69, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x6A, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x6B, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x6C, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x6D, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x6E, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x6F, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "L"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x70, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x71, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x72, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x73, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x74, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x75, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x76, "HALT", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x77, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "HL"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x78, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x79, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x7A, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x7B, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x7C, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x7D, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x7E, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x7F, "LD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x80, "ADD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x81, "ADD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x82, "ADD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x83, "ADD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x84, "ADD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x85, "ADD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x86, "ADD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x87, "ADD", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x88, "ADC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x89, "ADC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x8A, "ADC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x8B, "ADC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x8C, "ADC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x8D, "ADC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x8E, "ADC", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x8F, "ADC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"}, Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x90, "SUB", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x91, "SUB", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x92, "SUB", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x93, "SUB", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x94, "SUB", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x95, "SUB", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x96, "SUB", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x97, "SUB", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Set, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x98, "SBC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x99, "SBC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x9A, "SBC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x9B, "SBC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x9C, "SBC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x9D, "SBC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x9E, "SBC", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0x9F, "SBC", 1, OpCycleTime{4}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xA0, "AND", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA1, "AND", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA2, "AND", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA3, "AND", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA4, "AND", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA5, "AND", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA6, "AND", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA7, "AND", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xA8, "XOR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xA9, "XOR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xAA, "XOR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xAB, "XOR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xAC, "XOR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xAD, "XOR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xAE, "XOR", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xAF, "XOR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Set, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB0, "OR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB1, "OR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB2, "OR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB3, "OR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB4, "OR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB5, "OR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB6, "OR", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB7, "OR", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xB8, "CP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xB9, "CP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xBA, "CP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xBB, "CP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xBC, "CP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xBD, "CP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xBE, "CP", 1, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xBF, "CP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Set, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xC0, "RET", 1, OpCycleTime{20,8}, std::array<Operand, 1>{Operand {0, true, "NZ"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC1, "POP", 1, OpCycleTime{12}, std::array<Operand, 1>{Operand {0, true, "BC"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC2, "JP", 3, OpCycleTime{16,12}, std::array<Operand, 2>{Operand {0, true, "NZ"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC3, "JP", 3, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC4, "CALL", 3, OpCycleTime{24,12}, std::array<Operand, 2>{Operand {0, true, "NZ"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC5, "PUSH", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "BC"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC6, "ADD", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xC7, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "00H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC8, "RET", 1, OpCycleTime{20,8}, std::array<Operand, 1>{Operand {0, true, "Z"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC9, "RET", 1, OpCycleTime{16}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCA, "JP", 3, OpCycleTime{16,12}, std::array<Operand, 2>{Operand {0, true, "Z"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCB, "PREFIX", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCC, "CALL", 3, OpCycleTime{24,12}, std::array<Operand, 2>{Operand {0, true, "Z"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCD, "CALL", 3, OpCycleTime{24}, std::array<Operand, 1>{Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCE, "ADC", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xCF, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "08H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD0, "RET", 1, OpCycleTime{20,8}, std::array<Operand, 1>{Operand {0, true, "NC"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD1, "POP", 1, OpCycleTime{12}, std::array<Operand, 1>{Operand {0, true, "DE"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD2, "JP", 1, OpCycleTime{16,12}, std::array<Operand, 2>{Operand {0, true, "NC"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD3, "ILLEGAL_D3", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD4, "CALL", 3, OpCycleTime{24,12}, std::array<Operand, 2>{Operand {0, true, "NC"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD5, "PUSH", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "DE"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD6, "SUB", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xD7, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "10H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD8, "RET", 1, OpCycleTime{20,8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD9, "RETI", 1, OpCycleTime{16}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDA, "JP", 3, OpCycleTime{16,12}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDB, "ILLEGAL_DB", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDC, "CALL", 3, OpCycleTime{24,12}, std::array<Operand, 2>{Operand {0, true, "C"},Operand {0, true, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDD, "ILLEGAL_DD", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDE, "SBC", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xDF, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "18H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE0, "LDH", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, false, "a8"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE1, "POP", 1, OpCycleTime{12}, std::array<Operand, 1>{Operand {0, true, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE2, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, false, "C"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE3, "ILLEGAL_E3", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE4, "ILLEGAL_E4", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE5, "PUSH", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE6, "AND", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Reset}},

        Opcode{0xE7, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "20H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE8, "ADD", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "SP"},Operand {0, true, "r8"}},
               FlagAffections{FlagEffect::Reset, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xE9, "JP", 1, OpCycleTime{4}, std::array<Operand, 1>{Operand {0, true, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEA, "LD", 3, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, false, "a16"},Operand {0, true, "A"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEB, "ILLEGAL_EB", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEC, "ILLEGAL_EC", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xED, "ILLEGAL_ED", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEE, "XOR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xEF, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "28H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF0, "LDH", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "a8"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF1, "POP", 1, OpCycleTime{12}, std::array<Operand, 1>{Operand {0, true, "AF"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::OpDependant,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xF2, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "C"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF3, "DI", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF4, "ILLEGAL_F4", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF5, "PUSH", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "AF"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF6, "OR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0xF7, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "30H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF8, "LD", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "HL"},Operand {0, true, "SP+r8"}},
               FlagAffections{FlagEffect::Reset, FlagEffect::Reset,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xF9, "LD", 1, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "SP"},Operand {0, true, "HL"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFA, "LD", 3, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "A"},Operand {0, false, "a16"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFB, "EI", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFC, "ILLEGAL_FC", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFD, "ILLEGAL_FD", 1, OpCycleTime{4}, std::array<Operand, 0>{},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFE, "CP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "d8"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Set,
                              FlagEffect::OpDependant, FlagEffect::OpDependant}},

        Opcode{0xFF, "RST", 1, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, true, "38H"}},
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}}
    };


/// 0xCB PREFIX table
    constexpr std::array<Opcode, 0x100> PrefixOpCodeTable{
        Opcode{0x00, "RLC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x01, "RLC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x02, "RLC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x03, "RLC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x04, "RLC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x05, "RLC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x06, "RLC", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x07, "RLC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x08, "RRC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x09, "RRC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x0A, "RRC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x0B, "RRC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x0C, "RRC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x0D, "RRC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x0E, "RRC", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x0F, "RRC", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x10, "RL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x11, "RL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x12, "RL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x13, "RL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x14, "RL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x15, "RL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x16, "RL", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x17, "RL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x18, "RR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x19, "RR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x1A, "RR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x1B, "RR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x1C, "RR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x1D, "RR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x1E, "RR", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x1F, "RR", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x20, "SLA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x21, "SLA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "c"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x22, "SLA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x23, "SLA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x24, "SLA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x25, "SLA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x26, "SLA", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x27, "SLA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x28, "SRA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x29, "SRA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x2A, "SRA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x2B, "SRA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x2C, "SRA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x2D, "SRA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x2E, "SRA", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x2F, "SRA", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x30, "SWAP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x31, "SWAP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x32, "SWAP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x33, "SWAP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x34, "SWAP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x35, "SWAP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x36, "SWAP", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x37, "SWAP", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::Reset}},

        Opcode{0x38, "SRL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "B"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x39, "SRL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "C"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x3A, "SRL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "D"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x3B, "SRL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "E"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x3C, "SRL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "H"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x3D, "SRL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "L"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x3E, "SRL", 2, OpCycleTime{16}, std::array<Operand, 1>{Operand {0, false, "HL"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x3F, "SRL", 2, OpCycleTime{8}, std::array<Operand, 1>{Operand {0, true, "A"}},
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Reset, FlagEffect::OpDependant}},

        Opcode{0x40, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x41, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x42, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x43, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x44, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x45, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x46, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x47, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x48, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x49, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x4A, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x4B, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x4C, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x4D, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x4E, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x4F, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x50, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x51, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x52, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x53, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x54, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x55, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x56, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x57, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x58, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x59, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x5A, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x5B, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x5C, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x5D, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x5E, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x5F, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x60, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x61, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x62, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x63, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x64, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x65, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x66, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x67, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x68, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x69, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x6A, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x6B, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x6C, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x6D, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x6E, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x6F, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x70, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x71, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x72, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x73, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x74, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x75, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x76, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x77, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x78, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x79, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x7A, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x7B, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x7C, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x7D, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x7E, "BIT", 2, OpCycleTime{12}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x7F, "BIT", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::OpDependant, FlagEffect::Reset,
                              FlagEffect::Set, FlagEffect::Noop}},

        Opcode{0x80, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x81, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x82, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x83, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x84, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x85, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x86, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x87, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x88, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x89, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x8A, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x8B, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x8C, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x8D, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x8E, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x8F, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x90, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x91, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x92, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x93, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x94, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x95, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x96, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x97, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x98, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x99, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x9A, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x9B, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x9C, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x9D, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x9E, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0x9F, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA0, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA1, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA2, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA3, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA4, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA5, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA6, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA7, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA8, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xA9, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xAA, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xAB, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xAC, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xAD, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xAE, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xAF, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

       Opcode{0xB0, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB1, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB2, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB3, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB4, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB5, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB6, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB7, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB8, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xB9, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xBA, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xBB, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xBC, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xBD, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xBE, "RES", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xBF, "RES", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC0, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC1, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC2, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC3, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC4, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC5, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC6, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC7, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "0"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC8, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xC9, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCA, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCB, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCC, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCD, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCE, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xCF, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "1"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD0, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD1, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD2, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD3, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD4, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD5, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD6, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD7, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "2"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD8, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xD9, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDA, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDB, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDC, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDD, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDE, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xDF, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "3"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE0, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE1, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE2, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE3, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE4, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE5, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE6, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE7, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "4"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE8, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xE9, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEA, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEB, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEC, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xED, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEE, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xEF, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "5"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF0, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF1, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF2, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF3, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF4, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF5, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF6, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF7, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "6"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF8, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "B"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xF9, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "C"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFA, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "D"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFB, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "E"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFC, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "H"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFD, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "L"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFE, "SET", 2, OpCycleTime{16}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, false, "HL"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}},

        Opcode{0xFF, "SET", 2, OpCycleTime{8}, std::array<Operand, 2>{Operand {0, true, "7"},Operand {0, true, "A"}},    //0x40
               FlagAffections{FlagEffect::Noop, FlagEffect::Noop,
                              FlagEffect::Noop, FlagEffect::Noop}}
    };
}

#endif //GIBA_OPCODES_H

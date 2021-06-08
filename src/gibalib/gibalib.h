//
// Created by daholland on 3/23/21.
//

#ifndef GIBA_GIBALIB_H
#define GIBA_GIBALIB_H

#include <filesystem>
#include "opcodes.h"
#include <vector>


namespace gibalib {
    class PPU {}; //technically a part of the cpu
    class APU {};
    class Input {
    public:
        bool A = false;
        bool B = false;
        bool Start = false;
        bool Select = false;
        bool Up = false;
        bool Down = false;
        bool Left = false;
        bool Right = false;
        uint8_t as_byte();
    };
    class LCD {
        //should just focus on producing a bitmap representing the LCD;
        //let spock take care of bitmap -> SDL -> screen drawing
        // "The LCD controller operates on a 2^22 Hz = 4.194 MHz dot clock.
        // An entire frame is 154 scanlines, 70224 dots, or 16.74 ms"
    };
    class Memory {
    public:
        void write(uint16_t address, uint8_t val);
        void write(uint16_t address, uint16_t val);

        uint8_t read_byte(uint16_t address);
        uint16_t read_word(uint16_t address);
    private:
        std::array<uint8_t, 0x10000> mem{};
    };

    class MBC {
        //different types, writing to rom in main memory
        // controls bank selection
        // various peripherals too here
        //contains registers itself, use
        //part of the cartridge
    };
    class Cartridge {
    public:
        std::vector<uint8_t> rom;
        std::vector<uint8_t> ram;
        //time? peripherals?
        Cartridge();
        explicit Cartridge(std::filesystem::path filePath);
    };

    struct CPURegisters {
        uint8_t A;
        uint8_t F;
        uint8_t B;
        uint8_t C;
        uint8_t D;
        uint8_t E;
        uint8_t H;
        uint8_t L;
        uint16_t PC;
    };

    class CPU {
        //register class itself?
        CPURegisters registers;

        const int cycle_speed = clock_speed / 4; //m-cycle
        const int clock_speed = 4.194304 * 1'000'000; //t-cycle
        const std::array<opcodes::Opcode, 256> opcodes = opcodes::OpCodeTable;
        const std::array<opcodes::Opcode, 256> opcodes_prefixed = opcodes::PrefixOpCodeTable;

        uint16_t BC();
        void BC(uint16_t);
        uint16_t DE();
        void DE(uint16_t);
        uint16_t HL();
        void HL(uint16_t);
        //tick(); //t-cycle? m-cycle? probably t-cycle so ppu can do its thing?

    public:
        opcodes::Opcode fetch_opcode(uint8_t instruction);
        opcodes::Opcode fetch_opcode_prefixed(uint8_t prefixed_instruction);
    };
    class ColorCPU : CPU {
        const int clock_speed = 4.194304 * 1'000'000 * 2;
    };

    class Motherboard {
    public:
        bool powerStatus = false;
        void load_cart(std::unique_ptr<Cartridge> cartInserted);
    private:
        friend class Gibalib;
        std::unique_ptr<Cartridge> cart{std::make_unique<Cartridge>()};
        std::unique_ptr<Input> input{std::make_unique<Input>()};
        std::unique_ptr<Memory> memory{std::make_unique<Memory>()};
        std::unique_ptr<CPU> cpu{std::make_unique<CPU>()};
    };

class Gibalib {
public:
    Gibalib();
    void cleanup();

    bool load_cart_from_file(std::filesystem::path filePath);
    void power_on(){ system->powerStatus = true; };
    void power_off(){ system->powerStatus = false; };
    void insert_cart_and_power_on(std::filesystem::path filePath);
    void foobar();

    [[nodiscard]] const Input& get_input() const;
    void set_input(Input& input);

    [[nodiscard]] const Cartridge& get_cart() const;

    [[nodiscard]] const Memory& get_memory() const;
private:
    std::unique_ptr<Motherboard> system;

    /*
     * CPU
     * - Registers AF BC DE HL PC SP
     * - timers
     * - opcodes
     * DMA Controller
     *
     * MEMORY
     * - regions
     * - cart banking MBC
     * - 0x0000 - 0xFFFF
     * PPU
     * LCD
     * CART
     * - different MBC/RAM configs
     * AUDIO
     * INPUT
     *
     * CPU 	8-bit Sharp LR35902
     * Clock 	4.194304 MHz 		8.388608 MHz 	4.295454MHz (SGB1, NTSC)
     * Work RAM 	8 KB 		32 KB
     * Video RAM 	8 KB 		16 KB
     * Screen Size 	LCD 4,7 x 4,3 cm 	LCD 4,8 x 4,4 cm 	TFT 4,4 x 4 cm
     * Resolution 	160x144 			256x224 (including border)
     * Sprites 	Max 40 per screen, 10 per line
     * Palettes 	1x4 BG, 2x3 OBJ 		8x4 BG, 8x3 OBJ 	1+4x3, 4x15 (border)
     * Colors 	4 grayshades 		32768 colors
     * Horizontal Sync 	9.198 KHz 			9.41986 KHz
     * Vertical Sync 	59.73 Hz 			61.1679 Hz
     * Sound 	4 channels with stereo sound
     * Power 	DC6V 0.7W
     *
Start 	End 	Description 	Notes
0000 	3FFF 	16 KiB ROM bank 00 	From cartridge, usually a fixed bank
4000 	7FFF 	16 KiB ROM Bank 01~NN 	From cartridge, switchable bank via mapper (if any)
8000 	9FFF 	8 KiB Video RAM (VRAM) 	In CGB mode, switchable bank 0/1
A000 	BFFF 	8 KiB External RAM 	From cartridge, switchable bank if any
C000 	CFFF 	4 KiB Work RAM (WRAM)
D000 	DFFF 	4 KiB Work RAM (WRAM) 	In CGB mode, switchable bank 1~7
E000 	FDFF 	Mirror of C000~DDFF (ECHO RAM) 	Nintendo says use of this area is prohibited.
FE00 	FE9F 	Sprite attribute table (OAM)
FEA0 	FEFF 	Not Usable 	Nintendo says use of this area is prohibited
FF00 	FF7F 	I/O Registers
FF80 	FFFE 	High RAM (HRAM)
FFFF 	FFFF 	Interrupts Enable Register (IE)
     */
};
}

#endif //GIBA_GIBALIB_H

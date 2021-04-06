//
// Created by daholland on 3/23/21.
//

#ifndef GIBA_GIBALIB_H
#define GIBA_GIBALIB_H
namespace giba {

class Gibalib {
    int foo;

public:
    Gibalib(int f);
    void debugtest();
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

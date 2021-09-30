//
// Created by daholland on 3/23/21.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <span>

#include "gibalib.h"

using namespace gibalib;

namespace exceptions {
#include <stdexcept>

    class BadRomPathException : public std::runtime_error {
    public:
        explicit BadRomPathException(std::string pathTried) : std::runtime_error("Bad Rom Path: " + pathTried) {}
    };

    class CartNotInsertedException : public std::runtime_error {
    public:
        explicit CartNotInsertedException() : std::runtime_error("Cart Not inserted") {}
    };

}


Gibalib::Gibalib() : system{std::make_unique<Motherboard>()} {}

void Gibalib::cleanup() { std::cout << "giba clean" << std::endl; }

bool Gibalib::load_cart_from_file(std::filesystem::path file) {
    auto cart = std::make_unique<Cartridge>(file);

    system->load_cart(std::move(cart));

    return true;
}

const Cartridge& Gibalib::get_cart() const {
    return  *system->cart;
}

const Input& Gibalib::get_input() const {
    return *system->input;
}

void Gibalib::insert_cart_and_power_on(std::filesystem::path filePath) {
    load_cart_from_file(filePath);
    power_on();
}

void Gibalib::set_input(Input& input) {
    *system->input = input;
}

void Gibalib::foobar() {
   auto quuz = system->cpu->fetch_opcode(0x1E);
   std::cout << "fetched mnemonic: " << quuz.Mnemonic << std::endl;
}


void Motherboard::load_cart(std::unique_ptr<Cartridge> cartInserted) {
    cart = std::move(cartInserted);
}

uint8_t Input::as_byte() {
    //corresponds to $FF00
//    Bit 7 - Not used
//    Bit 6 - Not used
//    Bit 5 - P15 Select Action buttons    (0=Select)
//    Bit 4 - P14 Select Direction buttons (0=Select)
//    Bit 3 - P13 Input: Down  or Start    (0=Pressed) (Read Only)
//    Bit 2 - P12 Input: Up    or Select   (0=Pressed) (Read Only)
//    Bit 1 - P11 Input: Left  or B        (0=Pressed) (Read Only)
//    Bit 0 - P10 Input: Right or A        (0=Pressed) (Read Only)

    uint8_t ret = !(Right | A)
            | (!(Left | B)) << 1
            | (!(Up | Select)) << 2
            | (!(Down | Start)) << 3
            | 1 << 4
            | 1 << 5
            | 0 << 6
            | 0 << 7;

    return ret;
}

Cartridge::Cartridge(std::filesystem::path filePath) {
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Error when building " << filePath  << " " << std::endl;
        std::cout << "CWD: " << std::filesystem::current_path() << std::endl;

        throw exceptions::BadRomPathException(filePath.string());
    }

    size_t fileSize = (size_t)file.tellg();

    std::vector<uint8_t> buffer(fileSize / sizeof(uint8_t));

    file.seekg(0);

    file.read((char*)buffer.data(), fileSize);

    file.close();

    rom = buffer;
    rom.shrink_to_fit();

    std::vector<uint8_t> rambuff{1};
    ram = rambuff;
}

Cartridge::Cartridge() : rom{}, ram{} {};

opcodes::Opcode CPU::fetch_opcode(uint8_t instruction) {
    opcodes::Opcode fetched_op = opcodes[instruction];

    return fetched_op;
}

opcodes::Opcode CPU::fetch_opcode_prefixed(uint8_t instruction) {
    opcodes::Opcode fetched_op = opcodes_prefixed[instruction];

    return fetched_op;
}

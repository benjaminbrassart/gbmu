/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:44:58 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/16 00:20:29 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/mmu.hpp"
#include "gbmu/cartridge.hpp"
#include "gbmu/exception.hpp"

#include "format.hpp"

#include <algorithm>

namespace gbmu
{
    mmu::mmu(cartridge &cart) :
        cart(cart),
        io_regs(),
        ppu(),
        lcd(),
        interrupt_master(false),
        interrupt_mask(0b00011111),
        interrupt_flag(0b00011111),
        // wram(new std::uint8_t[WRAM_SIZE]()),
        // hram(new std::uint8_t[HRAM_SIZE]())
        wram(),
        hram()
    {
    }

    mmu::~mmu()
    {
        // delete[] this->wram;
        // delete[] this->hram;
    }

    std::uint8_t const mmu::BOOT_ROM[0x0100] {
        0x31, 0xFE, 0xFF,       // 0x0000 - LD SP, 0xFFFE
        0xAF,                   // 0x0003 - XOR A, A
        0x21, 0xFF, 0x9F,       // 0x0004 - LD HL, 0x9FFF
        0x32,                   // 0x0007 - LD [HL-], A
        0xCB, 0x7C,             // 0x0008 - BIT 7, H
        0x20, 0xFB,             // 0x000A - JR NZ, -5
        0x21, 0x26, 0xFF,       // 0x000C - LD HL, 0xFF26
        0x0E, 0x11,             // 0x000F - LD C, 0x11
        0x3E, 0x80,             // 0x0011 - LD A, 0x80
        0x32,                   // 0x0013 - LD [HL+], A
        0xE2,                   // 0x0014 - LD [C], A
        0x0C,                   // 0x0015 - INC C
        0x3E, 0xF3,             // 0x0016 - LD A, 0xF3
        0xE2,                   // 0x0018 - LD [C], A
        0x32,                   // 0x0019 - LD [HL+], A
        0x3E, 0x77,             // 0x001A - LD A, 0x77
        0x77,                   // 0x001C - LD [HL], A
        0x3E, 0xFC,             // 0x001D - LD A, 0xFC
        0xE0, 0x47,             // 0x001F - LDH [0x47], A
        0x11, 0x04, 0x01,       // 0x0021 - LD DE, 0x104
        0x21, 0x10, 0x80,       // 0x0024 - LD HL, 0x8010
        0x1A,                   // 0x0027 - LD A, [DE]
        0xCD, 0x95, 0x00,       // 0x0028 - CALL 0x95
        0xCD, 0x96, 0x00,       // 0x002B - CALL 0x96
        0x13,                   // 0x002E - INC DE
        0x7B,                   // 0x002F - LD A, E
        0xFE, 0x34,             // 0x0030 - CP A, 0x34
        0x20, 0xF3,             // 0x0032 - JR NZ, -13
        0x11, 0xD8, 0x00,       // 0x0034 - LD DE, 0xD8
        0x06, 0x08,             // 0x0037 - LD B, 0x08
        0x1A,                   // 0x0039 - LD A, [DE]
        0x13,                   // 0x003A - INC DE
        0x22,                   // 0x003B - LD [HL-], A
        0x23,                   // 0x003C - INC HL
        0x05,                   // 0x003D - DEC B
        0x20, 0xF9,             // 0x003E - JR NZ, -7
        0x3E, 0x19,             // 0x0040 - LD A, 0x19
        0xEA, 0x10, 0x99,       // 0x0042 - LD [0x9910], A
        0x21, 0x2F, 0x99,       // 0x0045 - LD HL, 0x992F
        0x0E, 0x0C,             // 0x0048 - LD C, 0x0C
        0x3D,                   // 0x004A - DEC A
        0x28, 0x08,             // 0x004B - JR Z, 8
        0x32,                   // 0x004D - LD [HL+], A
        0x0D,                   // 0x004E - DEC C
        0x20, 0xF9,             // 0x004F - JR NZ, -7
        0x2E, 0x0F,             // 0x0051 - LD L, 0x0F
        0x18, 0xF3,             // 0x0053 - JR -13
        0x67,                   // 0x0055 - LD H, A
        0x3E, 0x64,             // 0x0056 - LD A, 0x64
        0x57,                   // 0x0058 - LD D, A
        0xE0, 0x42,             // 0x0059 - LDH [0x42], A
        0x3E, 0x91,             // 0x005B - LD A, 0x91
        0xE0, 0x40,             // 0x005D - LDH [0x40], A
        0x04,                   // 0x005F - INC B
        0x1E, 0x02,             // 0x0060 - LD E, 0x02
        0x0E, 0x0C,             // 0x0062 - LD C, 0x0C
        0xF0, 0x44,             // 0x0064 - LDH A, [0x44]
        0xFE, 0x90,             // 0x0066 - CP A, 0x90
        0x20, 0xFA,             // 0x0068 - JR NZ, -6
        0x0D,                   // 0x006A - DEC C
        0x20, 0xF7,             // 0x006B - JR NZ, -9
        0x1D,                   // 0x006D - DEC E
        0x20, 0xF2,             // 0x006E - JR NZ, -14
        0x0E, 0x13,             // 0x0070 - LD C, 0x13
        0x24,                   // 0x0072 - INC H
        0x7C,                   // 0x0073 - LD A, H
        0x1E, 0x83,             // 0x0074 - LD E, 0x83
        0xFE, 0x62,             // 0x0076 - CP A, 0x62
        0x28, 0x06,             // 0x0078 - JR Z, 6
        0x1E, 0xC1,             // 0x007A - LD E, 0xC1
        0xFE, 0x64,             // 0x007C - CP A, 0x64
        0x20, 0x06,             // 0x007E - JR NZ, 6
        0x7B,                   // 0x0080 - LD A, E
        0xE2,                   // 0x0081 - LD [C], A
        0x0C,                   // 0x0082 - INC C
        0x3E, 0x87,             // 0x0083 - LD A, 0x87
        0xE2,                   // 0x0085 - LD [C], A
        0xF0, 0x42,             // 0x0086 - LDH A, [0x42]
        0x90,                   // 0x0088 - SUB A, B
        0xE0, 0x42,             // 0x0089 - LDH [0x42], A
        0x15,                   // 0x008B - DEC D
        0x20, 0xD2,             // 0x008C - JR NZ, -46
        0x05,                   // 0x008E - DEC B
        0x20, 0x4F,             // 0x008F - JR NZ, 79
        0x16, 0x20,             // 0x0091 - LD D, 0x20
        0x18, 0xCB,             // 0x0093 - JR -53
        0x4F,                   // 0x0095 - LD C, A
        0x06, 0x04,             // 0x0096 - LD B, 0x04
        0xC5,                   // 0x0098 - PUSH BC
        0xCB, 0x11,             // 0x0099 - RL C
        0x17,                   // 0x009B - RLA
        0xC1,                   // 0x009C - POP BC
        0xCB, 0x11,             // 0x009D - RL C
        0x17,                   // 0x009F - RLA
        0x05,                   // 0x00A0 - DEC B
        0x20, 0xF5,             // 0x00A1 - JR NZ, -11
        0x22,                   // 0x00A3 - LD [HL-], A
        0x23,                   // 0x00A4 - INC HL
        0x22,                   // 0x00A5 - LD [HL-], A
        0x23,                   // 0x00A6 - INC HL
        0xC9,                   // 0x00A7 - RET

        // Nintendo Logo
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,

        // Logo copyright sign
        0x3C, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x3C,

        0x21, 0x04, 0x01,       // 0x00E0 - LD HL, 0x104
        0x11, 0xA8, 0x00,       // 0x00E3 - LD DE, 0xA8
        0x1A,                   // 0x00E6 - LD A, [DE]
        0x13,                   // 0x00E7 - INC DE
        0xBE,                   // 0x00E8 - CP A, [HL]
        0x20, 0xFE,             // 0x00E9 - JR NZ, -2
        0x23,                   // 0x00EB - INC HL
        0x7D,                   // 0x00EC - LD A, L
        0xFE, 0x34,             // 0x00ED - CP A, 0x34
        0x20, 0xF5,             // 0x00EF - JR NZ, -11
        0x06, 0x19,             // 0x00F1 - LD B, 0x19
        0x78,                   // 0x00F3 - LD A, B
        0x86,                   // 0x00F4 - ADD A, [HL]
        0x23,                   // 0x00F5 - INC HL
        0x05,                   // 0x00F6 - DEC B
        0x20, 0xFB,             // 0x00F7 - JR NZ, -5
        0x86,                   // 0x00F9 - ADD A, [HL]
        0x20, 0xFE,             // 0x00FA - JR NZ, -2
        0x3E, 0x01,             // 0x00FC - LD A, 0x01
        0xE0, 0x50,             // 0x00FE - LDH [0x50], A
    };

    std::uint8_t mmu::_read_impl(std::uint16_t address) const
    {
        if (address < sizeof(mmu::BOOT_ROM) && this->read(0xFF50) == 0)
        {
            return mmu::BOOT_ROM[address];
        }

        /**/ if (address >= 0x0000 && address <= 0x7FFF)
        {
            return this->cart.read(address);
        }
        /* A000-BFFF - Cartridge RAM */
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            return this->cart.read(address);
        }
        /* C000-DFFF - Working RAM */
        else if (address >= 0xC000 && address <= 0xDFFF)
        {
            return this->wram[address - 0xC000];
        }
        /* E000-FDFF - Echo RAM */
        else if (address >= 0xE000 && address <= 0xFDFF)
        {
            TODO("read: Echo RAM");
        }
        /* FE00-FE9F - OAM */
        else if (address >= 0xFE00 && address <= 0xFE9F)
        {
            TODO("read: Object Attribute Memory");
        }
        /* FEA0-FEFF - Unused */
        else if (address >= 0xFEA0 && address <= 0xFEFF)
        {
            throw_located(memory_read_exception(address));
        }
        /* FF00-FF7F - I/O registers */
        else if (address >= 0xFF00 && address <= 0xFF7F)
        {
            switch (address)
            {
            case 0xFF40:
            case 0xFF41:
            case 0xFF42:
            case 0xFF43:
            case 0xFF44:
            case 0xFF45:
            case 0xFF4A:
            case 0xFF4B:
                return this->lcd.read(address);
            default:
                return this->io_regs[address];
            }
        }
        /* FF80-FFFE - High RAM */
        else if (address >= 0xFF80 && address <= 0xFFFE)
        {
            return this->hram[address - 0xFF80];
        }
        /* FFFF - Interrupt enable register */
        else if (address == 0xFFFF)
        {
            return this->interrupt_mask;
        }
        else
        {
            throw_located(memory_index_exception(address));
        }
    }

    void mmu::_write_impl(std::uint16_t address, std::uint8_t value)
    {
        /* 0000-3FFF - Cartridge ROM bank 0 */
        /* 4000-7FFF - Cartridge ROM bank N */
        /**/ if (address >= 0x0000 && address <= 0x7FFF)
        {
            this->cart.write(address, value);
        }
        /* 8000-97FF - Tile RAM */
        /* 9800-9FFF - Background map */
        else if (address >= 0x8000 && address <= 0x9FFF)
        {
            this->ppu.write(address, value);
        }
        /* A000-BFFF - Cartridge RAM */
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            this->cart.write(address, value);
        }
        /* C000-DFFF - Working RAM */
        else if (address >= 0xC000 && address <= 0xDFFF)
        {
            this->wram[address - 0xC000] = address;
        }
        /* E000-FDFF - Echo RAM */
        else if (address >= 0xE000 && address <= 0xFDFF)
        {
            TODO("write: Echo RAM");
        }
        /* FE00-FE9F - OAM */
        else if (address >= 0xFE00 && address <= 0xFE9F)
        {
            TODO("write: Object Attribute Memory");
        }
        /* FEA0-FEFF - Unused */
        else if (address >= 0xFEA0 && address <= 0xFEFF)
        {
            throw_located(memory_write_exception(address, value));
        }
        /* FF00-FF7F - I/O registers */
        else if (address >= 0xFF00 && address <= 0xFF7F)
        {
            switch (address)
            {
            case 0xFF40:
            case 0xFF41:
            case 0xFF42:
            case 0xFF43:
            case 0xFF44:
            case 0xFF45:
            case 0xFF4A:
            case 0xFF4B:
                this->lcd.write(address, value);
            default:
                this->io_regs[address] = value;
            }
        }
        /* FF80-FFFE - High RAM */
        else if (address >= 0xFF80 && address <= 0xFFFE)
        {
            this->hram[address - 0xFF80] = value;
        }
        /* FFFF - Interrupt enable register */
        else if (address == 0xFFFF)
        {
            this->interrupt_mask = value & 0b00011111;
        }
        else
        {
            throw_located(memory_index_exception(address));
        }
    }
}

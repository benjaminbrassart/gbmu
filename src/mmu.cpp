/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:44:58 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/13 23:04:40 by bbrassar         ###   ########.fr       */
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
        interrupt_master(false),
        interrupt_mask(0b00011111),
        interrupt_flag(0b00011111),
        wram(new std::uint8_t[WRAM_SIZE]()),
        hram(new std::uint8_t[HRAM_SIZE]())
    {
    }

    mmu::~mmu()
    {
        delete[] this->wram;
        delete[] this->hram;
    }

    std::uint8_t mmu::_read_impl(std::uint16_t address) const
    {
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
        /* FF00-FF7F - I/O registers */
        else if (address >= 0xFF00 && address <= 0xFF7F)
        {
            return this->io_regs.read(address);
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
        else if (address >= 0x8000 && address <= 0x97FF)
        {
            TODO("write: Tile RAM");
        }
        /* 9800-9FFF - Background map */
        else if (address >= 0x9800 && address <= 0x9FFF)
        {
            TODO("write: Background Map");
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
            TODO("write: Unused");
        }
        /* FF00-FF7F - I/O registers */
        else if (address >= 0xFF00 && address <= 0xFF7F)
        {
            this->io_regs.write(address, value);
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

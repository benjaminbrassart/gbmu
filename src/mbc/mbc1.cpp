/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mbc1.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 13:14:55 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 14:18:40 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/mbc.hpp"
#include "gbmu/exception.hpp"

namespace gbmu
{
    mbc1::mbc1() : mbc(),
        ram_enabled(false),
        rom_bank(1),
        ram_bank(1),
        mode(0)
    {
    }

    mbc1::~mbc1() = default;

    std::uint8_t mbc1::read(std::uint16_t address) const
    {
        /* 0000-3FFF - ROM bank 00 */
        /**/ if (address >= 0x0000 && address <= 0x3FFF)
        {
            return this->mbc::rom[address];
        }
        /* 4000-7FFF - ROM bank 01~NN */
        else if (address >= 0x4000 && address <= 0x7FFF)
        {
            return this->mbc::rom[(this->rom_bank * 0x4000) + (address - 0x4000)];
        }
        else if (address >= 0x8000 && address <= 0x9FFF)
        {

        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
        }

        throw memory_index_exception(address);
    }

    void mbc1::write(std::uint16_t address, std::uint8_t value)
    {
        /* 0000-1FFF - RAM Enable */
        /**/ if (address >= 0x0000 && address <= 0x1FFF)
        {
            // if lower nibble is $A, enable ram
            // otherwise disable it
            this->ram_enabled = (value & 0x0F) == 0x0A;
            return;
        }
        /* 2000-3FFF - ROM Bank Number */
        else if (address >= 0x2000 && address <= 0x3FFF)
        {
            int bank;

            bank = value & 0b00011111;

            if (bank == 0)
            {
                bank = 1;
            }

            return; // TODO
        }
        /* 4000-5FFF - RAM Bank Number OR Upper Bits of ROM Bank Number */
        else if (address >= 0x4000 && address <= 0x5FFF)
        {
            return; // TODO
        }
        /* 6000-7FFF - Banking Mode Select */
        else if (address >= 0x6000 && address <= 0x7FFF)
        {
            return; // TODO
        }

        throw memory_write_exception(address, value);
    }
}

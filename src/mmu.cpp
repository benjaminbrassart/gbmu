/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:44:58 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/07 16:34:52 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/mmu.hpp"

#include <algorithm>

namespace gbmu
{
    constexpr static std::uint8_t const PROGRAM[] {
        0x00, // NOP
        0xC3, 0x01, 0x00, // JP 0x0100
    };

    mmu::mmu() :
        _ram(new std::uint8_t[1 << 16])
    {
        std::copy(&PROGRAM[0], &PROGRAM[sizeof(PROGRAM)], &this->_ram[0x0100]);
    }

    mmu::~mmu()
    {
        delete[] this->_ram;
    }

    std::uint8_t &mmu::operator[](std::uint16_t address)
    {
        return this->_ram[address];
    }

    std::uint8_t mmu::operator[](std::uint16_t address) const
    {
        return this->_ram[address];
    }
}

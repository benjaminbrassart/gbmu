/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:44:58 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 16:46:19 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/mmu.hpp"
#include "gbmu/cartridge.hpp"

#include <algorithm>

namespace gbmu
{
    mmu::mmu(cartridge &cartridge) :
        _cartridge(cartridge)
    {
    }

    mmu::~mmu()
    {
    }

    std::uint8_t &mmu::operator[](std::uint16_t address)
    {
        if (address >= 0x0000 && address <= 0xDFFF)
        {
            return this->_cartridge[address];
        }
        else if (address >= 0xE000 && address <= 0xFDFF)
        {
            return this->operator[](address - 0x2000);
        }
        throw;
    }

    std::uint8_t mmu::operator[](std::uint16_t) const
    {
        throw;
    }
}

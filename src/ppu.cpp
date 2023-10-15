/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ppu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/15 19:31:44 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/15 22:52:33 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/ppu.hpp"
#include "gbmu/exception.hpp"

namespace gbmu
{
    ppu::ppu() :
        // vram(new std::uint8_t[0x0200])
        vram()
    {
        (void)this;
    }

    ppu::~ppu()
    {
        // delete[] this->vram;
    }

    std::uint8_t ppu::_read_impl(std::uint16_t address) const
    {
        return this->vram[address - 0x8000];
    }

    void ppu::_write_impl(std::uint16_t address, std::uint8_t value)
    {
        this->vram[address - 0x8000] = value;
    }
}

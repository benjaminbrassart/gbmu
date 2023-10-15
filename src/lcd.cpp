/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lcd.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 00:20:41 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/16 00:39:00 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/lcd.hpp"
#include "gbmu/exception.hpp"

namespace gbmu
{
    std::uint8_t lcd::_read_impl(std::uint16_t address) const
    {
        switch (address)
        {
            case 0xFF40: return lcdc;
            case 0xFF41: return stat;
            case 0xFF42: return scy;
            case 0xFF43: return scx;
            case 0xFF44: return ly;
            case 0xFF45: return lyc;
            case 0xFF4A: return wy;
            case 0xFF4B: return wx;
            default:
                throw_located(memory_index_exception(address));
        }
    }

    void lcd::_write_impl(std::uint16_t address, std::uint8_t value)
    {
        std::uint8_t *output;

        switch (address)
        {
            case 0xFF40: output = &lcdc;
            case 0xFF41: output = &stat;
            case 0xFF42: output = &scy;
            case 0xFF43: output = &scx;
            case 0xFF44: output = &ly;
            case 0xFF45: output = &lyc;
            case 0xFF4A: output = &wy;
            case 0xFF4B: output = &wx;
            default:
                throw_located(memory_index_exception(address));
        }

        *output = value;
    }
}

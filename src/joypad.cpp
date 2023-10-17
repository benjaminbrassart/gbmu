/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joypad.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 12:07:46 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 12:48:42 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/joypad.hpp"
#include "gbmu/exception.hpp"

namespace gbmu
{
    joypad::joypad() :
        bits(0b00110000)
    {
    }

    joypad::~joypad() = default;

    void joypad::press_button(joypad_button btn)
    {
        auto btn_bits = static_cast<std::uint8_t>(btn);

        this->bits &= ~btn_bits;
    }

    std::uint8_t joypad::_read_impl(std::uint16_t address) const
    {
        if (address == 0xFF00)
        {
            return this->bits;
        }
        else
        {
            throw_located(memory_index_exception(address));
        }
    }

    void joypad::_write_impl(std::uint16_t address, std::uint8_t value)
    {
        if (address == 0xFF00)
        {
            this->bits = value & 0x3F;
        }
        else
        {
            throw_located(memory_index_exception(address));
        }

    }
}

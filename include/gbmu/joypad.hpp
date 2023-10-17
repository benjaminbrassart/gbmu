/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joypad.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 12:05:08 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 12:42:49 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "gbmu/device.hpp"
#include "gbmu/interrupt.hpp"

#include <cstdint>

namespace gbmu
{
    /**
     * https://gbdev.io/pandocs/Joypad_Input.html
     */
    enum class joypad_button
    {
        A       = 0b00100001,
        B       = 0b00100010,
        Select  = 0b00100100,
        Start   = 0b00101000,
        Right   = 0b00010001,
        Left    = 0b00010010,
        Up      = 0b00010100,
        Down    = 0b00011000,
    };

    class joypad : public device<joypad>
    {
    private:
        std::uint8_t bits;

    public:
        joypad();
        ~joypad();

    public:
        void press_button(joypad_button btn);

    public:
        std::uint8_t _read_impl(std::uint16_t address) const;
        void _write_impl(std::uint16_t address, std::uint8_t value);
    };
}

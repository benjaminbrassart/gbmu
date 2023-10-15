/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lcd.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 00:08:44 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/16 00:38:36 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "gbmu/device.hpp"
#include "gbmu/interrupt.hpp"

#include <cstdint>

namespace gbmu
{
    class lcd : public device<lcd>, public interrupt_source<lcd, interrupt_flag::VBlank>
    {
    private:
        std::uint8_t lcdc; // 0xFF40
        std::uint8_t stat; // 0xFF41
        std::uint8_t scy; // 0xFF42
        std::uint8_t scx; // 0xFF43
        std::uint8_t ly; // 0xFF44
        std::uint8_t lyc; // 0xFF45
        // std::uint8_t bgp; // 0xFF47 // TODO CGB only
        std::uint8_t wy; // 0xFF4A
        std::uint8_t wx; // 0xFF4B

    public:
        std::uint8_t _read_impl(std::uint16_t address) const;
        void _write_impl(std::uint16_t address, std::uint8_t value);
        void _trigger_impl(interrupts &it);
    };
}

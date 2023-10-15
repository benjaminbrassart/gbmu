/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ppu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/15 18:52:27 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/15 22:52:27 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "gbmu/device.hpp"

#include <cstdint>

namespace gbmu
{
    class ppu : public device<ppu>
    {
    private:
        std::uint8_t vram[0x0200];

    public:
        ppu();
        ~ppu();

    public:
        std::uint8_t _read_impl(std::uint16_t address) const;
        void _write_impl(std::uint16_t address, std::uint8_t value);
    };
}

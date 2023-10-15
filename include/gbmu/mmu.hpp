/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:42:22 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/15 17:49:23 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "gbmu/device.hpp"
#include "gbmu/io_registers.hpp"

#include <cstdint>

namespace gbmu
{
    class cartridge;

    class mmu : public device<mmu>
    {
    public:
        constexpr static std::size_t const WRAM_SIZE = 8192;
        constexpr static std::size_t const HRAM_SIZE = 128;
        constexpr static std::size_t const IOREG_SIZE = 256;

        static std::uint8_t const BOOT_ROM[0x0100];

    private:
        cartridge &cart;
        io_registers io_regs;

    public:
        bool booting;
        bool interrupt_master;
        std::uint8_t interrupt_mask;
        std::uint8_t interrupt_flag;
        std::uint8_t *wram;
        std::uint8_t *hram;

    public:
        mmu(cartridge &cart);
        ~mmu();

    public:
        std::uint8_t _read_impl(std::uint16_t address) const;
        void _write_impl(std::uint16_t address, std::uint8_t value);
    };
}

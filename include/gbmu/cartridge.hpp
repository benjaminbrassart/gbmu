/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cartridge.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 19:27:46 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 14:23:09 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <memory>

#include <cstdint>

namespace gbmu
{
    enum class cartridge_type : std::uint8_t
    {
        ROM_ONLY                    = 0x00,
        MBC1                        = 0x01,
        MBC1_RAM                    = 0x02,
        MBC1_BATTERY                = 0x03,
        MBC2                        = 0x05,
        MBC2_BATTERY                = 0x06,
        ROM_RAM                     = 0x08,
        ROM_RAM_BATTERY             = 0x09,
        MMM01                       = 0x0B,
        MMM01_RAM                   = 0x0C,
        MMM01_RAM_BATTERY           = 0x0D,
        MBC3_TIMER_BATTERY          = 0x0F,
        MBC3_TIMER_RAM_BATTERY      = 0x10,
        MBC3                        = 0x11,
        MBC3_RAM                    = 0x12,
        MBC3_RAM_BATTERY            = 0x13,
        MBC5                        = 0x19,
        MBC5_RAM                    = 0x1A,
        MBC5_RAM_BATTERY            = 0x1B,
        MBC5_RUMBLE                 = 0x1C,
        MBC5_RUMBLE_RAM             = 0x1D,
        MBC5_RUMBLE_RAM_BATTERY     = 0x1E,
        POCKET_CAMERA               = 0x1F,
        BANDAI_TAMA5                = 0xFD,
        HUDSON_HUC_3                = 0xFE,
        HUDSON_HUC_1                = 0xFF,
    };

    struct cartridge_header
    {
        std::uint8_t entrypoint[4];
        std::uint8_t nintendo_logo[48];
        std::uint8_t title[11];
        std::uint8_t manufacturer_code[4];
        std::uint8_t cgb_flag;
        std::uint16_t new_licensee_code;
        std::uint8_t sgb_flag;
        cartridge_type cartridge_type;
        std::uint8_t rom_size;
        std::uint8_t ram_size;
        std::uint8_t destination_code;
        std::uint8_t old_licensee_code;
        std::uint8_t version_number;
        std::uint8_t header_checksum;
        std::uint16_t global_checksum;

        bool check_logo() const;
        std::uint8_t compute_header_checksum() const;
    };

    constexpr static std::size_t const INT_VECTORS_SIZE = 0x0100;
    constexpr static std::size_t const CARTRIDGE_HEADER_SIZE = 80;

    static_assert(sizeof(cartridge_header) == CARTRIDGE_HEADER_SIZE);

    class cartridge
    {
    public:
        static std::uint8_t const NINTENDO_LOGO[48];

    private:
        std::uint8_t *_rom;
        std::uint8_t *_ram;

    public:
        cartridge();
        cartridge(cartridge const &x) = delete;
        ~cartridge();

    public:
    bool open(std::istream &f);

    public:
        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };
}

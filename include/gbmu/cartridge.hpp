/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cartridge.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 19:27:46 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/08 00:27:41 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <memory>

#include <cstdint>

namespace gbmu
{
    class mbc;

    class cartridge
    {
    public:
        enum class type : std::uint8_t
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

    public:
        static std::uint8_t const NINTENDO_LOGO[48];

    private:
        std::uint8_t *_rom;
        std::uint8_t *_ram;
        mbc *_mbc;

    public:
        cartridge(std::istream &f);
        ~cartridge();

    public:
        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };

    class mbc
    {
    public:
        virtual ~mbc() = default;

        virtual std::uint8_t &operator[](std::uint16_t address) = 0;
        virtual std::uint8_t operator[](std::uint16_t address) const = 0;

    public:
        static mbc *get_mbc(cartridge::type t);
    };

    class rom : public mbc
    {
    public:
        rom(bool ram, bool battery);

        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };

    class mbc1 : public mbc
    {
    public:
        mbc1(bool ram, bool battery);

        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };

    class mbc2 : public mbc
    {
    public:
        mbc2(bool battery);

        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };

    class mbc3 : public mbc
    {
    public:
        mbc3(bool ram, bool battery, bool timer);

        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };

    class mbc5 : public mbc
    {
    public:
        mbc5(bool ram, bool battery, bool rumble);

        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };

    class mmm01 : public mbc
    {
    public:
        mmm01(bool ram, bool battery);

        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };
}
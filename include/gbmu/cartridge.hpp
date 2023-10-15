/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cartridge.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 19:27:46 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/15 14:49:52 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "gbmu/device.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>

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
        cartridge_type type;
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

    constexpr static std::uint8_t const NINTENDO_LOGO[48] {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
    };
    constexpr static std::size_t const INT_VECTORS_SIZE = 0x0100;
    constexpr static std::size_t const CARTRIDGE_HEADER_SIZE = 80;

    static_assert(sizeof(cartridge_header) == CARTRIDGE_HEADER_SIZE);

    /**
     * Basic cartridge, acts as both a concrete type for ROM-only cartridge
     * and base class for cartridges with memory controller (MBC)
     */
    class cartridge : public device<cartridge>
    {
    protected:
        std::uint8_t *rom;
        std::uint8_t *ram;

    public:
        cartridge(std::uint8_t *rom, std::uint8_t *ram);
        virtual ~cartridge();

        cartridge(cartridge const&) = delete;
        cartridge &operator=(cartridge const&) = delete;

    public:
        virtual std::uint8_t _read_impl(std::uint16_t address) const;
        virtual void _write_impl(std::uint16_t address, std::uint8_t value);

    public:
        static cartridge *create(std::istream &stream);
    };

    /**
     * Base abstract class for cartridges with memory controller (MBC)
     */
    class abstract_mbc : public cartridge
    {
    protected:
        bool ram_enabled;
        int rom_bank;
        int ram_bank;
        int mode;

    public:
        abstract_mbc(std::uint8_t *rom, std::uint8_t *ram);
        virtual ~abstract_mbc() = 0;

        virtual std::uint8_t _read_impl(std::uint16_t address) const override = 0;
        virtual void _write_impl(std::uint16_t address, std::uint8_t value) override = 0;
    };

    class mbc1 : public abstract_mbc
    {
    public:
        mbc1(std::uint8_t *rom, std::uint8_t *ram);
        ~mbc1();

        std::uint8_t _read_impl(std::uint16_t address) const override;
        void _write_impl(std::uint16_t address, std::uint8_t value) override;
    };

    class mbc2 : public abstract_mbc
    {
    public:
        mbc2(std::uint8_t *rom, std::uint8_t *ram);
        ~mbc2();

        std::uint8_t _read_impl(std::uint16_t address) const override;
        void _write_impl(std::uint16_t address, std::uint8_t value) override;
    };

    class mbc3 : public abstract_mbc
    {
    public:
        mbc3(std::uint8_t *rom, std::uint8_t *ram);
        ~mbc3();

        std::uint8_t _read_impl(std::uint16_t address) const override;
        void _write_impl(std::uint16_t address, std::uint8_t value) override;
    };

    class mbc5 : public abstract_mbc
    {
    public:
        mbc5(std::uint8_t *rom, std::uint8_t *ram);
        ~mbc5();

        std::uint8_t _read_impl(std::uint16_t address) const override;
        void _write_impl(std::uint16_t address, std::uint8_t value) override;
    };
}

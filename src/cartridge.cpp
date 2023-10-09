/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cartridge.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 20:42:01 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 04:06:34 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/cartridge.hpp"

#include "dbg.hpp"

#include <algorithm>

namespace gbmu
{
    std::uint8_t const cartridge::NINTENDO_LOGO[48] {
        0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
        0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
        0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
    };

    bool cartridge_header::check_logo() const
    {
        return std::equal(std::begin(cartridge::NINTENDO_LOGO), std::end(cartridge::NINTENDO_LOGO), std::begin(this->nintendo_logo));
    }

    std::uint8_t cartridge_header::compute_header_checksum() const
    {
        auto raw = reinterpret_cast<std::uint8_t const *>(this);
        std::uint8_t checksum = 0;

        for (std::uint16_t address = 0x0134; address <= 0x14C; address += 1)
        {
            checksum = checksum - raw[address - 0x0134] - 1;
        }

        return checksum;
    }

    cartridge::cartridge() : _mbc(nullptr)
    {
    }

    cartridge::~cartridge()
    {
        delete this->_mbc;
    }

    bool cartridge::open(std::istream &f)
    {
        char raw_header[gbmu::INT_VECTORS_SIZE + gbmu::CARTRIDGE_HEADER_SIZE];
        auto header = reinterpret_cast<cartridge_header *>(&raw_header[gbmu::INT_VECTORS_SIZE]);

        if (!dbg(f.read(raw_header, sizeof(raw_header))) || !dbg(header->check_logo()))
        {
            return false;
        }

        {
            auto chk = header->compute_header_checksum();

            std::cout << static_cast<int>(chk) << std::endl;

            if (!dbg(chk == header->header_checksum))
            {
                return false;
            }
        }

        /*
         * Get actual ROM size from id
         *
         * Some unofficial documentations mention other values:
         *   - 0x52 = 1.1 MiB, 72 banks
         *   - 0x53 = 1.2 MiB, 80 banks
         *   - 0x54 = 1.5 MiB, 96 banks
         *
         * It is unlikely that these will ever show up,
         * but support can easily be provided if needed.
         */

        std::size_t rom_size;

        switch (header->rom_size)
        {
        case 0x00: // 32 KiB
        case 0x01: // 64 KiB
        case 0x02: // 128 KiB
        case 0x03: // 256 KiB
        case 0x04: // 512 KiB
        case 0x05: // 1 MiB
        case 0x06: // 2 MiB
        case 0x07: // 4 MiB
        case 0x08: // 8 MiB
        {
            rom_size = 32 * 1024 * (1 << header->rom_size);
        } break;

        default:
        {
            return false;
        } break;
        }

        /*
         * Get actual RAM size from id
         *
         * Some unofficial documentations mention value 0x01 as 2 KiB RAM chip.
         * Although a 2 KiB RAM chip was never used in an actual cartridge,
         * various homebrew roms make use of RAM size id 0x01.
         */
        std::size_t ram_size;

        switch (header->ram_size)
        {
        case 0x00: {
            ram_size = 0;
        } break;

        case 0x01: {
            ram_size = 2 * 1024;
        } break;

        case 0x02: {
            ram_size = 8 * 1024;
        } break;

        case 0x03: {
            ram_size = 32 * 1024;
        } break;

        case 0x04: {
            ram_size = 128 * 1024;
        } break;

        case 0x05: {
            ram_size = 64 * 1024;
        } break;

        default: {
            return false;
        } break;
        }

        [[maybe_unused]] auto rom = new std::uint8_t[rom_size];
        [[maybe_unused]] auto ram = new std::uint8_t[ram_size];

        std::copy(std::begin(raw_header), std::end(raw_header), rom);

        this->_mbc = mbc::get_mbc(header->cartridge_type, rom_size, ram_size);

        if (dbg(this->_mbc == nullptr))
        {
            return false;
        }

        return true;
    }

    mbc *mbc::get_mbc(cartridge_type t, std::size_t rom_size, std::size_t ram_size)
    {
        (void)rom_size;
        (void)ram_size;

        switch (t)
        {
        // case cartridge::type::ROM_ONLY: return new rom(false, false);
        // case cartridge::type::MBC1: new mbc1(false, false);
        // case cartridge::type::MBC1_RAM: return new mbc1(true, false);
        // case cartridge::type::MBC1_BATTERY: return new mbc1(false, true);
        // case cartridge::type::MBC2: return new mbc2(false);
        // case cartridge::type::MBC2_BATTERY: new mbc2(true);
        // case cartridge::type::ROM_RAM: return new rom(true, false);
        // case cartridge::type::ROM_RAM_BATTERY: return new rom(true, true);
        // case cartridge::type::MMM01: return new mmm01(false, false);
        // case cartridge::type::MMM01_RAM: return new mmm01(true, false);
        // case cartridge::type::MMM01_RAM_BATTERY: return new mmm01(true, true);
        // case cartridge::type::MBC3_TIMER_BATTERY: return new mbc3(false, true, true);
        // case cartridge::type::MBC3_TIMER_RAM_BATTERY: return new mbc3(true, true, true);
        // case cartridge::type::MBC3: return new mbc3(false, false, false);
        // case cartridge::type::MBC3_RAM: return new mbc3(true, false, false);
        // case cartridge::type::MBC3_RAM_BATTERY: return new mbc3(true, true, false);
        // case cartridge::type::MBC5: return new mbc5(false, false, false);
        // case cartridge::type::MBC5_RAM: return new mbc5(true, false, false);
        // case cartridge::type::MBC5_RAM_BATTERY: return new mbc5(true, true, false);
        // case cartridge::type::MBC5_RUMBLE: return new mbc5(false, false, true);
        // case cartridge::type::MBC5_RUMBLE_RAM: return new mbc5(true, false, true);
        // case cartridge::type::MBC5_RUMBLE_RAM_BATTERY: return new mbc5(true, true, true);
        // case cartridge::type::POCKET_CAMERA: return nullptr;
        // case cartridge::type::BANDAI_TAMA5: return nullptr;
        // case cartridge::type::HUDSON_HUC_3: return nullptr;
        // case cartridge::type::HUDSON_HUC_1: return nullptr;
        default: return nullptr;
        }
    }
}

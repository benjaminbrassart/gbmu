/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cart.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 14:37:00 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 15:11:32 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/cartridge.hpp"

#include "dbg.hpp"

namespace gbmu2
{
    std::unordered_map<std::uint8_t, std::size_t> const ROM_SIZES {
        {0x00, 32 * 1024},
        {0x01, 64 * 1024},
        {0x02, 128 * 1024},
        {0x03, 256 * 1024},
        {0x04, 512 * 1024},
        {0x05, 1 * 1024 * 1024},
        {0x06, 2 * 1024 * 1024},
        {0x07, 3 * 1024 * 1024},
        {0x08, 4 * 1024 * 1024},

        /* uncomment this to enable non-official rom bank sizes */

        // {0x52, (128 + 1024) * 1024},
        // {0x53, (256 + 1024) * 1024},
        // {0x54, (512 + 1024) * 1024},
    };

    std::unordered_map<std::uint8_t, std::size_t> const RAM_SIZES {
        {0x00, 0},
        {0x01, 2 * 1024},
        {0x02, 8 * 1024},
        {0x03, 32 * 1024},
        {0x05, 64 * 1024},
        {0x04, 128 * 1024},
    };

    cartridge *cartridge::create(std::istream &f)
    {
        char raw_header[INT_VECTORS_SIZE + CARTRIDGE_HEADER_SIZE];
        auto header = reinterpret_cast<cartridge_header *>(&raw_header[gbmu::INT_VECTORS_SIZE]);

        if (!f.read(raw_header, sizeof(raw_header)) || !header->check_logo())
        {
            return nullptr; // TODO throw exception
        }

        // TODO verify checksum

        std::size_t rom_size;

        {
            auto it = ROM_SIZES.find(header->rom_size);

            if (it == ROM_SIZES.end())
            {
                return nullptr;
            }

            rom_size = it->second;
        }

        std::size_t ram_size;

        {
            auto it = RAM_SIZES.find(header->ram_size);

            if (it == RAM_SIZES.end())
            {
                return nullptr;
            }

            ram_size = it->second;
        }

        auto rom = new std::uint8_t[rom_size];
        auto ram = new std::uint8_t[ram_size];

        std::copy(std::begin(raw_header), std::end(raw_header), rom);

        switch (header->cartridge_type)
        {
            case cartridge_type::ROM_ONLY:
            case cartridge_type::ROM_RAM:
            case cartridge_type::ROM_RAM_BATTERY: {
                return new cartridge(rom, ram);
            } break;

            case cartridge_type::MBC1:
            case cartridge_type::MBC1_BATTERY:
            case cartridge_type::MBC1_RAM: {
                return new mbc1(rom, ram);
            } break;

            case cartridge_type::MBC2:
            case cartridge_type::MBC2_BATTERY: {
                return new mbc2(rom, ram);
            } break;

            case cartridge_type::MBC3:
            case cartridge_type::MBC3_RAM:
            case cartridge_type::MBC3_RAM_BATTERY:
            case cartridge_type::MBC3_TIMER_BATTERY:
            case cartridge_type::MBC3_TIMER_RAM_BATTERY: {
                return new mbc3(rom, ram);
            } break;

            case cartridge_type::MBC5:
            case cartridge_type::MBC5_RAM:
            case cartridge_type::MBC5_RAM_BATTERY:
            case cartridge_type::MBC5_RUMBLE:
            case cartridge_type::MBC5_RUMBLE_RAM:
            case cartridge_type::MBC5_RUMBLE_RAM_BATTERY: {
                return new mbc5(rom, ram);
            } break;

            default: {
            } break;
        }

        /* invalid or unsupported cartridge type */

        delete[] rom;
        delete[] ram;

        return nullptr;
    }
}

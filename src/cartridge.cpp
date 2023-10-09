/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cartridge.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 20:42:01 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 16:46:33 by bbrassar         ###   ########.fr       */
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
        std::uint16_t checksum = 0;

        for (std::uint16_t address = 0x0134; address <= 0x014C; address += 1)
        {
            checksum = checksum - raw[address - 0x0134] - 1;
        }

        return checksum & 0xFF;
    }

    cartridge::cartridge() : _rom(nullptr), _ram(nullptr)
    {
    }

    cartridge::~cartridge()
    {
        delete[] this->_rom;
        delete[] this->_ram;
    }

    bool cartridge::open(std::istream &f)
    {
        char raw_header[gbmu::INT_VECTORS_SIZE + gbmu::CARTRIDGE_HEADER_SIZE];
        auto header = reinterpret_cast<cartridge_header *>(&raw_header[gbmu::INT_VECTORS_SIZE]);

        if (!dbg(f.read(raw_header, sizeof(raw_header))) || !dbg(header->check_logo()))
        {
            return false;
        }

#if false
        {
            auto chk = header->compute_header_checksum();

            std::cout
                << "checksum: " << std::hex << static_cast<int>(header->header_checksum) << std::endl
                << "computed: " << std::hex << static_cast<int>(chk) << std::endl;

            if (!dbg(chk == header->header_checksum))
            {
                return false;
            }
        }
#endif

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

        this->_rom = new std::uint8_t[rom_size] {};
        this->_ram = new std::uint8_t[ram_size] {};

        std::copy(std::begin(raw_header), std::end(raw_header), this->_rom);

        return true;
    }

    std::uint8_t &cartridge::operator[](std::uint16_t)
    {
        return this->_rom[0x00]; // TODO implement mbc logic
    }

    std::uint8_t cartridge::operator[](std::uint16_t) const
    {
        return this->_rom[0x00]; // TODO implement mbc logic
    }

}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cartridge.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 20:42:01 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/08 09:47:07 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/cartridge.hpp"

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

    cartridge::cartridge(std::istream &f) : _rom(nullptr), _ram(nullptr), _mbc(nullptr)
    {
        f.seekg(0, std::ios::end);

        auto stream_size = f.tellg();

        f.seekg(0, std::ios::beg);

        this->_rom = new std::uint8_t[stream_size];

        f.read(reinterpret_cast<char *>(this->_rom), stream_size);

        if (!std::equal(std::begin(NINTENDO_LOGO), std::end(NINTENDO_LOGO), &this->_rom[0x0104], &this->_rom[0x0134]))
        {
            throw; // TODO
        }

        this->_mbc = mbc::get_mbc(static_cast<type>(this->_rom[0x0147]));

        if (this->_mbc == nullptr)
        {
            throw; // TODO
        }

        std::size_t ram_size;

        switch (this->_rom[0x0149])
        {
        case 0x00: {
            ram_size = 0;
        } break;

        case 0x01: {
            ram_size = 1 << 11;
        } break;

        case 0x02: {
            ram_size = 1 << 13;
        } break;

        case 0x03: {
            ram_size = 1 << 15;
        } break;

        case 0x04: {
            ram_size = 1 << 17;
        } break;

        case 0x05: {
            ram_size = 1 << 16;
        } break;

        default: {
            throw; // TODO
        } break;
        }

        this->_ram = new std::uint8_t[ram_size];
    }

    cartridge::~cartridge()
    {
        delete[] this->_rom;
        delete[] this->_ram;
        delete this->_mbc;
    }

    mbc *mbc::get_mbc(cartridge::type t)
    {
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

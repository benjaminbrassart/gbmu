/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cartridge.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 14:37:00 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/13 16:22:46 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/cartridge.hpp"
#include "gbmu/exception.hpp"

#include "dbg.hpp"
#include "features.hpp"

#include <algorithm>

namespace gbmu
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
#if GBMU_ENABLE_NON_OFFICIAL_ROM_SIZE
        {0x52, (128 + 1024) * 1024},
        {0x53, (256 + 1024) * 1024},
        {0x54, (512 + 1024) * 1024},
#endif
    };

    std::unordered_map<std::uint8_t, std::size_t> const RAM_SIZES {
        {0x00, 0},
#if GBMU_ENABLE_NON_OFFICIAL_RAM_SIZE
        {0x01, 2 * 1024},
#endif
        {0x02, 8 * 1024},
        {0x03, 32 * 1024},
        {0x05, 64 * 1024},
        {0x04, 128 * 1024},
    };

    bool cartridge_header::check_logo() const
    {
        return std::equal(std::begin(NINTENDO_LOGO), std::end(NINTENDO_LOGO), this->nintendo_logo);
    }

    cartridge *cartridge::create(std::istream &f)
    {
        char raw_header[INT_VECTORS_SIZE + CARTRIDGE_HEADER_SIZE];
        auto header = reinterpret_cast<cartridge_header *>(&raw_header[gbmu::INT_VECTORS_SIZE]);

        if (!f.read(raw_header, sizeof(raw_header)) || !header->check_logo())
        {
            return nullptr; // TODO throw exception
        }

#if !GBMU_SKIP_HEADER_CHECKSUM
        // TODO verify header checksum
#endif

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

        cartridge *cart;

        switch (header->cartridge_type)
        {
            case cartridge_type::ROM_ONLY:
            case cartridge_type::ROM_RAM:
            case cartridge_type::ROM_RAM_BATTERY: {
                cart = new cartridge(rom, ram);
            } break;

            case cartridge_type::MBC1:
            case cartridge_type::MBC1_BATTERY:
            case cartridge_type::MBC1_RAM: {
                cart = new mbc1(rom, ram);
            } break;

            case cartridge_type::MBC2:
            case cartridge_type::MBC2_BATTERY: {
                cart = new mbc2(rom, ram);
            } break;

            case cartridge_type::MBC3:
            case cartridge_type::MBC3_RAM:
            case cartridge_type::MBC3_RAM_BATTERY:
            case cartridge_type::MBC3_TIMER_BATTERY:
            case cartridge_type::MBC3_TIMER_RAM_BATTERY: {
                cart = new mbc3(rom, ram);
            } break;

            case cartridge_type::MBC5:
            case cartridge_type::MBC5_RAM:
            case cartridge_type::MBC5_RAM_BATTERY:
            case cartridge_type::MBC5_RUMBLE:
            case cartridge_type::MBC5_RUMBLE_RAM:
            case cartridge_type::MBC5_RUMBLE_RAM_BATTERY: {
                cart = new mbc5(rom, ram);
            } break;

            default: {
                /* invalid or unsupported cartridge type */
                cart = nullptr;
            } break;
        }

        if (cart != nullptr)
        {
            auto rom_body = reinterpret_cast<char*>(&rom[INT_VECTORS_SIZE + CARTRIDGE_HEADER_SIZE]);
            auto body_size = rom_size - (INT_VECTORS_SIZE + CARTRIDGE_HEADER_SIZE);

            std::copy(std::begin(raw_header), std::end(raw_header), rom);
            f.read(rom_body, body_size);

            if (!f.fail())
            {
#if !GBMU_SKIP_GLOBAL_CHECKSUM
                // TODO verify global checksum
#endif
                return cart;
            }
        }

        delete[] rom;
        delete[] ram;

        return nullptr;
    }

    cartridge::cartridge(std::uint8_t *rom, std::uint8_t *ram) :
        rom(rom),
        ram(ram)
    {
    }

    cartridge::~cartridge() = default;

    std::uint8_t cartridge::_read_impl(std::uint16_t address) const
    {
        /**/ if (address >= 0x0000 && address <= 0x7FFF)
        {
            return this->rom[address];
        }
        else if (address >= 0xA000 && address <= 0xBFFF && this->ram != nullptr)
        {
            return this->ram[address - 0xA000];
        }

        throw_located(memory_index_exception(address));
    }

    void cartridge::_write_impl(std::uint16_t address, std::uint8_t value)
    {
        /**/ if (address >= 0x0000 && address <= 0x7FFF)
        {
            throw_located(memory_write_exception(address, value));
        }
        else if (address >= 0xA000 && address <= 0xBFFF && this->ram != nullptr)
        {
            this->ram[address - 0xA000] = value;
            return;
        }

        throw_located(memory_index_exception(address));
    }

    abstract_mbc::abstract_mbc(std::uint8_t *rom, std::uint8_t *ram) : cartridge(rom, ram),
        ram_enabled(false),
        rom_bank(1),
        ram_bank(1),
        mode(0)
    {
    }

    abstract_mbc::~abstract_mbc() = default;

    mbc1::mbc1(std::uint8_t *rom, std::uint8_t *ram) : abstract_mbc(rom, ram)
    {
    }

    mbc1::~mbc1() = default;

    mbc2::mbc2(std::uint8_t *rom, std::uint8_t *ram) : abstract_mbc(rom, ram)
    {
    }

    mbc2::~mbc2() = default;

    mbc3::mbc3(std::uint8_t *rom, std::uint8_t *ram) : abstract_mbc(rom, ram)
    {
    }

    mbc3::~mbc3() = default;

    mbc5::mbc5(std::uint8_t *rom, std::uint8_t *ram) : abstract_mbc(rom, ram)
    {
    }

    mbc5::~mbc5() = default;

    std::uint8_t mbc1::_read_impl(std::uint16_t address) const
    {
        /* 0000-3FFF - ROM bank 00 */
        /**/ if (address >= 0x0000 && address <= 0x3FFF)
        {
            return this->cartridge::rom[address];
        }
        /* 4000-7FFF - ROM bank 01~NN */
        else if (address >= 0x4000 && address <= 0x7FFF)
        {
            return this->cartridge::rom[(this->rom_bank * 0x4000) + (address - 0x4000)];
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            if (this->ram_enabled)
            {
                // TODO return from RAM
            }

            return 0xFF;
        }

        throw_located(memory_index_exception(address));
    }

    void mbc1::_write_impl(std::uint16_t address, std::uint8_t value)
    {
        /* 0000-1FFF - RAM enable */
        /**/ if (address >= 0x0000 && address <= 0x1FFF)
        {
            // if lower nibble is $A, enable ram
            // otherwise disable it
            this->ram_enabled = (value & 0x0F) == 0x0A;
            return;
        }
        /* 2000-3FFF - ROM bank number */
        else if (address >= 0x2000 && address <= 0x3FFF)
        {
            int bank = (value & 0b00011111);

            if (bank == 0)
            {
                bank = 1;
            }

            this->rom_bank = bank;
            return;
        }
        /* 4000-5FFF - RAM bank number, or upper bits of ROM bank number */
        else if (address >= 0x4000 && address <= 0x5FFF)
        {
            // TODO
            return;
        }
        /* 6000-7FFF - Banking mode select */
        else if (address >= 0x6000 && address <= 0x7FFF)
        {
            this->mode = (value & 0b00000001);
            return;
        }

        throw_located(memory_index_exception(address));
    }

    std::uint8_t mbc2::_read_impl(std::uint16_t) const
    {
        return 0x00;
    }

    void mbc2::_write_impl(std::uint16_t, std::uint8_t)
    {
    }

    std::uint8_t mbc3::_read_impl(std::uint16_t) const
    {
        return 0x00;
    }

    void mbc3::_write_impl(std::uint16_t, std::uint8_t)
    {
    }

    std::uint8_t mbc5::_read_impl(std::uint16_t) const
    {
        return 0x00;
    }

    void mbc5::_write_impl(std::uint16_t, std::uint8_t)
    {
    }
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rom_only.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 11:19:56 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 11:57:57 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/mbc.hpp"
#include "gbmu/exception.hpp"

namespace gbmu
{
    rom_only::rom_only() = default;
    rom_only::~rom_only() = default;

    std::uint8_t rom_only::read(std::uint16_t address) const
    {
        if (address >= 0x0000 && address <= 0x7FFF)
        {
            // TODO read from rom
            return 0x00;
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            // TODO read from ram if any
            return 0x00;
        }

        throw memory_index_exception(address);
    }

    void rom_only::write(std::uint16_t address, std::uint8_t value)
    {
        if (address >= 0x0000 && address <= 0x7FFF)
        {
            throw memory_write_exception(address, value);
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            // TODO write to ram if any
            return;
        }

        throw memory_index_exception(address);
    }
}

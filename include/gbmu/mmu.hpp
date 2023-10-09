/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:42:22 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 02:27:18 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>

namespace gbmu
{
    class cartridge;

    class mmu
    {
    private:
        std::uint8_t *_ram;

    public:
        mmu();
        ~mmu();

    public:
        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;

        std::uint8_t read(std::uint16_t address) const;
        void write(std::uint16_t address, std::uint8_t value);
    };
}

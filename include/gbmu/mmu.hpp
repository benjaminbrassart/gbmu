/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:42:22 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 14:34:08 by bbrassar         ###   ########.fr       */
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
        cartridge &_cartridge;

    public:
        mmu(cartridge &cartridge);
        ~mmu();

    public:
        std::uint8_t &operator[](std::uint16_t address);
        std::uint8_t operator[](std::uint16_t address) const;
    };
}

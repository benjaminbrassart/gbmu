/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mbc.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 11:21:01 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 11:26:51 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/mbc.hpp"
#include "gbmu/cartridge.hpp"

namespace gbmu
{
    mbc::mbc() = default;

    mbc *mbc::get_mbc(cartridge_type type)
    {
        switch (type)
        {
            case cartridge_type::ROM_ONLY: return new rom_only();
            case cartridge_type::ROM_RAM: return new rom_only();
            case cartridge_type::ROM_RAM_BATTERY: return new rom_only();
            default: return nullptr;
        }
    }
}

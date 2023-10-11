/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mmu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:44:58 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 15:40:49 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/mmu.hpp"
#include "gbmu/cartridge.hpp"

#include <algorithm>

namespace gbmu
{
    mmu::mmu(cartridge &cartridge) :
        _cartridge(cartridge)
    {
    }

    mmu::~mmu()
    {
    }
}

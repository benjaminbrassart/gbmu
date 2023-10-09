/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 12:01:31 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 12:10:00 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "format.hpp"

#include <iomanip>
#include <iostream>

#include <cstdint>

hex::hex(std::uint8_t addr) : _addr(addr), _w(2) {}

hex::hex(std::uint16_t addr) : _addr(addr), _w(4) {}

hex::hex(std::uint32_t addr) : _addr(addr), _w(8) {}

hex::hex(std::uint64_t addr) : _addr(addr), _w(16) {}

hex::~hex() = default;

std::ostream &operator<<(std::ostream &os, hex const &a)
{
    return os << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(a._w) << a._addr;
}

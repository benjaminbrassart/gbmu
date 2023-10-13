/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_registers.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 19:44:10 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/13 22:44:27 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/io_registers.hpp"
#include "gbmu/exception.hpp"

namespace gbmu
{
    io_registers::io_registers()
    {
        std::fill(std::begin(this->_raw), std::end(this->_raw), 0x00);
    }

    io_registers::~io_registers() = default;

    std::uint8_t io_registers::_read_impl(std::uint16_t address) const
    {
        return this->_raw[address - 0xFF00];
    }

    void io_registers::_write_impl(std::uint16_t address, std::uint8_t value)
    {
        this->_raw[address - 0xFF00] = value;
    }
}

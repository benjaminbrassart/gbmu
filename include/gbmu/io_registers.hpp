/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_registers.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 14:38:53 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/13 22:41:45 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "gbmu/device.hpp"

#include <cstdint>

namespace gbmu
{
    class io_registers : public device<io_registers>
    {
    private:
        std::uint8_t _raw[120];

    public:
        io_registers();
        ~io_registers();

    public:
        std::uint8_t _read_impl(std::uint16_t address) const;
        void _write_impl(std::uint16_t address, std::uint8_t value);
    };
}

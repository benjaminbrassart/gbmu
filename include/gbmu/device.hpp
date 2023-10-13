/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   device.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/13 10:47:13 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/13 11:28:02 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <concepts>

#include <cstdint>

namespace gbmu
{
    template<class D>
    class device
    {
    public:
        std::uint8_t read(std::uint16_t address) const
        {
            return static_cast<D const *>(this)->_read_impl(address);
        }

        void write(std::uint16_t address, std::uint8_t value)
        {
            static_cast<D*>(this)->_write_impl(address, value);
        }

    public:
        std::uint8_t _read_impl(std::uint16_t address) const;
        void _write_impl(std::uint16_t address, std::uint8_t value);
    };
}

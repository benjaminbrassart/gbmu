/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 12:01:05 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 12:03:24 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include <cstdint>

struct hex
{
private:
    std::uint64_t _addr;
    std::size_t _w;

public:
    hex(std::uint8_t addr);
    hex(std::uint16_t addr);
    hex(std::uint32_t addr);
    hex(std::uint64_t addr);
    ~hex();

public:
    friend std::ostream &operator<<(std::ostream &os, hex const &a);
};

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   call_stack.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 11:18:50 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/16 11:27:56 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>
#include <iostream>
#include <stack>

#include "format.hpp"

namespace gbmu
{
    enum class element_type
    {
        Call,
        Push,
    };

    inline std::ostream &operator<<(std::ostream &stream, element_type const &type)
    {
        switch (type)
        {
        case element_type::Call:
            return stream << "CALL";
        case element_type::Push:
            return stream << "PUSH";
        default:
            return stream << "INVALID STACK ELEMENT";
        }
    }

    struct stack_element
    {
        std::uint16_t value;
        element_type type;
    };

    inline std::ostream &operator<<(std::ostream &stream, stack_element const &elem)
    {
        return stream << elem.type << " " << hex(elem.value);
    }
}

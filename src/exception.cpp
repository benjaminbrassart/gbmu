/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 11:59:20 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 12:21:10 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/exception.hpp"

#include "format.hpp"

#include <sstream>

namespace gbmu
{
    static std::string _illegal_instruction_message(std::uint8_t code)
    {
        std::stringstream ss;

        ss << "illegal instruction: " << hex(code);

        return ss.str();
    }

    illegal_instruction_exception::illegal_instruction_exception(std::uint8_t code) :
        _message(_illegal_instruction_message(code)),
        code(code)
    {
    }

    illegal_instruction_exception::~illegal_instruction_exception() = default;

    char const *illegal_instruction_exception::what() const throw()
    {
        return this->_message.c_str();
    }
}

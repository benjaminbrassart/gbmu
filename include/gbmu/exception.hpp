/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 11:57:31 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 12:21:02 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdexcept>
#include <string>

#include <cstdint>

namespace gbmu
{
    class illegal_instruction_exception : public std::exception
    {
    private:
        std::string _message;

    public:
        std::uint8_t code;

    public:
        illegal_instruction_exception(std::uint8_t code);
        virtual ~illegal_instruction_exception();

    public:
        char const *what() const throw() override;
    };
}

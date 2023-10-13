/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 11:59:20 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/13 17:40:51 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/exception.hpp"

#include "format.hpp"

#include <sstream>

/* located_exception */
namespace gbmu
{
    static std::string _located_message(std::exception const &base, std::string const &file, unsigned int line)
    {
        std::stringstream ss;

        ss << file << ":" << line << ": " << base.what();

        return ss.str();
    }

    located_exception::located_exception(std::exception const &base, std::string const &file, unsigned int line) :
        base(new std::exception(base)),
        file(file),
        line(line),
        _message(_located_message(base, file, line))
    {
    }

    located_exception::~located_exception()
    {
        delete this->base;
    }

    char const *located_exception::what() const throw()
    {
        return this->_message.c_str();
    }
}

namespace gbmu
{
    static std::string _not_implemented_message(std::string const message)
    {
        std::stringstream ss;

        ss << "Not implemented: " << message;

        return ss.str();
    }

    not_implemented_exception::not_implemented_exception(std::string const &message) :
        _message(_not_implemented_message(message))
    {
    }

    not_implemented_exception::~not_implemented_exception() = default;

    char const *not_implemented_exception::what() const throw()
    {
        return this->_message.c_str();
    }
}

/* illegal_instruction_exception */
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

/* memory_index_exception */
namespace gbmu
{
    static std::string _memory_index_message(std::uint16_t address)
    {
        std::stringstream ss;

        ss << "invalid address: " << hex(address);

        return ss.str();
    }

    memory_index_exception::memory_index_exception(std::uint16_t address) :
        _message(_memory_index_message(address)),
        address(address)
    {
    }

    memory_index_exception::~memory_index_exception() = default;

    char const *memory_index_exception::what() const throw()
    {
        return this->_message.c_str();
    }
}

/* memory_read_exception */
namespace gbmu
{
    static std::string _memory_read_message(std::uint16_t address)
    {
        std::stringstream ss;

        ss << "write-only memory read attempt at address " << hex(address);

        return ss.str();
    }

    memory_read_exception::memory_read_exception(std::uint16_t address) :
        _message(_memory_read_message(address)),
        address(address)
    {
    }

    memory_read_exception::~memory_read_exception() = default;

    char const *memory_read_exception::what() const throw()
    {
        return this->_message.c_str();
    }
}

/* memory_write_exception */
namespace gbmu
{
    static std::string _memory_write_message(std::uint16_t address, std::uint8_t value)
    {
        std::stringstream ss;

        ss << "write-only memory write attempt at address " << hex(address) << " with value " << hex(value);

        return ss.str();
    }

    memory_write_exception::memory_write_exception(std::uint16_t address, std::uint8_t value) :
        _message(_memory_write_message(address, value)),
        address(address),
        value(value)
    {
    }

    memory_write_exception::~memory_write_exception() = default;

    char const *memory_write_exception::what() const throw()
    {
        return this->_message.c_str();
    }
}

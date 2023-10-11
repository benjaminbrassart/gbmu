/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 11:57:31 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 11:54:52 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdexcept>
#include <string>

#include <cstdint>

namespace gbmu
{
    /**
     * Thrown when an illegal instruction is executed
     */
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

    /**
     * Thrown when an invalid address is accessed within an I/O device
     */
    class memory_index_exception : public std::exception
    {
    private:
        std::string _message;

    public:
        std::uint16_t address;

        memory_index_exception(std::uint16_t address);
        virtual ~memory_index_exception();

    public:
        char const *what() const throw() override;
    };

    /**
     * Thrown when something tries to read from write-only memory
     */
    class memory_read_exception : public std::exception
    {
    private:
        std::string _message;

    public:
        std::uint16_t address;

        memory_read_exception(std::uint16_t address);
        virtual ~memory_read_exception();

    public:
        char const *what() const throw() override;
    };

    /**
     * Thrown when something tries to write to read-only memory
     */
    class memory_write_exception : public std::exception
    {
    private:
        std::string _message;

    public:
        std::uint16_t address;
        std::uint8_t value;

        memory_write_exception(std::uint16_t address, std::uint8_t value);
        virtual ~memory_write_exception();

    public:
        char const *what() const throw() override;
    };
}

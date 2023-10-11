/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mbc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/11 11:14:02 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 14:10:12 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>

namespace gbmu
{
    enum class cartridge_type;

    // https://gbdev.gg8.se/wiki/articles/Memory_Bank_Controllers
    class mbc
    {
    protected:
        std::uint8_t *rom;
        std::uint8_t *ram;

    public:
        mbc();
        mbc(mbc const &) = delete;
        virtual ~mbc() = 0;

    public:
        virtual std::uint8_t read(std::uint16_t address) const = 0;
        virtual void write(std::uint16_t address, std::uint8_t value) = 0;

    public:
        static mbc *get_mbc(cartridge_type type);
    };

    class rom_only : public mbc
    {
    public:
        rom_only();
        ~rom_only();

    public:
        std::uint8_t read(std::uint16_t address) const;
        void write(std::uint16_t address, std::uint8_t value);
    };

    class mbc1 : public mbc
    {
    private:
        bool ram_enabled;
        int rom_bank;
        int ram_bank;
        int mode;

    public:
        mbc1();
        ~mbc1();

    public:
        std::uint8_t read(std::uint16_t address) const;
        void write(std::uint16_t address, std::uint8_t value);
    };

    class mbc2 : public mbc
    {
    public:
        mbc2();
        ~mbc2();

    public:
        std::uint8_t read(std::uint16_t address) const;
        void write(std::uint16_t address, std::uint8_t value);
    };

    class mbc3 : public mbc
    {
    public:
        mbc3();
        ~mbc3();

    public:
        std::uint8_t read(std::uint16_t address) const;
        void write(std::uint16_t address, std::uint8_t value);
    };

    class mbc5 : public mbc
    {
    public:
        mbc5();
        ~mbc5();

    public:
        std::uint8_t read(std::uint16_t address) const;
        void write(std::uint16_t address, std::uint8_t value);
    };
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:52:48 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 19:31:56 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/debugger.hpp"
#include "gbmu/cpu.hpp"
#include "gbmu/exception.hpp"
#include "gbmu/mmu.hpp"
#include "gbmu/renderer.hpp"
#include "gbmu/io_registers.hpp"
#include "gbmu/color.hpp"

#include "format.hpp"

#include <cctype>
#include <csignal>
#include <concepts>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace gbmu
{
    template<class T>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
    static T parse_unsigned(std::string const &s)
    {
        constexpr static T const max = std::numeric_limits<T>::max();

        T val = static_cast<T>(0);
        T temp;
        auto it = s.cbegin();
        auto end = s.cend();
        int base = 10;

        if (*it == '0')
        {
            it++;
            switch (*it)
            {
                case 'b': {
                    it++;
                    base = 2;
                } break;
                case 'o':
                case 'O': {
                    it++;
                    base = 8;
                } break;
                case 'x':
                case 'X': {
                    it++;
                    base = 16;
                } break;
                default: {
                } break;
            }
        }

        while (it != end)
        {
            switch (*it)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    temp = (*it) - '0';
                    break;
                case 'a':
                case 'b':
                case 'c':
                case 'd':
                case 'e':
                case 'f':
                    temp = 10 + (*it) - 'a';
                    break;
                case 'A':
                case 'B':
                case 'C':
                case 'D':
                case 'E':
                case 'F':
                    temp = 10 + (*it) - 'A';
                    break;
                default:
                    throw std::invalid_argument(s);
            }

            if (temp >= base)
            {
                throw std::invalid_argument(s);
            }

            // (val * base) + temp > max, without overflow
            if (val > (max - temp) / base)
            {
                throw std::out_of_range(s);
            }

            val = (val * base) + temp;
            it++;
        }

        return val;
    }

    debugger::debugger() :
        _breakpoints(),
        _pause(true),
        _step(false)
    {
    }

    debugger::~debugger() = default;

    void debugger::boot(cpu &cpu, mmu &mmu, renderer &renderer)
    {
#ifdef GBMU_DISABLE_BOOT_ROM
        mmu.write(io_registers::REG_BANK, 0x01);

        cpu.pc = 0x0100;
        cpu.sp = 0xFFFE;
#else
        mmu.write(io_registers::REG_BANK, 0x00);

        while (cpu.pc < 0x0100)
        {
            this->step(cpu, mmu, renderer);
        }
#endif
    }

    void debugger::step(cpu &cpu, mmu &mmu, renderer &renderer)
    {
        auto pc = cpu.pc;

        if (this->_breakpoints[pc])
        {
            std::cout
                << "Reached breakpoint at address "
                << hex(pc)
                << std::endl;

            this->_pause = true;
        }

        if (this->_pause)
        {
            this->_process_stdin(cpu, mmu);
            if (!this->_step)
            {
                return;
            }
        }

        this->_step = false;

        // std::cout << "STEP " << hex(pc) << ": " << hex(mmu.read(pc)) << std::endl;

        try
        {
            cpu.step(mmu, renderer);
            // this->_cmd_print(cpu, mmu, std::deque<std::string>());
        }
        catch (gbmu::illegal_instruction_exception const &e)
        {
            std::cout
                << std::endl
                << "  #########################" << std::endl
                << "  #  ILLEGAL INSTRUCTION  #" << std::endl
                << "  #########################" << std::endl
                << "  #                       #" << std::endl
                << "  # Address   Instruction #" << std::endl
                << "  # " << hex(pc) << "    " << hex(e.code) << "        #" << std::endl
                << "  #                       #" << std::endl
                << "  #########################" << std::endl
                << std::endl
            ;
            this->_pause = true;
            return;
        }
    }

    void debugger::handle_signal(int sig)
    {
        if (sig == SIGINT)
        {
            std::cout << "\n";
            this->_prompt();
            this->_pause = true;
        }
    }

    void debugger::_prompt() const
    {
        std::cout << "gbmu> " << std::flush;
    }

    void debugger::_process_stdin(cpu &cpu, mmu &mmu)
    {
        std::string line;

        this->_prompt();

        if (!std::getline(std::cin, line))
        {
            std::cout << "exit" << std::endl;
            std::exit(std::cin.fail() ? 1 : 0);
            return;
        }

        std::deque<std::string> args;

        {
            std::stringstream ss(line);
            std::string arg;

            while (std::getline(ss, arg, ' '))
            {
                if (!arg.empty())
                {
                    args.push_back(arg);
                }
            }
        }

        if (args.empty())
        {
            return;
        }

        std::string cmd_name = args.front();

        args.pop_front();

        auto cmd = debugger::get_command(cmd_name);

        if (cmd == debugger::commands.end())
        {
            std::cout << "Unknown command: " << cmd_name << std::endl;
            std::cout << "Type `help` for available commands" << std::endl;
            return;
        }

        cmd->second.handler(*this, cpu, mmu, args);
    }

    void debugger::_cmd_help(cpu &, mmu &, std::deque<std::string> const &args)
    {
        if (args.empty())
        {

        }
        else
        {

        }
    }

    void debugger::_cmd_print(cpu &cpu, mmu &, std::deque<std::string> const &args)
    {
        if (args.empty())
        {
            std::cout
                << "Registers" << std::endl
                << "  A  = " << hex(cpu.a)
                << "        " << "F  = " << hex(cpu.f)
                << "        " << "AF = " << hex(cpu.af) << std::endl
                << "  B  = " << hex(cpu.b)
                << "        " << "C  = " << hex(cpu.c)
                << "        " << "BC = " << hex(cpu.bc) << "" << std::endl
                << "  D  = " << hex(cpu.d)
                << "        " << "E  = " << hex(cpu.e)
                << "        " << "DE = " << hex(cpu.de) << "" << std::endl
                << "  H  = " << hex(cpu.h)
                << "        " << "L  = " << hex(cpu.l)
                << "        " << "HL = " << hex(cpu.hl) << "" << std::endl
                << "  SP = " << hex(cpu.sp)
                << "      " << "PC = " << hex(cpu.pc) << "" << std::endl
                << std::endl
                << "Flags" << std::endl
                << "  Z N H C" << std::endl
                << "  " << cpu._getflag(cpu_flag::z) << " " << cpu._getflag(cpu_flag::n) << " " << cpu._getflag(cpu_flag::h) << " " << cpu._getflag(cpu_flag::c) << std::endl
            ;
        }
    }

    void debugger::_cmd_continue(cpu &, mmu &, std::deque<std::string> const &)
    {
        this->_pause = false;
        this->_step = false;
    }

    void debugger::_cmd_next(cpu &, mmu &, std::deque<std::string> const &)
    {
        this->_step = true;
    }

    void debugger::_cmd_break(cpu &, mmu &, std::deque<std::string> const &args)
    {
        if (args.empty())
        {
            if (this->_breakpoints.none())
            {
                std::cout << "No breakpoint set." << std::endl;
                return;
            }
                std::cout << "Breakpoints:" << std::endl;

            for (decltype(this->_breakpoints.size()) bit = 0; bit < this->_breakpoints.size(); bit += 1)
            {
                if (this->_breakpoints[bit])
                {
                    std::cout << "  " << hex(static_cast<std::uint16_t>(bit)) << std::endl;
                }
            }
        }
        else
        {
            for (auto &arg : args)
            {
                try
                {
                    auto addr = parse_unsigned<std::uint16_t>(arg);

                    this->_breakpoints[addr] = true;

                    std::cout << "Add breakpoint at address " << hex(addr) << std::endl;
                }
                catch (std::invalid_argument const &)
                {
                    std::cout << "Error: unable to parse address: " << arg << std::endl;
                }
                catch (std::out_of_range const &)
                {
                    std::cout << "Error: address out of range: " << arg << std::endl;
                }
            }
        }
    }

    void debugger::_cmd_stack(cpu &cpu, mmu &, std::deque<std::string> const &)
    {
        cpu.dump_stack();
    }

    void debugger::_cmd_read(cpu &, mmu &mmu, std::deque<std::string> const &args)
    {
        std::uint16_t start;
        std::uint16_t end;

        if (args.size() != 1 && args.size() != 2)
        {
            std::cout << "Usage: read <address>" << std::endl;
            return;
        }

        {
            auto &arg = args[0];

            try
            {
                start = parse_unsigned<std::uint16_t>(arg);
            }
            catch (std::invalid_argument const &)
            {
                std::cout << "Error: unable to parse address: " << arg << std::endl;
                return;
            }
            catch (std::out_of_range const &)
            {
                std::cout << "Error: address out of range: " << arg << std::endl;
                return;
            }
        }

        end = start;

        if (args.size() == 2)
        {
            auto &arg = args[1];

            try
            {
                end = parse_unsigned<std::uint16_t>(arg);
            }
            catch (std::invalid_argument const &)
            {
                std::cout << "Error: unable to parse address: " << arg << std::endl;
                return;
            }
            catch (std::out_of_range const &)
            {
                std::cout << "Error: address out of range: " << arg << std::endl;
                return;
            }
        }

        if (end < start)
        {
            std::cout << "Error: end before start (u dumb?)" << std::endl;
            return;
        }

        constexpr static int const COLUMNS = 16;

        int count = 0;

        for (auto a = start; a <= end; a += 1)
        {
            if (count == 0)
            {
                std::cout << "[" << hex(a) << "] =";
            }

            try
            {
                try
                {
                    auto val = mmu.read(a);

                    std::cout << " " << hex(val);
                }
                catch (located_exception const &e)
                {
                    throw *e.base;
                }
            }
            catch (memory_read_exception const &e)
            {
                std::cout << " " << color::red("? WO");
            }
            catch (memory_index_exception const &e)
            {
                std::cout << " " << color::red("? ID");
            }
            catch (std::exception const& e)
            {
                std::cout << " " << color::red("????");
            }

            if (a == std::numeric_limits<std::uint16_t>::max())
            {
                break;
            }

            count += 1;

            if (count == COLUMNS)
            {
                std::cout << std::endl;
                count = 0;
            }
        }

        if (count != 0)
        {
            std::cout << std::endl;
        }
    }

    void debugger::_cmd_write(cpu &, mmu &mmu, std::deque<std::string> const &args)
    {
        if (args.size() != 2)
        {
            std::cout << "Usage: write <address> <value>" << std::endl;
            return;
        }

        std::uint16_t address;
        std::uint8_t value;

        try
        {
            address = parse_unsigned<std::uint16_t>(args[0]);
        }
        catch (std::invalid_argument const &)
        {
            std::cout << "Error: unable to parse address: " << args[0] << std::endl;
            return;
        }
        catch (std::out_of_range const &)
        {
            std::cout << "Error: address out of range: " << args[0] << std::endl;
            return;
        }

        try
        {
            value = parse_unsigned<std::uint8_t>(args[1]);
        }
        catch (std::invalid_argument const &)
        {
            std::cout << "Error: unable to parse value: " << args[1] << std::endl;
            return;
        }
        catch (std::out_of_range const &)
        {
            std::cout << "Error: value out of range: " << args[1] << std::endl;
            return;
        }

        try
        {
            try
            {
                mmu.write(address, value);
            }
            catch (located_exception const &e)
            {
                throw *e.base;
            }
        }
        catch (std::exception const& e)
        {
            std::cout << color::red("Cannot write to address: ") << color::red(e.what());
        }
    }

    void debugger::_cmd_clear(cpu &, mmu &, std::deque<std::string> const &args)
    {
        for (auto &arg : args)
        {
            try
            {
                auto addr = parse_unsigned<std::uint16_t>(arg);

                this->_breakpoints[addr] = false;

                std::cout << "Remove breakpoint at address " << hex(addr) << std::endl;
            }
            catch (std::invalid_argument const &)
            {
                std::cout << "Error: unable to parse address: " << arg << std::endl;
            }
            catch (std::out_of_range const &)
            {
                std::cout << "Error: address out of range: " << arg << std::endl;
            }
        }
    }

    void debugger::_cmd_exit(cpu &, mmu &, std::deque<std::string> const &)
    {
        std::exit(0);
    }

    std::unordered_map<std::string, command> const debugger::commands {
        {"help", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_help(cpu, mmu, args); },
            "help [<command>]",
        }},
        {"print", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_print(cpu, mmu, args); },
            "print [<entity>]",
        }},
        {"continue", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_continue(cpu, mmu, args); },
            "continue",
        }},
        {"next", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_next(cpu, mmu, args); },
            "next",
        }},
        {"break", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_break(cpu, mmu, args); },
            "break [<address>]",
        }},
        {"stack", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_stack(cpu, mmu, args); },
            "stack",
        }},
        {"read", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_read(cpu, mmu, args); },
            "read <address> [<end>]",
        }},
        {"write", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_write(cpu, mmu, args); },
            "write <address> <value>",
        }},
        {"clear", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_clear(cpu, mmu, args); },
            "clear [<address>]",
        }},
        {"exit", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_exit(cpu, mmu, args); },
            "exit",
        }},
    };

    std::unordered_map<std::string, std::string> const debugger::aliases {
        {"?", "help"},
        {"halp", "help"},
        {"p", "print"},
        {"c", "continue"},
        {"n", "next"},
        {"b", "break"},
        {"clr", "clear"},
    };

    decltype(debugger::commands)::const_iterator debugger::get_command(std::string const &name)
    {
        auto it = debugger::commands.find(name);

        if (it != debugger::commands.end())
        {
            return it;
        }

        auto alias_it = debugger::aliases.find(name);

        if (alias_it == debugger::aliases.end())
        {
            return debugger::commands.end();
        }

        return debugger::commands.find(alias_it->second);
    }
}

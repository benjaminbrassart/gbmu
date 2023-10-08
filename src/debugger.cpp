/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:52:48 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/08 12:19:07 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/debugger.hpp"
#include "gbmu/cpu.hpp"
#include "gbmu/mmu.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <deque>

namespace gbmu
{
    struct hex
    {
    private:
        std::uint64_t _addr;
        std::size_t _w;
    public:

        hex(std::uint8_t addr) : _addr(addr), _w(2) {}
        hex(std::uint16_t addr) : _addr(addr), _w(4) {}

    public:
        friend std::ostream &operator<<(std::ostream &os, hex const &a)
        {
            return os << "0x" << std::hex << std::setfill('0') << std::setw(a._w) << a._addr;
        }
    };

    debugger::debugger() :
        _breakpoints(),
        _pause(true)
    {
    }

    debugger::~debugger() = default;

    void debugger::step(cpu &cpu, mmu &mmu)
    {
        if (this->_breakpoints[cpu.pc])
        {
            std::cout
                << "Reached breakpoint at address 0x"
                << std::hex << std::setw(4) << std::setfill('0')
                << cpu.pc
                << std::endl;
        }
        else if (this->_pause)
        {
            this->_process_stdin(cpu, mmu);
            return;
        }

        cpu.step(mmu);
    }

    void debugger::_process_stdin(cpu &cpu, mmu &mmu)
    {
        std::string line;

        std::cout << "gbmu> " << std::flush;

        if (!std::getline(std::cin, line))
        {
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
                << "  A  = " << hex(cpu.a) << "        F  = " << hex(cpu.f) << "        AF = " << hex(cpu.af) << std::endl
                << "  B  = " << hex(cpu.b) << "        C  = " << hex(cpu.c) << "        BC = " << hex(cpu.bc) << "" << std::endl
                << "  D  = " << hex(cpu.d) << "        E  = " << hex(cpu.e) << "        DE = " << hex(cpu.de) << "" << std::endl
                << "  H  = " << hex(cpu.h) << "        L  = " << hex(cpu.l) << "        HL = " << hex(cpu.hl) << "" << std::endl
                << "  SP = " << hex(cpu.sp) << "      PC = " << hex(cpu.pc) << "" << std::endl
                << std::endl
                << "Flags" << std::endl
                << "  Z N H C" << std::endl
                << "  " << cpu.flags.z << " " << cpu.flags.n << " " << cpu.flags.h << " " << cpu.flags.c << std::endl
            ;
        }
    }

    std::unordered_map<std::string, command> const debugger::commands {
        {"help", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_help(cpu, mmu, args); },
            "help [command]",
        }},
        {"print", {
            [](auto &d, auto& cpu, auto &mmu, auto &args) { d._cmd_print(cpu, mmu, args); },
            "print [entity]",
        }},
    };

    std::unordered_map<std::string, std::string> const debugger::aliases {
        {"?", "help"},
        {"halp", "help"},
        {"p", "print"},
        {"c", "continue"},
        {"n", "next"},
        {"b", "break"},
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

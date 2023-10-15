/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:52:48 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/15 23:58:44 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/debugger.hpp"
#include "gbmu/cpu.hpp"
#include "gbmu/exception.hpp"
#include "gbmu/mmu.hpp"

#include "format.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <deque>

namespace gbmu
{
    debugger::debugger() :
        _breakpoints(),
        _pause(false),
        _step(false)
    {
        // this->_breakpoints[0x0042] = 1;
    }

    debugger::~debugger() = default;

    void debugger::boot(cpu &cpu, mmu &mmu)
    {
#ifdef GBMU_DISABLE_BOOT_ROM
        mmu.write(0xFF50, 0x01);

        cpu.pc = 0x0100;
        cpu.sp = 0xFFFE;
#else
        mmu.write(0xFF50, 0x00);

        while (cpu.pc < 0x0100)
        {
            this->step(cpu, mmu);
        }
#endif
    }

    void debugger::step(cpu &cpu, mmu &mmu)
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

        std::cout << "STEP " << hex(pc) << ": " << hex(mmu.read(pc)) << std::endl;

        try
        {
            cpu.step(mmu);
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

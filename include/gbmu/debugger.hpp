/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:47:30 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/08 11:50:46 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <bitset>
#include <deque>
#include <functional>
#include <string>
#include <unordered_map>

namespace gbmu
{
    class cpu;
    class mmu;
    class debugger;

    typedef void (*command_handler)(debugger &, cpu &, mmu &, std::deque<std::string> const &);

    struct command
    {
        command_handler handler;
        std::string help;
    };

    class debugger
    {
    private:
        static std::unordered_map<std::string, command> const commands;
        static std::unordered_map<std::string, std::string> const aliases;

    private:
        std::bitset<1 << 16> _breakpoints;
        bool _pause;

    public:
        debugger();
        ~debugger();

    public:
        void step(cpu &cpu, mmu &mmu);

    private:
        void _process_stdin(cpu &cpu, mmu &mmu);

        static decltype(debugger::commands)::const_iterator get_command(std::string const &name);

    private:
        void _cmd_help(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
        void _cmd_print(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
    };
}

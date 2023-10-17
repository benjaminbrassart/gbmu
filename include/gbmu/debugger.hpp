/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:47:30 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 12:01:31 by bbrassar         ###   ########.fr       */
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
    class renderer;

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
        bool _step;

    public:
        debugger();
        ~debugger();

    public:
        void boot(cpu &cpu, mmu &mmu, renderer &renderer);
        void step(cpu &cpu, mmu &mmu, renderer &renderer);

    private:
        void _process_stdin(cpu &cpu, mmu &mmu);

        static decltype(debugger::commands)::const_iterator get_command(std::string const &name);

    private:
        void _cmd_help(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
        void _cmd_print(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
        void _cmd_continue(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
        void _cmd_next(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
        void _cmd_break(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
        void _cmd_stack(cpu &cpu, mmu &mmu, std::deque<std::string> const &args);
    };
}

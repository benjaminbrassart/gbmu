/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:14:40 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 19:20:27 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dbg.hpp"
#include "gbmu/cartridge.hpp"
#include "gbmu/cpu.hpp"
#include "gbmu/debugger.hpp"
#include "gbmu/exception.hpp"
#include "gbmu/mmu.hpp"
#include "gbmu/renderer.hpp"

#include <fstream>
#include <iostream>

#include <cerrno>
#include <csignal>
#include <cstdlib>
#include <cstring>

static gbmu::debugger *DEBUGGER;

static void _handle_sig(int sig)
{
    DEBUGGER->handle_signal(sig);
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: gbmu <rom>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream f;

    f.open(argv[1], std::ios::in | std::ios::binary);

    if (!f)
    {
        auto e = errno;

        std::cerr << "gbmu: " << argv[1] << ": " << std::strerror(e) << std::endl;
        return EXIT_FAILURE;
    }

    auto cartridge = gbmu::cartridge::create(f);

    if (cartridge == nullptr)
    {
        std::cerr << "gbmu: " << argv[1] << ": Invalid rom" << std::endl;
        return EXIT_FAILURE;
    }

    gbmu::debugger debugger;

    DEBUGGER = &debugger;

    constexpr static int const SIGNALS[] {
        SIGINT,
        SIGQUIT,
    };

    for (auto sig : SIGNALS)
    {
        std::signal(sig, &_handle_sig);
    }

    gbmu::cpu cpu;
    gbmu::mmu mmu(*cartridge);
    gbmu::renderer renderer;

    debugger.boot(cpu, mmu, renderer);

    while (true)
    {
        try
        {
            debugger.step(cpu, mmu, renderer);
        }
        catch (std::exception const &e)
        {
            std::cerr << "gbmu: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

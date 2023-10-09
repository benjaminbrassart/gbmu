/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:14:40 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 14:28:10 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dbg.hpp"
#include "gbmu/cartridge.hpp"
#include "gbmu/cpu.hpp"
#include "gbmu/debugger.hpp"
#include "gbmu/exception.hpp"
#include "gbmu/mmu.hpp"

#include <fstream>
#include <iostream>

#include <cerrno>
#include <cstdlib>
#include <cstring>

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

    gbmu::cartridge cartridge;

    if (!cartridge.open(f))
    {
        std::cerr << "gbmu: " << argv[1] << ": Invalid rom" << std::endl;
        return EXIT_FAILURE;
    }

    gbmu::debugger debugger;
    gbmu::cpu cpu;
    gbmu::mmu mmu(cartridge);

    while (true)
    {
        try
        {
            debugger.step(cpu, mmu);
        }
        catch (std::exception const &e)
        {
            std::cerr << "gbmu: " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

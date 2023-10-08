/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:14:40 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/08 12:21:19 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dbg.hpp"
#include "gbmu/cartridge.hpp"
#include "gbmu/cpu.hpp"
#include "gbmu/debugger.hpp"
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

    char int_vectors[0x0100];
    char raw_header[gbmu::CARTRIDGE_HEADER_SIZE];
    auto header = reinterpret_cast<gbmu::cartridge_header *>(&raw_header[0]);

    if (
        !dbg(f.read(int_vectors, sizeof(int_vectors))) ||
        !dbg(f.read(raw_header, sizeof(raw_header))) ||
        !dbg(header->check_logo())
    )
    {
        std::cerr << "gbmu: " << argv[1] << ": Invalid rom" << std::endl;
        return EXIT_FAILURE;
    }

    gbmu::debugger debugger;
    gbmu::cpu cpu;
    gbmu::mmu mmu;

    while (true)
    {
        debugger.step(cpu, mmu);
    }

    return EXIT_SUCCESS;
}

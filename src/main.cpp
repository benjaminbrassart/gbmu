/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:14:40 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/07 16:21:40 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/cpu.hpp"
#include "gbmu/emulator.hpp"
#include "gbmu/mmu.hpp"

#include <cstdlib>

int main([[maybe_unused]] int argc, [[maybe_unused]] char const *argv[])
{
    gbmu::cpu cpu;
    gbmu::mmu mmu;

    while (true)
    {
        cpu.step(mmu);
    }

    return EXIT_SUCCESS;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   interrupt.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 00:31:52 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/16 00:40:17 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

namespace gbmu
{
    enum class interrupt_flag
    {
        VBlank,
        STAT,
        Timer,
        Serial,
        Joypad,
    };

    class interrupts
    {
    public:
        static interrupts INSTANCE;

    public:
        void trigger(interrupt_flag it);
    };

    template<class T, interrupt_flag Int>
    class interrupt_source
    {
    public:
        inline void trigger_interrupt() const
        {
            interrupts::INSTANCE.trigger(Int);
        }
    };
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_registers.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 14:38:53 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/09 16:04:46 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>

namespace gbmu
{
    struct io_registers
    {
        public: std::uint8_t joyp; // FF00
        public: std::uint8_t sb; // FF01
        public: std::uint8_t sc; // FF02
        private: std::uint8_t _FF03; // FF03
        public: std::uint8_t div; // FF04
        public: std::uint8_t tima; // FF05
        public: std::uint8_t tma; // FF06
        public: std::uint8_t tac; // FF07
        private: std::uint8_t _FF08_FF0E[7]; // FF08 - FF0E
        public: std::uint8_t intf; // FF0F
        public: std::uint8_t nr10; // FF10
        public: std::uint8_t nr11; // FF11
        public: std::uint8_t nr12; // FF12
        public: std::uint8_t nr13; // FF13
        public: std::uint8_t nr14; // FF14
        public: std::uint8_t nr21; // FF15
        private: std::uint8_t _FF16; // FF16
        public: std::uint8_t nr22; // FF17
        public: std::uint8_t nr23; // FF18
        public: std::uint8_t nr24; // FF19
        public: std::uint8_t nr30; // FF1A
        public: std::uint8_t nr31; // FF1B
        public: std::uint8_t nr32; // FF1C
        public: std::uint8_t nr33; // FF1D
        public: std::uint8_t nr34; // FF1E
        private: std::uint8_t _FF1F; // FF1F
        public: std::uint8_t nr41; // FF20
        public: std::uint8_t nr42; // FF21
        public: std::uint8_t nr43; // FF22
        public: std::uint8_t nr44; // FF23
        public: std::uint8_t nr50; // FF24
        public: std::uint8_t nr51; // FF25
        public: std::uint8_t nr52; // FF26
        private: std::uint8_t _FF27_FF2F[9]; // FF27 - FF2F
        public: std::uint8_t wave_ram[16]; // FF30 - FF3F
        public: std::uint8_t lcdc; // FF40
        public: std::uint8_t stat; // FF41
        public: std::uint8_t scy; // FF42
        public: std::uint8_t scx; // FF43
        public: std::uint8_t ly; // FF44
        public: std::uint8_t lyc; // FF45
        private: std::uint8_t _FF46; // FF46
        public: std::uint8_t bgp; // FF47
        public: std::uint8_t obp0; // FF48
        public: std::uint8_t obp1; // FF49
        public: std::uint8_t wy; // FF4A
        public: std::uint8_t wx; // FF4B
        private: std::uint8_t _FF4C; // FF4C
        public: std::uint8_t key1; // FF4D
        private: std::uint8_t _FF4E; // FF4E
        public: std::uint8_t vbk; // FF4F
        public: std::uint8_t dboot; // FF50
        public: std::uint8_t hdma1; // FF51
        public: std::uint8_t hdma2; // FF52
        public: std::uint8_t hdma3; // FF53
        public: std::uint8_t hdma4; // FF54
        public: std::uint8_t hdma5; // FF55
        public: std::uint8_t rp; // FF56
        private: std::uint8_t _FF57_FF67[16]; // FF57 - FF67
        public: std::uint8_t bcps; // FF68
        public: std::uint8_t bcpd; // FF69
        public: std::uint8_t ocps; // FF6A
        public: std::uint8_t ocpd; // FF6B
        public: std::uint8_t opri; // FF6C
        private: std::uint8_t _FF6D_FF6F[3]; // FF6D - FF6F
        public: std::uint8_t svbk; // FF70
    };

    constexpr static std::size_t const IO_REGISTERS_SIZE = sizeof(io_registers);

    static_assert(IO_REGISTERS_SIZE == 112);
}

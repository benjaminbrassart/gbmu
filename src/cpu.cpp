/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cpu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:27:35 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 13:00:46 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/cpu.hpp"
#include "gbmu/mmu.hpp"
#include "gbmu/exception.hpp"
#include "gbmu/renderer.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace gbmu
{
    union hi_lo
    {
        std::uint16_t value;
        struct
        {
            std::uint8_t lo;
            std::uint8_t hi;
        };

        hi_lo(std::uint16_t val) : value(val) {}
        hi_lo(std::uint8_t low, std::uint8_t high) : lo(low), hi(high) {}
    };

    cpu::cpu() :
        f(0x00),
        a(0x00),
        c(0x00),
        b(0x00),
        e(0x00),
        d(0x00),
        l(0x00),
        h(0x00),
        pc(0x0000),
        sp(0x0000)
    {
    }

    cpu::~cpu() = default;

    void cpu::boot(mmu &mmu, renderer &renderer)
    {
        while (this->pc < 0x0100)
        {
            this->step(mmu, renderer);
        }
    }

    void cpu::step(mmu &mmu, renderer &renderer)
    {
        renderer.poll_events(mmu);

        // handle interrupts
        if (mmu.interrupt_master)
        {
            int interrupts = cpu::IF_MASK & mmu.read(0xFF0F) & mmu.read(0xFFFF);

            for (int i = 0; i < 5; i += 1)
            {
                if ((interrupts & (1 << i)) != 0)
                {
                    this->_handle_interrupt(mmu, i);
                    return;
                }
            }
        }

        std::uint8_t code = this->_read_byte(mmu);

        this->_handle_instruction(mmu, code);

        renderer.render(mmu);
    }

    void cpu::dump_stack()
    {
        auto stack_copy = this->stack;
        int index = 0;

        while (!stack_copy.empty())
        {
            auto &elem = stack_copy.top();

            std::cout << index << ": " << elem << std::endl;

            index += 1;
            stack_copy.pop();
        }
    }

    std::uint8_t cpu::_read_byte(mmu &mmu)
    {
        std::uint8_t b = mmu.read(this->pc);

        this->pc += 1;

        return b;
    }

    std::uint16_t cpu::_read_word(mmu &mmu)
    {
        hi_lo pair(this->_read_byte(mmu), this->_read_byte(mmu));

        return pair.value;
    }

    void cpu::_handle_interrupt(mmu &mmu, int interrupt)
    {

        this->_call(mmu, 0x40 + ((interrupt << 5) * 8));

        std::uint8_t orig = mmu.read(0xFF0F);

        // disable interrupts
        mmu.interrupt_master = false;
        // reset current interrupt
        mmu.write(0xFF0F, orig & cpu::IF_MASK & ~interrupt);
    }

    void cpu::_handle_instruction(mmu &mmu, std::uint8_t code)
    {
        switch (code)
        {
        case 0x00: {
            this->NOP(mmu);
        } break;

        case 0x10: {
            this->STOP(mmu);
        } break;

        case 0x20: {
            this->JR(mmu, !this->_getflag(cpu_flag::z));
        } break;

        case 0x30: {
            this->JR(mmu, !this->_getflag(cpu_flag::c));
        } break;

        case 0x01: {
            this->LD(mmu, this->bc);
        } break;

        case 0x11: {
            this->LD(mmu, this->de);
        } break;

        case 0x21: {
            this->LD(mmu, this->hl);
        } break;

        case 0x31: {
            this->LD(mmu, this->sp);
        } break;

        case 0x02: {
            this->LD(mmu, this->bc, this->a);
        } break;

        case 0x12: {
            this->LD(mmu, this->de, this->a);
        } break;

        case 0x22: {
            this->LD(mmu, this->hl, this->a);
            this->hl += 1;
        } break;

        case 0x32: {
            this->LD(mmu, this->hl, this->a);
            this->hl -= 1;
        } break;

        case 0x03: {
            this->INC(mmu, this->bc);
        } break;

        case 0x13: {
            this->INC(mmu, this->de);
        } break;

        case 0x23: {
            this->INC(mmu, this->hl);
        } break;

        case 0x33: {
            this->INC(mmu, this->sp);
        } break;

        case 0x04: {
            this->INC(mmu, this->b);
        } break;

        case 0x14: {
            this->INC(mmu, this->d);
        } break;

        case 0x24: {
            this->INC(mmu, this->h);
        } break;

        case 0x34: {
            this->INC(mmu, this->hl, true);
        } break;

        case 0x05: {
            this->DEC(mmu, this->b);
        } break;

        case 0x15: {
            this->DEC(mmu, this->d);
        } break;

        case 0x25: {
            this->DEC(mmu, this->h);
        } break;

        case 0x35: {
            this->DEC(mmu, this->hl, true);
        } break;

        case 0x06: {
            this->LD(mmu, this->b);
        } break;

        case 0x16: {
            this->LD(mmu, this->d);
        } break;

        case 0x26: {
            this->LD(mmu, this->h);
        } break;

        case 0x36: {
            this->LD(mmu, this->hl, true);
        } break;

        case 0x07: {
            this->RLCA(mmu);
        } break;

        case 0x17: {
            this->RLA(mmu);
        } break;

        case 0x27: {
            this->DAA(mmu);
        } break;

        case 0x37: {
            this->SCF(mmu);
        } break;

        case 0x08: { // LD [a16] SP
            auto address = this->_read_word(mmu);
            hi_lo value(this->sp);

            mmu.write(address, value.lo);
            mmu.write(address + 1, value.hi);
        } break;

        case 0x18: {
            this->JR(mmu, true);
        } break;

        case 0x28: {
            this->JR(mmu, this->_getflag(cpu_flag::z));
        } break;

        case 0x38: {
            this->JR(mmu, this->_getflag(cpu_flag::c));
        } break;

        case 0x09: {
            this->ADD(mmu, this->hl, this->bc);
        } break;

        case 0x19: {
            this->ADD(mmu, this->hl, this->de);
        } break;

        case 0x29: {
            this->ADD(mmu, this->hl, this->hl);
        } break;

        case 0x39: {
            this->ADD(mmu, this->hl, this->sp);
        } break;

        case 0x0A: {
            this->LD(mmu, this->a, this->bc);
        } break;

        case 0x1A: {
            this->LD(mmu, this->a, this->de);
        } break;

        case 0x2A: {
            this->LD(mmu, this->a, this->hl);
            this->hl += 1;
        } break;

        case 0x3A: {
            this->LD(mmu, this->a, this->hl);
            this->hl -= 1;
        } break;

        case 0x0B: {
            this->DEC(mmu, this->bc);
        } break;

        case 0x1B: {
            this->DEC(mmu, this->de);
        } break;

        case 0x2B: {
            this->DEC(mmu, this->hl);
        } break;

        case 0x3B: {
            this->DEC(mmu, this->sp);
        } break;

        case 0x0C: {
            this->INC(mmu, this->c);
        } break;

        case 0x1C: {
            this->INC(mmu, this->e);
        } break;

        case 0x2C: {
            this->INC(mmu, this->l);
        } break;

        case 0x3C: {
            this->INC(mmu, this->a);
        } break;

        case 0x0D: {
            this->DEC(mmu, this->c);
        } break;

        case 0x1D: {
            this->DEC(mmu, this->e);
        } break;

        case 0x2D: {
            this->DEC(mmu, this->l);
        } break;

        case 0x3D: {
            this->DEC(mmu, this->a);
        } break;

        case 0x0E: {
            this->LD(mmu, this->c);
        } break;

        case 0x1E: {
            this->LD(mmu, this->e);
        } break;

        case 0x2E: {
            this->LD(mmu, this->l);
        } break;

        case 0x3E: {
            this->LD(mmu, this->a);
        } break;

        case 0x0F: {
            this->RRCA(mmu);
        } break;

        case 0x1F: {
            this->RRA(mmu);
        } break;

        case 0x2F: {
            this->CPL(mmu);
        } break;

        case 0x3F: {
            this->CCF(mmu);
        } break;

        case 0x40: {
            this->LD(mmu, this->b, this->b);
        } break;

        case 0x50: {
            this->LD(mmu, this->d, this->b);
        } break;

        case 0x60: {
            this->LD(mmu, this->h, this->b);
        } break;

        case 0x70: {
            this->LD(mmu, this->hl, this->b);
        } break;

        case 0x41: {
            this->LD(mmu, this->b, this->c);
        } break;

        case 0x51: {
            this->LD(mmu, this->d, this->c);
        } break;

        case 0x61: {
            this->LD(mmu, this->h, this->c);
        } break;

        case 0x71: {
            this->LD(mmu, this->hl, this->c);
        } break;

        case 0x42: {
            this->LD(mmu, this->b, this->d);
        } break;

        case 0x52: {
            this->LD(mmu, this->d, this->d);
        } break;

        case 0x62: {
            this->LD(mmu, this->h, this->d);
        } break;

        case 0x72: {
            this->LD(mmu, this->hl, this->d);
        } break;

        case 0x43: {
            this->LD(mmu, this->b, this->e);
        } break;

        case 0x53: {
            this->LD(mmu, this->d, this->e);
        } break;

        case 0x63: {
            this->LD(mmu, this->h, this->e);
        } break;

        case 0x73: {
            this->LD(mmu, this->hl, this->e);
        } break;

        case 0x44: {
            this->LD(mmu, this->b, this->h);
        } break;

        case 0x54: {
            this->LD(mmu, this->d, this->h);
        } break;

        case 0x64: {
            this->LD(mmu, this->h, this->h);
        } break;

        case 0x74: {
            this->LD(mmu, this->hl, this->h);
        } break;

        case 0x45: {
            this->LD(mmu, this->b, this->l);
        } break;

        case 0x55: {
            this->LD(mmu, this->d, this->l);
        } break;

        case 0x65: {
            this->LD(mmu, this->h, this->l);
        } break;

        case 0x75: {
            this->LD(mmu, this->hl, this->l);
        } break;

        case 0x46: {
            this->LD(mmu, this->b, this->hl);
        } break;

        case 0x56: {
            this->LD(mmu, this->d, this->hl);
        } break;

        case 0x66: {
            this->LD(mmu, this->h, this->hl);
        } break;

        case 0x76: {
            this->HALT(mmu);
        } break;

        case 0x47: {
            this->LD(mmu, this->b, this->a);
        } break;

        case 0x57: {
            this->LD(mmu, this->d, this->a);
        } break;

        case 0x67: {
            this->LD(mmu, this->h, this->a);
        } break;

        case 0x77: {
            this->LD(mmu, this->hl, this->a);
        } break;

        case 0x48: {
            this->LD(mmu, this->c, this->b);
        } break;

        case 0x58: {
            this->LD(mmu, this->e, this->b);
        } break;

        case 0x68: {
            this->LD(mmu, this->l, this->b);
        } break;

        case 0x78: {
            this->LD(mmu, this->a, this->b);
        } break;

        case 0x49: {
            this->LD(mmu, this->c, this->c);
        } break;

        case 0x59: {
            this->LD(mmu, this->e, this->c);
        } break;

        case 0x69: {
            this->LD(mmu, this->l, this->c);
        } break;

        case 0x79: {
            this->LD(mmu, this->a, this->c);
        } break;

        case 0x4A: {
            this->LD(mmu, this->c, this->d);
        } break;

        case 0x5A: {
            this->LD(mmu, this->e, this->d);
        } break;

        case 0x6A: {
            this->LD(mmu, this->l, this->d);
        } break;

        case 0x7A: {
            this->LD(mmu, this->a, this->d);
        } break;

        case 0x4B: {
            this->LD(mmu, this->c, this->e);
        } break;

        case 0x5B: {
            this->LD(mmu, this->e, this->e);
        } break;

        case 0x6B: {
            this->LD(mmu, this->l, this->e);
        } break;

        case 0x7B: {
            this->LD(mmu, this->a, this->e);
        } break;

        case 0x4C: {
            this->LD(mmu, this->c, this->h);
        } break;

        case 0x5C: {
            this->LD(mmu, this->e, this->h);
        } break;

        case 0x6C: {
            this->LD(mmu, this->l, this->h);
        } break;

        case 0x7C: {
            this->LD(mmu, this->a, this->h);
        } break;

        case 0x4D: {
            this->LD(mmu, this->c, this->l);
        } break;

        case 0x5D: {
            this->LD(mmu, this->e, this->l);
        } break;

        case 0x6D: {
            this->LD(mmu, this->l, this->l);
        } break;

        case 0x7D: {
            this->LD(mmu, this->a, this->l);
        } break;

        case 0x4E: {
            this->LD(mmu, this->c, this->hl);
        } break;

        case 0x5E: {
            this->LD(mmu, this->e, this->hl);
        } break;

        case 0x6E: {
            this->LD(mmu, this->l, this->hl);
        } break;

        case 0x7E: {
            this->LD(mmu, this->a, this->hl);
        } break;

        case 0x4F: {
            this->LD(mmu, this->c, this->a);
        } break;

        case 0x5F: {
            this->LD(mmu, this->e, this->a);
        } break;

        case 0x6F: {
            this->LD(mmu, this->l, this->a);
        } break;

        case 0x7F: {
            this->LD(mmu, this->a, this->a);
        } break;

        case 0x80: {
            this->ADD(mmu, this->a, this->b);
        } break;

        case 0x90: {
            this->SUB(mmu, this->a, this->b);
        } break;

        case 0xA0: {
            this->AND(mmu, this->a, this->b);
        } break;

        case 0xB0: {
            this->OR(mmu, this->a, this->b);
        } break;

        case 0x81: {
            this->ADD(mmu, this->a, this->c);
        } break;

        case 0x91: {
            this->SUB(mmu, this->a, this->c);
        } break;

        case 0xA1: {
            this->AND(mmu, this->a, this->c);
        } break;

        case 0xB1: {
            this->OR(mmu, this->a, this->c);
        } break;

        case 0x82: {
            this->ADD(mmu, this->a, this->d);
        } break;

        case 0x92: {
            this->SUB(mmu, this->a, this->d);
        } break;

        case 0xA2: {
            this->AND(mmu, this->a, this->d);
        } break;

        case 0xB2: {
            this->OR(mmu, this->a, this->d);
        } break;

        case 0x83: {
            this->ADD(mmu, this->a, this->e);
        } break;

        case 0x93: {
            this->SUB(mmu, this->a, this->e);
        } break;

        case 0xA3: {
            this->AND(mmu, this->a, this->e);
        } break;

        case 0xB3: {
            this->OR(mmu, this->a, this->e);
        } break;

        case 0x84: {
            this->ADD(mmu, this->a, this->h);
        } break;

        case 0x94: {
            this->SUB(mmu, this->a, this->h);
        } break;

        case 0xA4: {
            this->AND(mmu, this->a, this->h);
        } break;

        case 0xB4: {
            this->OR(mmu, this->a, this->h);
        } break;

        case 0x85: {
            this->ADD(mmu, this->a, this->l);
        } break;

        case 0x95: {
            this->SUB(mmu, this->a, this->l);
        } break;

        case 0xA5: {
            this->AND(mmu, this->a, this->l);
        } break;

        case 0xB5: {
            this->OR(mmu, this->a, this->l);
        } break;

        case 0x86: {
            this->ADD(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0x96: {
            this->SUB(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0xA6: {
            this->AND(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0xB6: {
            this->OR(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0x87: {
            this->ADD(mmu, this->a, this->a);
        } break;

        case 0x97: {
            this->SUB(mmu, this->a, this->a);
        } break;

        case 0xA7: {
            this->AND(mmu, this->a, this->a);
        } break;

        case 0xB7: {
            this->OR(mmu, this->a, this->a);
        } break;

        case 0x88: {
            this->ADC(mmu, this->a, this->b);
        } break;

        case 0x98: {
            this->SBC(mmu, this->a, this->b);
        } break;

        case 0xA8: {
            this->XOR(mmu, this->a, this->b);
        } break;

        case 0xB8: {
            this->CP(mmu, this->a, this->b);
        } break;

        case 0x89: {
            this->ADC(mmu, this->a, this->c);
        } break;

        case 0x99: {
            this->SBC(mmu, this->a, this->c);
        } break;

        case 0xA9: {
            this->XOR(mmu, this->a, this->c);
        } break;

        case 0xB9: {
            this->CP(mmu, this->a, this->c);
        } break;

        case 0x8A: {
            this->ADC(mmu, this->a, this->d);
        } break;

        case 0x9A: {
            this->SBC(mmu, this->a, this->d);
        } break;

        case 0xAA: {
            this->XOR(mmu, this->a, this->d);
        } break;

        case 0xBA: {
            this->CP(mmu, this->a, this->d);
        } break;

        case 0x8B: {
            this->ADC(mmu, this->a, this->e);
        } break;

        case 0x9B: {
            this->SBC(mmu, this->a, this->e);
        } break;

        case 0xAB: {
            this->XOR(mmu, this->a, this->e);
        } break;

        case 0xBB: {
            this->CP(mmu, this->a, this->e);
        } break;

        case 0x8C: {
            this->ADC(mmu, this->a, this->h);
        } break;

        case 0x9C: {
            this->SBC(mmu, this->a, this->h);
        } break;

        case 0xAC: {
            this->XOR(mmu, this->a, this->h);
        } break;

        case 0xBC: {
            this->CP(mmu, this->a, this->h);
        } break;

        case 0x8D: {
            this->ADC(mmu, this->a, this->l);
        } break;

        case 0x9D: {
            this->SBC(mmu, this->a, this->l);
        } break;

        case 0xAD: {
            this->XOR(mmu, this->a, this->l);
        } break;

        case 0xBD: {
            this->CP(mmu, this->a, this->l);
        } break;

        case 0x8E: {
            this->ADC(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0x9E: {
            this->SBC(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0xAE: {
            this->XOR(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0xBE: {
            this->CP(mmu, this->a, mmu.read(this->hl));
        } break;

        case 0x8F: {
            this->ADC(mmu, this->a, this->a);
        } break;

        case 0x9F: {
            this->SBC(mmu, this->a, this->a);
        } break;

        case 0xAF: {
            this->XOR(mmu, this->a, this->a);
        } break;

        case 0xBF: {
            this->CP(mmu, this->a, this->a);
        } break;

        case 0xC0: {
            this->RET(mmu, !this->_getflag(cpu_flag::z));
        } break;

        case 0xD0: {
            this->RET(mmu, !this->_getflag(cpu_flag::c));
        } break;

        case 0xE0: {
            auto a8 = this->_read_byte(mmu);

            this->LD(mmu, 0xFF00 + a8, this->a);
        } break;

        case 0xF0: {
            auto a8 = this->_read_byte(mmu);

            this->LD(mmu, this->a, mmu.read(0xFF00 + a8));
        } break;

        case 0xC1: {
            this->POP(mmu, this->bc);
        } break;

        case 0xD1: {
            this->POP(mmu, this->de);
        } break;

        case 0xE1: {
            this->POP(mmu, this->hl);
        } break;

        case 0xF1: {
            this->POP(mmu, this->af);
        } break;

        case 0xC2: {
            this->JP(mmu, !this->_getflag(cpu_flag::z));
        } break;

        case 0xD2: {
            this->JP(mmu, !this->_getflag(cpu_flag::c));
        } break;

        case 0xE2: {
            this->LD(mmu, 0xFF00 + this->c, this->a);
        } break;

        case 0xF2: {
            this->LD(mmu, this->a, mmu.read(0xFF00 + this->c));
        } break;

        case 0xC3: {
            this->JP(mmu, true);
        } break;

        case 0xD3: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xE3: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xF3: {
            this->DI(mmu);
        } break;

        case 0xC4: {
            this->CALL(mmu, !this->_getflag(cpu_flag::z));
        } break;

        case 0xD4: {
            this->CALL(mmu, !this->_getflag(cpu_flag::c));
        } break;

        case 0xE4: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xF4: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xC5: {
            this->PUSH(mmu, this->bc);
        } break;

        case 0xD5: {
            this->PUSH(mmu, this->de);
        } break;

        case 0xE5: {
            this->PUSH(mmu, this->hl);
        } break;

        case 0xF5: {
            this->PUSH(mmu, this->af);
        } break;

        case 0xC6: {
            this->ADD(mmu, this->a);
        } break;

        case 0xD6: {
            this->SUB(mmu, this->a);
        } break;

        case 0xE6: {
            this->AND(mmu, this->a);
        } break;

        case 0xF6: {
            this->OR(mmu, this->a);
        } break;

        case 0xC7: {
            this->RST(mmu, 0x00);
        } break;

        case 0xD7: {
            this->RST(mmu, 0x10);
        } break;

        case 0xE7: {
            this->RST(mmu, 0x20);
        } break;

        case 0xF7: {
            this->RST(mmu, 0x30);
        } break;

        case 0xC8: {
            this->RET(mmu, this->_getflag(cpu_flag::z));
        } break;

        case 0xD8: {
            this->RET(mmu, this->_getflag(cpu_flag::c));
        } break;

        case 0xE8: {
            this->ADD(mmu, this->sp);
        } break;

        case 0xF8: {
            auto e8 = this->_read_byte(mmu);

            this->LD(mmu, this->hl, static_cast<std::uint16_t>(this->sp + e8));
        } break;

        case 0xC9: {
            this->RET(mmu, true);
        } break;

        case 0xD9: {
            this->RETI(mmu);
        } break;

        case 0xE9: {
            this->JP(mmu, this->hl);
        } break;

        case 0xF9: {
            this->LD(mmu, this->sp, this->hl);
        } break;

        case 0xCA: {
            this->JP(mmu, this->_getflag(cpu_flag::z));
        } break;

        case 0xDA: {
            this->JP(mmu, this->_getflag(cpu_flag::c));
        } break;

        case 0xEA: {
            auto a16 = this->_read_word(mmu);

            this->LD(mmu, a16, this->a);
        } break;

        case 0xFA: {
            auto a16 = this->_read_word(mmu);

            this->LD(mmu, this->a, a16);
        } break;

        case 0xCB: {
            this->PREFIX(mmu);
        } break;

        case 0xDB: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xEB: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xFB: {
            this->EI(mmu);
        } break;

        case 0xCC: {
            this->CALL(mmu, this->_getflag(cpu_flag::z));
        } break;

        case 0xDC: {
            this->CALL(mmu, this->_getflag(cpu_flag::c));
        } break;

        case 0xEC: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xFC: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xCD: {
            this->CALL(mmu, true);
        } break;

        case 0xDD: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xED: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xFD: {
            this->ILLEGAL(mmu, code);
        } break;

        case 0xCE: {
            this->ADC(mmu, this->a);
        } break;

        case 0xDE: {
            this->SBC(mmu, this->a);
        } break;

        case 0xEE: {
            this->XOR(mmu, this->a);
        } break;

        case 0xFE: {
            this->CP(mmu, this->a);
        } break;

        case 0xCF: {
            this->RST(mmu, 0x08);
        } break;

        case 0xDF: {
            this->RST(mmu, 0x18);
        } break;

        case 0xEF: {
            this->RST(mmu, 0x28);
        } break;

        case 0xFF: {
            this->RST(mmu, 0x38);
        } break;
        default: {
            std::cout << "Unhandled instruction: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(code) << std::endl;
        } break;
        }
    }

    void cpu::_handle_instruction_prefix(mmu &mmu, std::uint8_t code)
    {
        switch (code)
        {
        case 0x00: {
            this->RLC(mmu, this->b);
        } break;

        case 0x01: {
            this->RLC(mmu, this->c);
        } break;

        case 0x02: {
            this->RLC(mmu, this->d);
        } break;

        case 0x03: {
            this->RLC(mmu, this->e);
        } break;

        case 0x04: {
            this->RLC(mmu, this->h);
        } break;

        case 0x05: {
            this->RLC(mmu, this->l);
        } break;

        case 0x06: {
            this->RLC(mmu, this->hl, true);
        } break;

        case 0x07: {
            this->RLC(mmu, this->a);
        } break;

        case 0x08: {
            this->RRC(mmu, this->b);
        } break;

        case 0x09: {
            this->RRC(mmu, this->c);
        } break;

        case 0x0A: {
            this->RRC(mmu, this->d);
        } break;

        case 0x0B: {
            this->RRC(mmu, this->e);
        } break;

        case 0x0C: {
            this->RRC(mmu, this->h);
        } break;

        case 0x0D: {
            this->RRC(mmu, this->l);
        } break;

        case 0x0E: {
            this->RRC(mmu, this->hl, true);
        } break;

        case 0x0F: {
            this->RRC(mmu, this->a);
        } break;

        case 0x10: {
            this->RL(mmu, this->b);
        } break;

        case 0x11: {
            this->RL(mmu, this->c);
        } break;

        case 0x12: {
            this->RL(mmu, this->d);
        } break;

        case 0x13: {
            this->RL(mmu, this->e);
        } break;

        case 0x14: {
            this->RL(mmu, this->h);
        } break;

        case 0x15: {
            this->RL(mmu, this->l);
        } break;

        case 0x16: {
            this->RL(mmu, this->hl, true);
        } break;

        case 0x17: {
            this->RL(mmu, this->a);
        } break;

        case 0x18: {
            this->RR(mmu, this->b);
        } break;

        case 0x19: {
            this->RR(mmu, this->c);
        } break;

        case 0x1A: {
            this->RR(mmu, this->d);
        } break;

        case 0x1B: {
            this->RR(mmu, this->e);
        } break;

        case 0x1C: {
            this->RR(mmu, this->h);
        } break;

        case 0x1D: {
            this->RR(mmu, this->l);
        } break;

        case 0x1E: {
            this->RR(mmu, this->hl, true);
        } break;

        case 0x1F: {
            this->RR(mmu, this->a);
        } break;

        case 0x20: {
            this->SLA(mmu, this->b);
        } break;

        case 0x21: {
            this->SLA(mmu, this->c);
        } break;

        case 0x22: {
            this->SLA(mmu, this->d);
        } break;

        case 0x23: {
            this->SLA(mmu, this->e);
        } break;

        case 0x24: {
            this->SLA(mmu, this->h);
        } break;

        case 0x25: {
            this->SLA(mmu, this->l);
        } break;

        case 0x26: {
            this->SLA(mmu, this->hl, true);
        } break;

        case 0x27: {
            this->SLA(mmu, this->a);
        } break;

        case 0x28: {
            this->SRA(mmu, this->b);
        } break;

        case 0x29: {
            this->SRA(mmu, this->c);
        } break;

        case 0x2A: {
            this->SRA(mmu, this->d);
        } break;

        case 0x2B: {
            this->SRA(mmu, this->e);
        } break;

        case 0x2C: {
            this->SRA(mmu, this->h);
        } break;

        case 0x2D: {
            this->SRA(mmu, this->l);
        } break;

        case 0x2E: {
            this->SRA(mmu, this->hl, true);
        } break;

        case 0x2F: {
            this->SRA(mmu, this->a);
        } break;

        case 0x30: {
            this->SWAP(mmu, this->b);
        } break;

        case 0x31: {
            this->SWAP(mmu, this->c);
        } break;

        case 0x32: {
            this->SWAP(mmu, this->d);
        } break;

        case 0x33: {
            this->SWAP(mmu, this->e);
        } break;

        case 0x34: {
            this->SWAP(mmu, this->h);
        } break;

        case 0x35: {
            this->SWAP(mmu, this->l);
        } break;

        case 0x36: {
            this->SWAP(mmu, this->hl, true);
        } break;

        case 0x37: {
            this->SWAP(mmu, this->a);
        } break;

        case 0x38: {
            this->SRL(mmu, this->b);
        } break;

        case 0x39: {
            this->SRL(mmu, this->c);
        } break;

        case 0x3A: {
            this->SRL(mmu, this->d);
        } break;

        case 0x3B: {
            this->SRL(mmu, this->e);
        } break;

        case 0x3C: {
            this->SRL(mmu, this->h);
        } break;

        case 0x3D: {
            this->SRL(mmu, this->l);
        } break;

        case 0x3E: {
            this->SRL(mmu, this->hl, true);
        } break;

        case 0x3F: {
            this->SRL(mmu, this->a);
        } break;

        case 0x40: {
            this->BIT(mmu, 0, this->b);
        } break;

        case 0x41: {
            this->BIT(mmu, 0, this->c);
        } break;

        case 0x42: {
            this->BIT(mmu, 0, this->d);
        } break;

        case 0x43: {
            this->BIT(mmu, 0, this->e);
        } break;

        case 0x44: {
            this->BIT(mmu, 0, this->h);
        } break;

        case 0x45: {
            this->BIT(mmu, 0, this->l);
        } break;

        case 0x46: {
            this->BIT(mmu, 0, this->hl, true);
        } break;

        case 0x47: {
            this->BIT(mmu, 0, this->a);
        } break;

        case 0x48: {
            this->BIT(mmu, 1, this->b);
        } break;

        case 0x49: {
            this->BIT(mmu, 1, this->c);
        } break;

        case 0x4A: {
            this->BIT(mmu, 1, this->d);
        } break;

        case 0x4B: {
            this->BIT(mmu, 1, this->e);
        } break;

        case 0x4C: {
            this->BIT(mmu, 1, this->h);
        } break;

        case 0x4D: {
            this->BIT(mmu, 1, this->l);
        } break;

        case 0x4E: {
            this->BIT(mmu, 1, this->hl, true);
        } break;

        case 0x4F: {
            this->BIT(mmu, 1, this->a);
        } break;

        case 0x50: {
            this->BIT(mmu, 2, this->b);
        } break;

        case 0x51: {
            this->BIT(mmu, 2, this->c);
        } break;

        case 0x52: {
            this->BIT(mmu, 2, this->d);
        } break;

        case 0x53: {
            this->BIT(mmu, 2, this->e);
        } break;

        case 0x54: {
            this->BIT(mmu, 2, this->h);
        } break;

        case 0x55: {
            this->BIT(mmu, 2, this->l);
        } break;

        case 0x56: {
            this->BIT(mmu, 2, this->hl, true);
        } break;

        case 0x57: {
            this->BIT(mmu, 2, this->a);
        } break;

        case 0x58: {
            this->BIT(mmu, 3, this->b);
        } break;

        case 0x59: {
            this->BIT(mmu, 3, this->c);
        } break;

        case 0x5A: {
            this->BIT(mmu, 3, this->d);
        } break;

        case 0x5B: {
            this->BIT(mmu, 3, this->e);
        } break;

        case 0x5C: {
            this->BIT(mmu, 3, this->h);
        } break;

        case 0x5D: {
            this->BIT(mmu, 3, this->l);
        } break;

        case 0x5E: {
            this->BIT(mmu, 3, this->hl, true);
        } break;

        case 0x5F: {
            this->BIT(mmu, 3, this->a);
        } break;

        case 0x60: {
            this->BIT(mmu, 4, this->b);
        } break;

        case 0x61: {
            this->BIT(mmu, 4, this->c);
        } break;

        case 0x62: {
            this->BIT(mmu, 4, this->d);
        } break;

        case 0x63: {
            this->BIT(mmu, 4, this->e);
        } break;

        case 0x64: {
            this->BIT(mmu, 4, this->h);
        } break;

        case 0x65: {
            this->BIT(mmu, 4, this->l);
        } break;

        case 0x66: {
            this->BIT(mmu, 4, this->hl, true);
        } break;

        case 0x67: {
            this->BIT(mmu, 4, this->a);
        } break;

        case 0x68: {
            this->BIT(mmu, 5, this->b);
        } break;

        case 0x69: {
            this->BIT(mmu, 5, this->c);
        } break;

        case 0x6A: {
            this->BIT(mmu, 5, this->d);
        } break;

        case 0x6B: {
            this->BIT(mmu, 5, this->e);
        } break;

        case 0x6C: {
            this->BIT(mmu, 5, this->h);
        } break;

        case 0x6D: {
            this->BIT(mmu, 5, this->l);
        } break;

        case 0x6E: {
            this->BIT(mmu, 5, this->hl, true);
        } break;

        case 0x6F: {
            this->BIT(mmu, 5, this->a);
        } break;

        case 0x70: {
            this->BIT(mmu, 6, this->b);
        } break;

        case 0x71: {
            this->BIT(mmu, 6, this->c);
        } break;

        case 0x72: {
            this->BIT(mmu, 6, this->d);
        } break;

        case 0x73: {
            this->BIT(mmu, 6, this->e);
        } break;

        case 0x74: {
            this->BIT(mmu, 6, this->h);
        } break;

        case 0x75: {
            this->BIT(mmu, 6, this->l);
        } break;

        case 0x76: {
            this->BIT(mmu, 6, this->hl, true);
        } break;

        case 0x77: {
            this->BIT(mmu, 6, this->a);
        } break;

        case 0x78: {
            this->BIT(mmu, 7, this->b);
        } break;

        case 0x79: {
            this->BIT(mmu, 7, this->c);
        } break;

        case 0x7A: {
            this->BIT(mmu, 7, this->d);
        } break;

        case 0x7B: {
            this->BIT(mmu, 7, this->e);
        } break;

        case 0x7C: {
            this->BIT(mmu, 7, this->h);
        } break;

        case 0x7D: {
            this->BIT(mmu, 7, this->l);
        } break;

        case 0x7E: {
            this->BIT(mmu, 7, this->hl, true);
        } break;

        case 0x7F: {
            this->BIT(mmu, 7, this->a);
        } break;

        case 0x80: {
            this->RES(mmu, 0, this->b);
        } break;

        case 0x81: {
            this->RES(mmu, 0, this->c);
        } break;

        case 0x82: {
            this->RES(mmu, 0, this->d);
        } break;

        case 0x83: {
            this->RES(mmu, 0, this->e);
        } break;

        case 0x84: {
            this->RES(mmu, 0, this->h);
        } break;

        case 0x85: {
            this->RES(mmu, 0, this->l);
        } break;

        case 0x86: {
            this->RES(mmu, 0, this->hl, true);
        } break;

        case 0x87: {
            this->RES(mmu, 0, this->a);
        } break;

        case 0x88: {
            this->RES(mmu, 1, this->b);
        } break;

        case 0x89: {
            this->RES(mmu, 1, this->c);
        } break;

        case 0x8A: {
            this->RES(mmu, 1, this->d);
        } break;

        case 0x8B: {
            this->RES(mmu, 1, this->e);
        } break;

        case 0x8C: {
            this->RES(mmu, 1, this->h);
        } break;

        case 0x8D: {
            this->RES(mmu, 1, this->l);
        } break;

        case 0x8E: {
            this->RES(mmu, 1, this->hl, true);
        } break;

        case 0x8F: {
            this->RES(mmu, 1, this->a);
        } break;

        case 0x90: {
            this->RES(mmu, 2, this->b);
        } break;

        case 0x91: {
            this->RES(mmu, 2, this->c);
        } break;

        case 0x92: {
            this->RES(mmu, 2, this->d);
        } break;

        case 0x93: {
            this->RES(mmu, 2, this->e);
        } break;

        case 0x94: {
            this->RES(mmu, 2, this->h);
        } break;

        case 0x95: {
            this->RES(mmu, 2, this->l);
        } break;

        case 0x96: {
            this->RES(mmu, 2, this->hl, true);
        } break;

        case 0x97: {
            this->RES(mmu, 2, this->a);
        } break;

        case 0x98: {
            this->RES(mmu, 3, this->b);
        } break;

        case 0x99: {
            this->RES(mmu, 3, this->c);
        } break;

        case 0x9A: {
            this->RES(mmu, 3, this->d);
        } break;

        case 0x9B: {
            this->RES(mmu, 3, this->e);
        } break;

        case 0x9C: {
            this->RES(mmu, 3, this->h);
        } break;

        case 0x9D: {
            this->RES(mmu, 3, this->l);
        } break;

        case 0x9E: {
            this->RES(mmu, 3, this->hl, true);
        } break;

        case 0x9F: {
            this->RES(mmu, 3, this->a);
        } break;

        case 0xA0: {
            this->RES(mmu, 4, this->b);
        } break;

        case 0xA1: {
            this->RES(mmu, 4, this->c);
        } break;

        case 0xA2: {
            this->RES(mmu, 4, this->d);
        } break;

        case 0xA3: {
            this->RES(mmu, 4, this->e);
        } break;

        case 0xA4: {
            this->RES(mmu, 4, this->h);
        } break;

        case 0xA5: {
            this->RES(mmu, 4, this->l);
        } break;

        case 0xA6: {
            this->RES(mmu, 4, this->hl, true);
        } break;

        case 0xA7: {
            this->RES(mmu, 4, this->a);
        } break;

        case 0xA8: {
            this->RES(mmu, 5, this->b);
        } break;

        case 0xA9: {
            this->RES(mmu, 5, this->c);
        } break;

        case 0xAA: {
            this->RES(mmu, 5, this->d);
        } break;

        case 0xAB: {
            this->RES(mmu, 5, this->e);
        } break;

        case 0xAC: {
            this->RES(mmu, 5, this->h);
        } break;

        case 0xAD: {
            this->RES(mmu, 5, this->l);
        } break;

        case 0xAE: {
            this->RES(mmu, 5, this->hl, true);
        } break;

        case 0xAF: {
            this->RES(mmu, 5, this->a);
        } break;

        case 0xB0: {
            this->RES(mmu, 6, this->b);
        } break;

        case 0xB1: {
            this->RES(mmu, 6, this->c);
        } break;

        case 0xB2: {
            this->RES(mmu, 6, this->d);
        } break;

        case 0xB3: {
            this->RES(mmu, 6, this->e);
        } break;

        case 0xB4: {
            this->RES(mmu, 6, this->h);
        } break;

        case 0xB5: {
            this->RES(mmu, 6, this->l);
        } break;

        case 0xB6: {
            this->RES(mmu, 6, this->hl, true);
        } break;

        case 0xB7: {
            this->RES(mmu, 6, this->a);
        } break;

        case 0xB8: {
            this->RES(mmu, 7, this->b);
        } break;

        case 0xB9: {
            this->RES(mmu, 7, this->c);
        } break;

        case 0xBA: {
            this->RES(mmu, 7, this->d);
        } break;

        case 0xBB: {
            this->RES(mmu, 7, this->e);
        } break;

        case 0xBC: {
            this->RES(mmu, 7, this->h);
        } break;

        case 0xBD: {
            this->RES(mmu, 7, this->l);
        } break;

        case 0xBE: {
            this->RES(mmu, 7, this->hl, true);
        } break;

        case 0xBF: {
            this->RES(mmu, 7, this->a);
        } break;

        case 0xC0: {
            this->SET(mmu, 0, this->b);
        } break;

        case 0xC1: {
            this->SET(mmu, 0, this->c);
        } break;

        case 0xC2: {
            this->SET(mmu, 0, this->d);
        } break;

        case 0xC3: {
            this->SET(mmu, 0, this->e);
        } break;

        case 0xC4: {
            this->SET(mmu, 0, this->h);
        } break;

        case 0xC5: {
            this->SET(mmu, 0, this->l);
        } break;

        case 0xC6: {
            this->SET(mmu, 0, this->hl, true);
        } break;

        case 0xC7: {
            this->SET(mmu, 0, this->a);
        } break;

        case 0xC8: {
            this->SET(mmu, 1, this->b);
        } break;

        case 0xC9: {
            this->SET(mmu, 1, this->c);
        } break;

        case 0xCA: {
            this->SET(mmu, 1, this->d);
        } break;

        case 0xCB: {
            this->SET(mmu, 1, this->e);
        } break;

        case 0xCC: {
            this->SET(mmu, 1, this->h);
        } break;

        case 0xCD: {
            this->SET(mmu, 1, this->l);
        } break;

        case 0xCE: {
            this->SET(mmu, 1, this->hl, true);
        } break;

        case 0xCF: {
            this->SET(mmu, 1, this->a);
        } break;

        case 0xD0: {
            this->SET(mmu, 2, this->b);
        } break;

        case 0xD1: {
            this->SET(mmu, 2, this->c);
        } break;

        case 0xD2: {
            this->SET(mmu, 2, this->d);
        } break;

        case 0xD3: {
            this->SET(mmu, 2, this->e);
        } break;

        case 0xD4: {
            this->SET(mmu, 2, this->h);
        } break;

        case 0xD5: {
            this->SET(mmu, 2, this->l);
        } break;

        case 0xD6: {
            this->SET(mmu, 2, this->hl, true);
        } break;

        case 0xD7: {
            this->SET(mmu, 2, this->a);
        } break;

        case 0xD8: {
            this->SET(mmu, 3, this->b);
        } break;

        case 0xD9: {
            this->SET(mmu, 3, this->c);
        } break;

        case 0xDA: {
            this->SET(mmu, 3, this->d);
        } break;

        case 0xDB: {
            this->SET(mmu, 3, this->e);
        } break;

        case 0xDC: {
            this->SET(mmu, 3, this->h);
        } break;

        case 0xDD: {
            this->SET(mmu, 3, this->l);
        } break;

        case 0xDE: {
            this->SET(mmu, 3, this->hl, true);
        } break;

        case 0xDF: {
            this->SET(mmu, 3, this->a);
        } break;

        case 0xE0: {
            this->SET(mmu, 4, this->b);
        } break;

        case 0xE1: {
            this->SET(mmu, 4, this->c);
        } break;

        case 0xE2: {
            this->SET(mmu, 4, this->d);
        } break;

        case 0xE3: {
            this->SET(mmu, 4, this->e);
        } break;

        case 0xE4: {
            this->SET(mmu, 4, this->h);
        } break;

        case 0xE5: {
            this->SET(mmu, 4, this->l);
        } break;

        case 0xE6: {
            this->SET(mmu, 4, this->hl, true);
        } break;

        case 0xE7: {
            this->SET(mmu, 4, this->a);
        } break;

        case 0xE8: {
            this->SET(mmu, 5, this->b);
        } break;

        case 0xE9: {
            this->SET(mmu, 5, this->c);
        } break;

        case 0xEA: {
            this->SET(mmu, 5, this->d);
        } break;

        case 0xEB: {
            this->SET(mmu, 5, this->e);
        } break;

        case 0xEC: {
            this->SET(mmu, 5, this->h);
        } break;

        case 0xED: {
            this->SET(mmu, 5, this->l);
        } break;

        case 0xEE: {
            this->SET(mmu, 5, this->hl, true);
        } break;

        case 0xEF: {
            this->SET(mmu, 5, this->a);
        } break;

        case 0xF0: {
            this->SET(mmu, 6, this->b);
        } break;

        case 0xF1: {
            this->SET(mmu, 6, this->c);
        } break;

        case 0xF2: {
            this->SET(mmu, 6, this->d);
        } break;

        case 0xF3: {
            this->SET(mmu, 6, this->e);
        } break;

        case 0xF4: {
            this->SET(mmu, 6, this->h);
        } break;

        case 0xF5: {
            this->SET(mmu, 6, this->l);
        } break;

        case 0xF6: {
            this->SET(mmu, 6, this->hl, true);
        } break;

        case 0xF7: {
            this->SET(mmu, 6, this->a);
        } break;

        case 0xF8: {
            this->SET(mmu, 7, this->b);
        } break;

        case 0xF9: {
            this->SET(mmu, 7, this->c);
        } break;

        case 0xFA: {
            this->SET(mmu, 7, this->d);
        } break;

        case 0xFB: {
            this->SET(mmu, 7, this->e);
        } break;

        case 0xFC: {
            this->SET(mmu, 7, this->h);
        } break;

        case 0xFD: {
            this->SET(mmu, 7, this->l);
        } break;

        case 0xFE: {
            this->SET(mmu, 7, this->hl, true);
        } break;

        case 0xFF: {
            this->SET(mmu, 7, this->a);
        } break;
        }
    }

    bool cpu::_getflag(cpu_flag flag)
    {
        return (this->f & static_cast<std::uint8_t>(flag)) == static_cast<std::uint8_t>(flag);
    }

    void cpu::_setflag(cpu_flag flag, bool value)
    {
        if (value)
        {
            this->f |= static_cast<std::uint8_t>(flag);
        }
        else
        {
            this->f &= ~(static_cast<std::uint8_t>(flag));
        }
    }

    void cpu::_jump(std::uint16_t address)
    {
        this->pc = address;
    }

    void cpu::_jump_relative(std::int8_t offset)
    {
        this->pc += offset;
    }

    void cpu::NOP(mmu &)
    {
    }

    void cpu::_call(mmu &mmu, std::uint16_t address)
    {
        this->_push(mmu, this->pc);
        this->_jump(address);
    }

    void cpu::_ret(mmu &mmu)
    {
        auto address = this->_pop(mmu);

        this->_jump(address);
    }

    void cpu::_push(mmu &mmu, std::uint16_t value)
    {
        hi_lo pair(value);

        mmu.write(this->sp, pair.hi);
        this->sp -= 1;
        mmu.write(this->sp, pair.lo);
        this->sp -= 1;
    }

    std::uint16_t cpu::_pop(mmu &mmu)
    {
        this->sp += 1;

        auto hi = mmu.read(this->sp);

        mmu.write(this->sp, 0x00);

        this->sp += 1;

        auto lo = mmu.read(this->sp);
        mmu.write(this->sp, 0x00);

        return hi_lo(hi, lo).value;
    }

    void cpu::STOP(mmu &mmu)
    {
        auto n8 = this->_read_byte(mmu);

        if (n8 != 0) {
            // throw; // TODO throw real exception
        }
    }

    void cpu::JR(mmu &mmu, bool condition)
    {
        auto e8 = this->_read_byte(mmu);

        if (condition)
        {
            this->_jump_relative(static_cast<int8_t>(e8));
        }
    }

    void cpu::LD(mmu &mmu, std::uint16_t &reg)
    {
        auto n16 = this->_read_word(mmu);

        reg = n16;
    }

    void cpu::LD(mmu &mmu, std::uint16_t out_reg, std::uint8_t in_reg)
    {
        mmu.write(out_reg, in_reg);
    }

    void cpu::INC(mmu &, std::uint16_t &reg)
    {
        reg += 1;
    }

    void cpu::INC(mmu &, std::uint8_t &reg)
    {
        std::uint8_t orig = reg;

        reg += 1;

        this->_setflag(cpu_flag::z, reg == 0);
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, (orig & 0x0F) == 0x0F);
    }

    void cpu::DEC(mmu &, std::uint8_t &reg)
    {
        std::uint8_t orig = reg;

        reg -= 1;

        this->_setflag(cpu_flag::z, (reg == 0));
        this->_setflag(cpu_flag::n, true);
        this->_setflag(cpu_flag::h, (orig & 0x0F) == 0);
    }

    void cpu::LD(mmu& mmu, std::uint8_t &reg)
    {
        auto val = this->_read_byte(mmu);

        reg = val;
    }

    void cpu::RLCA(mmu &mmu)
    {
        this->RLC(mmu, this->a);

        this->_setflag(cpu_flag::z, false);
    }

    void cpu::RLA(mmu &mmu)
    {
        this->RL(mmu, this->a);

        this->_setflag(cpu_flag::z, false);
    }

    void cpu::DAA(mmu &)
    {
        // TODO decimal adjust accumulator

        this->_setflag(cpu_flag::z, this->a == 0);
        this->_setflag(cpu_flag::h, false);
        // this->_setflag(cpu_flag::c, false); // TODO set carry flag
    }

    void cpu::SCF(mmu &)
    {
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, true);
    }

    void cpu::ADD(mmu &, std::uint16_t &reg_out, std::uint16_t reg_in)
    {
        std::uint16_t orig_out = reg_out;

        reg_out += reg_in;

        this->_setflag(cpu_flag::n, false);
        // this->_setflag(cpu_flag::h, false); // TODO set half-carry flag
        this->_setflag(cpu_flag::c, reg_in > (0xFFFF - orig_out) );
    }

    void cpu::LD(mmu &mmu, std::uint8_t &reg_out, std::uint16_t reg_in)
    {
        auto val = mmu.read(reg_in);

        reg_out = val;
    }

    void cpu::DEC(mmu &, std::uint16_t &reg)
    {
        reg -= 1;
    }

    void cpu::RRCA(mmu &mmu)
    {
        this->RRC(mmu, this->a);

        this->_setflag(cpu_flag::z, false);
    }

    void cpu::RRA(mmu &mmu)
    {
        this->RR(mmu, this->a);

        this->_setflag(cpu_flag::z, false);
    }

    void cpu::CPL(mmu &)
    {
        this->a = ~this->a;

        this->_setflag(cpu_flag::n, true);
        this->_setflag(cpu_flag::h, true);
    }

    void cpu::CCF(mmu &)
    {
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, !this->_getflag(cpu_flag::c));
    }

    void cpu::LD(mmu &, std::uint8_t &reg_out, std::uint8_t reg_in)
    {
        reg_out = reg_in;
    }

    void cpu::HALT(mmu &)
    {
        // TODO implement HALT
    }

    void cpu::ADD(mmu &, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        std::uint8_t orig_out = reg_out;

        reg_out += reg_in;

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        // this->_setflag(cpu_flag::h, false); // TODO set half-carry flag
        this->_setflag(cpu_flag::c, reg_in > (0xFFFF - orig_out));
    }

    void cpu::SUB(mmu &, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        std::uint8_t orig_out = reg_out;

        reg_out -= reg_in;

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, true);
        // this->_setflag(cpu_flag::h, false); // TODO set half-carry flag
        this->_setflag(cpu_flag::c, reg_in > orig_out);
    }

    void cpu::AND(mmu &, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out &= reg_in;

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, true);
        this->_setflag(cpu_flag::c, false);
    }

    void cpu::OR(mmu &, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out |= reg_in;

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, false);
    }

    void cpu::ADC(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        this->ADD(mmu, reg_out, reg_in + (this->_getflag(cpu_flag::c) ? 1 : 0));
    }

    void cpu::SBC(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        this->SUB(mmu, reg_out, reg_in + (this->_getflag(cpu_flag::c) ? 1 : 0));
    }

    void cpu::XOR(mmu &, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out ^= reg_in;

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, false);
    }

    void cpu::CP(mmu &mmu, std::uint8_t const &reg_out, std::uint8_t reg_in)
    {
        std::uint8_t orig_out = reg_out;

        this->SUB(mmu, orig_out, reg_in);
    }

    void cpu::RET(mmu &mmu, bool condition)
    {
        if (condition)
        {
            this->_ret(mmu);

            if (this->stack.empty())
            {
                std::cerr << "Erroneous RET instruction: empty stack" << std::endl;
            }
            else if (this->stack.top().type != element_type::Call)
            {
                std::cerr << "Erroneous RET instruction: not a CALL" << std::endl;
            }

            this->stack.pop();
        }
    }

    void cpu::POP(mmu &mmu, std::uint16_t& reg_out)
    {
        reg_out = this->_pop(mmu);

        if (this->stack.empty())
        {
            std::cerr << "Erroneous POP instruction: empty stack" << std::endl;
        }
        else if (this->stack.top().type != element_type::Push)
        {
            std::cerr << "Erroneous POP instruction: not a PUSH" << std::endl;
        }

        this->stack.pop();
    }

    void cpu::JP(mmu &mmu, bool condition)
    {
        auto a16 = this->_read_word(mmu);

        if (condition)
        {
            this->_jump(a16);
        }
    }

    void cpu::ILLEGAL(mmu &, std::uint8_t code)
    {
        throw_located(illegal_instruction_exception(code));
    }

    void cpu::DI(mmu &mmu)
    {
        mmu.write(0xFFFF, 0x00); // TODO delay IME = 0 by one instruction
        // mmu[0xFFFF] = 0x00;
    }

    void cpu::CALL(mmu &mmu, bool condition)
    {
        auto a16 = this->_read_word(mmu);

        if (condition)
        {
            this->_call(mmu, a16);
            this->stack.push({a16, element_type::Call});
        }
    }

    void cpu::PUSH(mmu &mmu, std::uint16_t reg)
    {
        this->_push(mmu, reg);
        this->stack.push({reg, element_type::Push});
    }

    void cpu::ADD(mmu &mmu, std::uint8_t& reg_out)
    {
        this->ADD(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::SUB(mmu &mmu, std::uint8_t& reg_out)
    {
        this->SUB(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::AND(mmu &mmu, std::uint8_t& reg_out)
    {
        this->AND(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::OR(mmu &mmu, std::uint8_t& reg_out)
    {
        this->OR(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::RST(mmu &mmu, std::uint8_t code)
    {
        this->_call(mmu, code);
    }

    void cpu::ADD(mmu &mmu, std::uint16_t& reg_out)
    {
        auto e8 = this->_read_byte(mmu);

        reg_out += e8;

        // TODO adjust flags
    }

    void cpu::RETI(mmu &mmu)
    {
        this->RET(mmu, true);
        this->EI(mmu);
    }

    void cpu::JP(mmu &, std::uint16_t reg)
    {
        this->pc = reg;
    }

    void cpu::LD(mmu &, std::uint16_t &reg_out, std::uint16_t reg_in)
    {
        reg_out = reg_in;
    }

    void cpu::PREFIX(mmu &mmu)
    {
        auto code = this->_read_byte(mmu);

        this->_handle_instruction_prefix(mmu, code);
    }

    void cpu::EI(mmu &mmu)
    {
        mmu.write(0xFFFF, 0x01); // TODO delay IME = 1 by one instruction
        // mmu[0xFFFF] = 0x01;
    }

    void cpu::ADC(mmu &mmu, std::uint8_t& reg_out)
    {
        this->ADC(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::SBC(mmu &mmu, std::uint8_t& reg_out)
    {
        this->SBC(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::XOR(mmu &mmu, std::uint8_t& reg_out)
    {
        this->XOR(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::CP(mmu &mmu, std::uint8_t& reg_out)
    {
        this->CP(mmu, reg_out, this->_read_byte(mmu));
    }

    void cpu::RLC(mmu &, [[maybe_unused]] std::uint8_t& reg_out)
    {
        std::uint8_t orig = reg_out;

        reg_out <<= 1;

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, (orig & (1 << 7)) != 0);
    }

    void cpu::RRC(mmu &, std::uint8_t& reg_out)
    {
        std::uint8_t orig = reg_out;

        reg_out >>= 1;

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, (orig & (1 << 0)) != 0);
    }

    void cpu::RL(mmu &, std::uint8_t& reg_out)
    {
        std::uint8_t carry_bit = this->_getflag(cpu_flag::c) ? 1 << 0 : 0;
        std::uint8_t orig = reg_out;

        reg_out = carry_bit | (orig << 1);

        this->_setflag(cpu_flag::z, reg_out == 0);
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, (orig & (1 << 7)) != 0);
    }

    void cpu::RR(mmu &, std::uint8_t& reg_out)
    {
        std::uint8_t carry_bit = this->_getflag(cpu_flag::c) ? 1 << 7 : 0;
        std::uint8_t orig = reg_out;

        reg_out = carry_bit | (orig >> 1);

        this->_setflag(cpu_flag::z, reg_out == 0);
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, (orig & (1 << 0)) != 0);
    }

    void cpu::SLA(mmu &, std::uint8_t& reg_out)
    {
        auto orig = reg_out;

        // shift left
        // set bit 0 to 0
        reg_out = (orig << 1) & ~(1 << 0);

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, (orig & (1 << 7)) != 0);
    }

    void cpu::SRA(mmu &, std::uint8_t& reg_out)
    {
        auto orig = reg_out;

        // set bit 7 to 0
        // shift right
        // set bit 7 to old bit 7
        reg_out = ((orig & ~(1 << 7)) >> 1) | (orig & (1 << 7));

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, (orig & (1 << 0)) != 0);
    }

    void cpu::SWAP(mmu &, std::uint8_t& reg_out)
    {
        auto orig = reg_out;

        reg_out = (orig >> 4) | (orig << 4);

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, false);
    }

    void cpu::SRL(mmu &, std::uint8_t& reg_out)
    {
        auto orig = reg_out;

        // shift right
        // set bit 7 to 0
        reg_out = (orig >> 1) & ~(1 << 7);

        this->_setflag(cpu_flag::z, (reg_out == 0));
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, false);
        this->_setflag(cpu_flag::c, (orig & (1 << 0)) != 0);
    }

    void cpu::BIT(mmu &, std::uint8_t bit, std::uint8_t& reg_out)
    {
        this->_setflag(cpu_flag::z, (reg_out & (1 << bit)) == 0);
        this->_setflag(cpu_flag::n, false);
        this->_setflag(cpu_flag::h, true);
    }

    void cpu::RES(mmu &, std::uint8_t bit, std::uint8_t& reg_out)
    {
        reg_out &= ~(1 << bit);
    }

    void cpu::SET(mmu &, std::uint8_t bit, std::uint8_t& reg_out)
    {
        reg_out |= (1 << bit);
    }

    void cpu::INC(mmu& mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->INC(mmu, n);

        mmu.write(address, n);
    }

    void cpu::DEC(mmu& mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->DEC(mmu, n);

        mmu.write(address, n);
    }

    void cpu::LD(mmu& mmu, std::uint16_t address, bool)
    {
        auto n = this->_read_byte(mmu);

        this->LD(mmu, n);

        mmu.write(address, n);
    }

    void cpu::RLC(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->RLC(mmu, n);

        mmu.write(address, n);
    }

    void cpu::RRC(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->RRC(mmu, n);

        mmu.write(address, n);
    }

    void cpu::RL(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->RL(mmu, n);

        mmu.write(address, n);
    }

    void cpu::RR(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->RR(mmu, n);

        mmu.write(address, n);
    }

    void cpu::SLA(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->SLA(mmu, n);

        mmu.write(address, n);
    }

    void cpu::SRA(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->SRA(mmu, n);

        mmu.write(address, n);
    }

    void cpu::SWAP(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->SWAP(mmu, n);

        mmu.write(address, n);
    }

    void cpu::SRL(mmu &mmu, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->SRL(mmu, n);

        mmu.write(address, n);
    }

    void cpu::BIT(mmu &mmu, std::uint8_t bit, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->BIT(mmu, bit, n);

        mmu.write(address, n);
    }

    void cpu::RES(mmu &mmu, std::uint8_t bit, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->RES(mmu, bit, n);

        mmu.write(address, n);
    }

    void cpu::SET(mmu &mmu, std::uint8_t bit, std::uint16_t address, bool)
    {
        auto n = mmu.read(address);

        this->SET(mmu, bit, n);

        mmu.write(address, n);
    }
}

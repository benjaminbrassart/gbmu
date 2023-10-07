/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cpu.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:27:35 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/07 16:34:01 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/cpu.hpp"
#include "gbmu/mmu.hpp"

#include <iostream>
#include <stdexcept>

namespace gbmu
{
    union hi_lo
    {
        std::uint16_t value;
        struct
        {
            std::uint8_t hi;
            std::uint8_t lo;
        };

        hi_lo(std::uint16_t val) : value(val) {}
        hi_lo(std::uint8_t low, std::uint8_t high) : hi(high), lo(low) {}
    };

    cpu::cpu() :
        a(0x00),
        f(0x00),
        b(0x00),
        c(0x00),
        d(0x00),
        e(0x00),
        h(0x00),
        l(0x00),
        pc(0x0100),
        sp(0xFFFE)
    {
    }

    cpu::~cpu() = default;

    void cpu::step(mmu &mmu)
    {
        std::uint8_t code = this->_read_byte(mmu);

        std::cout << static_cast<int>(code) << std::endl;

        this->_handle_instruction(mmu, code);
    }

    std::uint8_t cpu::_read_byte(mmu &mmu)
    {
        std::uint8_t b = mmu[this->pc];

        this->pc += 1;

        return b;
    }

    std::uint16_t cpu::_read_word(mmu &mmu)
    {
        hi_lo pair(this->_read_byte(mmu), this->_read_byte(mmu));

        return pair.value;
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
            this->JR(mmu, !this->flags.z);
        } break;

        case 0x30: {
            this->JR(mmu, !this->flags.c);
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
            this->INC(mmu, mmu[this->hl]);
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
            this->DEC(mmu, mmu[this->hl]);
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
            this->LD(mmu, mmu[this->hl]);
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

        case 0x08: {
            // TODO LD [a16] SP
        } break;

        case 0x18: {
            this->JR(mmu, true);
        } break;

        case 0x28: {
            this->JR(mmu, this->flags.z);
        } break;

        case 0x38: {
            this->JR(mmu, this->flags.c);
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
            this->LD(mmu, mmu[this->hl], this->b);
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
            this->LD(mmu, mmu[this->hl], this->c);
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
            this->LD(mmu, mmu[this->hl], this->d);
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
            this->LD(mmu, mmu[this->hl], this->e);
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
            this->LD(mmu, mmu[this->hl], this->h);
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
            this->LD(mmu, mmu[this->hl], this->l);
        } break;

        case 0x46: {
            this->LD(mmu, this->b, mmu[this->hl]);
        } break;

        case 0x56: {
            this->LD(mmu, this->d, mmu[this->hl]);
        } break;

        case 0x66: {
            this->LD(mmu, this->h, mmu[this->hl]);
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
            this->LD(mmu, mmu[this->hl], this->a);
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
            this->LD(mmu, this->c, mmu[this->hl]);
        } break;

        case 0x5E: {
            this->LD(mmu, this->e, mmu[this->hl]);
        } break;

        case 0x6E: {
            this->LD(mmu, this->l, mmu[this->hl]);
        } break;

        case 0x7E: {
            this->LD(mmu, this->a, mmu[this->hl]);
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
            this->ADD(mmu, this->a, mmu[this->hl]);
        } break;

        case 0x96: {
            this->SUB(mmu, this->a, mmu[this->hl]);
        } break;

        case 0xA6: {
            this->AND(mmu, this->a, mmu[this->hl]);
        } break;

        case 0xB6: {
            this->OR(mmu, this->a, mmu[this->hl]);
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
            this->ADC(mmu, this->a, mmu[this->hl]);
        } break;

        case 0x9E: {
            this->SBC(mmu, this->a, mmu[this->hl]);
        } break;

        case 0xAE: {
            this->XOR(mmu, this->a, mmu[this->hl]);
        } break;

        case 0xBE: {
            this->CP(mmu, this->a, mmu[this->hl]);
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
            this->RET(mmu, !this->flags.z);
        } break;

        case 0xD0: {
            this->RET(mmu, !this->flags.c);
        } break;

        case 0xE0: {
            // TODO
        } break;

        case 0xF0: {
            // TODO
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
            this->JP(mmu, !this->flags.z);
        } break;

        case 0xD2: {
            this->JP(mmu, !this->flags.c);
        } break;

        case 0xE2: {
            // TODO
        } break;

        case 0xF2: {
            // TODO
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
            this->CALL(mmu, !this->flags.z);
        } break;

        case 0xD4: {
            this->CALL(mmu, !this->flags.c);
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
            this->RET(mmu, this->flags.z);
        } break;

        case 0xD8: {
            this->RET(mmu, this->flags.z);
        } break;

        case 0xE8: {
            this->ADD(mmu, this->sp);
        } break;

        case 0xF8: {
            // TODO LD HL, SP + e8
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
            this->JP(mmu, this->flags.z);
        } break;

        case 0xDA: {
            this->JP(mmu, this->flags.c);
        } break;

        case 0xEA: {
            // TODO LD [a16] A
        } break;

        case 0xFA: {
            // TODO LD A [a16]
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
            this->CALL(mmu, this->flags.z);
        } break;

        case 0xDC: {
            this->CALL(mmu, this->flags.c);
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
            this->RLC(mmu, mmu[this->hl]);
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
            this->RRC(mmu, mmu[this->hl]);
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
            this->RL(mmu, mmu[this->hl]);
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
            this->RR(mmu, mmu[this->hl]);
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
            this->SLA(mmu, mmu[this->hl]);
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
            this->SRA(mmu, mmu[this->hl]);
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
            this->SWAP(mmu, mmu[this->hl]);
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
            this->SRL(mmu, mmu[this->hl]);
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
            this->BIT(mmu, 0, mmu[this->hl]);
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
            this->BIT(mmu, 1, mmu[this->hl]);
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
            this->BIT(mmu, 2, mmu[this->hl]);
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
            this->BIT(mmu, 3, mmu[this->hl]);
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
            this->BIT(mmu, 4, mmu[this->hl]);
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
            this->BIT(mmu, 5, mmu[this->hl]);
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
            this->BIT(mmu, 6, mmu[this->hl]);
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
            this->BIT(mmu, 7, mmu[this->hl]);
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
            this->RES(mmu, 0, mmu[this->hl]);
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
            this->RES(mmu, 1, mmu[this->hl]);
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
            this->RES(mmu, 2, mmu[this->hl]);
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
            this->RES(mmu, 3, mmu[this->hl]);
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
            this->RES(mmu, 4, mmu[this->hl]);
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
            this->RES(mmu, 5, mmu[this->hl]);
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
            this->RES(mmu, 6, mmu[this->hl]);
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
            this->RES(mmu, 7, mmu[this->hl]);
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
            this->SET(mmu, 0, mmu[this->hl]);
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
            this->SET(mmu, 1, mmu[this->hl]);
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
            this->SET(mmu, 2, mmu[this->hl]);
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
            this->SET(mmu, 3, mmu[this->hl]);
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
            this->SET(mmu, 4, mmu[this->hl]);
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
            this->SET(mmu, 5, mmu[this->hl]);
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
            this->SET(mmu, 6, mmu[this->hl]);
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
            this->SET(mmu, 7, mmu[this->hl]);
        } break;

        case 0xFF: {
            this->SET(mmu, 7, this->a);
        } break;
        }
    }

    void cpu::_jump(std::uint16_t address)
    {
        this->pc = address;
    }

    void cpu::_jump_relative(std::uint8_t offset)
    {
        this->pc += offset;
    }

    void cpu::NOP([[maybe_unused]] mmu &mmu)
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

        this->sp -= 1;
        mmu[this->sp] = pair.hi;
        this->sp -= 1;
        mmu[this->sp] = pair.lo;
    }

    std::uint16_t cpu::_pop(mmu &mmu)
    {
        auto lo = mmu[this->sp];

        this->sp -= 1;

        auto hi = mmu[this->sp];

        this->sp -= 1;

        hi_lo pair(hi, lo);

        return pair.value;
    }

    void cpu::STOP(mmu &mmu)
    {
        auto n8 = this->_read_byte(mmu);

        if (n8 != 0) {
            throw; // TODO
        }
    }

    void cpu::JR(mmu &mmu, bool condition)
    {
        auto e8 = this->_read_byte(mmu);

        if (condition)
        {
            this->_jump_relative(e8);
        }
    }

    void cpu::LD(mmu &mmu, std::uint16_t &reg)
    {
        auto n16 = this->_read_word(mmu);

        reg = n16;
    }

    void cpu::LD(mmu &mmu, std::uint16_t out_reg, std::uint8_t in_reg)
    {
        mmu[out_reg] = in_reg;
    }

    void cpu::INC([[maybe_unused]] mmu& mmu, std::uint16_t &reg)
    {
        reg += 1;
    }

    void cpu::INC([[maybe_unused]] mmu& mmu, std::uint8_t &reg)
    {
        reg += 1;

        this->flags.z = (reg == 0);
        this->flags.n = false;
        // this->flags.h = true; // TODO
    }

    void cpu::DEC([[maybe_unused]] mmu& mmu, std::uint8_t &reg)
    {
        reg -= 1;

        this->flags.z = (reg == 0);
        this->flags.n = true;
        // this->flags.h = true; // TODO
    }

    void cpu::LD(mmu& mmu, std::uint8_t &reg)
    {
        auto val = this->_read_byte(mmu);

        reg = val;
    }

    void cpu::RLCA([[maybe_unused]] mmu &mmu)
    {
        // TODO

        this->flags.z = false;
        this->flags.n = false;
        this->flags.h = false;
        // this->flags.c = false; // TODO
    }

    void cpu::RLA([[maybe_unused]] mmu &mmu)
    {
        // TODO

        this->flags.z = false;
        this->flags.n = false;
        this->flags.h = false;
        // this->flags.c = false; // TODO
    }

    void cpu::DAA([[maybe_unused]] mmu &mmu)
    {
        // TODO

        // this->flags.z = false; // TODO
        this->flags.h = false;
        // this->flags.c = false; // TODO
    }

    void cpu::SCF([[maybe_unused]] mmu &mmu)
    {
        // TODO

        this->flags.n = false;
        this->flags.h = false;
        this->flags.c = true;
    }

    void cpu::ADD([[maybe_unused]] mmu& mmu, std::uint16_t &reg_out, std::uint16_t reg_in)
    {
        reg_out += reg_in;

        this->flags.n = false;
        // this->flags.h = false; // TODO
        // this->flags.c = false; // TODO
    }

    void cpu::LD(mmu &mmu, std::uint8_t &reg_out, std::uint16_t reg_in)
    {
        auto val = mmu[reg_in];

        reg_out = val;
    }

    void cpu::DEC([[maybe_unused]] mmu& mmu, std::uint16_t &reg)
    {
        reg -= 1;
    }

    void cpu::RRCA([[maybe_unused]] mmu &mmu)
    {
        // TODO

        this->flags.z = false;
        this->flags.n = false;
        this->flags.h = false;
        // this->flags.c = false; // TODO
    }

    void cpu::RRA([[maybe_unused]] mmu &mmu)
    {
        // TODO

        this->flags.z = false;
        this->flags.n = false;
        this->flags.h = false;
        // this->flags.c = false; // TODO
    }

    void cpu::CPL([[maybe_unused]] mmu &mmu)
    {
        // TODO

        this->flags.n = true;
        this->flags.h = true;
    }

    void cpu::CCF([[maybe_unused]] mmu &mmu)
    {
        // TODO

        this->flags.n = false;
        this->flags.h = false;
        // this->flags.c = false; // TODO
    }

    void cpu::LD([[maybe_unused]] mmu &mmu, std::uint8_t &reg_out, std::uint8_t reg_in)
    {
        reg_out = reg_in;
    }

    void cpu::HALT([[maybe_unused]] mmu &mmu)
    {
        // TODO
    }

    void cpu::ADD([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out += reg_in;

        // TODO compute flags
    }

    void cpu::SUB([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out -= reg_in;

        // TODO compute flags
    }

    void cpu::AND([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out &= reg_in;

        // TODO compute flags
    }

    void cpu::OR([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out |= reg_in;

        // TODO compute flags
    }

    void cpu::ADC([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out += reg_in; // TODO

        // TODO compute flags
    }

    void cpu::SBC([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out -= reg_in; // TODO

        // TODO compute flags
    }

    void cpu::XOR([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out ^= reg_in;

        // TODO compute flags
    }

    void cpu::CP([[maybe_unused]] mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in)
    {
        reg_out = ~reg_in;

        // TODO compute flags
    }

    void cpu::RET(mmu &mmu, bool condition)
    {
        if (condition)
        {
            this->_ret(mmu);
        }
    }

    void cpu::POP(mmu &mmu, std::uint16_t& reg_out)
    {
        reg_out = this->_pop(mmu);
    }

    void cpu::JP(mmu &mmu, bool condition)
    {
        auto a16 = this->_read_word(mmu);

        if (condition)
        {
            this->_jump(a16);
        }
    }

    void cpu::ILLEGAL([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t code)
    {
        throw; // TODO
    }

    void cpu::DI(mmu &mmu)
    {
        mmu[0xFFFF] = 0x00;
    }

    void cpu::CALL(mmu &mmu, bool condition)
    {
        auto a16 = this->_read_word(mmu);

        if (condition)
        {
            this->_jump(a16);
        }
    }

    void cpu::PUSH(mmu &mmu, std::uint16_t reg)
    {
        this->_push(mmu, reg);
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
    }

    void cpu::RETI(mmu &mmu)
    {
        this->RET(mmu, true);
        this->EI(mmu);
    }

    void cpu::JP([[maybe_unused]] mmu &mmu, std::uint16_t reg)
    {
        this->pc = reg;
    }

    void cpu::LD([[maybe_unused]] mmu &mmu, std::uint16_t &reg_out, std::uint16_t reg_in)
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
        mmu[0xFFFF] = 0x01;
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

    void cpu::RLC([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::RRC([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::RL([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::RR([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::SLA([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::SRA([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::SWAP([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::SRL([[maybe_unused]] mmu &mmu, [[maybe_unused]] std::uint8_t& reg_out)
    {
    }

    void cpu::BIT([[maybe_unused]] mmu &mmu, std::uint8_t bit, std::uint8_t& reg_out)
    {
        this->flags.z = (reg_out & (1 << bit)) == 0;
        this->flags.n = false;
        this->flags.h = true;
    }

    void cpu::RES([[maybe_unused]] mmu &mmu, std::uint8_t bit, std::uint8_t& reg_out)
    {
        reg_out &= ~(1 << bit);
    }

    void cpu::SET([[maybe_unused]] mmu &mmu, std::uint8_t bit, std::uint8_t& reg_out)
    {
        reg_out |= (1 << bit);
    }

}

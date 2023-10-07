/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cpu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:22:32 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/07 16:33:48 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cstdint>

namespace gbmu
{
    class mmu;

    class cpu
    {
    public:
        union {
            std::uint16_t af;
            struct {
                std::uint8_t a;
                union {
                    std::uint8_t f;
                    struct {
                    public:
                        bool z: 1;
                        bool n: 1;
                        bool h: 1;
                        bool c: 1;
                    private:
                        std::uint8_t: 4;
                    } flags;
                };
            };
        };
        union {
            std::uint16_t bc;
            struct {
                std::uint8_t b;
                std::uint8_t c;
            };
        };
        union {
            std::uint16_t de;
            struct {
                std::uint8_t d;
                std::uint8_t e;
            };
        };
        union {
            std::uint16_t hl;
            struct {
                std::uint8_t h;
                std::uint8_t l;
            };
        };
        std::uint16_t pc;
        std::uint16_t sp;

    public:
        cpu();
        ~cpu();

    public:
        void step(mmu &mmu);

    private:
        std::uint8_t _read_byte(mmu &mmu);
        std::uint16_t _read_word(mmu &mmu);
        void _handle_instruction(mmu &mmu, std::uint8_t code);
        void _handle_instruction_prefix(mmu &mmu, std::uint8_t code);

    private:
        void _jump(std::uint16_t address);
        void _jump_relative(std::uint8_t offset);

        void _call(mmu &mmu, std::uint16_t address);
        void _ret(mmu &mmu);

        void _push(mmu &mmu, std::uint16_t value);
        std::uint16_t _pop(mmu &mmu);

    private:
        void NOP(mmu &mmu);
        void STOP(mmu &mmu);
        void JR(mmu &mmu, bool condition);
        void LD(mmu &mmu, std::uint16_t &reg);
        void LD(mmu &mmu, std::uint16_t out_reg, std::uint8_t in_reg);
        void INC(mmu& mmu, std::uint16_t &reg);
        void INC(mmu& mmu, std::uint8_t &reg);
        void DEC(mmu& mmu, std::uint8_t &reg);
        void LD(mmu& mmu, std::uint8_t &reg);
        void RLCA(mmu &mmu);
        void RLA(mmu &mmu);
        void DAA(mmu &mmu);
        void SCF(mmu &mmu);
        void ADD(mmu& mmu, std::uint16_t &reg_out, std::uint16_t reg_in);
        void LD(mmu &mmu, std::uint8_t &reg_out, std::uint16_t reg_in);
        void DEC(mmu& mmu, std::uint16_t &reg);
        void RRCA(mmu &mmu);
        void RRA(mmu &mmu);
        void CPL(mmu &mmu);
        void CCF(mmu &mmu);
        void LD(mmu &mmu, std::uint8_t &reg_out, std::uint8_t reg_in);
        void HALT(mmu &mmu);
        void ADD(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void SUB(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void AND(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void OR(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void ADC(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void SBC(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void XOR(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void CP(mmu &mmu, std::uint8_t& reg_out, std::uint8_t reg_in);
        void RET(mmu &mmu, bool condition);
        void POP(mmu &mmu, std::uint16_t& reg_out);
        void JP(mmu &mmu, bool condition);
        void ILLEGAL(mmu &mmu, std::uint8_t code);
        void DI(mmu &mmu);
        void CALL(mmu &mmu, bool condition);
        void PUSH(mmu &mmu, std::uint16_t reg);
        void ADD(mmu &mmu, std::uint8_t& reg_out);
        void SUB(mmu &mmu, std::uint8_t& reg_out);
        void AND(mmu &mmu, std::uint8_t& reg_out);
        void OR(mmu &mmu, std::uint8_t& reg_out);
        void RST(mmu &mmu, std::uint8_t code);
        void ADD(mmu &mmu, std::uint16_t& reg_out);
        void RETI(mmu &mmu);
        void JP(mmu &mmu, std::uint16_t reg);
        void LD(mmu &mmu, std::uint16_t &reg_out, std::uint16_t reg_in);
        void PREFIX(mmu &mmu);
        void EI(mmu &mmu);
        void ADC(mmu &mmu, std::uint8_t& reg_out);
        void SBC(mmu &mmu, std::uint8_t& reg_out);
        void XOR(mmu &mmu, std::uint8_t& reg_out);
        void CP(mmu &mmu, std::uint8_t& reg_out);
        void RLC(mmu &mmu, std::uint8_t& reg_out);
        void RRC(mmu &mmu, std::uint8_t& reg_out);
        void RL(mmu &mmu, std::uint8_t& reg_out);
        void RR(mmu &mmu, std::uint8_t& reg_out);
        void SLA(mmu &mmu, std::uint8_t& reg_out);
        void SRA(mmu &mmu, std::uint8_t& reg_out);
        void SWAP(mmu &mmu, std::uint8_t& reg_out);
        void SRL(mmu &mmu, std::uint8_t& reg_out);
        void BIT(mmu &mmu, std::uint8_t bit, std::uint8_t& reg_out);
        void RES(mmu &mmu, std::uint8_t bit, std::uint8_t& reg_out);
        void SET(mmu &mmu, std::uint8_t bit, std::uint8_t& reg_out);
    };
}

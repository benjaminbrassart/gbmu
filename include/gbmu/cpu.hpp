/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cpu.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/07 11:22:32 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 11:59:53 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "gbmu/call_stack.hpp"

#include <cstdint>
#include <stack>

namespace gbmu
{
    class mmu;
    class renderer;

    enum class cpu_flag
    {
        z = 1 << 7,
        n = 1 << 6,
        h = 1 << 5,
        c = 1 << 4,
    };

    class cpu
    {
    public:
        union {
            /** AF register pair */
            std::uint16_t af;
            struct {
                /** AF low byte */
                std::uint8_t f;
                /** AF high byte */
                std::uint8_t a;
            };
        };
        union {
            /** BC register pair */
            std::uint16_t bc;
            struct {
                /** BC low byte */
                std::uint8_t c;
                /** BC high byte */
                std::uint8_t b;
            };
        };
        union {
            /** DE register pair */
            std::uint16_t de;
            struct {
                /** DE low byte */
                std::uint8_t e;
                /** DE high byte */
                std::uint8_t d;
            };
        };
        union {
            /** HL register pair */
            std::uint16_t hl;
            struct {
                /** HL low byte */
                std::uint8_t l;
                /** HL high byte */
                std::uint8_t h;
            };
        };
        /** Program counter */
        std::uint16_t pc;
        /** Stack pointer */
        std::uint16_t sp;

    public:
        std::stack<stack_element> stack;

    private:
        constexpr static std::uint8_t const IF_MASK = 0x1F;

    public:
        cpu();
        ~cpu();

    public:
        void boot(mmu &mmu, renderer &renderer);

        /**
         * Handle highest-priority interrupt if any.
         * Handle instruction otherwise.
         */
        void step(mmu &mmu, renderer &renderer);

        void dump_stack();

    private:
        /**
         * Read the next byte, and increase the program counter
         */
        std::uint8_t _read_byte(mmu &mmu);

        /**
         * Read the next two bytes, and increase the program counter twice
         */
        std::uint16_t _read_word(mmu &mmu);

        /**
         * Handle incoming interrupt
         *
         * Interrupts:
         * - 0x40: VBlank
         * - 0x48: STAT
         * - 0x50: Timer
         * - 0x58: Serial
         * - 0x60: Joypad
         */
        void _handle_interrupt(mmu &mmu, int interrupt);

        /**
         * Handle CPU instruction
         */
        void _handle_instruction(mmu &mmu, std::uint8_t code);

        /**
         * Handle 0xCB-prefixed CPU instruction
         */
        void _handle_instruction_prefix(mmu &mmu, std::uint8_t code);

    private:
        bool _getflag(cpu_flag flag);

        void _setflag(cpu_flag flag, bool value);

        /**
         * Set program counter to absolute address
         */
        void _jump(std::uint16_t address);

        /**
         * Increment program counter by an offset
         */
        void _jump_relative(std::int8_t offset);

        /**
         * Push program counter to the stack, then jump
         */
        void _call(mmu &mmu, std::uint16_t address);

        /**
         * Pop program counter from the stack, then jump
         */
        void _ret(mmu &mmu);

        /**
         * Push 2 bytes on the stack
         */
        void _push(mmu &mmu, std::uint16_t value);

        /**
         * Pop 2 bytes from the stack
         */
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
        void CP(mmu &mmu, std::uint8_t const& reg_out, std::uint8_t reg_in);
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

        void INC(mmu& mmu, std::uint16_t address, bool);
        void DEC(mmu& mmu, std::uint16_t address, bool);
        void LD(mmu& mmu, std::uint16_t address, bool);
        void RLC(mmu &mmu, std::uint16_t address, bool);
        void RRC(mmu &mmu, std::uint16_t address, bool);
        void RL(mmu &mmu, std::uint16_t address, bool);
        void RR(mmu &mmu, std::uint16_t address, bool);
        void SLA(mmu &mmu, std::uint16_t address, bool);
        void SRA(mmu &mmu, std::uint16_t address, bool);
        void SWAP(mmu &mmu, std::uint16_t address, bool);
        void SRL(mmu &mmu, std::uint16_t address, bool);
        void BIT(mmu &mmu, std::uint8_t bit, std::uint16_t address, bool);
        void RES(mmu &mmu, std::uint8_t bit, std::uint16_t address, bool);
        void SET(mmu &mmu, std::uint8_t bit, std::uint16_t address, bool);

    public:
        friend class debugger;
    };
}

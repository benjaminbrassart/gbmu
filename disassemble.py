#!/usr/bin/env python3

import io
import os
import requests
import sys

def file_size(f: io.BufferedReader) -> int:
    old_pos = f.tell()

    f.seek(0, os.SEEK_END)
    size = f.tell()
    f.seek(old_pos, os.SEEK_SET)

    return size


if len(sys.argv) == 1 or sys.argv[1] == "-":
    f = sys.stdin.buffer
elif len(sys.argv) == 2:
    f = open(sys.argv[1], "rb")
else:
    print("Usage: disassemble <rom file>", file=sys.stderr)
    exit(1)

OPCODES_URL = "https://gbdev.io/gb-opcodes/Opcodes.json"

opcodes: dict = requests.get(OPCODES_URL).json()
cbprefixed: dict = opcodes["cbprefixed"]
unprefixed: dict = opcodes["unprefixed"]

address = 0x0000

while len(buffer := f.read(1)) == 1:
    address_offset = 1

    code = buffer[0]
    args = []

    if code == 0xCB:
        address_offset += 1

        args.append("0xCB")

        if len(buffer := f.read(1)) == 0:
            print("Error: incomplete PREFIX instruction", file=sys.stderr)
            exit(1)

        code = buffer[0]
        insset = cbprefixed
        argc_offset = 2
    else:
        insset = unprefixed
        argc_offset = 1

    op = insset.get(f"0x{code:02X}", None)

    comment = [op["mnemonic"]]
    argc = (op["bytes"] - argc_offset)
    address_offset += argc

    args.append(f"0x{code:02X}")

    if argc != 0 and len(buffer := f.read(argc)) == argc:
        args.extend([f"0x{b:02X}" for b in buffer])

    operands = op["operands"]

    for operand in operands:
        op_name = operand["name"]

        if (op_size := operand.get("bytes", None)) is None:
            if operand.get("increment", False):
                op_name = f"{op_name}-"
            elif operand.get("decrement", False):
                op_name = f"{op_name}+"

            op_value = op_name
        else:
            op_data = buffer[:op_size]
            buffer = buffer[op_size:]

            if op_name == "e8":
                op_value = str(int.from_bytes(op_data, "little", signed=True))
            else:
                v = int.from_bytes(op_data, "little", signed=False)
                op_value = f"0x{v:02X}"

        if not operand["immediate"]:
            op_value = f"[{op_value}]"

        comment.append(op_value)

    args_str = ", ".join(args) + ","
    comment_str = comment.pop(0)

    if len(comment) > 0:
        comment_str += " " + ", ".join(comment)

    print(f"{args_str:<24}// 0x{address:04X} - {comment_str}")

    address += address_offset
f.close()

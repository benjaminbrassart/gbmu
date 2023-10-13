#!/usr/bin/env python3

import sys

if len(sys.argv) != 2:
    print("Usage: inspect-rom <rom file>")
    exit(1)

HEADER_SIZE = 336
NINTENDO_LOGO = bytes([
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E,
])
NEW_LICENSEE_CODES = {
    b"00": "None",
    b"01": "Nintendo R&D1",
    b"08": "Capcom",
    b"13": "Electronic Arts",
    b"18": "Hudson Soft",
    b"19": "b-ai",
    b"20": "kss",
    b"22": "pow",
    b"24": "PCM Complete",
    b"25": "san-x",
    b"28": "Kemco Japan",
    b"29": "seta",
    b"30": "Viacom",
    b"31": "Nintendo",
    b"32": "Bandai",
    b"33": "Ocean/Acclaim",
    b"34": "Konami",
    b"35": "Hector",
    b"37": "Taito",
    b"38": "Hudson",
    b"39": "Banpresto",
    b"41": "Ubi Soft",
    b"42": "Atlus",
    b"44": "Malibu",
    b"46": "angel",
    b"47": "Bullet-Proof",
    b"49": "irem",
    b"50": "Absolute",
    b"51": "Acclaim",
    b"52": "Activision",
    b"53": "American sammy",
    b"54": "Konami",
    b"55": "Hi tech entertainment",
    b"56": "LJN",
    b"57": "Matchbox",
    b"58": "Mattel",
    b"59": "Milton Bradley",
    b"60": "Titus",
    b"61": "Virgin",
    b"64": "LucasArts",
    b"67": "Ocean",
    b"69": "Electronic Arts",
    b"70": "Infogrames",
    b"71": "Interplay",
    b"72": "Broderbund",
    b"73": "sculptured",
    b"75": "sci",
    b"78": "THQ",
    b"79": "Accolade",
    b"80": "misawa",
    b"83": "lozc",
    b"86": "Tokuma Shoten Intermedia",
    b"87": "Tsukuda Original",
    b"91": "Chunsoft",
    b"92": "Video system",
    b"93": "Ocean/Acclaim",
    b"95": "Varie",
    b"96": "Yonezawa/s'pal",
    b"97": "Kaneko",
    b"99": "Pack in soft",
    b"9H": "Bottom Up",
    b"A4": "Konami (Yu-Gi-Oh!)",
}
OLD_LICENSEE_CODES = {
    b'\x00': "None",
    b'\x01': "Nintendo",
    b'\x08': "Capcom",
    b'\x09': "Hot-B",
    b'\x0A': "Jaleco",
    b'\x0B': "Coconuts Japan",
    b'\x0C': "Elite Systems",
    b'\x13': "EA (Electronic Arts)",
    b'\x18': "Hudsonsoft",
    b'\x19': "ITC Entertainment",
    b'\x1A': "Yanoman",
    b'\x1D': "Japan Clary",
    b'\x1F': "Virgin Interactive",
    b'\x24': "PCM Complete",
    b'\x25': "San-X",
    b'\x28': "Kotobuki Systems",
    b'\x29': "Seta",
    b'\x30': "Infogrames",
    b'\x31': "Nintendo",
    b'\x32': "Bandai",
    b'\x34': "Konami",
    b'\x35': "HectorSoft",
    b'\x38': "Capcom",
    b'\x39': "Banpresto",
    b'\x3C': ".Entertainment i",
    b'\x3E': "Gremlin",
    b'\x41': "Ubisoft",
    b'\x42': "Atlus",
    b'\x44': "Malibu",
    b'\x46': "Angel",
    b'\x47': "Spectrum Holoby",
    b'\x49': "Irem",
    b'\x4A': "Virgin Interactive",
    b'\x4D': "Malibu",
    b'\x4F': "U.S. Gold",
    b'\x50': "Absolute",
    b'\x51': "Acclaim",
    b'\x52': "Activision",
    b'\x53': "American Sammy",
    b'\x54': "GameTek",
    b'\x55': "Park Place",
    b'\x56': "LJN",
    b'\x57': "Matchbox",
    b'\x59': "Milton Bradley",
    b'\x5A': "Mindscape",
    b'\x5B': "Romstar",
    b'\x5C': "Naxat Soft",
    b'\x5D': "Tradewest",
    b'\x60': "Titus",
    b'\x61': "Virgin Interactive",
    b'\x67': "Ocean Interactive",
    b'\x69': "EA (Electronic Arts)",
    b'\x6E': "Elite Systems",
    b'\x6F': "Electro Brain",
    b'\x70': "Infogrames",
    b'\x71': "Interplay",
    b'\x72': "Broderbund",
    b'\x73': "Sculptered Soft",
    b'\x75': "The Sales Curve",
    b'\x78': "t.hq",
    b'\x79': "Accolade",
    b'\x7A': "Triffix Entertainment",
    b'\x7C': "Microprose",
    b'\x7F': "Kemco",
    b'\x80': "Misawa Entertainment",
    b'\x83': "Lozc",
    b'\x86': "Tokuma Shoten Intermedia",
    b'\x8B': "Bullet-Proof Software",
    b'\x8C': "Vic Tokai",
    b'\x8E': "Ape",
    b'\x8F': "I'Max",
    b'\x91': "Chunsoft Co.",
    b'\x92': "Video System",
    b'\x93': "Tsubaraya Productions Co.",
    b'\x95': "Varie Corporation",
    b'\x96': "Yonezawa/S'Pal",
    b'\x97': "Kaneko",
    b'\x99': "Arc",
    b'\x9A': "Nihon Bussan",
    b'\x9B': "Tecmo",
    b'\x9C': "Imagineer",
    b'\x9D': "Banpresto",
    b'\x9F': "Nova",
    b'\xA1': "Hori Electric",
    b'\xA2': "Bandai",
    b'\xA4': "Konami",
    b'\xA6': "Kawada",
    b'\xA7': "Takara",
    b'\xA9': "Technos Japan",
    b'\xAA': "Broderbund",
    b'\xAC': "Toei Animation",
    b'\xAD': "Toho",
    b'\xAF': "Namco",
    b'\xB0': "acclaim",
    b'\xB1': "ASCII or Nexsoft",
    b'\xB2': "Bandai",
    b'\xB4': "Square Enix",
    b'\xB6': "HAL Laboratory",
    b'\xB7': "SNK",
    b'\xB9': "Pony Canyon",
    b'\xBA': "Culture Brain",
    b'\xBB': "Sunsoft",
    b'\xBD': "Sony Imagesoft",
    b'\xBF': "Sammy",
    b'\xC0': "Taito",
    b'\xC2': "Kemco",
    b'\xC3': "Squaresoft",
    b'\xC4': "Tokuma Shoten Intermedia",
    b'\xC5': "Data East",
    b'\xC6': "Tonkinhouse",
    b'\xC8': "Koei",
    b'\xC9': "UFL",
    b'\xCA': "Ultra",
    b'\xCB': "Vap",
    b'\xCC': "Use Corporation",
    b'\xCD': "Meldac",
    b'\xCE': ".Pony Canyon or",
    b'\xCF': "Angel",
    b'\xD0': "Taito",
    b'\xD1': "Sofel",
    b'\xD2': "Quest",
    b'\xD3': "Sigma Enterprises",
    b'\xD4': "ASK Kodansha Co.",
    b'\xD6': "Naxat Soft",
    b'\xD7': "Copya System",
    b'\xD9': "Banpresto",
    b'\xDA': "Tomy",
    b'\xDB': "LJN",
    b'\xDD': "NCS",
    b'\xDE': "Human",
    b'\xDF': "Altron",
    b'\xE0': "Jaleco",
    b'\xE1': "Towa Chiki",
    b'\xE2': "Yutaka",
    b'\xE3': "Varie",
    b'\xE5': "Epcoh",
    b'\xE7': "Athena",
    b'\xE8': "Asmik ACE Entertainment",
    b'\xE9': "Natsume",
    b'\xEA': "King Records",
    b'\xEB': "Atlus",
    b'\xEC': "Epic/Sony Records",
    b'\xEE': "IGS",
    b'\xF0': "A Wave",
    b'\xF3': "Extreme Entertainment",
    b'\xFF': "LJN",
}
CARTRIDGE_TYPES = {
    b'\x00': "ROM only",
    b'\x01': "MBC1",
    b'\x02': "MBC1 (ram)",
    b'\x03': "MBC1 (ram, battery)",
    b'\x05': "MBC2",
    b'\x06': "MBC2 (battery)",
    b'\x08': "ROM only (ram)",
    b'\x09': "ROM only (ram, battery)",
    b'\x0B': "MMM01",
    b'\x0C': "MMM01 (ram)",
    b'\x0D': "MMM01 (ram, battery)",
    b'\x0F': "MBC3 (timer, battery)",
    b'\x10': "MBC3 (timer, ram, battery)",
    b'\x11': "MBC3",
    b'\x12': "MBC3 (ram)",
    b'\x13': "MBC3 (ram, battery)",
    b'\x19': "MBC5",
    b'\x1A': "MBC5 RAM",
    b'\x1B': "MBC5 (ram, battery)",
    b'\x1C': "MBC5 (rumble)",
    b'\x1D': "MBC5 (rumble, ram)",
    b'\x1E': "MBC5 (rumble, ram, battery)",
    b'\x20': "MBC6",
    b'\x22': "MBC7 (sensor, rumble, ram, battery)",
    b'\xFC': "Pocket Camera",
    b'\xFD': "Bandai TAMA5",
    b'\xFE': "HuC3",
    b'\xFF': "HuC1 (ram, battery)",
}
ROM_SIZES = {
    b'\x00': 32 * 1024,
    b'\x01': 64 * 1024,
    b'\x02': 128 * 1024,
    b'\x03': 256 * 1024,
    b'\x04': 512 * 1024,
    b'\x05': 1 * 1024 * 1024,
    b'\x06': 2 * 1024 * 1024,
    b'\x07': 4 * 1024 * 1024,
    b'\x08': 8 * 1024 * 1024,
}
RAM_SIZES = {
    b'\x00': 0,
    b'\x01': 2 * 1024,
    b'\x02': 4 * 1024,
    b'\x03': 32 * 1024,
    b'\x04': 128 * 1024,
    b'\x05': 64 * 1024,
}

class Header:
    def __init__(self, buffer: bytes) -> None:
        self.entry_point = buffer[0x0100:0x0104]
        self.nintendo_logo = buffer[0x0104:0x0134]
        self.title = buffer[0x0134:0x0143]
        self.manufacturer_code = buffer[0x013F:0x0143]
        self.cgb_flag = buffer[0x0143:0x0144]
        self.new_licensee_code = buffer[0x0144:0x0146]
        self.sgb_flag = buffer[0x0146:0x0147]
        self.cartridge_type = buffer[0x0147:0x0148]
        self.rom_size = buffer[0x0148:0x0149]
        self.ram_size = buffer[0x0149:0x014A]
        self.destination_code = buffer[0x014A:0x014B]
        self.old_licensee_code = buffer[0x014B:0x014C]
        self.version_number = buffer[0x014C:0x014D]
        self.header_checksum = buffer[0x014D:0x014E]
        self.global_checksum = buffer[0x014E:0x0150]

with open(sys.argv[1], "rb") as f:
    raw_header = f.read(HEADER_SIZE)

    if len(raw_header) != HEADER_SIZE:
        print(f"inspect-rom: {f.name}: invalid rom file", file=sys.stderr)
        exit(1)

    header = Header(raw_header)

    print("Header")
    print()

    print(f"  Entrypoint         {header.entry_point.hex(' ')}")
    print(f"  Nintendo logo      {header.nintendo_logo.hex(' ')}")
    print(f"  Title              {header.title.hex(' ')}")
    print(f"  Manufacturer code  {header.manufacturer_code.hex(' ')}")
    print(f"  CGB flag           {header.cgb_flag.hex(' ')}")
    print(f"  New licensee code  {header.new_licensee_code.hex(' ')}")
    print(f"  SGB flag           {header.sgb_flag.hex(' ')}")
    print(f"  Cartridge type     {header.cartridge_type.hex(' ')}")
    print(f"  ROM size           {header.rom_size.hex(' ')}")
    print(f"  RAM size           {header.ram_size.hex(' ')}")
    print(f"  Destination code   {header.destination_code.hex(' ')}")
    print(f"  Old licensee code  {header.old_licensee_code.hex(' ')}")
    print(f"  Version number     {header.version_number.hex(' ')}")
    print(f"  Header checksum    {header.header_checksum.hex(' ')}")
    print(f"  Global checksum    {header.global_checksum.hex(' ')}")

    print()

    if header.nintendo_logo != NINTENDO_LOGO:
        print(f"inspect-rom: {f.name}: invalid Nintendo logo", file=sys.stderr)
        exit(1)

    if header.cgb_flag == b'\x80':
        cgb_mode = "Supported"
    elif header.cgb_flag == b'\xC0':
        cgb_mode = "Forced"
    else:
        cgb_mode = "Disabled"

    if header.sgb_flag == b'\x03':
        sgb_mode = "Supported"
    else:
        sgb_mode = "Disabled"

    if header.old_licensee_code == b'\x33':
        licensee_code = header.new_licensee_code
        licensee = NEW_LICENSEE_CODES.get(header.new_licensee_code, None)
        licensee_type = "new"
    else:
        licensee_code = header.old_licensee_code
        licensee = OLD_LICENSEE_CODES.get(header.old_licensee_code, None)
        licensee_type = "old"

    if licensee == None:
        print(f"inspect-rom: {f.name}: invalid {licensee_type} licensee code: {licensee_code.hex()}", file=sys.stderr)
        exit(1)

    cart_type = CARTRIDGE_TYPES.get(header.cartridge_type, None)

    if cart_type is None:
        print(f"inspect-rom: {f.name}: invalid cartridge type: {header.cartridge_type.hex()}", file=sys.stderr)
        exit(1)

    rom_size = ROM_SIZES.get(header.rom_size, None)

    if rom_size is None:
        print(f"inspect-rom: {f.name}: invalid ROM size: {header.rom_size.hex()}", file=sys.stderr)
        exit(1)

    rom_banks = int(rom_size / (16 * 1024))
    rom_size_unit = "KiB"
    rom_size_value = int(rom_size / 1024)

    if rom_size_value >= 1024:
        rom_size_unit = "MiB"
        rom_size_value = int(rom_size_value / 1024)

    ram_size = RAM_SIZES.get(header.ram_size, None)

    if ram_size is None:
        print(f"inspect-rom: {f.name}: invalid RAM size: {header.ram_size.hex()}", file=sys.stderr)
        exit(1)

    ram_banks = int(ram_size / (16 * 1024))
    ram_size_unit = "KiB"
    ram_size_value = int(ram_size / 1024)

    print("Information")
    print()

    print(f"  Licensee ({licensee_type})     {licensee}")
    print(f"  CGB mode           {cgb_mode}")
    print(f"  SGB mode           {sgb_mode}")
    print(f"  Cartridge type     {cart_type}")
    print(f"  ROM size           {rom_size_value} {rom_size_unit} ({rom_banks} banks)")
    print(f"  RAM size           {ram_size_value} {ram_size_unit} ({ram_banks} banks)")

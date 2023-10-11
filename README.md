# gbmu

This is an outer circle project at 42.
Its goal is to create a GameBoy and GameBoy Color emulator in the language of our choice.

This one is written in C++20.

## Resources

### Manual
- [`GameBoyProgManVer1.1.pdf`](https://ia903208.us.archive.org/9/items/GameBoyProgManVer1.1/GameBoyProgManVer1.1.pdf) &mdash; Official Game Boy programming manual by Nintendo
- [`GBCPUman.pdf`](http://marc.rawer.de/Gameboy/Docs/GBCPUman.pdf) &mdash; Unofficial manual, easy to read and understand
- [`1996_Sharp_Microcomputer_Data_Book.pdf`](http://www.bitsavers.org/components/sharp/_dataBooks/1996_Sharp_Microcomputer_Data_Book.pdf) &mdash; SHARP SM83 CPU technical specifications

### Websites
- https://gbdev.io/gb-opcodes/optables/dark &mdash; SM83 opcodes
- https://gbdev.io/ &mdash; Tons of resources and guides
- https://gbdev.gg8.se/ &mdash; Another great resource provider

## Cartridge

### Memory layout

```
[256 bytes] - interrupt vectors
[80  bytes] - header
[... bytes] - actual program
```

#### Interrupt vectors

Under construction

#### Header

```
[4  bytes] - entrypoint
[48 bytes] - nintendo logo
[11 bytes] - title
[4  bytes] - manufacturer code
[1  byte ] - CGB flag
[2  bytes] - new licensee code
[1  byte ] - SGB flag
[1  byte ] - cartridge type
[1  byte ] - rom size
[1  byte ] - ram size
[1  byte ] - destination code
[1  byte ] - old licensee code
[1  byte ] - version number
[1  byte ] - header checksum
[2  bytes] - global checksum
```

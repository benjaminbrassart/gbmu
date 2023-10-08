# gbmu

This is an outer circle project at 42.
Its goal is to create a GameBoy and GameBoy Color emulator in the language of our choice.

This one is written in C++20.

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

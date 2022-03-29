# FennixProject
##### Opeating System from scratch made in C and C++

[![Discord](https://img.shields.io/badge/Discord-5865F2?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/AYhW6N59Wu)
![AppVeyor tests](https://img.shields.io/appveyor/tests/EnderIce2/FennixProject?style=for-the-badge)

#### The content
- Fennix Project
    - Fennix-Bootloader
        - Coming Soon™
    - Fennix-Kernel
        - The core components of the operating system.
    - Fennix-Libc
        - The C standard library.
    - Fennix-Userspace
        - The userland of the operating system.

## Download, compile & run

- Clone this repository using

```bash
git clone --recurse-submodules https://github.com/EnderIce2/FennixProject
```

Before doing anything, you must install the following packages:

| Debian          |
| --------------- |
| qemu-system-x86 |
| mtools          |
| genisoimage     |
| ovmf            |
| nasm            |
| doxygen         |
| build-essential |
| bison           |
| flex            |
| libgmp3-dev     |
| libmpc-dev      |
| libmpfr-dev     |
| texinfo         |
| libisl-dev      |

- Ubuntu 20.04 LTS:
```bash
sudo apt-get install qemu-system-x86 mtools genisoimage ovmf nasm doxygen build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```

For other distributions, you have to install qemu, mtools, genisoimage, ovmf, nasm & doxygen. For the crosscompiler follow the instructions on the [osdev page](https://wiki.osdev.org/GCC_Cross-Compiler#:~:text=CLooG%20(optional)-,Installing%20Dependencies,-%E2%86%93%20Dependency%20/%20OS%20%E2%86%92).

Currently it was tested **only** in **Ubuntu 20.04**. I don't guarantee that it will work on other distributions or older versions of Ubuntu.

- Go to the local repository and run the following command

```bash
make tools
```

This command will download all the required files to compile the operating system.

- To run the OS, run the following command

```bash
make run
```
<sub><sup>or `make build` to build it without running.</sup></sub>

- To cleanup the code, run the following command

```bash
make clean
```

## Progress

- [ ] Rolling own bootloader
- [ ] Stivale Support
- [x] Stivale2 Support
- [ ] Multiboot Support
- [ ] Multiboor2 Support
- [ ] BOOTBOOT Support
- [ ] ACPI
- [ ] Paging
- [ ] Virtual Memory Manager
- [ ] Heap Manager
- [ ] Stack Manager
- [ ] Global Descriptor Table, Interrupt Descriptor Table & Task State Segment
- [ ] Timer (HPET & PIC)
- [ ] Symmetric Multiprocessing
- [ ] PCI
- [ ] PS2 Keyboard
- [ ] PS2 Mouse
- [ ] AHCI Driver
- [ ] ATA Driver
- [ ] Floppy Driver
- [ ] IDE Driver
- [ ] USTAR FileSystem
- [ ] EXT2 FileSystem
- [ ] ECHFS FileSystem
- [ ] FAT12 FileSystem
- [ ] INITRD FileSystem
- [ ] ISO9660 FileSystem
- [ ] FAT32 FileSystem
- [ ] Tasking
- [ ] Loading ELF Files
- [ ] Loading PE Files
- [ ] Loading MZ Files

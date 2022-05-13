# FennixProject
##### Opeating System from scratch made in C and C++

[![Discord](https://img.shields.io/discord/887406812612157451?style=for-the-badge&logo=discord&logoColor=white)](https://discord.gg/AYhW6N59Wu)
![AppVeyor](https://img.shields.io/appveyor/build/EnderIce2/fennixproject?style=for-the-badge)
[![CodeFactor](https://www.codefactor.io/repository/github/enderice2/fennixproject/badge?style=for-the-badge)](https://www.codefactor.io/repository/github/enderice2/fennixproject)
[![Codacy Badge](https://img.shields.io/codacy/grade/d00135a0a6304420a3cd021936f7be50?label=codacy&style=for-the-badge)](https://www.codacy.com/gh/EnderIce2/FennixProject/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=EnderIce2/FennixProject&amp;utm_campaign=Badge_Grade)
![GitHub](https://img.shields.io/github/license/EnderIce2/FennixProject?style=for-the-badge)
![GitHub commit activity](https://img.shields.io/github/commit-activity/w/EnderIce2/FennixProject?style=for-the-badge)

---

#### The content
- Fennix Project
    - boot
        - Coming Soon™
    - kernel
        - The core components of the operating system.
    - libc
        - The C standard library.
    - userspace
        - The userland of the operating system.

---
##### ! Warning !
`Before doing anything, remember that this project is in early stages of development. And most of the stuff are very unstable or broken. If you are interested to contribute, please do not hesitate to try it.`

---

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

And follow this [page](https://wiki.qemu.org/Hosts/Linux#:~:text=for%20both%20variants.-,Building%20QEMU%20for%20Linux,-Most%20Linux%20distributions) to install **ONLY** the required packages for Qemu. If you want to use the one that you have installed, go on `tools/Makefile` and comment `update_qemu` at line 26 (starts with `all:`).

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

After that, you should be able to compile the operating system.

---

- To run the OS:

```bash
make run
```
<sub><sup>or `make build` to build it without running.</sup></sub>

- To cleanup the code:

```bash
make clean
```

---

## Progress

- [ ] Rolling own bootloader
- [x] Stivale2 Protocol Support
- [ ] Limine Protocol Support
- [x] ACPI
- [x] ACPI Shutdown
- [x] MADT
- [x] DSDT
- [x] APIC
- [x] Paging
- [x] Virtual Memory Manager
- [x] Heap Manager
- [x] Stack Manager
- [x] Global Descriptor Table, Interrupt Descriptor Table & Task State Segment
- [x] Timer (APIC & HPET & PIC)
- [x] Symmetric Multiprocessing
- [x] PCI
- [x] PS2 Keyboard
- [x] PS2 Mouse
- [x] AHCI Driver
- [ ] ATA Driver
- [ ] Floppy Driver
- [ ] IDE Driver
- [x] Virtual FileSystem
- [x] USTAR FileSystem
- [ ] EXT2 FileSystem
- [ ] ECHFS FileSystem
- [ ] INITRD FileSystem
- [ ] ISO9660 FileSystem
- [ ] FAT12 FileSystem
- [ ] FAT16 FileSystem
- [ ] FAT32 FileSystem
- [x] Tasking
- [x] Usermode
- [x] Syscalls
- [x] Executing ELF Files
- [ ] Executing PE Files
- [ ] Executing NE Files
- [ ] Executing MZ Files

---

## Todo
- [ ] Figure out how to make the interrupts push es, fs & gs into REGISTERS structure
- [ ] Fix the context switching causing exceptions
- [ ] Fix the multitasking page table switching
- [ ] Complete implementation for driver loading
- [ ] Make a working version of process messaging (IPC)

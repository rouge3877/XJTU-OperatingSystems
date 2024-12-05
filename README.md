# an OS kernel

## Introduction

## Features

## Getting Started

## Documentation

## For Developers 
<!-- ![](https://github.com/rouge3877/ImageHosting/1faf5-1f3fc.png) -->

### Development Environment
The os kernel now tests on qemu. 
You can run the kernel by running `make qemu` or `make qemu-debug`
 in the root directory.

For environment setup, you need to install the following **packages**:
- `gcc`, `make`
    - `build-essential` is all you need 🤣
- `nasm`
    - `nasm` is enough (*for now*), for building the boot loader
- [`qemu`](https://www.qemu.org/download/#linux)
    QEMU is packaged by most Linux distributions, (*of course, you can build it from source: https://www.qemu.org/download/#source*):
    - Arch: `pacman -S qemu`
    - Debian/Ubuntu: `apt-get install qemu-system` for full system emulation, `apt-get install qemu-user-static` for emulating Linux binaries
    - Fedora: `dnf install @virtualization`
    - Gentoo: `emerge --ask app-emulation/qemu`
    - RHEL/CentOS: `yum install qemu-kvm`
    - SUSE: `zypper install qemu`
- `bximage`, `dd`
    - for creating a disk image
    - for `bximage`, Debian/Ubuntu: `apt-get install bximage`

### Kernel Development






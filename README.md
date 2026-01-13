# freebsd-s4-tools

A toolkit to fabricate FreeBSD S4 hibernate images (ELF format) and a UEFI Activator for resume testing.

This project consists of two main components:
1.  **Generator:** Python scripts to create a valid ELF64 image containing `PT_FREEBSD_S4_PCB` and `PT_FREEBSD_S4_TRAMPOLINE` headers.
2.  **Activator:** A FreeBSD stand/efi UEFI application to load the image and restore context.

## Current Status

* [x] **S4 Image Generation:** Functional (Python).
* [x] **UEFI Build System:** FreeBSD stand/efi build system integration.
* [x] **Activator (Phase 1):** Successfully reads `s4_image.bin` from swap partition.
* [x] **Activator (Phase 2):** Parsing ELF/S4 headers and allocating physical memory.
* [x] **Activator (Phase 3):** Context restoration and trampoline jump.

## Prerequisites & Installation
Before building, ensure you have the necessary system packages installed.

### FreeBSD
Requires FreeBSD source tree, `nasm` for assembly, and virtualization tools for testing.

```bash
pkg install nasm mtools
```


## Project Structure

* `generator/`: Python scripts and configuration for fabricating S4 images.
* `freebsd-src/`: FreeBSD source tree (git submodule).
  * `stand/efi/s4_activator/`: UEFI activator source (in-tree build).
* `scripts/`: Helper scripts for building and running Bhyve simulation.

## Initialization (One-time Setup)

### 1. Clone the Repository

```bash
git clone --recurse-submodules https://github.com/rickywu0421/freebsd-s4-tools.git
cd freebsd-s4-tools
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init --depth 1
```

## Development Cycle
Once initialized, use the following workflow to generate images and test the loader.

### 1. Fabricate S4 Image

Prepare your configuration and run the generator:

```bash
python3 generator/main.py \
    generator/layout/example.toml \
    generator/trampoline/hello_world.bin
```

### 2. Build UEFI Activator

Build the FreeBSD stand/efi application (requires FreeBSD):

```bash
cd freebsd-src/stand/efi/s4_activator
make
```

Output: `s4_activator.efi`

### 3. Run Simulation

Launch Bhyve with UEFI firmware. The script automatically creates a GPT disk image containing the S4 image in swap partition and redirects the virtual serial port (COM1) to the terminal.

```bash
sudo ./scripts/run_bhyve.sh
```

Note: Root privileges are required for Bhyve device creation.

## Configuration
Modify `generator/layout/example.toml` to define:

ELF Layout: p_paddr, p_vaddr, and alignment for segments.

Context State: Initial values for cr3, rsp, rip in the PCB.
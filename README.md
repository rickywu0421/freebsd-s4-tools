# freebsd-s4-tools

A minimal tool to fabricate FreeBSD S4 hibernate images (ELF format) for UEFI resume testing.

This tool combines a generic binary payload (trampoline) with a TOML-defined memory layout to generate a valid ELF64 image containing `PT_FREEBSD_S4_PCB` and `PT_FREEBSD_S4_TRAMPOLINE` headers.

## Requirements

* Python 3.11+ (Required for `tomllib`)

## Project Structure

* `generator/`: Python scripts and configuration for fabricating S4 images.
* `uefi/`: Placeholder for future UEFI bootloader/activator development.


## Usage

1. Prepare your configuration (see `layout/example.toml`) and your trampoline binary.
2. Run the builder:


```bash
python3 generator/main.py \
generator/layout/example.toml \
generator/trampoline/hello_world.bin \
generator/bin/s4img
```

You can replace the above parameters with your own/modified TOML, trampoline binary, and the output path.

## Configuration
Modify `generator/layout/example.toml` to define:

* ELF Layout: p_paddr, p_vaddr, and alignment for segments.

* Context State: Initial values for cr3, rsp, rip in the PCB.
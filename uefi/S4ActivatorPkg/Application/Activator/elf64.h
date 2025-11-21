#ifndef _ELF64_H_
#define _ELF64_H_

#include <Uefi.h>

//
// ================= Type Aliases ====================
// Mapping ELF types to UEFI types
//
typedef UINT64  Elf64_Addr;
typedef UINT64  Elf64_Off;
typedef UINT16  Elf64_Half;
typedef UINT32  Elf64_Word;
typedef INT32   Elf64_Sword;
typedef UINT64  Elf64_Xword;
typedef INT64   Elf64_Sxword;

//
// ================== ELF Header =====================
//

//
// e_ident[] Indices
//
#define EI_MAG0         0   // File identification
#define EI_MAG1         1   // File identification
#define EI_MAG2         2   // File identification
#define EI_MAG3         3   // File identification
#define EI_CLASS        4   // File class
#define EI_DATA         5   // Data encoding
#define EI_VERSION      6   // File version
#define EI_OSABI        7   // Operating system/ABI identification
#define EI_ABIVERSION   8   // ABI version
#define EI_PAD          9   // Start of padding bytes
#define EI_NIDENT       16  // Size of e_ident[]

//
// Magic Number Values
//
#define ELFMAG0         0x7f
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'

//
// File Class (EI_CLASS)
//
#define ELFCLASSNONE    0  // Invalid class
#define ELFCLASS32      1  // 32-bit objects
#define ELFCLASS64      2  // 64-bit objects

//
// Data Encoding (EI_DATA)
//
#define ELFDATANONE     0  // Invalid data encoding
#define ELFDATA2LSB     1  // LSB
#define ELFDATA2MSB     2  // MSB

//
// File Version (EI_VERSION)
//
#define EV_NONE         0  // Invalid version
#define EV_CURRENT      1  // Current version

//
// OS ABI (EI_OSABI)
//
#define ELFOSABI_NONE           0   // No extensions or unspecified
#define ELFOSABI_HPUX           1   // Hewlett-Packard HP-UX
#define ELFOSABI_NETBSD         2   // NetBSD
#define ELFOSABI_GNU            3   // GNU
#define ELFOSABI_LINUX          3   // Linux (historical - alias for ELFOSABI_GNU)
#define ELFOSABI_SOLARIS        6   // Sun Solaris
#define ELFOSABI_AIX            7   // AIX
#define ELFOSABI_IRIX           8   // IRIX
#define ELFOSABI_FREEBSD        9   // FreeBSD
#define ELFOSABI_TRU64          10  // Compaq TRU64 UNIX
#define ELFOSABI_MODESTO        11  // Novell Modesto
#define ELFOSABI_OPENBSD        12  // Open BSD
#define ELFOSABI_OPENVMS        13  // Open VMS
#define ELFOSABI_NSK            14  // Hewlett-Packard Non-Stop Kernel
#define ELFOSABI_AROS           15  // Amiga Research OS
#define ELFOSABI_FENIXOS        16  // The FenixOS highly scalable multi-core OS
#define ELFOSABI_CLOUDABI       17  // Nuxi CloudABI
#define ELFOSABI_OPENVOS        18  // Stratus Technologies OpenVOS
#define ELFOSABI_RESERVED_MIN   64
#define ELFOSABI_RESERVED_MAX   255

//
// Object File Type (e_type)
//
#define ET_NONE                 0        // No file type
#define ET_REL                  1        // Relocatable file
#define ET_EXEC                 2        // Executable file
#define ET_DYN                  3        // Shared object file
#define ET_CORE                 4        // Core file
#define ET_LOOS                 0xfe00   // Operating system-specific
#define ET_HIOS                 0xfeff   // Operating system-specific
#define ET_LOPROC               0xff00   // Processor-specific
#define ET_HIPROC               0xffff   // Processor-specific

// [Custom] FreeBSD S4 image specific
#define ET_FREEBSD_S4_IMAGE     (ET_LOOS + 1)

//
// Machine Architecture (e_machine)
//
#define EM_NONE          0   // No machine
#define EM_M32           1   // AT&T WE 32100
#define EM_SPARC         2   // SPARC
#define EM_386           3   // Intel 80386
#define EM_68K           4   // Motorola 68000
#define EM_88K           5   // Motorola 88000
#define EM_IAMCU         6   // Intel MCU
#define EM_860           7   // Intel 80860
#define EM_MIPS          8   // MIPS I Architecture
#define EM_S370          9   // IBM System/370 Processor
#define EM_MIPS_RS3_LE   10  // MIPS RS3000 Little-endian
#define EM_PARISC        15  // Hewlett-Packard PA-RISC
#define EM_VPP500        17  // Fujitsu VPP500
#define EM_SPARC32PLUS   18  // Enhanced instruction set SPARC
#define EM_960           19  // Intel 80960
#define EM_PPC           20  // PowerPC
#define EM_PPC64         21  // 64-bit PowerPC
#define EM_S390          22  // IBM System/390 Processor
#define EM_SPU           23  // IBM SPU/SPC
#define EM_V800          36  // NEC V800
#define EM_FR20          37  // Fujitsu FR20
#define EM_RH32          38  // TRW RH-32
#define EM_RCE           39  // Motorola RCE
#define EM_ARM           40  // ARM 32-bit architecture (AARCH32)
#define EM_ALPHA         41  // Digital Alpha
#define EM_SH            42  // Hitachi SH
#define EM_SPARCV9       43  // SPARC Version 9
#define EM_TRICORE       44  // Siemens TriCore embedded processor
#define EM_ARC           45  // Argonaut RISC Core
#define EM_H8_300        46  // Hitachi H8/300
#define EM_H8_300H       47  // Hitachi H8/300H
#define EM_H8S           48  // Hitachi H8S
#define EM_H8_500        49  // Hitachi H8/500
#define EM_IA_64         50  // Intel IA-64 processor architecture
#define EM_MIPS_X        51  // Stanford MIPS-X
#define EM_COLDFIRE      52  // Motorola ColdFire
#define EM_68HC12        53  // Motorola M68HC12
#define EM_MMA           54  // Fujitsu MMA Multimedia Accelerator
#define EM_PCP           55  // Siemens PCP
#define EM_NCPU          56  // Sony nCPU embedded RISC processor
#define EM_NDR1          57  // Denso NDR1 microprocessor
#define EM_STARCORE      58  // Motorola Star*Core processor
#define EM_ME16          59  // Toyota ME16 processor
#define EM_ST100         60  // STMicroelectronics ST100 processor
#define EM_TINYJ         61  // Advanced Logic Corp. TinyJ
#define EM_X86_64        62  // AMD x86-64 architecture
#define EM_PDSP          63  // Sony DSP Processor
#define EM_PDP10         64  // Digital Equipment Corp. PDP-10
#define EM_PDP11         65  // Digital Equipment Corp. PDP-11
#define EM_FX66          66  // Siemens FX66 microcontroller
#define EM_ST9PLUS       67  // STMicroelectronics ST9+
#define EM_ST7           68  // STMicroelectronics ST7
#define EM_68HC16        69  // Motorola MC68HC16
#define EM_68HC11        70  // Motorola MC68HC11
#define EM_68HC08        71  // Motorola MC68HC08
#define EM_68HC05        72  // Motorola MC68HC05
#define EM_SVX           73  // Silicon Graphics SVx
#define EM_ST19          74  // STMicroelectronics ST19
#define EM_VAX           75  // Digital VAX
#define EM_CRIS          76  // Axis Communications 32-bit embedded processor
#define EM_JAVELIN       77  // Infineon Technologies 32-bit embedded processor
#define EM_FIREPATH      78  // Element 14 64-bit DSP Processor
#define EM_ZSP           79  // LSI Logic 16-bit DSP Processor
#define EM_MMIX          80  // Donald Knuth's educational 64-bit processor
#define EM_HUANY         81  // Harvard University machine-independent object files
#define EM_PRISM         82  // SiTera Prism
#define EM_AVR           83  // Atmel AVR 8-bit microcontroller
#define EM_FR30          84  // Fujitsu FR30
#define EM_D10V          85  // Mitsubishi D10V
#define EM_D30V          86  // Mitsubishi D30V
#define EM_V850          87  // NEC v850
#define EM_M32R          88  // Mitsubishi M32R
#define EM_MN10300       89  // Matsushita MN10300
#define EM_MN10200       90  // Matsushita MN10200
#define EM_PJ            91  // picoJava
#define EM_OPENRISC      92  // OpenRISC 32-bit embedded processor
#define EM_ARC_COMPACT   93  // ARC International ARCompact
#define EM_XTENSA        94  // Tensilica Xtensa Architecture
#define EM_VIDEOCORE     95  // Alphamosaic VideoCore processor
#define EM_TMM_GPP       96  // Thompson Multimedia General Purpose Processor
#define EM_NS32K         97  // National Semiconductor 32000 series
#define EM_TPC           98  // Tenor Network TPC processor
#define EM_SNP1K         99  // Trebia SNP 1000 processor
#define EM_ST200         100 // STMicroelectronics ST200
#define EM_IP2K          101 # Ubicom IP2xxx
#define EM_MAX           102 // MAX Processor
#define EM_CR            103 // National Semiconductor CompactRISC
#define EM_F2MC16        104 // Fujitsu F2MC16
#define EM_MSP430        105 // Texas Instruments msp430
#define EM_BLACKFIN      106 // Analog Devices Blackfin (DSP)
#define EM_SE_C33        107 # S1C33 Family
#define EM_SEP           108 // Sharp embedded microprocessor
#define EM_ARCA          109 // Arca RISC Microprocessor
#define EM_UNICORE       110 // PKU-Unity Ltd.
#define EM_EXCESS        111 // eXcess
#define EM_DXP           112 // Icera Semiconductor Inc. Deep Execution Processor
#define EM_ALTERA_NIOS2  113 // Altera Nios II
#define EM_CRX           114 // National Semiconductor CompactRISC CRX
#define EM_XGATE         115 // Motorola XGATE
#define EM_C166          116 // Infineon C16x/XC16x
#define EM_M16C          117 # Renesas M16C
#define EM_DSPIC30F      118 # Microchip Technology dsPIC30F
#define EM_CE            119 // Freescale Communication Engine
#define EM_M32C          120 # Renesas M32C
#define EM_TSK3000       131 # Altium TSK3000
#define EM_RS08          132 // Freescale RS08
#define EM_SHARC         133 // Analog Devices SHARC
#define EM_ECOG2         134 // Cyan Technology eCOG2
#define EM_SCORE7        135 // Sunplus S+core7
#define EM_DSP24         136 // New Japan Radio (NJR) 24-bit DSP
#define EM_VIDEOCORE3    137 // Broadcom VideoCore III
#define EM_LATTICEMICO32 138 // Lattice FPGA
#define EM_SE_C17        139 // Seiko Epson C17
#define EM_TI_C6000      140 // TI TMS320C6000
#define EM_TI_C2000      141 // TI TMS320C2000
#define EM_TI_C5500      142 // TI TMS320C55x
#define EM_TI_ARP32      143 // TI Application Specific RISC
#define EM_TI_PRU        144 // TI Programmable Realtime Unit
#define EM_MMDSP_PLUS    160 // STMicroelectronics 64bit VLIW DSP
#define EM_CYPRESS_M8C   161 // Cypress M8C
#define EM_R32C          162 // Renesas R32C
#define EM_TRIMEDIA      163 // NXP TriMedia
#define EM_QDSP6         164 // QUALCOMM DSP6
#define EM_8051          165 // Intel 8051
#define EM_STXP7X        166 // STMicroelectronics STxP7x
#define EM_NDS32         167 // Andes Technology
#define EM_ECOG1         168 // Cyan Technology eCOG1X
#define EM_ECOG1X        168 // Cyan Technology eCOG1X
#define EM_MAXQ30        169 // Dallas Semiconductor MAXQ30
#define EM_XIMO16        170 // New Japan Radio (NJR) 16-bit DSP
#define EM_MANIK         171 // M2000 Reconfigurable RISC
#define EM_CRAYNV2       172 // Cray Inc. NV2
#define EM_RX            173 // Renesas RX
#define EM_METAG         174 // Imagination Technologies META
#define EM_MCST_ELBRUS   175 // MCST Elbrus
#define EM_ECOG16        176 // Cyan Technology eCOG16
#define EM_CR16          177 // National Semiconductor CompactRISC CR16
#define EM_ETPU          178 // Freescale Extended Time Processing Unit
#define EM_SLE9X         179 // Infineon Technologies SLE9X
#define EM_L10M          180 // Intel L10M
#define EM_K10M          181 // Intel K10M
#define EM_AARCH64       183 // ARM 64-bit architecture (AARCH64)
#define EM_AVR32         185 // Atmel 32-bit
#define EM_STM8          186 // STMicroeletronics STM8
#define EM_TILE64        187 // Tilera TILE64
#define EM_TILEPRO       188 // Tilera TILEPro
#define EM_MICROBLAZE    189 // Xilinx MicroBlaze
#define EM_CUDA          190 # NVIDIA CUDA
#define EM_TILEGX        191 // Tilera TILE-Gx
#define EM_CLOUDSHIELD   192 // CloudShield
#define EM_COREA_1ST     193 // KIPO-KAIST Core-A 1st gen
#define EM_COREA_2ND     194 // KIPO-KAIST Core-A 2nd gen
#define EM_ARC_COMPACT2  195 // Synopsys ARCompact V2
#define EM_OPEN8         196 // Open8 8-bit RISC
#define EM_RL78          197 // Renesas RL78
#define EM_VIDEOCORE5    198 // Broadcom VideoCore V
#define EM_78KOR         199 // Renesas 78KOR
#define EM_56800EX       200 // Freescale 56800EX DSC
#define EM_BA1           201 // Beyond BA1
#define EM_BA2           202 // Beyond BA2
#define EM_XCORE         203 // XMOS xCORE
#define EM_MCHP_PIC      204 // Microchip 8-bit PIC
#define EM_INTEL205      205 // Reserved by Intel
#define EM_INTEL206      206 // Reserved by Intel
#define EM_INTEL207      207 // Reserved by Intel
#define EM_INTEL208      208 // Reserved by Intel
#define EM_INTEL209      209 // Reserved by Intel
#define EM_KM32          210 // KM211 KM32
#define EM_KMX32         211 // KM211 KMX32
#define EM_KMX16         212 // KM211 KMX16
#define EM_KMX8          213 // KM211 KMX8
#define EM_KVARC         214 // KM211 KVARC
#define EM_CDP           215 // Paneve CDP
#define EM_COGE          216 # Cognitive Smart Memory Processor
#define EM_COOL          217 # Bluechip Systems CoolEngine
#define EM_NORC          218 # Nanoradio Optimized RISC
#define EM_CSR_KALIMBA   219 # CSR Kalimba
#define EM_Z80           220 # Zilog Z80
#define EM_VISIUM        221 # Controls and Data Services VISIUMcore
#define EM_FT32          222 # FTDI Chip FT32
#define EM_MOXIE         223 # Moxie
#define EM_AMDGPU        224 # AMD GPU
#define EM_RISCV         243 # RISC-V
#define EM_BPF           247 # Linux BPF
#define EM_CSKY          252 # C-SKY
#define EM_LOONGARCH     258 # LoongArch
#define EM_FRV           0x5441 # Fujitsu FR-V


//
// ================ Program Header ===================
//

//
// Segment Types (p_type)
//
#define PT_NULL         0
#define PT_LOAD         1
#define PT_DYNAMIC      2
#define PT_INTERP       3
#define PT_NOTE         4
#define PT_SHLIB        5
#define PT_PHDR         6
#define PT_TLS          7
#define PT_LOOS         0x60000000
#define PT_HIOS         0x6fffffff
#define PT_LOPROC       0x70000000
#define PT_HIPROC       0x7fffffff

// [Custom] FreeBSD S4 image specific
#define PT_FREEBSD_S4_PCB        0x61000054
#define PT_FREEBSD_S4_TRAMPOLINE 0x61000055

//
// Segment Flags (p_flags)
//
#define PF_X            0x1           // Execute
#define PF_W            0x2           // Write
#define PF_R            0x4           // Read
#define PF_MASKOS       0x0ff00000    // Unspecified
#define PF_MASKPROC     0xf0000000    // Unspecified

//
// ================== Data Structures ===================
//

//
// ELF64 File Header
//
typedef struct {
    UINT8       e_ident[EI_NIDENT]; /* Magic number and other info */
    Elf64_Half  e_type;             /* Object file type */
    Elf64_Half  e_machine;          /* Architecture */
    Elf64_Word  e_version;          /* Object file version */
    Elf64_Addr  e_entry;            /* Entry point virtual address */
    Elf64_Off   e_phoff;            /* Program header table file offset */
    Elf64_Off   e_shoff;            /* Section header table file offset */
    Elf64_Word  e_flags;            /* Processor-specific flags */
    Elf64_Half  e_ehsize;           /* ELF header size in bytes */
    Elf64_Half  e_phentsize;        /* Program header table entry size */
    Elf64_Half  e_phnum;            /* Program header table entry count */
    Elf64_Half  e_shentsize;        /* Section header table entry size */
    Elf64_Half  e_shnum;            /* Section header table entry count */
    Elf64_Half  e_shstrndx;         /* Section header string table index */
} Elf64_Ehdr;

//
// ELF64 Program Header
//
typedef struct {
    Elf64_Word  p_type;             /* Segment type */
    Elf64_Word  p_flags;            /* Segment flags */
    Elf64_Off   p_offset;           /* Segment file offset */
    Elf64_Addr  p_vaddr;            /* Segment virtual address */
    Elf64_Addr  p_paddr;            /* Segment physical address */
    Elf64_Xword p_filesz;           /* Segment size in file */
    Elf64_Xword p_memsz;            /* Segment size in memory */
    Elf64_Xword p_align;            /* Segment alignment */
} Elf64_Phdr;

//
// S4 PCB Payload
//
typedef struct {
    UINT64  cr0;
    UINT64  cr3;
    UINT64  cr4;
    UINT64  rsp;
    UINT64  rip;
    UINT64  gsbase;
    UINT32  acpic_facs_hwsig;
} S4Pcb;

#endif
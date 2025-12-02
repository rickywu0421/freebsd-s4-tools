#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Protocol/PartitionInfo.h>
#include <Guid/FileInfo.h>

#include "elf64.h"

#define S4_IMAGE_NAME L"\\s4_image.bin"

// FreeBSD Swap Partition GUID: 516e7cb5-6ecf-11d6-8ff8-00022d09712b
EFI_GUID gFreeBSDSwapGuid = {
    0x516e7cb5, 0x6ecf, 0x11d6, {0x8f, 0xf8, 0x00, 0x02, 0x2d, 0x09, 0x71, 0x2b}
};

VOID
EFIAPI
AsmTransferControl (
  IN UINT64 EntryPoint,
  IN UINT64 Context
);

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
    EFI_STATUS                         Status;
    // EFI_LOADED_IMAGE_PROTOCOL          *LoadedImage       = NULL;
    // EFI_SIMPLE_FILE_SYSTEM_PROTOCOL    *FileSystem        = NULL;
    // EFI_FILE_PROTOCOL                  *Root              = NULL;
    // EFI_FILE_PROTOCOL                  *S4ImgFile         = NULL;
    UINTN                              HandleCount;
    EFI_HANDLE                         *HandleBuffer      = NULL;
    EFI_PARTITION_INFO_PROTOCOL        *PartInfo          = NULL;
    EFI_BLOCK_IO_PROTOCOL              *BlkIO             = NULL;
    EFI_BLOCK_IO_PROTOCOL              *SwapBlkIO         = NULL;
    Elf64_Ehdr                         Elf64Ehdr;
    Elf64_Phdr                         *Elf64PhdrTable    = NULL;
    EFI_PHYSICAL_ADDRESS               PcbPAddr           = 0;
    EFI_PHYSICAL_ADDRESS               TrampolinePAddr    = 0;

    Print(L"[Activator] Locate swap partition handle...\n");


    // ============================================================
    // Locate FreeBSD swap partition BlockIOProtocol
    // ============================================================
    Status = gBS->LocateHandleBuffer(ByProtocol,
                                     &gEfiBlockIoProtocolGuid,
                                     NULL,
                                     &HandleCount,
                                     &HandleBuffer);
    if (EFI_ERROR(Status)) {
      Print(L"Error: LocateHandleBuffer failed: %r\n", Status);
      goto ErrorExit;
    }

    Print(L"[Debug] Found %d BlockIO handles\n", HandleCount);

    for (UINTN i = 0; i < HandleCount; i++) {
      Status = gBS->HandleProtocol(HandleBuffer[i],
                          &gEfiPartitionInfoProtocolGuid,
                          (VOID **)&PartInfo);
      if (EFI_ERROR(Status)) {
        Print(L"[Debug] Handle %d: No PartitionInfo\n", i);
        continue;
      }
      
      if (PartInfo->Type == PARTITION_TYPE_GPT) {
        Print(L"[Debug] Handle %d: GPT Partition found\n", i);

        if (CompareGuid(&gFreeBSDSwapGuid, &PartInfo->Info.Gpt.PartitionTypeGUID)) {
          Status = gBS->HandleProtocol(HandleBuffer[i],
                                       &gEfiBlockIoProtocolGuid,
                                       (VOID **)&BlkIO);
          if (!EFI_ERROR(Status)) {
            Print(L"[Activator] Found FreeBSD Swap Partition at Handle %x\n", HandleBuffer[i]);
            SwapBlkIO = BlkIO;
            
            // According to UEFI 2.11 spec, Chapter 7.3.15, caller to 
            // gBS->LocateHandleBuffer is responsible to free the pool.
            FreePool(HandleBuffer);
            break;
          }
          else {
            Print(L"Error: Failed to open BlockIO on Swap Handle: %r\n", Status);
          }
        }
      }
    }

    if (!SwapBlkIO) {
      Print(L"Error: FreeBSD Swap Partition Not Found\n");
      goto ErrorExit;
    }

    // ============================================================
    // From here, We have located the FreeBSD swap partition BlockIOProtocol.
    // Now let's read the S4 image from swap partition.
    // Phase 1: Read the ELF64 header from the S4 image.
    // Phase 2: Read the program headers from the S4 image, and get the
    //          filesz and paddr for segments.
    // Phase 3: Based on the type of the program header, perform different operations:
    //          - PT_LOAD: Allocate physical memory pages and load the segment into them.
    //          - PT_FREEBSD_S4_PCB: Like PT_LOAD, allocate a physical memory page,
    //                               and load the semgnet (PCB payload) into it.
    //          - PT_FREEBSD_S4_TRAMPOLINE: Only record the trampoline address (paddr)
    // ============================================================


    // Phase 1: Read the ELF64 header from the S4 image.
    UINT32 MediaId = SwapBlkIO->Media->MediaId;
    UINT32 BlockSize = SwapBlkIO->Media->BlockSize;
    EFI_LBA Lba = 0; // XXX: We assume the S4 image is at the beginning of the swap partition


    VOID *BlockBuffer = AllocatePool(BlockSize);
    if (!BlockBuffer)
      goto ErrorExit;

    SwapBlkIO->ReadBlocks(SwapBlkIO,
                          MediaId,
                          Lba,
                          BlockSize,
                          BlockBuffer);

    CopyMem(&Elf64Ehdr, BlockBuffer, sizeof(Elf64_Ehdr));

    FreePool(BlockBuffer);

    // Validate the ELF header of the S4 image
    if (((UINT8 *)&Elf64Ehdr)[EI_MAG0] != ELFMAG0 || 
        ((UINT8 *)&Elf64Ehdr)[EI_MAG1] != ELFMAG1 ||
        ((UINT8 *)&Elf64Ehdr)[EI_MAG2] != ELFMAG2 ||
        ((UINT8 *)&Elf64Ehdr)[EI_MAG3] != ELFMAG3) {
      Print(L"Error: File %s is not an ELF file\n", S4_IMAGE_NAME);
      goto ErrorExit;
    }

    if (((UINT8 *)&Elf64Ehdr)[EI_CLASS] != ELFCLASS64) {
      Print(L"Error: We only support 64-bits ELF files for now\n");
      goto ErrorExit;
    }

    if (((UINT8 *)&Elf64Ehdr)[EI_DATA] != ELFDATA2LSB) {
      Print(L"Error: We only support LSB ELF files for now\n");
      goto ErrorExit;
    }

    if (Elf64Ehdr.e_machine != EM_X86_64) {
      Print(L"Error: We only support x86_64 ELF files for now\n");
      goto ErrorExit;
    }


    // Phase 2: Read the program headers from the S4 image, and get the
    //          filesz and paddr for segments.

    // Allocate heap space for program headers table
    // Note that we need to read extra data if the offset of
    // the program header table dosen't equal to the block size.
    UINT16 PhdrNum = Elf64Ehdr.e_phnum;
    UINT16 PhdrEntSize = Elf64Ehdr.e_phentsize;
    UINT64 PhdrOffset = Elf64Ehdr.e_phoff;

    UINTN PhdrTableSize = PhdrNum * PhdrEntSize;
    EFI_LBA StartLba = PhdrOffset / BlockSize;
    UINTN OffsetInBlock = PhdrOffset % BlockSize;
    UINTN TotalReadSize = OffsetInBlock + PhdrTableSize;
    if (TotalReadSize % BlockSize != 0)
      TotalReadSize = (TotalReadSize / BlockSize + 1) * BlockSize;

    VOID *PhdrTableBlockBuffer = AllocatePool(TotalReadSize);
    if (!PhdrTableBlockBuffer)
      goto ErrorExit;

    SwapBlkIO->ReadBlocks(SwapBlkIO,
                          MediaId,
                          StartLba,
                          TotalReadSize,
                          PhdrTableBlockBuffer);


    Elf64PhdrTable = AllocatePool(PhdrTableSize);
    if (Elf64PhdrTable == NULL) {
      Print(L"Error: Out of memory!\n");
      goto ErrorExit;
    }

    CopyMem(Elf64PhdrTable, (UINT8 *)PhdrTableBlockBuffer + OffsetInBlock,
            sizeof(Elf64_Phdr) * PhdrNum);

    FreePool(PhdrTableBlockBuffer);


    Print(L"[Activator] %d program headers in %s. Parse program headers...\n", 
          PhdrNum, S4_IMAGE_NAME);
    
    // Phase 3: Based on the type of the program header, perform different operations:
    //          - PT_LOAD: Allocate physical memory pages and load the segment into them.
    //          - PT_FREEBSD_S4_PCB: Like PT_LOAD, allocate a physical memory page,
    //                               and load the semgnet (PCB payload) into it.
    //          - PT_FREEBSD_S4_TRAMPOLINE: Only record the trampoline address (paddr)
    for (UINT16 Num = 0; Num < PhdrNum; Num++) {
      Elf64_Phdr *CurrentPhdr = &Elf64PhdrTable[Num];

      EFI_PHYSICAL_ADDRESS PAddr = CurrentPhdr->p_paddr;
      UINTN FileSize = CurrentPhdr->p_filesz;
      UINTN MemSize = CurrentPhdr->p_memsz;
      UINTN SegmentOffset = CurrentPhdr->p_offset;

      switch (CurrentPhdr->p_type) {
        case PT_LOAD: {
          UINTN Pages = EFI_SIZE_TO_PAGES(MemSize);

          Print(L"[Activator] Segment %d: PT_LOAD segment\n", Num);
          Print(L"[Activator] Allocate %d pages for PT_LOAD segment at %lx...\n", Pages, PAddr);

          Status = gBS->AllocatePages(AllocateAddress,
                                      EfiLoaderData,
                                      Pages,
                                      &PAddr);
          if (EFI_ERROR(Status)) {
            Print(L"Error: Failed to allocate pages at 0x%lx: %r\n", CurrentPhdr->p_paddr, Status);
            goto ErrorExit;
          }

          Print(L"[Activator] Read PT_LOAD segment at file offset %d to %lx...\n", SegmentOffset, PAddr);
          
          // We assume the PT_LOAD segment offset is align with block size
          EFI_LBA StartLba = SegmentOffset / BlockSize;
          UINTN ExtraSize = BlockSize - (FileSize % BlockSize);
          VOID *ExtraStart = (UINT8 *)PAddr + FileSize;
          UINTN TotalReadSize = FileSize;
          if (FileSize % BlockSize != 0)
            TotalReadSize = (TotalReadSize / BlockSize + 1) * BlockSize;

          Status = SwapBlkIO->ReadBlocks(SwapBlkIO,
                                         MediaId,
                                         StartLba,
                                         TotalReadSize,
                                         (VOID *)PAddr);
          if (EFI_ERROR(Status)) {
            Print(L"Error: Read PT_LOAD segment (size = %d) to %lx failed: %r\n", 
                    FileSize, PAddr, Status);
            goto ErrorExit;
          }

          ZeroMem(ExtraStart, ExtraSize);

          Print(L"[Activator] Successfully loaded PT_LOAD segment at file offset %d to %lx...\n", 
                SegmentOffset, PAddr);

          break;
        }
        case PT_FREEBSD_S4_PCB: {
          Print(L"[Activator] Segment %d: PT_FREEBSD_S4_PCB segment\n", Num);

          UINTN Pages = EFI_SIZE_TO_PAGES(FileSize);

          // XXX: not knowing if putting PCB payload in UEFI allocated pages
          // would be a good idea.
          Status = gBS->AllocatePages(AllocateAnyPages,
                                      EfiLoaderData,
                                      Pages,
                                      &PcbPAddr);
          if (EFI_ERROR(Status)) {
            Print(L"ERROR: AllocatePages for PCB payload failed\n");
            goto ErrorExit;
          }

          // We assume the PT_FREEBSD_S4_PCB segment offset is align with block size
          EFI_LBA StartLba = SegmentOffset / BlockSize;
          UINTN ExtraSize = BlockSize - (FileSize % BlockSize);
          VOID *ExtraStart = (UINT8 *)PcbPAddr + FileSize;
          UINTN TotalReadSize = FileSize;
          if (FileSize % BlockSize != 0)
            TotalReadSize = (TotalReadSize / BlockSize + 1) * BlockSize;

          Status = SwapBlkIO->ReadBlocks(SwapBlkIO,
                                         MediaId,
                                         StartLba,
                                         TotalReadSize,
                                         (VOID *)PcbPAddr);
          if (EFI_ERROR(Status)) {
            Print(L"Error: Read PT_FREEBSD_S4_PCB segment (size = %d) to %lx failed: %r\n", 
                    FileSize, PcbPAddr, Status);
            goto ErrorExit;
          }

          ZeroMem(ExtraStart, ExtraSize);

          Print(L"PCB Loaded at: 0x%lx\n", PcbPAddr);

          break;
        }
        case PT_FREEBSD_S4_TRAMPOLINE: {
          Print(L"[Activator] Segment %d: PT_FREEBSD_S4_TRAMPOLINE segment\n", Num);

          TrampolinePAddr = PAddr;

          break;
        }
        default: 
          break;
      }
    }

    if (PcbPAddr == 0 || TrampolinePAddr == 0) {
      Print(L"Error: PCB or Trampoline missing in S4 image!\n");
      goto ErrorExit;
    }

    Print(L"[Activator] Ready to jump to S4 trampoline!\n");
    Print(L"Trampoline physical address: %lx\n", TrampolinePAddr);
    Print(L"Press any key to jump to S4 Image...\n");

    gST->ConIn->Reset(gST->ConIn, FALSE);
    EFI_INPUT_KEY Key;
    while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) == EFI_NOT_READY);


    // ============================================================
    // Exit Boot Services & Jump
    // ============================================================

    // The memory map here is only for the sake of ExitBootServices() API
    // since when resume from S4, OS will use the original memory map,
    // which has already been in the system memory.
    UINTN  MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN  MapKey;
    UINTN  DescriptorSize;
    UINT32 DescriptorVersion;

    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey,
                               &DescriptorSize, &DescriptorVersion);
    if (Status != EFI_BUFFER_TOO_SMALL) {
      goto ErrorExit;
    }

    MemoryMapSize += 8 * DescriptorSize;
    MemoryMap = AllocatePool(MemoryMapSize);

    Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey,
                                 &DescriptorSize, &DescriptorVersion);
    if (EFI_ERROR(Status)) {
      Print(L"Error: GetMemoryMap failed\n");
      goto ErrorExit;
    }

    Status = gBS->ExitBootServices(ImageHandle, MapKey);
    if (EFI_ERROR(Status)) {
      Status = gBS->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey,
                                 &DescriptorSize, &DescriptorVersion);
      if (!EFI_ERROR(Status)) {
        Status = gBS->ExitBootServices(ImageHandle, MapKey);
      }
    }

    if (EFI_ERROR(Status)) {
      goto ErrorExit;
    }

    AsmTransferControl(TrampolinePAddr, PcbPAddr);

    CpuDeadLoop();

ErrorExit:
    if (Elf64PhdrTable != NULL) {
        FreePool(Elf64PhdrTable);
    }
    if (HandleBuffer != NULL) {
        FreePool(HandleBuffer);
    }

    return Status;
}
# Two-Level Page Table Simulator

A C++ simulator of virtual-to-physical address translation through a two-level page table, the way an operating system and MMU find where a virtual address really lives in memory.

Built for Computing Systems Architecture (NYU Tandon, Fall 2026) with my lab partner [@limb0007](https://github.com/limb0007). The course provided the program skeleton and test files; we implemented the translation logic and memory reads together.

## What it does

For each 14-bit virtual address in a request file, it:

1. Splits the address into an outer page index (4 bits), an inner page index (4 bits) and a page offset (6 bits).
2. Reads the outer page table entry at `PTBR + outer index × 4` and checks its valid bit.
3. If valid, follows it to the inner page table and reads that entry's valid bit.
4. If both are valid, builds the 12-bit physical address from the frame number and the offset, and reads the 32-bit value stored there.

It writes one line per request to `pt_results.txt` (outer valid, inner valid, physical address, value), for example:

```
0, 0, 0x000, 0x00000000
1, 1, 0x19c, 0x00000800
1, 0, 0x000, 0x00000000
```

Invalid entries stop the walk early and report `0x000` and `0x00000000`.

## How it works

- **Physical memory:** 64 KB of bytes loaded from `pt_initialize.txt`. Words are read big-endian, four bytes at a time.
- **Page table base register (PTBR):** a 12-bit address read from `PTBR.txt`, where the outer table starts.
- **Bit manipulation:** shifts and masks on `std::bitset` values extract the indexes, valid bits and frame numbers.

## Running it

The program reads `pt_initialize.txt` from the folder you run it in, so run it inside `sample`, which has a full test set:

```bash
make
cd sample
../PageTable pt_requests.txt PTBR.txt
diff pt_results.txt "expected results/pt_results.ans.txt"
```

## Built with

C++ (g++), Make

## Authors

Uzoma Peters ([@JoshuaPeters-hub](https://github.com/JoshuaPeters-hub)) and [@limb0007](https://github.com/limb0007)

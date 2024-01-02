# MeMS - Memory Management System

## Introduction

This repository contains the implementation of a custom memory management system (MeMS) using the C programming language. MeMS utilizes the system calls `mmap` and `munmap` for memory allocation and deallocation, respectively.

## Constraints and Requirements

- **System Calls:** MeMS can only use the system calls `mmap` and `munmap` for memory management.
- **Page Size:** MeMS should request memory from the OS using `mmap` in multiples of the system's `PAGE_SIZE`.
- **Free List Structure:** MeMS maintains a doubly linked list known as the free list, comprising main chains and sub-chains.
- **Memory Allocation:** MeMS allocates memory to user programs by reusing segments from the free list or by requesting more memory from the OS using `mmap`.
- **MeMS Virtual and Physical Addresses:** MeMS maintains a mapping from MeMS virtual address space to MeMS physical address space.

## Free List Structure

The free list comprises main chains and sub-chains. Each main chain represents memory allocated to a user program or a hole. Sub-chains contain segments of memory, marked as either PROCESS (allocated to user program) or HOLE (unallocated).

### Main Chain Features:

- A new node is added to the main chain when MeMS requests memory from the OS.
- Each node points to a sub-chain.
- Nodes represent allocated memory to user programs or holes.

### Sub-Chain Features:

- Segments represent parts of memory, marked as PROCESS or HOLE.
- HOLE segments can be reallocated to new requests, creating new HOLE segments if space remains after allocation.
- Avoids memory fragmentation within the free list.

## MeMS Virtual Address and MeMS Physical Address

- MeMS virtual address is returned by `mmap` and used by the user process.
- MeMS maintains a mapping from virtual address space to physical address space.

## Function Implementations

### 1. `void mems_init()`

- Initializes required parameters for MeMS.
- Parameters include the head of the free list and the starting MeMS virtual address.

### 2. `void mems_finish()`

- Called at the end of the MeMS system.
- Unmaps allocated memory using `munmap` system call.

### 3. `void* mems_malloc(size_t size)`

- Allocates memory of specified size.
- Reuses a segment from the free list if available; otherwise, uses `mmap` to allocate more memory.

### 4. `void mems_free(void* ptr)`

- Frees memory pointed by `ptr`.
- Marks corresponding sub-chain node as HOLE.

### 5. `void mems_print_stats()`

- Prints total mapped pages, unused memory in bytes, and details about each node and segment in the free list.

### 6. `void *mems_get(void* v_ptr)`

- Returns MeMS physical address mapped to `v_ptr` (MeMS virtual address).

## Submission Guidelines

1. Download the skeleton template from [GitHub Repository](https://github.com/Rahul-Agrawal-09/MeMS-Skeleton-code).
2. Submit the C code with detailed explanations of the approach.
3. Provide a demonstration of the implementation with test cases covering various scenarios.

## Grading Criteria

- Documentation
- Viva/Demo
- Error handling
- Handling edge cases (e.g., joining adjacent hole nodes)
- Correct `mems_virtual_address` corresponding to `mems_physical_address`
- Compilation and output format
- Use of defined data structures
- GitHub version control

## Note

1. Use the `PAGE_SIZE` macro for flexibility.
2. Use GitHub for version control.

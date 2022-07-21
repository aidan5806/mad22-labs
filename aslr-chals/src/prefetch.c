/*
 * MIT 6.888 Lab 5: Address Space Layout Randomization
 * Part 1B: Prefetch
 *
 * Modified for MAD 2022 at ISCA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <mmintrin.h>
#include <xmmintrin.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#include <x86intrin.h>
#include "mad_aslr.h"

inline __attribute__((always_inline)) void prefetch(void* p)
{
    asm volatile ("prefetchnta (%0)" : : "r" (p));
    asm volatile ("prefetcht2 (%0)" : : "r" (p));
}

inline __attribute__((always_inline)) uint64_t rdtsc_begin() {
  uint64_t a, d;
  asm volatile ("mfence\n\t"
    "RDTSCP\n\t"
    "mov %%rdx, %0\n\t"
    "mov %%rax, %1\n\t"
    "xor %%rax, %%rax\n\t"
    "CPUID\n\t"
    : "=r" (d), "=r" (a)
    :
    : "%rax", "%rbx", "%rcx", "%rdx");
  a = (d<<32) | a;
  return a;
}

inline __attribute__((always_inline)) uint64_t rdtsc_end() {
  uint64_t a, d;
  asm volatile(
    "xor %%rax, %%rax\n\t"
    "CPUID\n\t"
    "RDTSCP\n\t"
    "mov %%rdx, %0\n\t"
    "mov %%rax, %1\n\t"
    "mfence\n\t"
    : "=r" (d), "=r" (a)
    :
    : "%rax", "%rbx", "%rcx", "%rdx");
  a = (d<<32) | a;
  return a;
}

/*
 * Lab 5 Part 1
 * Find and return the single mapped address within the range [low_bound, upper_bound).
 */
uint64_t find_address(uint64_t low_bound, uint64_t high_bound) {
    size_t min_val = 100000;
    uint64_t min_addr;

    for (uint64_t addr = low_bound; addr < high_bound; addr += PAGE_SIZE) {
        // TODO: Figure out if "addr" is the correct address or not.
        void * ptr = (void *)addr;

        size_t time = rdtsc_begin();
        prefetch(ptr);
        size_t delta = rdtsc_end() - time;

        if (delta < min_val) {
            min_val = delta;
            min_addr = addr;
        }
    }

    return min_addr;
}

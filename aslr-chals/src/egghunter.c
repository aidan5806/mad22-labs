/*
 * MIT 6.888 Lab 5: Address Space Layout Randomization
 * Part 1A: Egghunter
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

/*
 * Lab 5 Part 1
 * Find and return the single mapped address within the range [low_bound, upper_bound).
 */
uint64_t find_address(uint64_t low_bound, uint64_t high_bound) {
    for (uint64_t addr = low_bound; addr < high_bound; addr += PAGE_SIZE) {
        // printf("Trying address: %lx\n", addr);
        access((void *)addr, 0);

        if (errno != EFAULT) {
            return addr;
        }
    }

    return 0;
}

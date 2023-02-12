#include <stdio.h>

#include <pico/rand.h>
#include <pico/stdlib.h>

static uint32_t histogram[16];

int main() {
    stdio_init_all();

    printf("PICO_RAND_ENTROPY_SRC_ROSC = %d\n", PICO_RAND_ENTROPY_SRC_ROSC);
    printf("PICO_RAND_ENTROPY_SRC_TIME = %d\n", PICO_RAND_ENTROPY_SRC_TIME);
    printf("PICO_RAND_ENTROPY_SRC_BUS_PERF_COUNTER = %d\n", PICO_RAND_ENTROPY_SRC_BUS_PERF_COUNTER);
    printf("PICO_RAND_SEED_ENTROPY_SRC_ROSC = %d\n", PICO_RAND_SEED_ENTROPY_SRC_ROSC);
    printf("PICO_RAND_SEED_ENTROPY_SRC_TIME = %d\n", PICO_RAND_SEED_ENTROPY_SRC_TIME);
    printf("PICO_RAND_SEED_ENTROPY_SRC_BOARD_ID = %d\n", PICO_RAND_SEED_ENTROPY_SRC_BOARD_ID);
    printf("PICO_RAND_SEED_ENTROPY_SRC_RAM_HASH = %d\n\n", PICO_RAND_SEED_ENTROPY_SRC_RAM_HASH);

    printf("Using get_rand_64()\n\n");

    for (int pass = 1; pass <= 2; ++pass) {
        printf("Upper 4 bits - pass %d\n", pass);
        for (int i = 0; i < 16; ++i)
            histogram[i] = 0;
        for (int i = 0; i < 1000000; ++i)
            ++histogram[get_rand_64() >> 60];
        for (int i = 0; i < 16; ++i)
            printf("%2d - %d\n", i, histogram[i]);
    }
    for (int pass = 1; pass <= 2; ++pass) {
        printf("Lower 4 bits - pass %d\n", pass);
        for (int i = 0; i < 16; ++i)
            histogram[i] = 0;
        for (int i = 0; i < 1000000; ++i)
            ++histogram[get_rand_64() & 0xf];
        for (int i = 0; i < 16; ++i)
            printf("%2d - %d\n", i, histogram[i]);
    }
}

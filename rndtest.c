#include <stdio.h>

#include <pico/rand.h>
#include <pico/stdlib.h>

static uint32_t histogram[64];

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

    printf("Upper 4 bits distribution\n");
    for (int i = 0; i < 16; ++i)
        histogram[i] = 0;
    for (int i = 0; i < 1000000; ++i)
        ++histogram[get_rand_64() >> 60];
    for (int i = 0; i < 16; ++i)
        printf("%2d - %d\n", i, histogram[i]);
    printf("\nLower 4 bits distribution\n");
    for (int i = 0; i < 16; ++i)
        histogram[i] = 0;
    for (int i = 0; i < 1000000; ++i)
        ++histogram[get_rand_64() & 0xf];
    for (int i = 0; i < 16; ++i)
        printf("%2d - %d\n", i, histogram[i]);
    printf("\nSingle bit distribution\n");
    int ones = 0, zeros = 0;
    for (int i = 0; i < 1000000; ++i) {
        int64_t n = get_rand_64();
        for (int j = 0; j < 64; ++j) {
            if (n < 0)
                ++ones;
            else
                ++zeros;
            n <<= 1;
        }
    }
    printf("zeros = %d\n", zeros);
    printf("ones  = %d\n", ones);
    printf("\nRun lengths distributions\n");
    for (int i = 0; i < 32; ++i)
        histogram[i] = 0;
    int last = -1, last_count;
    for (int i = 0; i < 1000000; ++i) {
        int64_t n = get_rand_64();
        for (int j = 0; j < 64; ++j) {
            int next = n < 0 ? 1 : 0;
            n <<= 1;
            if (last < 0) {
                last = next;
                last_count = 1;
                continue;
            }
            if (next == last) {
                ++last_count;
                continue;
            }
            if (last_count > 31)
                last_count = 31;
            ++histogram[last * 32 + last_count];
            last = next;
            last_count = 1;
        }
    }
    if (last_count > 31)
        last_count = 31;
    ++histogram[last * 32 + last_count];
    double expected = 1000000.0 * 64 / 4;
    for (int i = 1; i < 32; ++i) {
        printf("%2d - zeros %-7d  ones %-7d  total %-8d  expected %.0f\n", i, histogram[i],
               histogram[i + 32], histogram[i] + histogram[i + 32], expected);
        expected /= 2.0;
    }
}

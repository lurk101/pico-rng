#include <math.h>
#include <stdio.h>

#include <pico/rand.h>
#include <pico/stdlib.h>

static uint32_t histogram[64];

static const int ITERATIONS = 1000000;

static double sum;
static double sq_sum;

static void std_dev_init(void) {
    sum = 0;
    sq_sum = 0;
}

static void std_dev_calc(int v) {
    sum += v;
    sq_sum += (double)v * v;
}

static double std_dev_mean(int n) { return sum / n; }

static double std_dev(int n) {
    double mean = sum / n;
    return sqrt(sq_sum / n - mean * mean);
}

int main() {
    stdio_init_all();

    printf("Pico SDK version %d.%d.%d\n\n", PICO_SDK_VERSION_MAJOR, PICO_SDK_VERSION_MINOR,
           PICO_SDK_VERSION_REVISION);

    printf("PICO_RAND_ENTROPY_SRC_ROSC = %d\n", PICO_RAND_ENTROPY_SRC_ROSC);
    printf("PICO_RAND_ENTROPY_SRC_TIME = %d\n", PICO_RAND_ENTROPY_SRC_TIME);
    printf("PICO_RAND_ENTROPY_SRC_BUS_PERF_COUNTER = %d\n", PICO_RAND_ENTROPY_SRC_BUS_PERF_COUNTER);
    printf("PICO_RAND_SEED_ENTROPY_SRC_ROSC = %d\n", PICO_RAND_SEED_ENTROPY_SRC_ROSC);
    printf("PICO_RAND_SEED_ENTROPY_SRC_TIME = %d\n", PICO_RAND_SEED_ENTROPY_SRC_TIME);
    printf("PICO_RAND_SEED_ENTROPY_SRC_BOARD_ID = %d\n", PICO_RAND_SEED_ENTROPY_SRC_BOARD_ID);
    printf("PICO_RAND_SEED_ENTROPY_SRC_RAM_HASH = %d\n\n", PICO_RAND_SEED_ENTROPY_SRC_RAM_HASH);

    printf("Using get_rand_64()\n\n");

    printf("Monte carlo test. The final result should aproximate the value of"
           " π=3.14159265358979323846\n");
    uint32_t in_circle = 0;
    for (int i = 0; i < ITERATIONS; ++i) {
        double x = get_rand_64() / 18446744073709551616.0;
        double y = get_rand_64() / 18446744073709551616.0;
        if (x * x + y * y <= 1.0)
            ++in_circle;
    }
	double r = in_circle / (double)ITERATIONS * 4.0;
    printf("Result %f, diff %f\n\n", r, fabs(r - 3.14159265358979323846));

    // Bucket test
    // The focus of this test is to determine that values are evenly distributed
    // across the range
    printf("Upper 4 bits distribution. All buckets should be aproximately equal\n");
    for (int i = 0; i < 16; ++i)
        histogram[i] = 0;
    for (int i = 0; i < ITERATIONS; ++i)
        ++histogram[get_rand_64() >> 60];
    std_dev_init();
    for (int i = 0; i < 16; ++i) {
        std_dev_calc(histogram[i]);
        printf("%2d - %-8d expected %-.0f\n", i, histogram[i], ITERATIONS / 16.0);
    }
    printf("Mean      %.2f\n", std_dev_mean(16));
    printf("Std. dev. %.2f\n", std_dev(16));

    printf("\nLower 4 bits distribution\n");
    for (int i = 0; i < 16; ++i)
        histogram[i] = 0;
    for (int i = 0; i < ITERATIONS; ++i)
        ++histogram[get_rand_64() & 0xf];
    std_dev_init();
    for (int i = 0; i < 16; ++i) {
        std_dev_calc(histogram[i]);
        printf("%2d - %-8d expected %-.0f\n", i, histogram[i], ITERATIONS / 16.0);
    }
    printf("Mean      %.2f\n", std_dev_mean(16));
    printf("Std. dev. %.2f\n", std_dev(16));

    // Frequency (Monobit) Test.
    // The purpose of this test is to determine whether the number of ones and
    // zeros in a sequence are approximately the same as would be expected for
    // a truly random sequence.
    printf(
        "\nSingle bit distribution. There should be aproximately the same number of 0s and 1s\n");
    int ones = 0, zeros = 0;
    for (int i = 0; i < ITERATIONS; ++i) {
        int64_t n = get_rand_64();
        for (int j = 0; j < 64; ++j) {
            if (n < 0)
                ++ones;
            else
                ++zeros;
            n <<= 1;
        }
    }
    printf("zeros = %-8d  expected %.0f\n", zeros, ITERATIONS * 64.0 / 2);
    printf("ones  = %-8d  expected %.0f\n", ones, ITERATIONS * 64.0 / 2);
    std_dev_init();
    std_dev_calc(zeros);
    std_dev_calc(ones);
    printf("Mean      %.2f\n", std_dev_mean(2));
    printf("Std. dev. %.2f\n", std_dev(2));

    // Runs test
    // The focus of this test is the total number of runs in the sequence, where
    // a run is an uninterrupted sequence of identical bits. A run of length k
    // consists of exactly k identical bits and is bounded before and after with
    // a bit of the opposite value. The purpose of the runs test is to determine
    // whether the number of runs of ones and zeros of various lengths is as
    // expected for a random sequence
    printf("\nRun lengths distributions. The number of 0 runs and 1 runs should\n"
           "be aproximately the same, and total runs should be aproximately as\n"
           "expected\n");
    for (int i = 0; i < 32; ++i)
        histogram[i] = 0;
    int last = -1, last_count;
    for (int i = 0; i < ITERATIONS; ++i) {
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
    double expected = ITERATIONS * 64.0 / 4;
    for (int i = 1; i < 32; ++i) {
        printf("%2d - zeros %-7d  ones %-7d  total %-8d  expected %.0f\n", i, histogram[i],
               histogram[i + 32], histogram[i] + histogram[i + 32], expected);
        expected /= 2.0;
    }
}

#include <stdio.h>
#include <stdint.h>

uint64_t fibonacci(int n) {
    uint64_t a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        uint64_t temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

int main() {
    int n = 70;
    printf("Fibonacci(%d) = %lu\n", n, fibonacci(n));
    return 0;
}

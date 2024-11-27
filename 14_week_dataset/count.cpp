#include <stdio.h>

int main() {
    double m1 = 1.0;
    double m2 = 1e18;
    double v1 = 0;
    double v2 = -12312313;
    long count = 0;

    while (v1 < 0 || v2 < 0 || v1 > v2) {
        if (count % 2 == 1) {
            v1 = -v1;
        } else {
            double temp_v1 = (v1 * (m1 - m2) + 2 * m2 * v2) / (m1 + m2);
            double temp_v2 = (v2 * (m2 - m1) + 2 * m1 * v1) / (m1 + m2);
            v1 = temp_v1;
            v2 = temp_v2;
        }
        count++;
    }

    printf("충돌 횟수: %ld\n", count);

    return 0;
}

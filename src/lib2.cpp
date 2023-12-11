#include "../include/lib.hpp"

extern "C" {
    char* itoa(int val, int base){
        static char buf[100000] = {0};
        int i = 30;

        for(; val && i ; --i, val /= base)
            buf[i] = "0123456789abcdef"[val % base];

        return &buf[i + 1];

    }

    char* translation(long x) {
        return itoa(x, 3);
    }

    float e(int x) {
        float res = 1;
        for (int i = 1; i < x; ++i) {
            res += (1 / tgamma(i + 1));
        }
        return res;
    }
}
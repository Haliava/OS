#include "../include/lib.hpp"

// указываем сишное соглашение об именах
extern "C" {
    char* itoa(int val, int base){
        static char buf[100000] = {0};
        int i = 30;

        for(; val && i ; --i, val /= base)
            buf[i] = "0123456789abcdef"[val % base];

        return &buf[i + 1];

    }

    char* translation(long x) {
        return itoa(x, 2);
    }

    float e(int x) {
        return pow((float) (1 + 1 / x), (float) x);
    }
}
#pragma once
#include <iostream>
#include <bitset>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
    // X from base 10 to base 2/3
    char* translation(long x);
    // calculate value of E
    float e(int x);
}
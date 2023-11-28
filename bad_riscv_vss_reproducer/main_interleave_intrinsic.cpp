#include <iostream>
#include <stdio.h>

void interleave(uint32_t src[], uint32_t dst[]);

int main(int argc, char **argv){
    uint32_t u0[] = {1, 2, 3, 4, 5, 6, 7, 8};
    uint32_t u1[] = {0, 0, 0, 0, 0, 0, 0, 0};
    interleave(u0, u1);

    int count = 0;
    for (int i = 0; i < 8; i++) {
        std::cout << "rez : i = " << i << " : " << u0[i] << " " << u1[i] << "\n";
    }


    return 0;
}

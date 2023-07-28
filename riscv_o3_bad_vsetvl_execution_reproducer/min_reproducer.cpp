#include <stdio.h>
#include <string.h>
alignas(128) float A[64] = {2,2,3,3,4,4,5,5,};
alignas(128) float B[64] = {-1,-1,-1,-1,1,1,1,1,};

extern "C" {

int __attribute__ ((noinline))  vsetvl_mispredicted_execution(long to_mispredict, float *a, float *b) {
    asm volatile("vsetivli zero, 4, e32, m1, tu, ma");
//    vsetivli t2,4,e32,m1,ta,mu
    if (to_mispredict <= 0) {
        asm volatile("vsetivli zero, 0, e32, m1, tu, ma");
//        vsetivli zero,0,e32,m1,ta,mu
    }
    asm volatile("vle32.v v8, (%[vb])" : : [vb] "r"(a));
    asm volatile("vse32.v v8, (%[vb])" : : [vb] "r"(b));
    return 0;
}

}

int main(int argc, char * argv[]) {
    int to_mispredict = 1;
    if (argc > 1) {
        if (strcmp(argv[1], "well_predicted") == 0)
            to_mispredict = 0;
    }
    vsetvl_mispredicted_execution(to_mispredict, A, B);

    printf("to_mispredict: %d, argv[1]: %s\n \
    	to_mispredict == 1(true) by default.\n\
	Say \"well_predicted\" to make to_mispredict == 0(false)),\nA[0:4] : [%f %f %f %f %f]\n",
           to_mispredict, argv[1], A[0], A[1], A[2], A[3], A[4]);
    printf("B[0:4] : [%f %f %f %f %f]\n", B[0], B[1], B[2], B[3], B[4]);
    return 0;
}

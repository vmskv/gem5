#include <riscv_vector.h>

void interleave(uint32_t src[], uint32_t dst[]){

    size_t vl = __riscv_vsetvl_e32m1(8);
    vuint32m1_t v1 = __riscv_vle32_v_u32m1(src, vl);
    vuint32m1_t v2 = __riscv_vle32_v_u32m1(src + vl, vl);
    __riscv_vsseg2e32_v_u32m1(dst, v2, v1, vl); // <= problem is here
}

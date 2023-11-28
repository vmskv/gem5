#build riscv rvv intrinsics toolchain
https://github.com/riscv-collab/riscv-gnu-toolchain/

export RISCV_TOOLS_REPO_DIR=<riscv_tools_repo_dir>
cd ${RISCV_TOOLS_REPO_DIR}
git checkout d0d07301f9df660396651273d52ea0403bf3d5a2
git submodule update --init --recursive

export RISCV_TOOLS_DIR=${HOME}/riscv64-gnu-toolchain
./configure --prefix=${RISCV_TOOLS_DIR} --with-arch=rv64gcv_zfh --enable-llvm --enable-linux --with-abi=lp64d
make -j6 linux

#build qemu
export QEMU_DIR=<qemu dir>
cd ${QEMU_DIR}
git checkout master  # (4705fc0c8511d073bee4751c3c974aab2b10a970)
git submodule update --init --recursive
mkdir build
cd build/
../configure
make -j8


#build gem5
export gem5_src_dir=<gem5_repo_dir>
cd ${gem5_src_dir}
git checkout vmskv/bad_interleave_reproducer
scons build/RISCV/gem5.opt -j 6 USE_RISCV_ISA=1


#build riscv binary
cd bad_riscv_vss_reproducer
${RISCV_TOOLS_DIR}/bin/clang++ -c interleave.cpp -O3 -o interleave.o -march=rv64gcv -mabi=lp64d
${RISCV_TOOLS_DIR}/bin/riscv64-unknown-linux-gnu-g++ main_interleave_intrinsic.cpp -o interleave_intrinsic_rvv interleave.o -O3 -march=rv64gcv -mabi=lp64d  -static
#${RISCV_TOOLS_DIR}/bin/riscv64-unknown-linux-gnu-objdump -D interleave_intrinsic_rvv > interleave_inrinsic_rvv.S
#${RISCV_TOOLS_DIR}/bin/riscv64-unknown-linux-gnu-objdump -d interleave_intrinsic_rvv > interleave_intrinsic_rvv.d.S

#run reproducer
QEMU_LD_PREFIX=${RISCV_TOOLS_DIR}/sysroot ${QEMU_DIR}/qemu-riscv64 interleave_intrinsic_rvv
rez : i = 0 : 1 5
rez : i = 1 : 2 1
rez : i = 2 : 3 6
rez : i = 3 : 4 2
rez : i = 4 : 5 7
rez : i = 5 : 6 3
rez : i = 6 : 7 8
rez : i = 7 : 8 4


${gem5_src_dir}/build/RISCV/gem5.opt --outdir=my_test ${gem5_src_dir}/configs/deprecated/example/se.py --cpu-type RiscvO3CPU --caches --l2cache --cmd=./interleave_intrinsic_rvv
..................................
gem5 version DEVELOP-FOR-23.1
gem5 compiled Nov 20 2023 10:27:13
gem5 started Nov 24 2023 00:41:41
..................................
**** REAL SIMULATION ****
..................................
rez : i = 0 : 1 0
rez : i = 1 : 2 0
rez : i = 2 : 3 0
rez : i = 3 : 4 0
rez : i = 4 : 5 0
rez : i = 5 : 6 0
rez : i = 6 : 7 0
rez : i = 7 : 8 0
Exiting @ tick 68849000 because exiting with last active thread context

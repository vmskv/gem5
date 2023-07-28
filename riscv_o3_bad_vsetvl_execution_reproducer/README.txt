https://github.com/vmskv/gem5/tree/vmskv/rvv_bad_o3_vsetvl_execuion
# build gem5: 
export gem5_src_dir=<gem5 dir>
cd ${gem5_src_dir}
scons build/RISCV/gem5.opt -j 8



#build reproducer binary

#prepare tools for build example
https://github.com/riscv-collab/riscv-gnu-toolchain/tree/master (d0d07301f9df660396651273d52ea0403bf3d5a2)
export RISCV_TOOLS_REPO_DIR=<riscv_tools_repo_dir>
cd ${RISCV_TOOLS_REPO_DIR}
export RISCV_TOOLS_DIR=${HOME}/riscv64-gnu-toolchain
./configure --prefix=${RISCV_TOOLS_DIR} --with-arch=rv64gcv --with-abi=lp64d
make -j6 linux

#build reproducer
cd ${gem5_src_dir}/riscv_o3_bad_vsetvl_execution_reproducer
${RISCV_TOOLS_DIR}/bin/riscv64-unknown-linux-gnu-g++ min_reproducer.cpp -o min_reproducer -O3 -march=rv64gcv -mabi=lp64d  -static





# 'well predicted' branch executed where vsetvl set to 0.
# Both O3 and Atomic set vl to 0 elements and the vector is not copied
${gem5_src_dir}/build/RISCV/gem5.opt --outdir=my_test --debug-flags=Exec --debug-file=min_reproducer_AtomicSimpleCPU_well_predictedExec.log ${gem5_src_dir}/configs/deprecated/example/se.py --cpu-type RiscvAtomicSimpleCPU --caches --l2cache --cmd=./min_reproducer --options=well_predicted

${gem5_src_dir}/build/RISCV/gem5.opt --outdir=my_test --debug-flags=Exec --debug-file=min_reproducer_O3CPU_well_predictedExec.log ${gem5_src_dir}/configs/deprecated/example/se.py --cpu-type RiscvO3CPU --caches --l2cache --cmd=./min_reproducer --options=well_predicted


# 'mis-predicted' branch. The branch where vsetvl set to 0 should not be executed.
# In Atomic vsetvl=0 doesn't execute and the vector is copied
# but in O3 vsetvl=0 executed and sqauashed afterwards but the 'vl' value in decoder remains zero
# and vle/vse instruction afterwards are decoded as 'vnop' (the vector is not copied)

${gem5_src_dir}/build/RISCV/gem5.opt --outdir=my_test --debug-flags=Exec --debug-file=min_reproducer_AtomicSimpleCPU_mis_predictedExec.log ${gem5_src_dir}/configs/deprecated/example/se.py --cpu-type RiscvAtomicSimpleCPU --caches --l2cache --cmd=./min_reproducer --options=mis_predicted

stdout:
........................................
to_mispredict: 1, argv[1]: mis_predicted
     	to_mispredict == 1(true) by default.
	Say "well_predicted" to make to_mispredict == 0(false)),
A[0:4] : [2.000000 2.000000 3.000000 3.000000 4.000000]
B[0:4] : [2.000000 2.000000 3.000000 3.000000 1.000000]



${gem5_src_dir}/build/RISCV/gem5.opt --outdir=my_test --debug-flags=Exec --debug-file=min_reproducer_O3CPU_mis_predictedExec.log ${gem5_src_dir}/configs/deprecated/example/se.py --cpu-type RiscvO3CPU --caches --l2cache --cmd=./min_reproducer --options=mis_predicted
(
or with more flags:
${gem5_src_dir}/build/RISCV/gem5.opt --outdir=my_test --debug-flags=Exec,FmtTicksOff,ExecEnable,ExecUser,ExecResult,ExecRegDelta,MemoryAccess,Registers,ExecSymbol,IEW,O3CPUAll,Rename --debug-file=min_reproducer_O3CPUExec_allflags.log ${gem5_src_dir}/configs/deprecated/example/se.py --cpu-type RiscvO3CPU --caches --l2cache --cmd=./min_reproducer --options=mis_predicted
)

stdout:
........................................
to_mispredict: 1, argv[1]: mis_predicted

     	to_mispredict == 1(true) by default.
	Say "well_predicted" to make to_mispredict == 0(false)),
A[0:4] : [2.000000 2.000000 3.000000 3.000000 4.000000]
B[0:4] : [-1.000000 -1.000000 -1.000000 -1.000000 1.000000]






min_reproducer binary added
min_reproducer objdump disassembler 
(${RISCV_TOOLS_DIR}/bin/riscv64-unknown-linux-gnu-objdump -D min_reproducer > min_reproducer_disassembler.s) added

debug tracing files in my_test directory added (min_reproducer_AtomicSimpleCPU_mis_predictedExec.log, min_reproducer_O3CPU_mis_predictedExec.log)

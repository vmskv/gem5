	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0_v1p0_zfh1p0_zve32f1p0_zve32x1p0_zve64d1p0_zve64f1p0_zve64x1p0_zvl128b1p0_zvl32b1p0_zvl64b1p0"
	.globl	interleave                      # -- Begin function interleave
	.p2align	1
	.type	interleave,@function
interleave:                             # @interleave
# %bb.0:
	vsetivli	a2, 8, e32, m1, ta, mu
	vle32.v	v9, (a0)
	slli	a2, a2, 2
	add	a0, a0, a2
	vle32.v	v8, (a0)
	vsseg2e32.v	v8, (a1)
	ret
.Lfunc_end0:
	.size	interleave, .Lfunc_end0-interleave
                                        # -- End function

# Makefile for conv_layer_direct

ifndef XILINX_SDX
	$(error XILINX_SDX is required and should point the SDAccel install location)
endif

SDA_FLOW = hw
XOCC = xocc
KERNEL_NAME = crp_layer_hwcn_cpfp
KERNEL_SRCS = $(KERNEL_NAME).cpp
NK = 1

#DSA = xilinx:adm-pcie-8k5:2ddr:3.2
DSA = xilinx:kcu1500:4ddr-xpr:4.0

XCLBIN_NAME=$(KERNEL_NAME)

INCLUDE_DIR=../../../include/

ifeq (${SDA_FLOW}, cpu_emu)
	XCL_OPT += -t sw_emu
	XCLBIN = ${XCLBIN_NAME}_cpu_emu.xclbin
else ifeq (${SDA_FLOW}, hw_emu)
	XCL_OPT += -t hw_emu
	XCLBIN = ${XCLBIN_NAME}_hw_emu.xclbin
else ifeq (${SDA_FLOW}, hw)
	XCL_OPT += -t hw
	XCLBIN = ${XCLBIN_NAME}.xclbin
endif

XCL_OPT += --platform ${DSA} --report estimate --nk ${KERNEL_NAME}:${NK} --kernel ${KERNEL_NAME} -I ${INCLUDE_DIR} -DSYNTHESIS -s -o ${XCLBIN}

${XCLBIN}: ${KERNEL_SRCS}
	${XOCC} ${XCL_OPT} ${KERNEL_SRCS}

clean:
	${RM} -rf ${XCLBIN_NAME}*.xclbin _xocc_${XCLBIN_NAME}_*.dir .Xil sdaccel_profile_summary.* system_estimate.xtxt

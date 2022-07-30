cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += dfu.o

ccflags-y += \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/bsp \
    -Iplatform/sys/ \
    -Iframework/common \
    -Iutils/crc16 \
    -Iprojects/folk/main \
    -Iapp/uart_comm

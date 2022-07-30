cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += boot.o
obj-y += dfu/

ccflags-y += \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/bsp \
    -Iplatform/sys/ \
    -Iprojects/boot/main/dfu \
    -Iframework/common \
    -Iapp/uart_comm

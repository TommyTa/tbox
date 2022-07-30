cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += comm_core.o
obj-y += comm_uart.o
obj-y += comm_box.o
obj-y += comm_tws.o

ccflags-y += \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/bsp \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS \
    -Iplatform/sys \
    -Iframework/common

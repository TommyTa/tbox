cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += sys_cmsis.o

ccflags-y += \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/sys \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/bsp/N32G4FR \

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += dtcmd.o

ccflags-y += \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/bsp \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS \
    -Iplatform/sys \
    -Iframework/comm \
    -Iframework/common \
    -Iutils/xstring \
    -Iframework/atcmd \
    -Iutils/crc16
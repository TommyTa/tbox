cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += trace_dump.o

ccflags-y += \
    -Iplatform/sys \
    -Iplatform/bsp \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS \
    -Iplatform/chip/CMSIS/core \
    -Iprojects/folk/main \
    -Iframework/common \
    -Iplatform/bsp \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iframework/dtcmd \
    -Iframework/trace


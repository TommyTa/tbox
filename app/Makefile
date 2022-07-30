cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += fota/
obj-y += trace_dump/
obj-y += ux/
obj-y += ui/

subdir-ccflags-y += \
    -Iframework/common \
    -Iframework/object \
    -Iframework/device/include \
    -Iplatform/sys \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS \
    -Icomponent/xlist \
    -Idrivers/include \
    -Iplatform/bsp \
    -Iframework/key

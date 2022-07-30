cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += led/
obj-y += fuelgauge/

subdir-ccflags-y += \
    -Iframework/device/include \
    -Iframework/common \
    -Iframework/object \
    -Icomponent/xlist \
    -Iplatform/sys \
    -Iplatform/bsp \
    -Idrivers/include

ccflags-y += \
    -Iplatform/os/FreeRTOS \
    -Iplatform/chip/CMSIS/core \

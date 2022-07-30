cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += object.o
obj-y += device.o
obj-y += driver.o

subdir-ccflags-y += \
    -Iframework/device/include \
    -Iframework/common \
    -Icomponent/xlist \
    -Iplatform/sys \
    -Iplatform/bsp \

ccflags-y += \
    -Iplatform/os/FreeRTOS \
    -Iplatform/chip/CMSIS/core \

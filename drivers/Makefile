cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += i2c/
obj-y += fuelgauge/
obj-y += led/

subdir-ccflags-y += \
    -Iframework/device \
    -Iframework/common \
    -Iframework/object \
    -Iplatform/sys \
    -Iplatform/bsp \
    -Idrivers/include \
    -Icomponent/xlist

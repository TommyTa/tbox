cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += fota.o

ccflags-y += \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/bsp \
    -Iplatform/os \
    -Iplatform/sys \
    -Iframework/comm \
    -Iframework/common \
    -Iframework/atcmd \
    -Iprojects/folk/main \
    -Iapp/fota/ \
    -Iutils/crc16

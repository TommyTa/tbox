cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += ux.o

ccflags-y += \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/bsp \
    -Iplatform/os \
    -Iplatform/sys \
    -Iframework/comm \
    -Iframework/common \
    -Iframework/atcmd \
    -Iprojects/folk/main \
    -Iapp/ux \
    -Iapp/ui \
    -Iutils/xstring \
    -Iutils/crc16

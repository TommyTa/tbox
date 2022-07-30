cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += FreeRTOS/

obj-y += cmsis_os.o

ccflags-y += \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/sys

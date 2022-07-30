cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += bsp/
obj-y += chip/
obj-y += os/
obj-y += sys/

ccflags-y += \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS


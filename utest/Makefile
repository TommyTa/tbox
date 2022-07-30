cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += test_bsp_led.o
obj-y += test_bsp_i2c.o
obj-y += test_bsp_spi.o
obj-y += test_bsp_flash.o
obj-y += test_md5.o
obj-y += test_rsa.o
obj-y += test_trace.o
obj-y += test_bsp_pwm.o

ccflags-y += \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/bsp \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/os \
    -Iplatform/os/FreeRTOS \
    -Iplatform/sys \
    -Iframework/common \
    -Iframework/atcmd \
    -Iframework/trace \
    -Iutils/xstring \
    -Iutils/md5 \
    -Iutils/rsa \
    -Iprojects/folk/main


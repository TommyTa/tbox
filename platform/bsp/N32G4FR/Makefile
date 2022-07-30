cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += n32g4fr_it.o
obj-y += system_n32g4fr.o

# BSP
obj-y += bsp_uart.o
obj-y += bsp_led.o
obj-y += bsp_tim.o
obj-y += bsp_flash.o
obj-y += bsp_i2c.o
obj-y += bsp_spi.o
obj-y += bsp_pwm.o
obj-y += bsp_gpio.o

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
    -Iutils/xstring

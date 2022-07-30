cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += tasks.o
obj-y += croutine.o
obj-y += event_groups.o
obj-y += heap_4.o
obj-y += list.o
obj-y += port.o
obj-y += queue.o
obj-y += timers.o

ccflags-y += \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/chip/CMSIS/core \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/bsp \
    -Iapp/uart_comm

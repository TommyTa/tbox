cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj_s := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)startup/*gcc.S))
obj_c := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)hal/n32g4fr_std_periph_driver/src/*.c))
obj_cpp := $(patsubst $(cur_dir)%,%,$(wildcard $(cur_dir)*.cpp))

obj-y := $(obj_s:.S=.o) $(obj_c:.c=.o) $(obj_cpp:.cpp=.o)
obj-y += sys_api.o

subdir-ccflags-y += \
    -Iplatform/chip/N32G4FR/hal/n32g4fr_std_periph_driver/inc \
    -Iplatform/bsp/N32G4FR \
    -Iplatform/chip/CMSIS/core

cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

CHIP            := N32G4FR

init-y          :=
core-y          := projects/
core-y          += platform/bsp/
core-y          += platform/chip/
core-y          += platform/sys/
core-y          += utils/crc16/
core-y          += framework/common/

LDS_FILE        := n32g4fr_flash.lds
LDS_SRC_STEM    := $(cur_dir)/$(LDS_FILE)

ifeq ($(CHIP),N32G4FR)
KBUILD_CPPFLAGS += -DN32G4FR
KBUILD_CPPFLAGS += -DUSE_STDPERIPH_DRIVER
endif

# Support bootloader
KBUILD_CPPFLAGS += -DSUPPORT_BOOTLOADER=1

# Uart communication mode
KBUILD_CPPFLAGS += -DCOMM_MODE_HALF_DUPLEX_USART0=0
KBUILD_CPPFLAGS += -DCOMM_MODE_HALF_DUPLEX_USART1=1
KBUILD_CPPFLAGS += -DCOMM_MODE_HALF_DUPLEX_USART2=1

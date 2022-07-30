cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

CHIP            := N32G4FR

init-y          :=
core-y          := app/
core-y          += projects/
core-y          += platform/
core-y          += framework/
core-y          += drivers/
core-y          += utils/
core-y          += utest/

LDS_FILE        := n32g4fr_flash.lds
LDS_SRC_STEM    := $(cur_dir)/$(LDS_FILE)

KBUILD_CPPFLAGS += -DCHIP=$(CHIP)

ifeq ($(CHIP),N32G4FR)
KBUILD_CPPFLAGS += -DN32G4FR
KBUILD_CPPFLAGS += -DUSE_STDPERIPH_DRIVER
endif

# Use the trace log
export USE_TRACE_ID ?= 1
ifeq ($(USE_TRACE_ID),1)
export TRACE_STR_SECTION ?= 1
endif

# Support bootloader
VECT_TAB_OFFSET := 0x4000
export VECT_TAB_OFFSET
export SUPPORT_BOOTLOADER ?= 1
ifeq ($(SUPPORT_BOOTLOADER),1)
KBUILD_CPPFLAGS += -DSUPPORT_BOOTLOADER=1
KBUILD_CPPFLAGS += -DVECT_TAB_OFFSET=0x4000
else
KBUILD_CPPFLAGS += -DSUPPORT_BOOTLOADER=0
endif

# Version info
REVISION_INFO_S := 0.1.0.0
REVISION_TYPE_S := factory

export REVISION_INFO_S
export REVISION_TYPE_S

KBUILD_CPPFLAGS += -DREVISION_INFO_S=\"$(REVISION_INFO_S)\"
KBUILD_CPPFLAGS += -DREVISION_TYPE_S=\"$(REVISION_TYPE_S)\"

# Uart communication mode
KBUILD_CPPFLAGS += -DCOMM_MODE_HALF_DUPLEX_USART0=0
KBUILD_CPPFLAGS += -DCOMM_MODE_HALF_DUPLEX_USART1=1
KBUILD_CPPFLAGS += -DCOMM_MODE_HALF_DUPLEX_USART2=1

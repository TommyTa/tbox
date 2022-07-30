
add_if_exists = $(foreach d,$(1),$(if $(wildcard $(srctree)/$(d)),$(d) ,))

# -------------------------------------------
# CHIP selection
# -------------------------------------------

export CHIP
ifeq ($(CHIP),)
$(error Invalid CHIP: $(CHIP))
endif

ifeq ($(CHIP),N32G4FR)
CPU := m4
export CHIP_HAS_FPU := 1
endif

export CPU
ifeq ($(CPU),)
CPU := m4
endif

CPU_CFLAGS := -mthumb
ifeq ($(CPU),a7)
CPU_CFLAGS += -march=armv7-a
else ifeq ($(CPU),m33)
ifeq ($(CPU_NO_DSP),1)
CPU_CFLAGS += -mcpu=cortex-m33+nodsp
else
CPU_CFLAGS += -mcpu=cortex-m33
endif
else
CPU_CFLAGS += -mcpu=cortex-m4
endif

export UNALIGNED_ACCESS ?= 1
ifeq ($(UNALIGNED_ACCESS),1)
KBUILD_CPPFLAGS += -DUNALIGNED_ACCESS
else
CPU_CFLAGS += -mno-unaligned-access
endif

ifeq ($(CHIP_HAS_FPU),1)
ifeq ($(CPU),a7)
CPU_CFLAGS += -mfpu=vfpv3-d16
else ifeq ($(CPU),m33)
CPU_CFLAGS += -mfpu=fpv5-sp-d16
else
CPU_CFLAGS += -mfpu=fpv4-sp-d16
endif
ifeq ($(SOFT_FLOAT_ABI),1)
CPU_CFLAGS += -mfloat-abi=softfp
else
CPU_CFLAGS += -mfloat-abi=hard
endif
else
CPU_CFLAGS += -mfloat-abi=soft
endif

# -------------------------------------------
# DEBUG functions
# -------------------------------------------
ifneq ($(OPT_LEVEL),)
KBUILD_CFLAGS	+= -O$(OPT_LEVEL)
else
KBUILD_CFLAGS	+= -O2
endif

ifneq ($(ALLOW_WARNING),1)
KBUILD_CPPFLAGS += -Werror
endif

ifeq ($(PIE),1)
ifneq ($(TOOLCHAIN),armclang)
ifneq ($(NOSTD),1)
$(error PIE can only work when NOSTD=1)
endif
KBUILD_CPPFLAGS += -msingle-pic-base
endif
KBUILD_CPPFLAGS += -fPIE
# -pie option will generate .dynamic section
#LDFLAGS += -pie
#LDFLAGS += -z relro -z now
endif

# -------------------------------------------
# Standard C library
# -------------------------------------------
export NOSTD
export LIBC_ROM

ifeq ($(NOSTD),1)

ifeq ($(MBED),1)
$(error Invalid configuration: MBED needs standard C library support)
endif
ifeq ($(RTOS),1)
$(error Invalid configuration: RTOS needs standard C library support)
endif

ifneq ($(NO_LIBC),1)
core-y += utils/libc/
endif

SPECS_CFLAGS :=

LIB_LDFLAGS := $(filter-out -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys,$(LIB_LDFLAGS))

KBUILD_CPPFLAGS += -ffreestanding -Iutils/libc/inc
ifeq ($(TOOLCHAIN),armclang)
# 1) Avoid -nostdinc
#    CMSIS header files need arm_compat.h, which is one of toolchain's standard header files
# 2) Always -nostdlib for compiling C/C++ files
#    Never convert standard API calls to non-standard library calls, but just emit standard API calls
# 3) Avoid -nostdlib for linking final image
#    Some 64-bit calculations and math functions need toolchain's standard library
KBUILD_CPPFLAGS += -nostdlib
else
KBUILD_CPPFLAGS += -nostdinc
CFLAGS_IMAGE += -nostdlib
endif

KBUILD_CPPFLAGS += -DNOSTD

else # NOSTD != 1

ifeq ($(LIBC_ROM),1)
core-y += utils/libc/
endif

ifeq ($(TOOLCHAIN),armclang)
LIB_LDFLAGS := $(filter-out -lsupc++,$(LIB_LDFLAGS))
else
#SPECS_CFLAGS := --specs=nano.specs
SPECS_CFLAGS := --specs=nosys.specs

LIB_LDFLAGS += -lm -lc -lgcc -lnosys
endif

endif # NOSTD != 1

KBUILD_CPPFLAGS += $(CPU_CFLAGS) $(SPECS_CFLAGS)
LINK_CFLAGS += $(CPU_CFLAGS) $(SPECS_CFLAGS)
CFLAGS_IMAGE += $(CPU_CFLAGS) $(SPECS_CFLAGS)

# Save 100+ bytes by filling less alignment holes
# TODO: Array alignment?
#LDFLAGS += --sort-common --sort-section=alignment

ifeq ($(CTYPE_PTR_DEF),1)
ifeq ($(TOOLCHAIN),armclang)
$(error CTYPE_PTR_DEF is not supported in $(TOOLCHAIN))
else
LDFLAGS_IMAGE += --defsym __ctype_ptr__=0
endif
endif

ifeq ($(TRACE_STR_SECTION),1)
KBUILD_CPPFLAGS += -DTRACE_STR_SECTION
CPPFLAGS_${LDS_FILE} += -DTRACE_STR_SECTION
endif

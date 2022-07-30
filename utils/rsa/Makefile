cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y := rsa2048.o
obj-y += bignum/

ccflags-y += \
	-Iutils/rsa/bignum\
	-Iutils/rsa/

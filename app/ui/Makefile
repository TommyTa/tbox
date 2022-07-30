cur_dir := $(dir $(lastword $(MAKEFILE_LIST)))

obj-y += ui_indicator.o
obj-y += ui_event.o
obj-y += ui_ear_led.o
obj-y += ui_box_led.o

ccflags-y += \
    -Iplatform/os \
    -Iplatform/sys \
    -Iframework/atcmd \
    -Iapp/ux

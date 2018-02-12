# Makefilie (!not makefile)
obj-m := 8253_drive.o
KDIR := /lib/modules/`uname -r`/build
PWD :=$(shell pwd)
default:
$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

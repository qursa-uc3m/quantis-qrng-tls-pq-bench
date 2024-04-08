obj-m += quantis_chip_pcie.o
quantis_chip_pcie-objs := idq-rng.o xdma-core.o xdma-sgm.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build

PWD       := $(shell pwd)

ROOT := $(dir $(M))
XILINXINCLUDE := -I$(ROOT)../include -I$(ROOT)/include

GCCVERSION = $(shell gcc -dumpversion | sed -e 's/\.\([0-9][0-9]\)/\1/g' -e 's/\.\([0-9]\)/0\1/g' -e 's/^[0-9]\{3,4\}$$/&00/')

# The flag "date-time" has been introduced in GCC 4.9.x series.
# But tests show that it was not present in 4.9.2.
# I set the limit to 4.9.4 but it may have to be set more accurately if problems are encountered. 
GCC49 := $(shell expr $(GCCVERSION) \>= 40904)

all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install: all
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules_install
	depmod -a

clean:
	rm -rf *.o *.o.d *~ core .depend .*.cmd *.ko *.ko.unsigned *.mod.c .tmp_versions *.symvers .#* *.save *.bak Modules.* modules.order Module.markers *.bin

CFLAGS_xdma-core.o := -Wall -DDEBUG $(XILINXINCLUDE)

ifeq ($(GCC49),1)
	CFLAGS_xdma-core.o += -Wno-error=date-time
endif

CFLAGS_idq-rng.o   := $(XILINXINCLUDE)
CFLAGS_xdma-sgm.o := $(XILINXINCLUDE)

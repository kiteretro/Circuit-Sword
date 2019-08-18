KVER=$(shell echo $(KERNELRELEASE) | cut -d '.' -f1,2)
KERNEL_HEADERS=/lib/modules/$(KERNELRELEASE)/build

define download_sources =
	echo "Need a subversion for this!"
	svn export https://github.com/raspberrypi/linux/branches/rpi-$(KVER).y/drivers/staging/rtl8723bs $(KVER)
endef

define patch_makefile =
	patch $(KVER)/Makefile patches/makefile.patch
endef

define upgrade_sources =
	cp -r 4.14 $(KVER)
	patch --strip 1 -d $(KVER) < patches/4.14-to-4.19.patch
endef

all: | $(KVER)
	CONFIG_RTL8723BS=m $(MAKE) -C $(KERNEL_HEADERS) M=$(PWD)/$(KVER)
	cp $(PWD)/$(KVER)/r8723bs.ko $(PWD)

clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD)/$(KVER) clean

install:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD)/$(KVER) modules_install
$(KVER):
	@echo "Source code for $(KVER) does not exists"
	@echo "Checking for upgrade patch"

ifeq ($(wildcard patches/4.14-to-$(KVER).patch),)
	@echo "Patch not found"
	@echo "Trying to download a new sources.."
	$(download_sources)
	$(patch_makefile)
else
	@echo "Patch found"
	$(upgrade_sources)
endif


.PHONY: all

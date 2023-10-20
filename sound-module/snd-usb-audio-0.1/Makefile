KVER=$(shell echo $(KERNELRELEASE) | cut -d '.' -f1,2)
KERNEL_HEADERS=/lib/modules/$(KERNELRELEASE)/build

define download_sources =
	echo "Need a subversion for this!"
	svn export https://github.com/raspberrypi/linux/branches/rpi-$(KVER).y/sound/usb $(KVER)
endef

define patch_sources =
	patch --strip 1 -d $(KVER) < patches/fix-volume.patch
endef

define upgrade_sources =
	cp -r 5.10 $(KVER)
	patch --strip 1 -d $(KVER) < patches/5.10-to-$(KVER).patch
endef

all: | $(KVER)
	CONFIG_SND_USB_AUDIO=m $(MAKE) -C $(KERNEL_HEADERS) M=$(PWD)/$(KVER)
	cp $(PWD)/$(KVER)/snd-usb-audio.ko $(PWD)

clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD)/$(KVER) clean

install:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(PWD)/$(KVER) modules_install
$(KVER):
	@echo "Source code for $(KVER) does not exists"
	@echo "Checking for upgrade patch"

ifeq ($(wildcard patches/5.10-to-$(KVER).patch),)
	@echo "Patch not found"
	@echo "Trying to download new sources.."
	$(download_sources)
	$(patch_sources)
else
	@echo "Patch found"
	$(upgrade_sources)
endif


.PHONY: all

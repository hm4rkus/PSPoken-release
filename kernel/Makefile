# Builds pspoken_kernel.prx, PSPoken's kernel-mode helper module.
TARGET = pspoken_kernel
# exports.o is generated from exports.exp; build_prx.mak doesn't add it on its own.
OBJS = main.o exports.o

CFLAGS = -Os -G0 -Wall -Wextra
ASFLAGS = $(CFLAGS)

# A PRX (relocatable module) with our export table, linked against the kernel versions of the
# system libraries instead of the user-mode ones, and without the user-mode C library.
BUILD_PRX = 1
USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1
# systemctrl: ARK's sctrlHENFindFunction, to find the Go's Bluetooth manager at run time.
LIBS = -lpspsysmem_kernel_660 -lpspsystemctrl_kernel

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak

# Guard: a module without an export table loads fine, but nothing can link to it (every call
# from the EBOOT fails with 0x8002013A, "library not yet linked"). Fail the build instead.
all: verify-exports
verify-exports: $(TARGET).prx
	@psp-objdump -h $(TARGET).elf | grep -q ' \.lib\.ent ' || \
		(echo "error: $(TARGET).elf has no export table (.lib.ent); is exports.o in OBJS?"; exit 1)
.PHONY: verify-exports

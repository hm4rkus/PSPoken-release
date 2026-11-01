/*
 * pspoken_kernel.prx: PSPoken's kernel-mode helper module.
 *
 * WHY THIS EXISTS
 * The PSP splits code into two privilege levels, like an OS splits user programs from the
 * kernel. Our EBOOT runs in user mode, and some functions only accept calls from kernel mode:
 * switching the display off, setting the backlight brightness, and reading the button bits
 * that user mode can't see (the PSP Go's slider sensor, for example).
 *
 * Custom firmware lets an app load its own kernel-mode module. This file is that module: a
 * handful of thin wrappers that the EBOOT calls like normal functions. It is NOT a system
 * plugin: nothing is installed, it's loaded only by PSPoken, and it's unloaded when PSPoken
 * exits. It writes nothing to flash. A bug here can freeze the PSP (hold POWER to recover).
 *
 * HOW A USER-MODE CALL REACHES US
 * The functions below are listed in exports.exp with flag 0x4001, which exports them to user
 * mode. When the EBOOT calls pk_display_off(), the call goes through a syscall into this
 * module, still on the EBOOT's thread.
 *
 * THE K1 REGISTER
 * The kernel uses the CPU register k1 to remember "this call came from user mode". Kernel
 * functions check it and refuse user callers (or reject user-space pointers). Each wrapper
 * therefore clears k1 before calling the kernel function, and restores it afterwards, so the
 * kernel treats the call as coming from us. This is the standard pattern for PSP kernel helpers.
 */
#include <pspctrl.h>
#include <pspdisplay_kernel.h>
#include <pspkernel.h>
#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <systemctrl.h>

#include "pspoken_kernel.h"

/* 0x1006 = kernel module (0x1000) that can only be loaded and started once (0x0002 | 0x0004). */
PSP_MODULE_INFO("PSPokenKernel", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

/*
 * The SDK's sceDisplay_driver library provides these, but its headers don't declare them.
 * sceDisplayDisable stops the display (screen and backlight off); sceDisplayEnable restarts it.
 */
int sceDisplayEnable(void);
int sceDisplayDisable(void);

/* The system's settings (sceImpose_driver, in libpspkernel's kernel stubs): the main volume, 0 to 30,
 * and whether the volume is muted. The SDK's driver header declares them, but we only need these. */
#define IMPOSE_MAIN_VOLUME 0x1
#define IMPOSE_MUTE 0x8
int sceImposeGetParam(int param);

/* ---- k1 handling ------------------------------------------------------------------------- */

/* Marks the rest of the call as kernel-originated. Returns the caller's k1 for restoring. */
static unsigned int enter_kernel_context(void)
{
    return pspSdkSetK1(0);
}

static void leave_kernel_context(unsigned int caller_k1)
{
    pspSdkSetK1(caller_k1);
}

/* ---- exported functions (see exports.exp and pspoken_kernel.h) --------------------------- */

int pk_display_on(void)
{
    unsigned int caller_k1 = enter_kernel_context();
    int result = sceDisplayEnable();
    leave_kernel_context(caller_k1);
    return result;
}

int pk_display_off(void)
{
    unsigned int caller_k1 = enter_kernel_context();
    int result = sceDisplayDisable();
    leave_kernel_context(caller_k1);
    return result;
}

/* Backlight brightness level as the display driver sees it (0 = dark). */
int pk_get_brightness(void)
{
    unsigned int caller_k1 = enter_kernel_context();
    int level = 0;
    int unknown = 0;
    sceDisplayGetBrightness(&level, &unknown);
    leave_kernel_context(caller_k1);
    return level;
}

void pk_set_brightness(int level)
{
    unsigned int caller_k1 = enter_kernel_context();
    sceDisplaySetBrightness(level, 0);
    leave_kernel_context(caller_k1);
}

/*
 * Reads the buttons from kernel mode, which includes bits user mode doesn't get, such as the
 * PSP Go slider (0x20000000 while open). Returns the raw button mask.
 *
 * We read into a local struct rather than writing straight to the caller's pointer: with k1
 * cleared, the kernel would happily write to any address, so we never pass it user pointers.
 */
unsigned int pk_read_buttons(void)
{
    unsigned int caller_k1 = enter_kernel_context();
    SceCtrlData pad;
    sceCtrlPeekBufferPositive(&pad, 1);
    leave_kernel_context(caller_k1);
    return pad.Buttons;
}

/*
 * The volume the PSP's volume buttons set, 0 to 30, or 0 while it's muted. The app
 * only shows it (ui/volume_bar); the hardware does the changing. Negative if the system says no.
 */
int pk_get_volume(void)
{
    unsigned int caller_k1 = enter_kernel_context();
    int muted = sceImposeGetParam(IMPOSE_MUTE);
    int level = sceImposeGetParam(IMPOSE_MAIN_VOLUME);
    leave_kernel_context(caller_k1);
    if (level < 0)
        return level;
    return muted > 0 ? 0 : level;
}

/* The hardware model: 0 = PSP-1000, 1 = 2000, 2 = 3000, 3 = 3000 (later boards), 4 = Go, 10 = Street. */
int pk_get_model(void)
{
    unsigned int caller_k1 = enter_kernel_context();
    int model = sceKernelGetModel();
    leave_kernel_context(caller_k1);
    return model;
}

/* ---- the Go's Bluetooth headphone buttons ------------------------------------------------ */

/*
 * Bluetooth headphones send their buttons as AVRCP "pass through" commands. On the Go, the
 * kernel's Bluetooth manager (bsman.prx, module "sceBSMan") keeps them in a queue of six and
 * hands them out one at a time to whoever asks. The XMB's UI framework (paf.prx) asks every
 * frame, so the XMB reacts; a game or homebrew never asks, so the queue fills and the manager
 * answers the headphones "rejected". Found by reading the Go's 6.61 firmware.
 *
 * The function has no name in any NID list: it's sceBSMan_driver 0x51D1F30D (the same code as the
 * user export sceBSMan 0x46ACDAE3). It copies the oldest event, 64 bytes, into the buffer it's
 * given and returns how many were waiting (0: none), or an error (0x802F011B: no headphones with
 * AVRCP connected). The lookup goes through ARK's systemctrl, so on models without the module
 * it simply finds nothing.
 */
#define BSMAN_MODULE_NAME "sceBSMan"
#define BSMAN_DRIVER_LIBRARY "sceBSMan_driver"
#define BSMAN_DRIVER_READ_AVRCP_NID 0x51D1F30D
#define BSMAN_USER_LIBRARY "sceBSMan"
#define BSMAN_USER_READ_AVRCP_NID 0x46ACDAE3
/* The function's first instruction in the 6.61 firmware we studied (addiu sp, sp, -32). Anything
 * else means a firmware we haven't read, and the function isn't called: btfree-ark checks its own
 * sceBSMan function the same way before touching it. */
#define READ_AVRCP_FIRST_INSTRUCTION 0x27BDFFE0u
#define AVRCP_EVENT_BYTES 64
typedef int (*avrcp_read_function)(void *event);

/* Where the firmware's function is: the driver library's export, else the user library's (the
 * same code), or 0. */
static unsigned int locate_avrcp_read(void)
{
    unsigned int address = sctrlHENFindFunction(BSMAN_MODULE_NAME, BSMAN_DRIVER_LIBRARY, BSMAN_DRIVER_READ_AVRCP_NID);
    if (!address)
        address = sctrlHENFindFunction(BSMAN_MODULE_NAME, BSMAN_USER_LIBRARY, BSMAN_USER_READ_AVRCP_NID);
    return address;
}

/* The firmware's function, found on the first call that succeeds, if it's the code we studied. */
static avrcp_read_function find_avrcp_read(void)
{
    static avrcp_read_function found;
    if (!found) {
        unsigned int address = locate_avrcp_read();
        if (address && *(volatile unsigned int *)address == READ_AVRCP_FIRST_INSTRUCTION)
            found = (avrcp_read_function)address;
    }
    return found;
}

/* Whether [address, address + size) is all user memory: user addresses have the top bit clear. */
static int is_user_range(const void *address, int size)
{
    unsigned int start = (unsigned int)address;
    return ((start | (start + (unsigned int)size)) & 0x80000000u) == 0;
}

int pk_read_avrcp(unsigned char *out, int size)
{
    avrcp_read_function read_avrcp = find_avrcp_read();
    if (!read_avrcp)
        return PK_AVRCP_UNAVAILABLE;
    /* The event is read into our own buffer with k1 cleared, and only then copied to the
     * caller's, once we know that one is user memory (see pk_read_buttons). */
    if (!out || size <= 0 || size > AVRCP_EVENT_BYTES || !is_user_range(out, size))
        return PK_AVRCP_BAD_BUFFER;
    unsigned char event[AVRCP_EVENT_BYTES];
    unsigned int caller_k1 = enter_kernel_context();
    int waiting = read_avrcp(event);
    leave_kernel_context(caller_k1);
    if (waiting > 0)
        for (int i = 0; i < size; i++)
            out[i] = event[i];
    return waiting;
}

/* ---- module lifecycle -------------------------------------------------------------------- */

/* Called when the EBOOT loads us. Nothing to set up. */
int module_start(SceSize args, void *argp)
{
    (void)args;
    (void)argp;
    return 0;
}

int module_stop(SceSize args, void *argp)
{
    (void)args;
    (void)argp;
    return 0;
}

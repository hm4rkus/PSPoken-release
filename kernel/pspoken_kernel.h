#ifndef PSPOKEN_KERNEL_H
#define PSPOKEN_KERNEL_H

/*
 * The functions pspoken_kernel.prx exports to the EBOOT (see kernel/main.c for why the module
 * exists). They only work after the module has been loaded with pspSdkLoadStartModule; before
 * that, calling one crashes, because the import stub hasn't been linked to the module yet.
 */

/* Display power. Off means screen and backlight off; returns < 0 on failure. */
int pk_display_on(void);
int pk_display_off(void);

/* Backlight level as the display driver uses it (0 = dark). */
int pk_get_brightness(void);
void pk_set_brightness(int level);

/* Button mask read in kernel mode: includes bits user mode can't see. */
unsigned int pk_read_buttons(void);

/* Hardware model: 0 = PSP-1000, 1 = 2000, 2/3 = 3000, 4 = Go, 10 = Street. */
int pk_get_model(void);

/* The volume set by the volume buttons, 0 to 30 (0 while muted); negative if it can't be read. */
int pk_get_volume(void);

/* The oldest Bluetooth headphone button event the Go's Bluetooth manager has queued (AVRCP):
 * copies its first `size` bytes (at most 64) into `out`. Returns how many events
 * were waiting, this one included (0: none, nothing copied), PK_AVRCP_UNAVAILABLE without the
 * Bluetooth manager (every model but the Go), PK_AVRCP_BAD_BUFFER, or the firmware's error
 * (0x802F011B: no headphones with AVRCP connected). The event: u16 the AVRCP operation (0x44
 * play, 0x46 pause, 0x4B forward, 0x4C backward, 0x41/0x42 volume, 0x7E vendor), u16 1 while
 * pressed and 0 on release, u16 the operands' length, then the operands. */
int pk_read_avrcp(unsigned char *out, int size);
#define PK_AVRCP_UNAVAILABLE (-1)
#define PK_AVRCP_BAD_BUFFER (-2)

/* PSP Go only: set in the kernel button mask while the slider is open (cleared while closed). */
#define PK_BUTTON_SLIDE_OPEN 0x20000000u

#endif

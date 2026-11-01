# pspoken_kernel.prx

The optional kernel module PSPoken loads on the PSP. This folder is its complete source. It's the same code the releases ship: this repository's Actions build each release's module from it ([`.github/workflows/kernel.yml`](../.github/workflows/kernel.yml)), and its SHA-256 is in the release's `SHA256SUMS.txt`.

## Running without it

PSPoken continues to work if this file is absent or fails to load. Leave it next to `EBOOT.PBP` to enable the [optional PSP features](../README.md#optional-psp-kernel-module), or omit it to run without kernel code. Playback, browsing, progress saving, HOLD locking, wired-remote support and CPU clock reduction remain available. Hardware display/backlight control, Go slider detection, the in-app volume bar and Go Bluetooth media buttons require the module.

## Why it exists

The PSP runs apps in user mode, and a few things only kernel mode may do. PSPoken needs four of them:
- turning the screen and its backlight off and on while a book plays;
- reading the buttons only the system sees, such as the PSP Go's slider;
- reading the volume the console's volume buttons set, so the app can show it;
- receiving the PSP Go's Bluetooth headphone buttons.

Custom firmware lets an app load a small module of its own into kernel mode. This is that module: a handful of thin wrappers the app calls like ordinary functions.

## What it does, and what it doesn't

**It offers the app eight calls** (declared in [`pspoken_kernel.h`](pspoken_kernel.h), exported in [`exports.exp`](exports.exp)):

| Call | What it does | The system call it wraps |
| --- | --- | --- |
| `pk_display_on`, `pk_display_off` | The screen and its backlight on or off | `sceDisplayEnable`, `sceDisplayDisable` |
| `pk_get_brightness`, `pk_set_brightness` | Read or set the backlight's level | `sceDisplayGetBrightness`, `sceDisplaySetBrightness` |
| `pk_read_buttons` | The buttons as kernel mode sees them, the Go's slider included | `sceCtrlPeekBufferPositive` |
| `pk_get_model` | Which PSP model it is | `sceKernelGetModel` |
| `pk_get_volume` | The volume, 0 to 30 (0 while muted) | `sceImposeGetParam` |
| `pk_read_avrcp` | The oldest Bluetooth headphone button event the PSP Go has queued (on other models: "unavailable") | The Go's Bluetooth manager, found through the custom firmware's systemctrl |

**It doesn't:**
- modify the system: no hooks, no patches;
- write anything to the Memory Stick or to flash;
- use the network;
- start threads of its own.

It runs only while PSPoken does: it's loaded by PSPoken from PSPoken's own folder, it isn't installed anywhere, and it's unloaded when PSPoken exits. A bug in it could freeze the PSP (holding the power switch recovers), so save your progress before testing a rebuilt module.

[`main.c`](main.c) explains each call and the reasoning behind the less obvious ones: the k1 register, the Go's Bluetooth queue.

**What this does and doesn't prove.** Reading this code tells you what PSPoken does in kernel mode through this module, and checking the hash tells you the module you run is this code. It doesn't tell you what the rest of PSPoken does. The app is closed source, and on custom firmware any homebrew can load kernel code of its own. Publishing this module is about being open where it matters most, not a technical guarantee about the closed part.

## Building it and checking a release

The module builds with the PSP toolchain's Docker image, pinned to the exact image the releases are built with:

```sh
docker run --rm -v "$PWD/kernel:/src" -w /src \
  pspdev/pspdev@sha256:54895e6f5afb71b8f4f6915ee6d5023e1e087ca7afdf2dcb1d7a694a5233e45f make
shasum -a 256 kernel/pspoken_kernel.prx
```

The SHA-256 printed should match the one for `pspoken_kernel.prx` in the release's `SHA256SUMS.txt`. The standalone `pspoken_kernel.prx` attached to the release is identical to the one inside the PSP ZIP. The build is reproducible: the same source and image give the same bytes, on a Mac or on Linux.

You can also see it built: the [Actions](../../../actions) tab shows each build of this folder, with the hash in its log and the module as a download.

## Licence

- **My code here** is under the MIT license ([`LICENSE`](LICENSE)).
- **The built module** also contains the custom firmware's systemctrl import stubs (from the PSP toolchain's `psp-cfw-sdk` package, GPL-3.0), so `pspoken_kernel.prx` is distributed under the GPL-3.0. Its source is this folder.

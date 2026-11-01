# Third-party notices

PSPoken's builds include the software below, each under its own licence. The PS Vita build includes none of the PSP-only libraries (intraFont, libjpeg-turbo): it uses the Vita's own fonts and JPEG decoder.

## intraFont (PSP)

Uses intraFont by BenHur. Used unmodified, to draw text in the PSP's own system fonts (the fonts themselves are the console's, read from it at run time, and not distributed with PSPoken).

Licensed under the Creative Commons Attribution-ShareAlike 3.0 Unported license: <https://creativecommons.org/licenses/by-sa/3.0/>. Source: <https://github.com/pspdev/libintrafont>.

## libjpeg-turbo (PSP)

This software is based in part on the work of the Independent JPEG Group.

PSPoken's PSP build uses libjpeg-turbo 3.1.3 (<https://libjpeg-turbo.org>) through its libjpeg API, which is covered by the IJG License. The IJG's legal notice, from the README that comes with the library:

```
In plain English:

1. We don't promise that this software works.  (But if you find any bugs,
   please let us know!)
2. You can use this software for whatever you want.  You don't have to pay us.
3. You may not pretend that you wrote this software.  If you use it in a
   program, you must acknowledge somewhere in your documentation that
   you've used the IJG code.

In legalese:

The authors make NO WARRANTY or representation, either express or implied,
with respect to this software, its quality, accuracy, merchantability, or
fitness for a particular purpose.  This software is provided "AS IS", and you,
its user, assume the entire risk as to its quality and accuracy.

This software is copyright (C) 1991-2020, Thomas G. Lane, Guido Vollbeding.
All Rights Reserved except as specified below.

Permission is hereby granted to use, copy, modify, and distribute this
software (or portions thereof) for any purpose, without fee, subject to these
conditions:
(1) If any part of the source code for this software is distributed, then this
README file must be included, with this copyright and no-warranty notice
unaltered; and any additions, deletions, or changes to the original files
must be clearly indicated in accompanying documentation.
(2) If only executable code is distributed, then the accompanying
documentation must state that "this software is based in part on the work of
the Independent JPEG Group".
(3) Permission for use of this software is granted only if the user accepts
full responsibility for any undesirable consequences; the authors accept
NO LIABILITY for damages of any kind.

These conditions apply to any software derived from or based on the IJG code,
not just to the unmodified library.  If you use our work, you ought to
acknowledge us.

Permission is NOT granted for the use of any IJG author's name or company name
in advertising or publicity relating to this software or products derived from
it.  This software may be referred to only as "the Independent JPEG Group's
software".

We specifically permit and encourage the use of this software as the basis of
commercial products, provided that all warranty or liability claims are
assumed by the product vendor.
```

## PSPSDK (PSP)

The PSP build is made with the PSPSDK and its libraries (<https://github.com/pspdev/pspsdk>):

```
Copyright (c) 2005  adresd
Copyright (c) 2005  Marcus R. Brown
Copyright (c) 2005  James Forshaw
Copyright (c) 2005  John Kelley
Copyright (c) 2005  Jesper Svennevid
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The names of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

## ARK's systemctrl library (PSP kernel module only)

The kernel module, `pspoken_kernel.prx`, links the import stubs of the custom firmware's systemctrl library (`libpspsystemctrl_kernel`, from the PSP toolchain's `psp-cfw-sdk` package, which is licensed under the GNU General Public License v3.0: <https://www.gnu.org/licenses/gpl-3.0.html>). The full licence text is included in [`licenses/GPL-3.0.txt`](licenses/GPL-3.0.txt). So the built module is distributed under the GPL-3.0; its complete source is in [`kernel/`](https://github.com/hm4rkus/PSPoken-release/tree/main/kernel), and how to build it in [`kernel/README.md`](https://github.com/hm4rkus/PSPoken-release/blob/main/kernel/README.md). The app itself (`EBOOT.PBP`) doesn't link it.

## VitaSDK (PS Vita)

The PS Vita build is made with VitaSDK (<https://vitasdk.org>) and its C library. The VitaSDK header/import-stub MIT licence is reproduced in [`licenses/VITASDK.txt`](licenses/VITASDK.txt).

## newlib

Both builds link the newlib C library. Its upstream copyright, permission and warranty notices are included verbatim for the [PSP toolchain](licenses/NEWLIB-PSP.txt) and the [Vita toolchain](licenses/NEWLIB-VITA.txt). These collections cover multiple components; each component retains its applicable licence. Upstream: <https://sourceware.org/newlib/COPYING.NEWLIB>.

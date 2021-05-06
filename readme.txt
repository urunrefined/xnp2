How to use this program:

* Compilation
* Add bitmap font
* Add proper FDD / HardDisk images and reset

Compilation:
cd x11/
make clean
make

If this doesn't work it means you either have a
missing dependency or you have different 
include paths. The Makefile is easy to understand
so you should be able to change it to fit your
current distribution. The compilation step should produce a
xnp2 executable artifact inside the x11 directory.

Bitmap Font:
After starting the xnp2 executable be sure to add the
font via the Emulate -> Font option. This is a very
crucial step, as when no font is loaded the text
in some applications will not appear properly
(actually you may just see a black screen, depending
on if you have loaded an image ).

Yes indeed, the font you choose will be loaded into
ROM and may be used by the PC-98 applications.

One thing you can try is the anex86.bmp font,
which surely you can find for yourself.

Why is the font not included? No idea, but there are
probably some legal reasons behind this, and I am
not willing to go down that rabbit-hole to find out.

There is an autogeneration of the font built-in,
but as far as I can tell it is broken, so supplying
your own font is (most likely) required.

Development goals:
* Proper borderless window fullscreen support 
(no "real" fullscreen support as this does not work
properly with any machine I have ever seen)
* Get rid of a truckload of compiler warnings
* Purge glib (Why are people still using this)
* Linux as the first development priority
* Modularisation and general cleanup of the code
* Hoist the code up to C++17. I like plain C (C99),
but sometimes a heavier language such as C++ is warranted.
* Get rid of the INSANE amount of preprocessor
switches. Yikes! Create one straight forward way
to build the modules
* More documentation on how to use this software
* Proper developer inspection of the current state
of the machine
* More visual feedback on what is actually happening
on the machine
* Decoupling of toolkit (gtk2) and main "x11" code. I realize
x11 doesn't really mean "x11" - lets just say modern
Linux/UNIX OSes, or even stuff like SerenityOS. I am
not against other platforms, just against adding platforms
which will never be used in production. There is no
use building something which will never be used.
* Three screen "Modes" - Integer scaling (already implemented),
stretch to fill (do not keep aspect), stretch to fill (keep aspect)  
* Build support via CMake... Yes, CMake is... not very
good, however I realize the autogeneration of includes and
library search paths are essential for smooth building
especially with IDEs like CLion, which only have experimental
Makefile support. Therefore CMake (or equivalent) 
will be supported in the future along with the Makefile, 
which should ALWAYS be kept, even just as a reminder 
what the hell is actually necessary to build the thing
* Add back IA32 support. Currently only the Intel 286 machines
and the NEC V30 286 clones are considered, so if you see any
error message with _386_ in it, then this is the reason.
If you need 386 support please try the original np2 project
files.

While all of the menus are in English,
you will need some experience in Japanese
to fully enjoy most of the software available for
this system. Even just knowing hiragana and
katakana will go a long way.

If you would like to contribute go ahead.
Please keep any PRs small. I would
rather get many PRs with small (self-contained)
commits than one massive commit. Sometimes
a massive commit is unavoidable however,
and this is fine as well.

Currently I am using a WM without window decorations,
so "borderless fullscreen" is already
working for me. I realize most people will
be running some sort of DE with decorations,
which I will try to support as well 
(but if anyone is willing... (nudge nudge)...

There is no doubt that the restructuring of this
program will result in some functionality being
broken. This is the nature of the beast.

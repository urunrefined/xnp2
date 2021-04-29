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
current distro. The compilation step should produce a
xnp2 artifact inside the x11 directory.

Bitmap Font:
After starting the xnp2 executable be sure to add the
font via the Emulate -> Font option. This is a very
crucial step, as when no font is loaded the text
in some applications will not appear properly.

Yes indeed, the font you choose will be loaded into
ROM and may be used by the PC-98 applications.

One thing you can try is the anex86.bmp font,
which surely you can find for yourself.

Why is the font not included? No idea, but there are
probably some legal reasons behind this and I am
not willing to go down that rabbit-hole to find out.

There is an autogeneration of fonts built-in,
but as far as I can tell it is broken, so supplying
your own fonts is (most likely) required.

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

While all of the menus are in English,
you will need some experience in Japanese
to fully enjoy most of the software available for
this system. Even just knowing hiragana and
katakana will go a long way.

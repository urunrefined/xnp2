This is the Vulkan/GLFW backend branch and is under heavy
development. Use the latest release on the 
master branch instead.

Building on Arch:

Install the following packages
glslang (for glslangValidator glsl -> spirv compiler)
glfw-wayland OR glfw-x11 depending on what DE/WM you are running
sdl2 (for sound & controller support)
fontconfig, harfbuzz, freetype for rendering fonts
cd into the "x11" directory and call "make".
For i386 support use "make cputype=i386" instead

Running:
To run you need:
glfw-wayland OR glfw-x11
sdl2
vulkan-icd-loader,
fontconfig
harfbuzz
freetype

Vulkan should already have been installed via your graphics drivers.
To run simply start ./xnp2 inside the x11 directory. You do not need
to set any config option except the font and whatever images you are using.
If you do not have a config already run the app once, then look into
the ~/.np2/ directory for the config and change the relevant params
(such as the path to the font). It should be self-explanatory.

If you see any message regarding (i386) you (most likely) need to compile
this emu with the cputype=i386 flag (see above).

Mouse capture is done via "clicking" on the emulator screen. To get out of
this mode press F12.

TODO:
* Create a new config file. The old one has a thousand options - most should
be discarded. Valid options IMHO are: viewport-mode, cpu type, memory,
attached FDDs/HDDs, MIDI modules / attached sound modules
Not sure what the format will be... TOML? JSON?
* Figure out what the FPU code is doing in i386. Disabling the FPU
preprocessor flag doesn't work
* Split i286 and i386 code, which is not contained in their respective
folders, into modules. The load i286 and i386 code dynamically depending
on a config option, meaning there will be no separate compiling of
different binaries
* OSD - There are some libs which make it easy to display text -- but is the
additonal dependency worth it? Simply use FreeType libs instead?
* Commandline options for all features -- Meaning you can program each and
every one of the available settings via the commandline
* The keymapping is incomplete and needs to be completed
* Proper mouse support (mouse is in but seems to me to be incorrect)
* Currently only one Vulkan command buffer is being used (there is only
one texture image). This needs to be done properly.
* Pass all midi data directly to timidity / fluidsynth without
parsing the timidity cfg
+ What exactly is the diff between i286c and i286x cores?

Building on Arch via PKGBUILD. You can build a package of this
program via makepkg. An PKGBUILD file is provided.
However, please note because of a hen/egg problem
you need to use the commit _after_ the release tag, as the archive on
github is only created (and the sha only calculated) after the git tag has
already been placed, due to the PKGBUILD file being inside the repo itself.

This is the Vulkan/GLFW backend branch and is under heavy
development. Use the latest release on the 
master branch instead.

Building on Arch:

Install the following packages
glslang (for glslangValidator glsl -> spirv compiler)
glfw-wayland OR glfw-x11 depending on what DE/WM you are running
sdl2 (for sound & controller support)

Running:
To run you need:
glfw-wayland OR glfw-x11
sdl2
vulkan-icd-loader 

Vulkan should already have been installed via your graphics drivers.

To run simply start ./xnp2 inside the x11 directory


TODO:
* Create a new config file. The old one has a thousand options - most should
be discarded. Valid options IMHO are: viewport-mode, cpu type, memory,
attached FDDs/HDDs, MIDI modules / attached sound modules.
Not sure what the format will be... TOML? JSON?
* OSD - There are some libs which make it easy to display text -- but is the
additonal dependency worth it? Simply use FreeType libs instead?
* Commandline options for all features -- Meaning you can program each and
every one of the available settings via the commandline
* The keymapping is incomplete and needs to be completed
* Proper mouse support (mouse is in but seems to me to be incorrect)
* Currently only one vk command buffer is being used (there is only
one texture image). This needs to be done properly.

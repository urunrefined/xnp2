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

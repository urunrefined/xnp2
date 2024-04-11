# XNP2 Katze

This is the Vulkan/GLFW backend branch and is under heavy
development. Use the latest release on the 
master branch instead.


## Building on Arch

Install the following packages
glslang (for glslangValidator glsl -> spirv compiler)
glfw for window creation
fontconfig, harfbuzz, freetype for rendering fonts
Simply call "make".

Running:
To run you need:
glfw
vulkan-icd-loader,
fontconfig
harfbuzz
freetype

Vulkan should already have been installed via your graphics drivers.
To run simply start ./xnp2. You do not need
to set any config option except the font and whatever images you are using.
If you do not have a config already run the app once, then look into
the ~/.np2/ directory for the config and change the relevant params
(such as the path to the font). It should be self-explanatory.

Mouse capture is done via "clicking" on the emulator screen. To get out of
this mode press F12.

To access the bios press F11 right at the start. If you miss it,
press SUPER-I to reset and press F11 right after.

i286 code is removed until general cleanup is finished.

Building on Arch via PKGBUILD. You can build a package of this
program via makepkg. A PKGBUILD file is provided.
However, please note because of a hen/egg problem
you need to use the commit _after_ the release tag, as the archive on
github is only created (and the SHA-hash only calculated) after the git tag has
already been placed, due to the PKGBUILD file being inside the repo itself.

The config locations have changed.
~.np2/res/ will be used if an artifact has a relative filename
~.np2/config contains all config files which can be given at the cmdline as a relative path
~.np2/config will also contain all other stuff pertaining to this config,
such as savestates (not working yet)

To show which configs are available use `xnp2 -l` or `xnp2 --listconfig`
To create a new config use `xnp2 -n` or `xnp2 --newconfig` (can't contain '/')

## MIDI
MIDI is only supported through two options:
* sending the midi bytes to a raw MIDI device
* sending the midi bytes to a file

For the raw alsa option set the midi configuration of "comxtype" to
"alsaraw". For the rawalsa mode set the option comxalhwto the hardware address of the raw midi
device to 

Example:
```
com1type = alsaraw
com1mdhw = hw:2,0
```

For the tty option set the option configuration to "tty".
In this case the config value in "com{X}ttyn" will be used.

Example:
```
com1type = tty
com1ttyn = /dev/ttyS0
```

### Raw ALSA MIDI:

Make sure the snd_virmidi module is loaded, if it is a module in your
system and is not automatically loaded.

You can do this via 
```
modprobe snd_virmidi
```

if the module was built.

Afterwards check that midi files are available in /dev/snd/
They should look something like:

midiC{X}D{Y}, midiC{X}D{Y} ... etc

X and Y being determined by various kernel configurations.

Use one of these as the destination, for example as the "com1mdhw" device. Make sure you connect the associated port to whatever sink is you want via `aconnect src dest`, src being the client port associated with the raw midi device, and dest being the synthesizer (sink) port.

A device is different from a port. In general anything with "hw:" prefix is
a device, anything without it is a port.

```
amidi -l
```
prints _devices_, example:

```
Dir Device    Name
[...]
IO  hw:2,0    Virtual Raw MIDI (16 subdevices)
IO  hw:2,1    Virtual Raw MIDI (16 subdevices)
[...]
```
The device nodes in dev can be examined:
`ls -la /dev/snd/midi*`

as an example results in:

```
[...]
crw-rw----+ 1 root audio 116, 15 Apr  2 18:39 /dev/snd/midiC2D0
crw-rw----+ 1 root audio 116, 16 Apr  2 18:39 /dev/snd/midiC2D1
[...]
```

If you don't have a proper file-listing, you most likely need to load the driver.
Make sure the snd_rawmidi driver is loaded, not only the snd_seq_virmidi
driver, which are two related but not identical things.


Ports can be printed the following way:

```
aconnect -l
```

Result:
```
client 0: 'System' [type=kernel]
    0 'Timer           '
    1 'Announce        '
[...]
client 14: 'Midi Through' [type=kernel]
    0 'Midi Through Port-0'
client 24: 'Virtual Raw MIDI 2-0' [type=kernel,card=2]
    0 'VirMIDI 2-0     '
client 25: 'Virtual Raw MIDI 2-1' [type=kernel,card=2]
    0 'VirMIDI 2-1     '
[...]
```

These listed ports are addressed as "0:0", "0:1", "14:0", "24:0", 25:0", etc.

You can test if this is working in general by using the standard alsa-utils
tooling.

Example:
```
modprobe snd_virmidi
amidi -p virtual -d
```

In this case amidi is waiting for data

```
aconnect -l`
```

gives:

```
[...]
client 24: 'Virtual Raw MIDI 2-0' [type=kernel,card=2]
    0 'VirMIDI 2-0     '
[...]
client 128: 'Client-128' [type=user,pid=17295]
    0 'Virtual RawMIDI '
[...]
```

client 24:0 is the port associated with the raw midi device.
client 128:0 is the port just created by amidi

Connect the raw midi port with the client port by amidi:
```
aconnect 24:0 128:0
```

Check if everything is proper:


```
[...]
client 24: 'Virtual Raw MIDI 2-0' [type=kernel,card=2]
    0 'VirMIDI 2-0     '
	Connecting To: 128:0
[...]
client 128: 'Client-128' [type=user,pid=17295]
    0 'Virtual RawMIDI '
	Connected From: 24:0
[...]
```
 
This is a modified example from the amidi documentation:
```
amidi -p "hw:2,0" -S 'F0 43 10 4C 00 00 7E 00 F7'
```

Make sure to replace the example address hw:2,0 with whatever is present on
your system. The amidi client should print:
```
F0 43 10 4C 00 00 7E 00 F7
```

If this is not the case, something else is wrong.

Please note that if you are using a software synthesizer, you will most likely
need to configure the synthesizer to use pulseaudio, as using alsa as an
pcm output (which is most likely what the sw synth is going to produce), will
conflict the pulseaudio ouput of xnp2.

### MIDI to file

In the file case the config value "ttyfile" will be used.
Please note this data is raw midi data (not a midi file), as such, to work the file must:

* already exist and
* not be a regular file

Otherwise just midi commands without any timing information
would be written to a regular file, which is not what is needed.

An example of this would be creating a pseudoterminal.

This of course also works with a real terminal, so you can attach
any serial midi synth without any specific alsa driver, and the
midi synthesizer should work as expected.

Please note in the original xnp2 the tty could be configured in the config
itself.  Instead in xnp2katze, this procedure is left to the user before starting.
See the man-page for stty:
```
man stty
```

To test this one could use:
```
socat pty,raw,echo=0,link=$PWD/socatpty1 pty,raw,echo=0,link=$PWD/socatpty2
```

In this example use com1ttyn set to socatpty1 (if the current working
directory is the same), and use for example, hexdump for the output
socatpty2:

```
hexdump -C socatpty2
```

## TODO
* Create a new config file. The old one has a thousand options - most should
be discarded. Valid options IMHO are: viewport-mode, cpu type, memory,
attached FDDs/HDDs, MIDI modules / attached sound modules
Not sure what the format will be... TOML? JSON?
* Figure out what the FPU code is doing in i386. Disabling the FPU
preprocessor flag doesn't work
* OSD - There are some libs which make it easy to display text -- but is the
additonal dependency worth it? Simply use FreeType libs instead?
* Commandline options for all features -- Meaning you can program each and
every one of the available settings via the commandline
* The keymapping is incomplete and needs to be completed
* Proper mouse support (mouse is in but seems to me to be incorrect)
* Currently only one Vulkan command buffer is being used (there is only
one texture image). This needs to be done properly
* Add back an i286 option

# TGAReader

This is a simple, cross-platform library for reading, writing, and manipulating
TGA images.

This library falls under the MIT license.

## Currently Supported Features

### Reading

* Truecolor
* Truecolor RLE
* Monochrome
* Monochrome RLE
* ColorMapped
* ColorMapped RLE

*Note:* Reading support is currently incomplete for Version 2.0

### Writing

* Truecolor
* Monochrome

### Modify

* Truecolor
* Truecolor RLE
* Monochrome
* Monochrome RLE

### Other Notes

As noted below in "Known Standard Breaks", the project currently does not support arbitrary-length bit-depths. 

The project currently does not actually read any of the version 2.0 specific TGA Information, except to verify that the footer is consistent with the Version 2.0 Spec. Reading a 2.0 image will currently work with no known issues, but the ability to modify or actually read the developer or extension fields does not work. Likewise, write support for these fields has not been implemented.

## Building

To build the project, simply make a build folder and run cmake.

```bash
mkdir build
cd build
cmake ..
```

From there, either run the makefile or open the Visual Studio Solution to build.

## Known Standard Breaks

Currently, the TGAReader library does not support arbitrary bit-depth images. The implementation currently supports 8-, 16-, 24-, and 32-bit TGAImages. There are currently no plans to support arbitrary bit-depth.

# TGAReader

This is a simple, cross-platform library for reading, writing, and manipulating
TGA images.

This library falls under the MIT license.

**Note:** This library is currently a work-in-progres and doesn't support color-mapped or encoded
TGA files. It also does not (yet) support writing files to disk. If you wish to contribute, any
interest is appreciated. Eventually I would like to have some convenience functions for loading the
TGA images into other libraries (e.g. loading into a pixbuf for gdk/gtk+), but that is sometime off
in the future.

##Building

To build the project, simply make a build folder and run cmake.

```bash
mkdir build
cd build
cmake ..
```

From there, either run the makefile or open the Visual Studio Solution to build.

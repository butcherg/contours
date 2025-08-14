# contours
Command line program to pull OpenSCAD polygons from photographs

## Introduction

In an endeavor to CAD-model a random stone pattern, I finally gave up and started to pick at using a picture of it directly.  Doing so I resurrected code I'd used with OpenCV, a humoungous collection of image maniuplation routines; a cursory search found what I needed, a routine that finds contours in an image.  Aptly called "FindContours", this is the core of the command-line program contours, the resident of this repository.

contours uses that routine to produce one of two outputs: 1) a .png of the input image with the identified contours drawn upon it, or 2) a text stream of OpenSCAD code that defines an array of polygon arrays corresponding to each identified contour.  Use the first output to test parameters and the viability of the input image, use the latter piped to a text file to capture the OpenSCAD array definition for use or include in your OpenSCAD script.  It's a command-line program, works pretty much the same way in either a Unix shell or Windows CMD window.

## Usage

```
$ contours <inputimagefile> [param=value [param=value]... ]
```
Where param can be any of the following, in any order to the right of the input image file:

- threshold: rubicon between black and white for the gray->binary translation, betwee 0 and 255.  Default: 128
- resize: if defined, resizes the input image using WxH, e.g., 'resize:640x480'.  If either width or height is not specified, the resize of that dimension is calculated with the proportion of the defined dimension to the original image dimension, e.g., 'resize:640', or 'resize:x480'
- epsilon: The value used to specify the degree of simplification of contours, larger is simpler.  Set to 0.0 to disable.  Default: 3.0
- border: if defined, draws a white border around the image, useful for isolating contours that butt up against the image edge.  Default width: 1
- minarea: minimum area of a valid contour.  Default: 0
- minpoints: culls polygons with number of points less than this number.  Default: 4
- destimage: if defined, outputs the original image wtih the contours drawn in red.  If not defined, program dumps an OpenSCAD array called 'p', contains the contours defined as point lists.

Note that if destimage is omitted, the OpenSCAD array definition prints to stdout, so it needs to be piped to a file to capture it.

The generated OpenSCAD code is a collection of arrays-of-arrays:

- p: array of the contour polygon arrays.
- pc: x,y center points of each contour.
- pw: w,h width/height coordinates of each contour.
- pt: translate coordinates for each polygon.  Note, the polygon coordinates in p are already translated, this array is for moving textures to align wtih contours.
- pr: a set of 0,0,0 coordinates for post-generation user translations, e.g., if you want a particular stone to cantilever out of the wall, set its pr z to some positive number and use it in translate().

## Building

If OpenCV is available for compiling/linking at a system location:

```
$ mkdir build && cd build
$ cmake ..
```
If one wants to build a copy of OpenCV specifically for contours:

```$ mkdir build && cd build
$ cmake -DBUILD_OPENCV=GITHUB|SRCPKG|<tar.gz-file> ..
```
The options are:
- GITHUB: The OpenCV library is git-cloned from its GitHub repository and built.
- SRCPKG: The OpenCV library .tar.gz is downloaded, unpacked, and built.
- .tar.gz-file: This is a path to an already-downloaded OpenCV .tar.gz source code release file

Recommend using -DBUILD_OPENCV=SRCPKG, as it will download version 4.12.0 which updates the minimum CMake versions to avoid bailing because new CMake is persnickity. 



## License
MIT

# contours
Command line program to pull OpenSCAD polygons from photographs

## Introduction

In an endeavor to CAD-model a random stone pattern, I finally gave up and started to pick at using a picture of it directly.  Doing so I resurrected code I'd used with OpenCV, a humoungous collection of image maniuplation routines; a cursory search found what I needed, a routine that finds contours in an image.  Aptly called "FindContours", this is the core of the command-line program contours, the resident of this repository.

contours uses that routine to produce one of two outputs: 1) a .png of the input image with the identified contours drawn upon it, or 2) a text stream of OpenSCAD code that defines an array of polygon arrays corresponding to each identified contour.  Use the first output to test parameters and the viability of the input image, use the latter piped to a text file to capture the OpenSCAD array definition for use or include in your OpenSCAD script.  It's a command-line program, works pretty much the same way in either a Unix shell or Windows CMD window.

## Usage

## Building

## License

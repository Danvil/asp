# Adaptive Superpixels

Adaptive Superpixels is a collection of superpixel algorithms for non-constant density functions.

**Algorithms:**
 * Depth-Adaptive Superpixels for RGB-D images
 * Adaptive Superpixels for color images with a user-defined density function
 * SLIC for color images -- see [ivrg.epfl.ch/research/superpixels](http://ivrg.epfl.ch/research/superpixels)

## Examples

#### Adaptive Superpixels

![Adaptive Superpixels](https://content.wuala.com/contents/Danvil/Public/asp/asp.png)
(left: color image, middle: user defined density function, right: ASP superpixels)

#### Depth-Adaptive Superpixels

![Depth-Adaptive Superpixels](https://content.wuala.com/contents/Danvil/Public/asp/dasp.png)
(left: color image, middle: depth image from Kinect, right: DASP superpixels)

## Installation

asp was tested under Ubuntu 14.04.

asp uses C++11 and requires an up to date compiler like GCC 4.8.x.

### Requirements

* Build essential: `sudo apt-get install build-essential g++ cmake cmake-qt-gui`
* [Eigen](http://eigen.tuxfamily.org) 3.x: `sudo apt-get install libeigen3-dev`
* slimage: Clone from https://github.com/Danvil/slimage (header only - no build required)
* OpenCV: `sudo apt-get install libopencv-dev` libasp itself does not require Qt or OpenCV, however the provided executable uses OpenCV to load and display images.

### Installation instructions (Linux)

1. `git clone git://github.com/Danvil/asp.git`
2. `cd asp; mkdir build; cd build`
3. `cmake-gui ..`
4. Press 'Configure', select 'Unix Makefiles' and press 'Finish'. Change `CMAKE_BUILD_TYPE` to `Release`! Adapt the other variables accordingly. Press 'Generate' and close the cmake gui.
5. `make`

### Things to try

* `bin/asp --method SLIC --color ../examples/toy_color.png`
* `bin/asp --method ASP --color ../examples/toy_color.png --density ../examples/density_squares.pgm`
* `bin/asp --method DASP --color ../examples/toy_color.png --depth ../examples/toy_depth.pgm`

## Scientific publications

David Weikersdorfer, **Efficiency by Sparsity: Depth-Adaptive Superpixels and Event-based SLAM** ([pdf](https://content.wuala.com/contents/Danvil/Public/publications/David%20Weikersdorfer%20-%20Efficiency%20by%20Sparsity.pdf)). *Technische Universität München*, 2014.

David Weikersdorfer, David Gossow, Michael Beetz, **Depth-Adaptive Superpixels** ([pdf](https://content.wuala.com/contents/Danvil/Public/dasp/weikersdorfer2012dasp.pdf)). *21-st International Conference on Patter Recognition (ICPR)*, 2012.

## I want to contribute 

Contact me on [GitHub](https://github.com/Danvil/) 

## License

asp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

asp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with asp. If not, see [www.gnu.org/licenses](http://www.gnu.org/licenses/).

# Video Classifier
## Description
Real time video classifier for visual points of attention during a road test.
## How to use
The program needs a video file (preferably in mp4 format) with a timestamp in the lower left corner and a csv file with the classified VPoA of the road test.
Dataset file must contain the spot column with one of the five valid VPoAs which are: CAMINO, IZQUIERDA, DERECHA, PIERNAS & AUDIO/CLIMA.
## Build and installation
Project depends on [OpenCV](https://docs.opencv.org/4.x/d0/d3d/tutorial_general_install.html) and [Tesseract](https://github.com/tesseract-ocr/tesseract), instructions on how to install both libraries can be found in the official websites for the libraries.
The project can be build with CMake and It's only tested to work under a linux environment.
To install excecute the following commands:
```bash 
cmake .
make
```

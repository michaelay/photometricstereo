# Photometric stereo prototype
Prototype of photometric stereo project on Macbook Pro with Nvidia chips. 
Build using Eclipse CDT

## Dependencies

* boost 
* opencv 
* glfw
* devil 

## Install instruction 

* upgrade xcode and install xcode command line tools in xcode preferences
* install Nvidia CUDA (optional)
* brew install opencv 
* brew install boost
* brew install devil
* brew install glfw

* add the line `#define ILUT_USE_OPENGL` in ilut.h (under /usr/local/Cellar/devil/[version]/include/IL)

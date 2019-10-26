# A Shader of Ice and Fire
The goal of this project is to create dynamic water, ice, lava and fire in OpenGL4.



## Build and run
```shell=sh
# Build
mkdir cmake-build && cd cmake-build
cmake ../ -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j

# Run
./a-shader-of-ice-and-fire
```


## Structure
* `lib/`: external libraries
* `resources/`: `OBJ` models
* `shaders/`: `GLSL` source code
* `src/`:`C++` source code and headers


## Libraries used
* GLAD (using OpenGL Loader-Generator):
    https://glad.dav1d.de
* GLFW3 (OpenGL window manager):
    https://github.com/glfw/glfw
* GLM (OpenGL mathematics):
    https://github.com/g-truc/glm
* Assimp (Asset import):
    https://github.com/assimp/assimp
* STB (Image loading):
    https://github.com/nothings/stb

## Results
Coming soon...

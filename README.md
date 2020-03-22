# A Shader of Ice and Fire
The goal of this project is to create dynamic water, ice, lava and fire in OpenGL 4.


## Build and run
```shell=sh
# Install irrKlang (needed for audio)
cp lib/irrKlang/libIrrKlang.so /usr/lib/

# Build
mkdir cmake-build && cd cmake-build
cmake ../ -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j

# Run
./a-shader-of-ice-and-fire
```


## Structure
* `audio/`: sound files
* `lib/`: external libraries
* `models/`: `OBJ` models
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
* irrKlang (Sound):
    https://www.ambiera.com/irrklang

## Results

### The scene
We decided to create our own scene on MagicaVoxel to be as free as possible.
However, we decided to keep the models simple, so that our work on the lava and ice would be more visible. 

### Fire mode
- The water computes its __refraction__ and __reflection__. The distortion effect is made using a __displacement map__. It also uses the __tesselation__ and __geometry__ shaders to create a tiny wave effect (changin the height locally).
- The lava texture is made using __Perlin noise__. It also uses the tesselation and geometry shaders.
- We added __HDR__ and __bloom__ effects.
- Some __particles__ are here to simulate lava ember (above the lava) and fireflies (on the water surface and gathered on the lamps). We used the __compute shader__ and __geometry shader__ to optimize it.

![](https://i.imgur.com/FZBX48l.jpg)

![](https://i.imgur.com/BGB5KOi.png)

![](https://i.imgur.com/joQEZlI.png)

### Ice mode
You can activate the Ice mode by pressing 'I'.
The color of the lights switch to blue and the water freeze progressively. 
Some __snow particles__ are falling down, and a snowy texture made wit __Perlin noise on 3 octaves__ appears slowly on the volcano.

![](https://i.imgur.com/k3wjo3y.jpg)


##### Transition
The transition between those 2 modes is pretty cool.
It is progressive, and there is a nice 2D effect (using a __FBO__ which renders the scene on a quad).

![](https://i.imgur.com/PBPU6B0.jpg)

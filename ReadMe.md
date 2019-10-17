# About this

This is a relatively simple whitted style ray tracer boasting the following features:
* Various primitives: Box, Disc, Plane, Rectangle, Sphere, and Triangle.
* Model loading and rendering.
* Texture loading and sampling, supporting everything stb\_image supports.
* Frustum culling.
* Camera with aspect ratio and FOV.
* BVH acceleration structure for both the scene and models.
* Programmable shaders.
* Per object materials.
* Krzysztof Narkowicz style ACES Filmic tone mapping.
* Point lights.
* Anti-aliasing, up to 16 rays per pixel.
* Chunked multi-threading.
* Per object transforms, for position, rotation, and scale.
* Bump mapping, and specular mapping.

## Screenshots

![Image of Georgia Tech's Chinese Dragon rendered using a glass material and 4 point lights](Images/Rendered-2019-10-17-06-02-18.png)

This is Georgia Tech's Chinese Dragon rendered as a glass material and 4 point lights for style points. It took roughly 16 minutes to render with 16x AA.

![Image of Crytek's Crysis 2 Nano Suit rendered with bump mapping, specular mapping, and light reflection](Images/Rendered-2019-10-17-18-12-05.png)

This is Crytek's Crysis 2 Nano Suit rendered with bump mapping, specular mapping, and light reflection.

![Image of a sphere with volumetric absorbtion](Images/Rendered-2019-10-17-21-26-26.png)

This is a sphere rendered with volumetric absorbtion, which code is in the abandoned VolumetricAbsorbtion branch. The branch is abandoned because it's pretty much a hack on the current architecture, which doesn't support giving the information volumetric absorbtion needs.

# Rough TODO/upcoming

These are the things I want to do or add, in no particular order, before I consider this project finished:
* Get rid of the useless disc primitive.
* Remove Color and simply use vectors instead.
* SIMD processing of multiple rays at once.
* Custom model format and loading, including conversion from OBJ.
* Post processing shaders.
* Pre processing shaders.
* Shadows for transparent objects.
* Volumetric absorbtion.
* Ray marched volumetric objects.
* Different and more accurate light types, such as area lights, emissive objects, directional lights, and point lights.
* Path tracing.
* Many many many optimizations.

# How to build

After cloning run `git submodule init && git submodule update` to fetch the `LotusWaf` dependency, which is needed for building.

## Linux (X11)

Install the following (development) packages:
* glfw-3
* libglm
* clang
* python3

```
./waf configure --target-configuration release
./waf
```

## Linux (headless)

Install the following (development) packages:
* libglm
* clang
* python3

```
./waf configure --target-configuration release --without-glfw-3 --without-OpenGL
./waf
```

## Windows

Coming soon after MSVC compilation support is back.
Does compile in MSYS2, see Linux steps on how to get it to compile.

# Running it

Next run it by executing `build/RayTracer` from this directory.
After rendering has completed, a window will pop up showing the render, if configured.
In all configurations, the render will output to a png in the working directory.

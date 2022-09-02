# HAWK v0.0.1

A small OpenGL program written while I was bored in programming class. It has been cleaned up and documented, I most likley won't be working on this in the future.

## Dependencies

+ SDL2
+ OpenGL
+ GLEW
+ CGLM

+ fast\_obj is distributed inside of this project and is created by 
  [Richard Knight](https://github.com/thisistherk/) under the 
  MIT License. 

+ stb\_image.h is also distributed with this project. stb header 
  files are created by [Sean Barrett](https://github.com/nothings) 
  and are distributed under the Public Domain.

## Writing Shaders

All vertex shaders should include these:

```
in vec3 aPos;    // The vertex positions
in vec3 aNormal; // The vertex Normal
in vec2 aTex;    // The texture Coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection; 
```

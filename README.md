# HAWK

A small OpenGL program written while I was bored in programming class.

## Dependencies

+ SDL2
+ OpenGL
+ GLEW
+ CGLM

+ fast\_obj is distributed inside of this project and is created by 
  [Richard Knight](https://github.com/thisistherk/) under the 
  Public Domain. 

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

It would be a possibility to the the MVP calculation on the CPU.

## Cool Shit To Do in The Future

Another future step is to import things like light positions through 
a 1D texture.

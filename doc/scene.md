# Scene Specification

This document will summarize how the scene files are structured.

## Boilerplate XML

```
<?xml version="1.0"?>
<SCENE bounds="res/models/apartment_floor_mesh.obj" >
	<object transform="0.0,0.0,0.0" rotation="0" path="res/models/apartment.obj" />
	<light position="0.0,1.8,0.0" color="1.0,0.8,0.8" constant="1.0" linear="0.3" quadratic="0.3" />
</SCENE>
```

The root element is `<SCENE>` which contains different objects and lights. At 
the moment nesting these objects inside of each other has no effect. All 
elements in a scene has properties which are used to render that element. Any 
properties that are not filled are defaulted to 0, unless anything else is 
specified here. 

## Elements

+ **SCENE**: The document root 
    - *bounds*: defines the walkable floor of a level

+ **object**: Describes a 3D object
    - *transform*: The object position in 3D space with three floats
      "x,y,z".
    - *rotation*: The rotation of the object.
    - *path*: Path to a .obj file. 

+ **light**
    - *position*: The light position in 3D space with three floats
    - *color*: The color of the light with three floats
    - *constant*: this should be kept at 1.0 so the denominator doesn't go below 
      1.0 but could be raised for a darkened effect. Default value: 1.0f.
    - *linear*: the linear reduces the light in a linear fasion relative to 
      distance. Default value: 1.0f.
    - *quadratic*: this will get much larger with distance compared to linear.
      Default value: 1.0f.

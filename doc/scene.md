# Scene Specification

This document will summarize how the scene files are structured.

## Basic XML

```
<?xml version="1.0"?>
<SCENE>
	<object transform="3.0,0.0,2.0" rotation="90" path="res/models/cabinet.obj" />
	<object transform="0.0,0.0,0.0" rotation="0" path="res/models/apartment.obj" />
</SCENE>
```

The root element is `<SCENE>` which contains *objects*.

Elements:

+ SCENE: The document root 
+ **object**: Describes a 3D object
  - *transform*: The object position in 3D space with three floats
                 "x,y,z".
  - *rotation*: The rotation of the object.
  - *path*: Path to a .obj file. 

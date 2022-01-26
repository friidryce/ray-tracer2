# Simple CPU-based Ray-Tracer in C++

**Authors: Jason Au, Allen Weng**

This program will take in a scene file (.test), parse it, raytrace the scene, and output it as <input_file_name>.png.

Examples of it in action can be found in the root directory:

![scene4-specular](https://user-images.githubusercontent.com/48192134/150923806-2e2aa96f-5d54-4600-ad94-0b7684973990.png)
![scene5](https://user-images.githubusercontent.com/48192134/150923816-4109f26a-6b9e-4a3a-8f07-d35999eec0ed.png)
![scene6](https://user-images.githubusercontent.com/48192134/150923854-2861e0ef-a8be-4ef4-b3b3-2d4a99fb24e4.png)

As of now, this program can be ran by compiling with the .sln file in Visual Studios, and either specifying the input .test file on Visual Studio's debugging settings or entering it as an argument on the command line.

## Guidance on Writing the Scene File
  
### Inputfile Format

- \# comments: This is a line of comments. Ignore any line starting with a #
- Blank line: Blank lines are ignored.
- command parameter1 parameter2 ...: Syntax for writing parsable information.

### Mandatory Commands

- size width height: The size command must be the first command of the file, which
controls the image size.
- maxdepth depth: The maximum depth (number of bounces) for a ray (default is 5).
- output filename: The name of the image output file (default is raytrace.png).
  
### Camera

- camera lookfromx lookfromy lookfromz lookatx lookaty lookatz upx upy
upz fov: 
  - Specifies camera properies. 
  - fov is in the y direction.
  - fov in the x direction will be determined by the image size. 
  - The world aspect ratio (distinct from the width and height that determine image aspect ratio) is always 1
  - A sphere at the center of a screen will look like a circle, not an ellipse, independent of the image aspect ratio.

### Geometry

- sphere x y z radius: Defines a sphere with a given position and radius.
- vertex x y z: Defines a vertex at the given location. The vertex is put into a pile, starting to be numbered at 0.
- tri v1 v2 v3: Create a triangle out of the vertices involved (which have previously been specified with the vertex command). The vertices are assumed to be specified in counterclockwise order. 

### Transformations

- translate x y z: A translation 3-vector.
- rotate x y z angle: Rotate by angle (in degrees) about the given axis.
- scale x y z: Scale by the corresponding amount in each axis (a non-uniform scaling).
- pushTransform: Push the current modeling transform on the stack.
- popTransform: Pop the current transform from the stack.

### Lights

- directional x y z r g b: The direction to the directional light source and the color in RGB.
- point x y z r g b: The location of a point light source and the color in RGB.
- attenuation const linear quadratic: Sets the constant, linear and quadratic attenuations (default 1,0,0).
- ambient r g b: The global ambient color to be added for each object (default is 0.2,0.2,0.2).

### Materials</h3>

- diffuse r g b: specifies the diffuse color of the geometry surface.
- specular r g b: specifies the specular color of the geometry surface.
- shininess s: specifies the shininess of the geometry surface.
- emission r g b: gives the emissive color of the geometry surface.

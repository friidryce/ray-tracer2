<h1>Simple CPU-based Ray-Tracer in C++</h1>

This program will take in a scene file (.test), parse it, raytrace the scene, and output it as <input_file_name>.png.

Examples of it in action can be found in the root directory:

![scene4-specular](https://user-images.githubusercontent.com/48192134/150923806-2e2aa96f-5d54-4600-ad94-0b7684973990.png)
![scene5](https://user-images.githubusercontent.com/48192134/150923816-4109f26a-6b9e-4a3a-8f07-d35999eec0ed.png)
![scene6](https://user-images.githubusercontent.com/48192134/150923854-2861e0ef-a8be-4ef4-b3b3-2d4a99fb24e4.png)

As of now, this program can be ran by compiling with the .sln file in Visual Studios, and either specifying the input .test file on Visual Studio's debugging settings or entering it as an argument on the command line.

<h2>Guidance on Writing the Scene File</h2>
  
<h3>Inputfile Format</h3>

• # comments: This is a line of comments. Ignore any line starting with a #.

• Blank line: Blank lines are ignored.

• command parameter1 parameter2 ...: Syntax for writing parsable information.

<h3>Mandatory Commands</h3>

• size width height: The size command must be the first command of the file, which
controls the image size.

• maxdepth depth: The maximum depth (number of bounces) for a ray (default is 5).

• output filename: The name of the image output file (default is raytrace.png).
  

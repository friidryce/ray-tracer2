/*****************************************************************************/
/* This is modified from the program skeleton for homework 2 in CSE167 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

/*****************************************************************************/
// IMPORTANT!!
// --------------
// If you want to use these functions,
// you need to either work this function into your object oriented approach,
// return the necessary variables as some sort of struct, or use global variables to get
// the scene info to your raytracer.
/*****************************************************************************/


/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix
// transformations for a stack (matransform) and to rightmultiply the
// top of a stack.  These functions are given to aid in setting up the
// transformations properly, and to use glm functions in the right way.
// Their use is optional in your program.


// The functions readvals and readfile do basic parsing.  You can of course
// rewrite the parser as you wish, but we think this basic form might be
// useful to you.  It is a very simple parser.

// Some parts are implementation dependent (You can use this as a guide, or
// connect this to your existing CSE 167 HW 2 skeleton.)
// *****************************************************************************/

using namespace std;
#include "file.h"
#include <glm/gtx/string_cast.hpp>

int canvas_width = 0;
int canvas_height = 0;
int MAX_DEPTH = 5;
std::string output_file;

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<glm::mat4> &transfstack, float* values) //was GLFloat*
{
    glm::mat4 transform = transfstack.top();
    glm::vec4 valvec = glm::vec4(values[0],values[1],values[2],values[3]);
    glm::vec4 newval = transform * valvec;
    for (int i = 0; i < 4; i++) values[i] = newval[i];
}

void goodtransform(stack<glm::mat4>& transfstack, glm::vec4 &values)
{
    glm::mat4 transform = transfstack.top();
    values = transform * values;
}

void rightmultiply(const glm::mat4 & M, stack<glm::mat4> &transfstack)
{
    glm::mat4 &T = transfstack.top();
    T = T * M;
}

// Function to read the input data values
bool readvals(stringstream &s, const int numvals, float* values) //was GLFloat**
{
    for (int i = 0; i < numvals; i++) 
    {
        s >> values[i];
        if (s.fail()) 
        {
            cout << "Failed reading value " << i << " will skip\n";
            return false;
        }
    }
    return true;
}

void readfile(const char* filename, Scene* scene)
{
    string str, cmd;
    ifstream in;

    // YOUR CODE HERE
    // We define some variables here as guides.  You will need to either include this function in some sort of class
    // to get access to them or define the variables globally.

    //Variable Definitions

    //Material Parameter Defaults
    glm::vec4 ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 specular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 emision = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float shininess = 10.0f;

    vector<glm::vec3> vertices;

    in.open(filename);
    if (in.is_open()) 
    {      
        // I need to implement a matrix stack to store transforms.
        // This is done using standard STL Templates
        stack <glm::mat4> transfstack;
        transfstack.push(glm::mat4(1.0f)); //identity matrix starting push
       
        getline (in, str);
        while (in) 
        {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) 
            {
                // Ruled out comment and blank lines
                
                stringstream s(str);
                s >> cmd;
                int i;
                float values[10]; // Position and color for light, colors for others
                // Up to 10 params for cameras.
                bool validinput; // Validity of input
                
                // Process the light, add it to database.
                // Lighting Command
                if (cmd == "point") 
                {
                    /*
                    if (numused == numLights) { // No more Lights
                        cerr << "Reached Maximum Number of Lights " << numused << " Will ignore further lights\n";
                    } 
                    else
                    {
                    */
                   
                    validinput = readvals(s, 6, values); // Position/color for lts.
                    if (validinput)
                    {
                        glm::vec3 rgb = glm::vec3(values[3], values[4], values[5]);

                        float c[] = { values[0], values[1], values[2], 1.0f };
                        matransform(transfstack, c);
                        glm::vec3 xyz = glm::vec3(c[0], c[1], c[2]);

                        //Transform into view space
                        glm::vec4 xyz1 = scene->camera->view * glm::vec4(xyz, 1.0f); 

                        scene->light.push_back(new Light(true, xyz1, rgb));
                    }
                }
                else if (cmd == "directional") 
                {
                    // YOUR CODE HERE.  You can use point lights as an example, or 
                    // implement both your own way.
                    validinput = readvals(s, 6, values);
                    if (validinput)
                    {
                        glm::vec3 rgb = glm::vec3(values[3], values[4], values[5]);

                        glm::mat3 view3(scene->camera->view);
                        glm::mat3 normalMatrix = glm::inverseTranspose(view3);

                        //Apply transformations
                        float c[] = { values[0], values[1], values[2], 1.0f };
                        matransform(transfstack, c);
                        glm::vec3 xyz = glm::vec3(c[0], c[1], c[2]);

                        //Transform into view space
                        glm::vec3 xyz1 = normalMatrix * xyz; 

                        scene->light.push_back(new Light(false, glm::vec4(xyz1, 0.0f), rgb));
                    }

                }
                else if (cmd == "attenuation") 
                {
                    // YOUR CODE HERE.
                    validinput = readvals(s, 3, values);
                    if (validinput)
                    {
                        scene->attenuation = glm::vec3(values[0], values[1], values[2]);
                    }
                }

                else if (cmd == "ambient") 
                {
                    validinput = readvals(s, 3, values); // colors
                    // YOUR CODE HERE
                    if(validinput)
                        ambient = glm::vec4(values[0], values[1], values[2], 1.0f);
                }
                // Material Commands
                // Ambient, diffuse, specular, shininess properties for each object.
                // Note that no transforms/stacks are applied to the colors.
                
                else if (cmd == "diffuse") 
                {
                    validinput = readvals(s, 3, values);
                    if (validinput) 
                    {
                        diffuse = glm::vec4(values[0], values[1], values[2], 1.0f);
                    }
                } 
                else if (cmd == "specular") 
                {
                    validinput = readvals(s, 3, values);
                    if (validinput) 
                    {
                        specular = glm::vec4(values[0], values[1], values[2], 1.0f);
                    }
                } 
                else if (cmd == "emission") 
                {
                    validinput = readvals(s, 3, values);
                    if (validinput) 
                    {
                        emision = glm::vec4(values[0], values[1], values[2], 1.0f);
                    }
                } 
                else if (cmd == "shininess") 
                {
                    validinput = readvals(s, 1, values);
                    if (validinput) 
                    {
                        shininess = values[0];
                    }
                } 
                else if (cmd == "size") 
                {
                    validinput = readvals(s,2,values);
                    if (validinput) 
                    {
                        // YOUR CODE HERE
                        // This the the image size, as width, height.
                        // Get these values and store them for use with your raytracer.
                        canvas_width = values[0];
                        canvas_height = values[1];
                    }
                }
                else if (cmd == "maxdepth") 
                {
                    validinput = readvals(s, 1, values);
                    // YOUR CODE HERE
                    if (validinput)
                        MAX_DEPTH = (int) values[0];
                }
                else if (cmd == "output") 
                {
                    //validinput = readvals(s, 1, values);
                    // YOUR CODE HERE
                    s >> output_file;
                }
                else if (cmd == "camera") 
                {
                    validinput = readvals(s,10,values); // 10 values eye cen up fov
                    if (validinput) 
                    {
                        // YOUR CODE HERE
                        // You'll need to read these values and use them for your own
                        // camera implementation.  Keep in mind for the raytracer the camera will be static.
                        
                        //eye
                        float x1 = values[0];
                        float y1 = values[1];
                        float z1 = values[2];

                        //target
                        float x2 = values[3];
                        float y2 = values[4];
                        float z2 = values[5];

                        //up
                        float upx = values[6];
                        float upy = values[7];
                        float upz = values[8];

                        //fov
                        float fov = values[9];

                        scene->camera->eye = glm::vec3(x1, y1, z1);
                        scene->camera->target = glm::vec3(x2, y2, z2);
                        scene->camera->up = glm::vec3(upx, upy, upz);
                        scene->camera->fovy = fov;
                        scene->camera->computeMatrices();
                    }
                 }
                              
                else if (cmd == "translate") 
                {
                    validinput = readvals(s,3,values);
                    if (validinput) 
                    {
                        glm::mat4 translateMatrix; 

                        // YOUR CODE HERE
                        // Implement a translation matrix.  You can just use glm built in functions
                        // if you want.

                        translateMatrix = glm::translate(glm::vec3(values[0], values[1], values[2]));
                      
                        rightmultiply(translateMatrix, transfstack);
                    }
                }
                else if (cmd == "scale") 
                {
                    validinput = readvals(s,3,values);
                    if (validinput) 
                    {
                        glm::mat4 scaleMatrix;

                        // YOUR CODE HERE
                        // Implement a scale matrix.  You can just use glm built in functions
                        // if you want.

                        scaleMatrix = glm::scale(glm::vec3(values[0], values[1], values[2]));

                        rightmultiply(scaleMatrix, transfstack);
                    }
                }
                else if (cmd == "rotate") 
                {
                    validinput = readvals(s,4,values);
                    if (validinput) 
                    {
                        float angle = (float)values[3];
                        glm::vec3 axis = glm::normalize(glm::vec3(values[0], values[1], values[2]));
                        glm::mat4 rotateMatrix;

                        // YOUR CODE HERE
                        // Implement a rotation matrix.  You can just use glm built in functions
                        // if you want.

                        angle = glm::radians(angle);
                        rotateMatrix = glm::rotate(angle, axis);

                        rightmultiply(rotateMatrix, transfstack);
                    }
                }

                else if (cmd == "maxverts") 
                {
                    validinput = readvals(s, 1, values);
                    // YOUR CODE HERE
                }
                else if (cmd == "sphere") 
                {
                    validinput = readvals(s, 4, values);
                    // YOUR CODE HERE
                    if (validinput)
                    {
                        glm::vec3 C = glm::vec3(values[0], values[1], values[2]);
                        float r = (float)values[3];

                        //Apply Transformations
                        float c[] = { C.x, C.y, C.z, 1.0f };
                        matransform(transfstack, c);
                        C = glm::vec3(c[0], c[1], c[2]);

                        //Convert into camera view coordinate after transformations
                        glm::vec3 newC = glm::vec3(scene->camera->view * glm::vec4(C, 1.0f));
                        
                        //Assign material values
                        Material m = { ambient, diffuse, specular, emision, shininess };

                        //Add to scene
                        //scene->geometry.push_back(new Geometry(m, newC, r, transfstack.top(), scene->camera->view));
                        scene->geometry.push_back(new Geometry(m, C, r*transfstack.top()[0][0], transfstack.top(), glm::mat4(1)));
                        //cout << glm::to_string(transfstack.top()) << endl;
                    }

                }
                else if (cmd == "tri") 
                {
                    validinput = readvals(s, 3, values);
                    // YOUR CODE HERE
                    if (validinput)
                    {
                        //Gather vertices
                        glm::vec3 v1 = vertices[values[0]];
                        glm::vec3 v2 = vertices[values[1]];
                        glm::vec3 v3 = vertices[values[2]];

                        //Apply transformations
                        float p1[] = { v1.x, v1.y, v1.z, 1.0f };
                        float p2[] = { v2.x, v2.y, v2.z, 1.0f };
                        float p3[] = { v3.x, v3.y, v3.z, 1.0f };
                        matransform(transfstack, p1);
                        matransform(transfstack, p2);
                        matransform(transfstack, p3);
                        v1 = glm::vec3(p1[0], p1[1], p1[2]);
                        v2 = glm::vec3(p2[0], p2[1], p2[2]);
                        v3 = glm::vec3(p3[0], p3[1], p3[2]);

                        //Convert into camera view coordinate after transformations
                        /*v1 = glm::vec3(scene->camera->view * glm::vec4(v1, 1.0f));
                        v2 = glm::vec3(scene->camera->view * glm::vec4(v2, 1.0f));
                        v3 = glm::vec3(scene->camera->view * glm::vec4(v3, 1.0f));*/

                        //Assign material values
                        Material m = { ambient, diffuse, specular, emision, shininess };

                        //Add to scene
                        scene->geometry.push_back(new Geometry(m, v1, v2, v3));
                    }
                }
                else if (cmd == "vertex") 
                {
                    validinput = readvals(s, 3, values);
                    // YOUR CODE HERE
                    if (validinput)
                    {
                        vertices.push_back(glm::vec3(values[0], values[1], values[2]));
                    }
                }


                
                // I include the basic push/pop code  for matrix stacks
                else if (cmd == "pushTransform") 
                {
                    transfstack.push(glm::mat4(1.0f));
                } 
                else if (cmd == "popTransform") //TODO
                {
                    if (transfstack.size() <= 1) 
                    {
                        cerr << "Stack has no elements.  Cannot Pop\n";
                    } 
                    else 
                    {
                        transfstack.pop();
                    }
                }
                
                else 
                {
                    cerr << "Unknown Command: " << cmd << " Skipping \n";
                }
            }
            getline (in, str);
        }
        

    } 
    else 
    {
        cerr << "Unable to Open Input Data File " << filename << "\n";
        throw 2;
    }
}

void saveimg(std::vector<BYTE> pixels, int width, int height, const char* filename) 
{
    // You need to get your image into the pixel vector.  How you do so is up to you.
    // Also, make sure you follow the directions in the writeup, and call FreeImage_Initialise() before using this function.

    FreeImage_Initialise();

    FIBITMAP* img = FreeImage_ConvertFromRawBits(pixels.data(), width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);

    std::cout << "Saving screenshot: " << filename << std::endl;

    FreeImage_Save(FIF_PNG, img, filename, 0);

    FreeImage_DeInitialise();
}


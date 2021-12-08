/**
 *  Scene File Parser
 */

using namespace std;
#include "file.h"

//Global variables initialization
int canvas_width = 0;
int canvas_height = 0;
int MAX_DEPTH = 5;
std::string output_file;

//Applies the appropriate transform to a 4-vector
void matransform(stack<glm::mat4> &transfstack, float* values) //was GLFloat*
{
    glm::mat4 transform = transfstack.top();
    glm::vec4 valvec = glm::vec4(values[0],values[1],values[2],values[3]);
    glm::vec4 newval = transform * valvec;
    for (int i = 0; i < 4; i++) values[i] = newval[i];
}

//Right multiplies the top of the transform stack with the desired transformation matrix to apply
void rightmultiply(const glm::mat4 & M, stack<glm::mat4> &transfstack)
{
    glm::mat4 &T = transfstack.top();
    T = T * M;
}

//Function to read the input data values
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

//Parse the file and initialize data structures and variables with the correct values
void readfile(const char* filename, Scene* scene)
{
    string str, cmd;
    ifstream in;

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
        stack <glm::mat4> transfstack;
        transfstack.push(glm::mat4(1.0f)); //identity matrix starting push
       
        getline (in, str);
        while (in) 
        {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) 
            {
                //Ruled out comment and blank lines
                
                stringstream s(str);
                s >> cmd;
                int i;
                float values[10]; //position and color for light, colors for others
                //Up to 10 params for cameras.
                bool validinput; //validity of input
                
                //Process the light, add it to database.
                //Lighting Command
                if (cmd == "point") 
                {                
                    validinput = readvals(s, 6, values); //Position/color for lts.
                    if (validinput)
                    {
                        //Read in color values
                        glm::vec3 rgb = glm::vec3(values[3], values[4], values[5]);

                        //Read in position and apply transformations to position
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
                    validinput = readvals(s, 6, values);
                    if (validinput)
                    {
                        //Read in color values
                        glm::vec3 rgb = glm::vec3(values[3], values[4], values[5]);

                        //Read in position and apply transformations to position
                        float c[] = { values[0], values[1], values[2], 1.0f };
                        matransform(transfstack, c);
                        glm::vec3 xyz = glm::vec3(c[0], c[1], c[2]);

                        //Transform into view space
                        glm::vec4 xyz1 = scene->camera->view * glm::vec4(xyz, 1.0f);

                        scene->light.push_back(new Light(false, xyz1, rgb));
                    }
                }
                else if (cmd == "attenuation") 
                {
                    validinput = readvals(s, 3, values);
                    if (validinput)
                    {
                        scene->attenuation = glm::vec3(values[0], values[1], values[2]);
                    }
                }

                //Material parameters parsing
                else if (cmd == "ambient") 
                {
                    validinput = readvals(s, 3, values);
                    if(validinput)
                        ambient = glm::vec4(values[0], values[1], values[2], 1.0f);
                }
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

                //Image size
                else if (cmd == "size") 
                {
                    validinput = readvals(s,2,values);
                    if (validinput) 
                    {
                        canvas_width = values[0];
                        canvas_height = values[1];
                    }
                }

                //Max depth of specular reflection recursion
                else if (cmd == "maxdepth") 
                {
                    validinput = readvals(s, 1, values);
                    if (validinput)
                        MAX_DEPTH = (int) values[0];
                }

                //Output file
                else if (cmd == "output") 
                {
                    s >> output_file;
                }

                //Camera parameters parsing
                else if (cmd == "camera") 
                {
                    validinput = readvals(s,10,values); 
                    if (validinput) 
                    {     
                        //Eye
                        float x1 = values[0];
                        float y1 = values[1];
                        float z1 = values[2];

                        //Target
                        float x2 = values[3];
                        float y2 = values[4];
                        float z2 = values[5];

                        //Up
                        float upx = values[6];
                        float upy = values[7];
                        float upz = values[8];

                        //Fov
                        float fov = values[9];

                        //Change camera attributes to provided values
                        scene->camera->eye = glm::vec3(x1, y1, z1);
                        scene->camera->target = glm::vec3(x2, y2, z2);
                        scene->camera->up = glm::vec3(upx, upy, upz);
                        scene->camera->fovy = fov;
                        scene->camera->computeMatrices();
                    }
                 }
                
                //Transformations parsing
                else if (cmd == "translate") 
                {
                    validinput = readvals(s,3,values);
                    if (validinput) 
                    {
                        glm::mat4 translateMatrix; 

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

                        angle = glm::radians(angle);
                        rotateMatrix = glm::rotate(angle, axis);

                        rightmultiply(rotateMatrix, transfstack);
                    }
                }

                //Maximum vertices parsing (stub)
                else if (cmd == "maxverts") 
                {
                    validinput = readvals(s, 1, values);
                }

                //Objects parsing
                else if (cmd == "sphere") 
                {
                    validinput = readvals(s, 4, values);
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
                        scene->geometry.push_back(new Geometry(m, newC, r, transfstack.top(), scene->camera->view));
                    }
                }
                else if (cmd == "tri") 
                {
                    validinput = readvals(s, 3, values);
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
                        v1 = glm::vec3(scene->camera->view * glm::vec4(v1, 1.0f));
                        v2 = glm::vec3(scene->camera->view * glm::vec4(v2, 1.0f));
                        v3 = glm::vec3(scene->camera->view * glm::vec4(v3, 1.0f));

                        //Assign material values
                        Material m = { ambient, diffuse, specular, emision, shininess };

                        //Add to scene
                        scene->geometry.push_back(new Geometry(m, v1, v2, v3));
                    }
                }

                //Vertices for triangles parsing
                else if (cmd == "vertex") 
                {
                    validinput = readvals(s, 3, values);
                    if (validinput)
                    {
                        vertices.push_back(glm::vec3(values[0], values[1], values[2]));
                    }
                }

                //Matrix stack operations parsing
                else if (cmd == "pushTransform") 
                {
                    transfstack.push(glm::mat4(1.0f));
                } 
                else if (cmd == "popTransform")
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

//Use FreeImage to generate a png
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


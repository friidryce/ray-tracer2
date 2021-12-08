#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Material.h"
#include "Scene.h"
#include "Geometry.h"
#include "FreeImage.h";
#include <cstring>
#include <string>

//Global variable to pass to RayTrace.cpp
typedef unsigned char BYTE;
extern int canvas_width;
extern int canvas_height;
extern int MAX_DEPTH;
extern std::string output_file;

using namespace std;
void matransform (stack<glm::mat4> &transfstack, float* values) ;
void rightmultiply (const glm::mat4 & M, stack<glm::mat4> &transfstack) ;
bool readvals (stringstream &s, const int numvals, float* values) ;
void readfile (const char * filename, Scene* scene) ;
void saveimg(std::vector<BYTE> pixels, int width, int height, const char* filename);
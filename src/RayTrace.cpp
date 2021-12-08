/**
 * Driver program for the ray-tracer 
 */

#include "RayTrace.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
using namespace std;

//Send a ray from the camera to a pixel
Ray RayTrace::RayThruPixel(Camera* cam, int i, int j, int width, int height)
{
	float a = 2 * ((i + 0.5f) / width) - 1; //x value of center of pixel (i, j)
	float b = 1 - 2 * ((j + 0.5f) / height); //y value of center of pixel (i, j)
	float aspectRatio = width / (float) height;

	//Convert FOV to radians
	float fovy_rad = cam->fovy * M_PI / 180.0f;

	//Return the direction of the ray in view space
	float aScale = a * (aspectRatio * glm::tan(fovy_rad / 2.0f));
	float bScale = b * glm::tan(fovy_rad / 2.0f);
	glm::vec3 d = glm::vec3(aScale, bScale, -1.0f);

	return Ray(glm::vec3(0.0f, 0.0f, 0.0f), d); //return a ray from eye (0,0,0) -> center of pixel (i, j), in view space
}

//Go through every pixel, shoot a ray from the camera to the pixel, check which geometry it intersects, and shade based on that intersection
vector<vector<glm::vec4>> RayTrace::Trace(Camera* cam, const int width, const int height)
{
	vector<vector<glm::vec4>> image(width, vector<glm::vec4>(height)); //initialize 2d vector[width][height] to represent screen

	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			Ray ray = RayThruPixel(cam, i, j, width, height); //generate a ray
			Intersection hit = Intersection(ray, scene); //check if that ray hits a geometry
			image[i][j] = FindColor(hit, 0); //color the pixel based on that hit
		}
		cout << j << endl; //print to console for each vertical row shaded (tracks progress)
	}
	return image;
}

//Assign a color based on the intersection
glm::vec4 RayTrace::FindColor(Intersection hit, int depth) //TODO
{
	glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); //default background color

	if (depth == MAX_DEPTH) //recursion exit condition
	{
		return color;
	}

	bool visible = true;

	if (hit.intersects) //only shade if ray hits a geometry
	{
		Material material = hit.object->material;
		vector<Light*> light = scene->light;
		glm::vec4 ambient = material.ambient;
		glm::vec4 diffuse = material.diffuse;
		glm::vec4 specular = material.specular;
		glm::vec4 emision = material.emision;
		float shininess = material.shininess;
		glm::vec4 sum = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		glm::vec3 N = hit.normal;
		glm::vec3 V = hit.toRay;
		
		
		for (unsigned int i = 0; i < light.size(); i++) //go through every light source
		{
			visible = true; //reset visibility variable for next light
			
			Ray shadowRay;

			glm::vec3 L; //direction to the light
			float R = 0.0f;

			if (light[i]->isPoint) //if light source is a point light
			{
				L = glm::normalize(glm::vec3(light[i]->position) - hit.position);
				shadowRay = Ray(hit.position + (0.001f * N), L);
				R = glm::distance(hit.position, glm::vec3(light[i]->position));
			}
			else //if light source is a directional light
			{
				L = glm::normalize(-glm::vec3(light[i]->direction));
				shadowRay = Ray(hit.position + (0.001f * N), L);
			}

			//check if shadow ray hits an object (if it does, that means another object is blocking the light from reaching our intersection)
			for (unsigned int j = 0; j < scene->geometry.size(); j++)
			{
				
				Intersection blockingHit = Intersection(shadowRay, scene->geometry[j]);
				
				if (light[i]->isPoint) //point light
				{
					if (scene->geometry[j] != hit.object && blockingHit.intersects && blockingHit.distance < R) //make sure no self-intersects, no intersection if object is behind light
					{
						visible = false;
						break; //skip adding in the bling-phong for this light if not visible
					}
				}
				else //directional light
				{
					if (scene->geometry[j] != hit.object && blockingHit.intersects) //make sure no self-intersects
					{
						visible = false;
						break;
					}
				}
			}

			if (visible) //shade the object with the diffuse and blinn-phong specular contribution from this light if it's not obstructed
			{
				glm::vec3 H = glm::normalize(V + L);
				sum += ((light[i]->color / (scene->attenuation[0] + (scene->attenuation[1] * R) + (scene->attenuation[2] * (R * R)))) *
					   ((diffuse * glm::max(glm::dot(N, L), 0.0f)) + (specular * pow(glm::max(glm::dot(N, H), 0.0f), shininess))));
			}
		}

		color = ambient + emision + sum; //add object's ambient and emision to the final color
		
		//Shoot a reflection ray from the object
		glm::vec3 reflectionDir = glm::normalize(glm::reflect(-V, N));
		glm::vec3 offset = 0.001f * reflectionDir;
		Ray ray2 = Ray(hit.position + offset, reflectionDir);
		
		//Check the reflection intersection and add specular color recursively
		Intersection hit2 = Intersection(ray2, scene);
		color += specular * FindColor(hit2, depth+1);
	}
	return abs(color);
}

int main(int argc, char* argv[])
{
	//Read the input file
	const char* filename = NULL;
	filename = argv[1];

	std::cout << "Input file: " << filename << std::endl;

	//Initiate a ray-tracing sequence
	RayTrace* rayTrace = new RayTrace();

	readfile(filename, rayTrace->scene);

	//Set up output parameters
	int width = canvas_width;
	int height = canvas_height;
	char* output = &output_file[0];

	//Collect color data
	vector<vector<glm::vec4>> pixels;
	pixels = rayTrace->Trace(rayTrace->scene->camera, width, height);

	//Convert color data matrix into a byte array
	vector<BYTE> prep;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			prep.push_back((BYTE)(255.0f * (pixels[i][j])[2]));
			prep.push_back((BYTE)(255.0f * (pixels[i][j])[1]));
			prep.push_back((BYTE)(255.0f * (pixels[i][j])[0]));
		}
	}

	//Use FreeImage to generate a png based on color data byte array
	saveimg(prep, width, height, output);

	return 0;
}
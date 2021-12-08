#pragma once

/** 
 * Data structure for a scene, holds information on the camera, objects present, and lights present
 */

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <Geometry.h>
#include "Camera.h"
#include "Light.h"

class Scene
{
public:
	Camera* camera;
	std::vector<Geometry*> geometry;
	std::vector<Light*> light;
	glm::vec3 attenuation = glm::vec3(1.0f, 0.0f, 0.0f);

	Scene()
	{
		camera = new Camera;
	}
	
	~Scene()
	{
		delete camera;

		for(auto i: geometry) //delete geometry
			delete i;

		for (auto i : light) //delete light
			delete i;
	}
};
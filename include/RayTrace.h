#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Camera.h"
#include "Scene.h"
#include "Ray.h"
#include "Intersection.h"
#include "Material.h" //probably don't need this in final
#include "file.h";
#include <vector>
#include <math.h>
#include <iostream>

#ifndef __RAYTRACE_H__
#define __RAYTRACE_H__

using namespace std;

class RayTrace
{
public:
	Scene* scene;

	RayTrace()
	{
		scene = new Scene();
	}

	~RayTrace()
	{
		delete scene;
	}

	Ray RayThruPixel(Camera* cam, int i, int j, int width, int height);
	vector<vector<glm::vec4>> Trace(Camera* cam, const int width, const int height);
	glm::vec4 FindColor(Intersection hit, int depth);
};

#endif
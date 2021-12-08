/**
 * Container for information on a ray-object intersection
 */

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <iostream>

#include "Geometry.h"
#include "Ray.h"
#include "Scene.h"

#ifndef __INTERSECTION_H__
#define __INTERSECTION_H__

class Intersection
{
public:
	glm::vec3 position; //position of the intersection
	glm::vec3 normal; //surface normal
	glm::vec3 toRay; //direction towards the source of the incoming ray;
	bool intersects; //whether the intersection point is in the geometry
	Geometry* object; //pointer to object;
	float distance; //distance to the source of the ray

	Intersection(const Ray ray, Geometry* geo); //ray-object intersection
	Intersection(const Ray ray, Scene* scene); //ray-scene intersection

	Intersection() //trivial default constructor
	{
		intersects = false;
		position = glm::vec3(0, 0, 0);
		normal = glm::vec3(0, 0, 0);
		distance = FLT_MAX;
		toRay = glm::vec3(0, 0, 0);
		object = nullptr;
	}
};

#endif
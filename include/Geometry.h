/**
 * Data structure for triangles or spheres
 */

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/normal.hpp>
#include "Material.h"

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

class Geometry 
{
public:
	bool isTriangle;
	Material material;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	glm::vec3 n;
	glm::vec3 c;
	glm::mat4 scale; //transformation matrix for sphere scaling
	glm::mat4 view; //view matrix for sphere scaling
	float r;

	Geometry(Material m, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	Geometry(Material m, glm::vec3 center, float radius, glm::mat4 trans, glm::mat4 view);
};

#endif 

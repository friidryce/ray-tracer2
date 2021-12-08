#include "Geometry.h"

//Create a triangle geometry
Geometry::Geometry(Material m, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	//Useful
	isTriangle = true;
	material = m;
	v1 = p1;
	v2 = p2;
	v3 = p3;
	n = glm::normalize(glm::triangleNormal(v1, v2, v3));
	
	//Garbage
	c = glm::vec3(0, 0, 0);
	r = 0.0f;
	scale = glm::mat4(1.0f);
	this->view = glm::mat4(1.0f);
}

//Create a sphere geometry
Geometry::Geometry(Material m, glm::vec3 center, float radius, glm::mat4 trans, glm::mat4 view)
{
	//Useful
	isTriangle = false;
	material = m;
	c = center;
	r = radius;
	scale = trans; //transformation matrix of the sphere
	this->view = view; //view matrix passed to convert coordinates later

	//Garbage
	v1 = glm::vec3(0, 0, 0);
	v2 = glm::vec3(0, 0, 0);
	v3 = glm::vec3(0, 0, 0);
	n = glm::vec3(0, 0, 1);
}


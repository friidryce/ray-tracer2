#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#ifndef __RAY_H__
#define __RAY_H__

class Ray
{
public:
	glm::vec3 origin;
	glm::vec3 direction;
	Ray()
	{
		origin = glm::vec3(0, 0, 0);
		direction = glm::vec3(1, 0, 0);
	}
	Ray(glm::vec3 o, glm::vec3 d);
	glm::vec3 calculatePosition(float t);
};

#endif 
#include "Ray.h"

Ray::Ray(glm::vec3 o, glm::vec3 d)
{
	origin = o;
	direction = glm::normalize(d);
}

glm::vec3 Ray::calculatePosition(float t)
{
	return glm::vec3(origin + t*direction);
}
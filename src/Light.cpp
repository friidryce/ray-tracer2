#include "Light.h"

Light::Light(bool p, glm::vec4 xyz1, glm::vec3 rgb)
{
	isPoint = p;
	if (isPoint)
	{
		position = xyz1;
	}
	else
	{
		direction = glm::normalize(xyz1);
	}

	color = glm::vec4(rgb, 1.0f);
}
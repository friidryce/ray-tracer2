/**************************************************
Light is a class for a camera object.
*****************************************************/
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#ifndef __LIGHT_H__
#define __LIGHT_H__

class Light 
{
public:
    bool isPoint;
    glm::vec4 position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 direction = glm::normalize(glm::vec4(1.0f));
    glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    Light(bool p, glm::vec4 xyz1, glm::vec3 rgb);
};

#endif 

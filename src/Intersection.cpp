#include "Intersection.h"
#include <glm/gtx/string_cast.hpp>

void IntersectCircle(glm::vec3 origin, glm::vec3 direction, 
                     glm::vec3 center, float radius, float& distance, bool &intersects, glm::vec3 &iPos, glm::vec3 &iNormal)
{
    glm::vec3 oc = origin - center;
    float a = glm::dot(direction, direction);
    float b = 2.0 * glm::dot(oc, direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        intersects = false;
        distance = FLT_MAX;
    }
    else {
        intersects = true;
        distance = (-b - sqrt(discriminant)) / (2.0 * a);
        float distance2 = (-b + sqrt(discriminant)) / (2.0 * a);
        if (distance < 0)
            distance = distance2;
        if (distance2 < distance)
            distance = distance2;

        iPos = origin + distance * direction;
        iNormal = glm::normalize(iPos - center);
    }
}

Intersection::Intersection(const Ray ray, Geometry* geo)
{
    if (geo->isTriangle)
    {
        glm::vec3 baryCoord;

        glm::vec4 p0 = glm::vec4(ray.origin, 1);
        glm::vec4 p1 = glm::vec4(geo->v1, 1.0f);
        glm::vec4 p2 = glm::vec4(geo->v2, 1.0f);
        glm::vec4 p3 = glm::vec4(geo->v3, 1.0f);
        glm::vec4 p4 = glm::vec4(-ray.direction, 0);

        glm::mat4 pointMatrix = glm::mat4(p1, p2, p3, p4);
        pointMatrix = glm::inverse(pointMatrix);

        glm::vec4 barycentric = pointMatrix * p0;
        float l1 = barycentric[0];
        float l2 = barycentric[1];
        float l3 = barycentric[2];
        float t = barycentric[3];

        if (l1 >= 0 && l2 >= 0 && l3 >= 0 && t >= 0) //we have an intersection
        {
            intersects = true;
            position = l1 * geo->v1 + l2 * geo->v2 + l3 * geo->v3;
            normal = geo->n;

            float ndot = glm::dot(normal, geo->v1);
            if (ndot > 0)
                normal = -normal;

            distance = glm::distance(ray.origin, position);

            toRay = -ray.direction;
            object = geo;
        }
        else
        {
            intersects = false;
            position = glm::vec3(0, 0, 0);
            normal = glm::vec3(0, 0, 0);
            distance = FLT_MAX;
            toRay = glm::vec3(0, 0, 0);
            object = geo;
        }

    }
    else //Intersection Circle
    {
        glm::vec3 iPos = glm::vec3(0,0,0);
        glm::vec3 iNormal;

        if (geo->scale[0][0] == geo->scale[1][1] && geo->scale[1][1] == geo->scale[2][2]) //sphere
        {
            //std::cout << "sphere" << std::endl;
            
            /*if (glm::intersectRaySphere(ray.origin, -ray.direction, geo->c, geo->r, iPos, iNormal))
            {
                intersects = true;
                position = iPos;
                normal = glm::normalize(iNormal);
                distance = glm::distance(ray.origin, position);
                toRay = -ray.direction;
                object = geo;
            }
            else
            {
                intersects = false;
                position = glm::vec3(0, 0, 0);
                normal = glm::vec3(0, 0, 0);
                distance = FLT_MAX;
                toRay = glm::vec3(0, 0, 0);
                object = geo;
            }*/
            
            
            
            IntersectCircle(ray.origin, ray.direction, geo->c, geo->r, distance, intersects, iPos, iNormal);

            if (intersects)
            {
                position = iPos;
                normal = glm::normalize(iPos - geo->c);
                toRay = -ray.direction;
                object = geo;
            }
            else
            {
                position = glm::vec3(0, 0, 0);
                normal = glm::vec3(0, 0, 0);
                distance = FLT_MAX;
                toRay = glm::vec3(0, 0, 0);
                object = geo;
            }

            //float delta = sqrt(pow(dot(ray.direction, ray.origin - geo->c), 2) - pow(glm::length(ray.origin - geo->c), 2) + pow(geo->r, 2));

            //if (delta >= 0)
            //{
            //    intersects = true;
            //    float t = dot(-ray.direction, ray.origin - geo->c) - delta;
            //    position = ray.origin + t * ray.direction;
            //    normal = normalize(position - geo->c);
            //    distance = glm::distance(ray.origin, position);
            //    //normal = glm::vec3(.5);
            //    toRay = -ray.direction;
            //    object = geo;
            //}
            //else
            //{
            //    intersects = false;
            //    position = glm::vec3(0, 0, 0);
            //    normal = glm::vec3(0, 0, 0);
            //    distance = FLT_MAX;
            //    toRay = glm::vec3(0, 0, 0);
            //    object = geo;
            //}

        }
        else
        {
            //std::cout << "ellip" << std::endl;
            //Get a ray in world coordinate (revert view matrix transformation)
            glm::vec3 viewOrigin = glm::vec3(glm::inverse(geo->view) * glm::vec4(ray.origin, 1));
            glm::vec3 viewDirection = glm::normalize(glm::inverse(glm::mat3(geo->view)) * ray.direction);

            //Get a ray in pre-transformation coordinate (before transformation and view)
            glm::vec3 modelOrigin = glm::vec3(glm::inverse(geo->scale) * glm::vec4(viewOrigin, 1));
            glm::vec3 modelDirection = glm::normalize(glm::inverse(glm::mat3(geo->scale)) * viewDirection);

            glm::vec3 viewC = glm::vec3(glm::inverse(geo->view) * glm::vec4(geo->c, 1.0f));
            glm::vec3 modelC = glm::vec3(glm::inverse(geo->scale) * glm::vec4(viewC, 1.0f));

            if (glm::intersectRaySphere(modelOrigin, modelDirection, modelC, geo->r, iPos, iNormal))
            {
                intersects = true;

                glm::vec4 viewPosition4 = geo->scale * glm::vec4(iPos, 1.0f);
                glm::vec3 viewPosition = glm::vec3(viewPosition4);
                glm::vec4 position4 = geo->view * glm::vec4(viewPosition, 1.0f);
                position = glm::vec3(position4);
                
                glm::vec3 viewNormal = glm::normalize(glm::inverseTranspose(glm::mat3(geo->scale)) * iNormal);
                normal = glm::normalize(glm::inverseTranspose(glm::mat3(geo->view)) * viewNormal);
                //normal = glm::vec3(1);

                distance = glm::distance(ray.origin, position);
                toRay = -ray.direction;
                object = geo;
            }
            else
            {
                intersects = false;
                position = glm::vec3(0, 0, 0);
                normal = glm::vec3(0, 0, 0);
                distance = FLT_MAX;
                toRay = glm::vec3(0, 0, 0);
                object = geo;
            }
            

            /*
            IntersectCircle(modelOrigin, modelDirection, modelC, geo->r, distance, intersects, iPos, iNormal);

            if (intersects)
            {
                glm::vec4 viewPosition4 = geo->scale * glm::vec4(iPos, 1.0f);
                glm::vec3 viewPosition = glm::vec3(viewPosition4);
                glm::vec4 position4 = geo->view * glm::vec4(viewPosition, 1.0f);
                position = glm::vec3(position4);

                glm::vec3 viewNormal = glm::normalize(glm::inverseTranspose(glm::mat3(geo->scale)) * iNormal);
                normal = glm::normalize(glm::inverseTranspose(glm::mat3(geo->view)) * viewNormal);

                distance = glm::distance(ray.origin, position);
                toRay = -ray.direction;
                object = geo;
            }
            else
            {
                position = glm::vec3(0, 0, 0);
                normal = glm::vec3(0, 0, 0);
                distance = FLT_MAX;
                toRay = glm::vec3(0, 0, 0);
                object = geo;
            }
            */
            
        }
    }
}

Intersection::Intersection(const Ray ray, Scene* scene)
{
    float mindist = FLT_MAX;
    Intersection hit;

    for (auto object : scene->geometry) 
    {
        Intersection hit_tmp = Intersection(ray, object);

        if (hit_tmp.distance < mindist)
        {
            mindist = hit_tmp.distance;
            hit = hit_tmp;
        }
    }

    intersects = hit.intersects;
    position = hit.position;
    normal = hit.normal;
    distance = hit.distance;
    toRay = hit.toRay;
    object = hit.object;
}


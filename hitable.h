#ifndef HITABLEH
#define HITABLEH

#include "ray.h"
#include "material.h"

typedef struct hit_record {
    // the parameter for ray
    float t;
    // the vector of hit point of the sphere
    Vec3 p;
    // the  normal vector of hit point (will be a unit vector)
    Vec3 normal;
    
    Material material;

    // the index of the scece list
    int in_scene_index;
} hit_record;

//* Virtual class Hit is the parent of all objects that can be hit by rays.
class Hitable {
    public:
        virtual bool hit(const Ray &ray, float t_min, float t_max, hit_record &record) const = 0;
        //* Check if there is intersection of the sphere.
        //* ray is the incident light.
        //* t_min and t_max are the visual effect distances.
        //* (Because <0 and too far are equivalent to see no effect.)
        //* record is record of the cloest intersected point.
};

#endif
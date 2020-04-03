#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"
#include "material.h"

class Sphere : public Hitable {
    public:
        /* constructors */
        Sphere() {}
        Sphere(Vec3 cen, float r, Material m = Material()) {
            center = cen;
            radius = r;
            material = m;
        }

        Material get_material() const {
            return material;
        }

        /* override virtual method of Hitable */
        virtual bool hit(const Ray &ray, float t_min, float t_max, hit_record &record) const;

        bool shadow_hit(const Ray &ray, float &t_max) const;

    private:
        Vec3 center;
        float radius;
        Material material;
};

bool Sphere::hit(const Ray &ray, float t_min, float t_max, hit_record &record) const {
    //* Check there is intersection of the sphere.

    Vec3 OC = ray.origin() - center;
    // According to quadratic formula.
    float a = dot(ray.direction(), ray.direction());
    float b = 2.0 * dot(ray.direction(), OC);
    float c = dot(OC, OC) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant > 0) {
        // Check from -t whether it is in the visible range (t_min ~ t_max),
        // because it will usually be a cloest hit point.
        // And if we see this cloest point, it will obscure the far point (+ t),
        // so there is no need to check + t.
        float temp = (-b - sqrt(discriminant)) / (2.0 * a);
        if (temp < t_max && temp > t_min) {
            record.t = temp;
            record.p = ray.point_at_parameter(record.t);
            // (record.p - center) / radius = unit the normal vector
            // = (record.p - center).make_unit_vector
            record.normal = (record.p - center) / radius;
            record.material = material;

            return true;
        }

        temp = (-b + sqrt(discriminant)) / (2.0 * a);
        if (temp < t_max && temp > t_min) {
            record.t = temp;
            record.p = ray.point_at_parameter(record.t);
            record.normal = (record.p - center) / radius;
            record.material = material;

            return true;
        }
    }
    // else {
    //     return false;
    // }
    // If use else and return, need to remember add return at last again.
    // Or, some information will be lost.
    // (Because defalt return 1, but it must false and be skybox.
    // And then, the record is defalt value to return to deal with a sphere)
    return false;
}

bool Sphere::shadow_hit(const Ray &ray, float &t_min) const {
    //* Check this sphere is be hit by shadow ray or not.
    //* Just need to find that there is intersection point or no, do not need to
    //* find the closest.

    Vec3 OC = ray.origin() - center;
    float a = dot(ray.direction(), ray.direction());
    float b = 2.0 * dot(ray.direction(), OC);
    float c = dot(OC, OC) - radius * radius;
    float discriminant = b * b - 4.0 * a * c;

    if (discriminant > 0) {
        float temp = (-b - sqrt(discriminant)) / (2.0 * a);
        if (temp > t_min) {
            t_min = temp;
            return true;
        }

        temp = (-b + sqrt(discriminant)) / (2.0 * a);
        if (temp > t_min) {
            t_min = temp;
            return true;
        }
    }
    return false;
}

#endif
#ifndef MATERIALH
#define MATERIALH

#include <math.h>

#include "hitable.h"
#include "material.h"

class Material {
    public:
        Material() : kd(Vec3(1.0, 1.0, 1.0)), w_r(0.0), w_t(0.0) {};
        Material(const Vec3 &_kd, float w_ri, float w_ti) : kd(_kd), w_r(w_ri), w_t(w_ti) {};

        Vec3 get_kd() const {
            return kd;
        } 

        float get_wr() const {
            return w_r;
        }

        float get_wt() const {
            return w_t;
        }

    private:
        // Diffuse coefficient with color
        Vec3 kd;
        // Reflected coefficient
        float w_r;
        // Refracted coefficient
        float w_t;
};

Vec3 reflect(const Vec3 &direction, const Vec3 &normal) {
    //* Compute the reflected direction.

    Vec3 unit_direction = unit_vector(direction);
    // Formula is 2N(dot(N, Rin)) - Rin
    // Rin is -direction because of opposite direction.
    // So, formula = direction - 2N(dot(N, direction)).
    return unit_direction - dot(direction, normal) * 2 * normal;
}

Vec3 refract(const Vec3 &direction, const Vec3 &normal, float n_over_nt) {
    //* Compute the refracted direction.
    //* n_over_nt is the coefficient calculated using Snell law = n/nt.

    Vec3 refract_normal = normal;
    Vec3 unit_direction = unit_vector(direction);
    float cos_theta = dot(unit_direction, refract_normal);

    // Judge out or in.
    if (cos_theta < 0) {
        // Outside the surface.
        // Correct the cos(theta) to be positive.
        cos_theta = -cos_theta;
    } else {
        // Inside the surface.
        // cos(theta) is already positive but reverse normal direction.
        refract_normal = -refract_normal;
        // Need to swap the refracted coefficent.
        n_over_nt = 1 / n_over_nt;
    }

    // Calculate the refracted direction.
    // sin^2(lo) = 1 - cos^2(lo)
    // n_over_nt^2 * sin^2(theta) = sin^2(lo) (because of n * sin(theta) = nt * sin(lo))
    // 1 - sin^2 (lo) = cos^2(lo)
    // Check the lo is >0 (lo: 0~90)
    float discriminant = 1.0 - n_over_nt * n_over_nt * (1 - cos_theta * cos_theta);
    if (discriminant > 0) {
        // Draw vector and you will know why by diagonal proportionality and vector subtraction...
        Vec3 refracted = n_over_nt * (unit_direction + normal * cos_theta) - normal * sqrt(discriminant);
        return refracted;
    }
    // total internal reflection.
    return Vec3(0, 0, 0);
}

#endif
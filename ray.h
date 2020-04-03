#ifndef RAYH
#define RAYH

#include "vec3.h"

class Ray {
    public:
        /* constructors */
        Ray() {} 
        Ray(const Vec3& vector_a, const Vec3& vector_b) {
            // Notice the vectors are not unit vector.
            O = vector_a;
            D = vector_b;
        }
        Ray(const Ray& ray) {
            O = ray.origin();
            D = ray.direction();
        }

        /* accessors */
        Vec3 origin() const {
            return O;
        }

        Vec3 direction() const {
            return D;
        }

        /* other methods */
        Vec3 point_at_parameter(float t) const {
            return O + t * D;
        }

    private:
        Vec3 O;
        Vec3 D;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &r) {
    os << "Ray <" << r.origin() << ", " << r.direction() << ">";
    return os;
}

#endif
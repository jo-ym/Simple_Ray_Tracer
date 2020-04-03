#ifndef CAMERAH
#define CAMERAH

#include <math.h>
#define M_PI 3.14159265358979323846
#include "ray.h"

class Camera {
    public:
        /* constructors */
        Camera() {
            lower_left_corner = Vec3(-2.0, -1.0, -1.0);
            origin = Vec3(0.0, 0.0, 1.0);
            horizontal = Vec3(4.0, 0.0, 0.0);
            vertical = Vec3(0.0, 2.0, 0.0);
        }

        Camera(Vec3 lower_left_corner, Vec3 origin,
               Vec3 horizontal, Vec3 vertical) {
            this->lower_left_corner = lower_left_corner;
            this->origin = origin;
            this->horizontal = horizontal;
            this->vertical = vertical;
        }
        // Camera(Vec3 look_from, Vec3 look_at, Vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
        //     //* vup: The camera's tilt direction is view up (referred to as vup, generally set to (0, 1, 0)).
        //     //* vfov: top to bottom in degrees
        //     //* aspect = width / height
        //     lens_radius = aperture / 2;
        //     float theta = vfov * M_PI / 180;
        //     float half_height = tan(theta / 2);
        //     float half_width = aspect * half_height;
            
        //     origin = look_from;
        //     w = unit_vector(look_from - look_at);
        //     u = unit_vector(cross(vup, w));
        //     v = cross(w, u);

        //     lower_left_corner = origin - half_width * focus_dist * u - half_height * focus_dist*v - focus_dist*w;
        //     horizontal = focus_dist * 2 * half_width * u;
        //     vertical = focus_dist * 2 * half_height * v;
        // }
        

        /* other methods */
        Ray get_ray(float u, float v) {
            // The vector in the () is the pixel point vector of the plane.
            // direction = the pixel point vector - origin
            return Ray(origin, (lower_left_corner + u * horizontal + v * vertical) - origin);
        }

        // Ray get_ray(float s, float t) {
        //     Vec3 rd = lens_radius * random_in_unit_disk();
        //     Vec3 offset = u * rd.x() + v * rd.y();
        //     return Ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);
        // }

        Vec3 random_in_unit_disk() {
            //* On the z = 0 plane, a vector with a starting point at the origin,
            //* a length less than 1, and a random direction is generated.
            //* Why the z = 0 plane is related to the tilt direction of the camera.
            Vec3 p;
            do {
                p = 2.0 * Vec3((rand() % (100) / (float)(100)), (rand() % (100) / (float)(100)), 0) - Vec3(1, 1, 0);
            } while (dot(p, p) >= 1.0);
            return p;
        }

    private:
        Vec3 lower_left_corner;
        Vec3 origin;
        Vec3 horizontal;
        Vec3 vertical;
        Vec3 u, v, w;
        float lens_radius;
};

#endif
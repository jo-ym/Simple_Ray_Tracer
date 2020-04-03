#include <iostream>
#include <fstream>
#include <float.h> // for FLT_EPSILON, FLT_MAX
#include <ctime>   // for time()
#include <vector>

#include "camera.h"
#include "sphere.h"
#include "material.h"

#define random(a, b) (rand() % (b - a + 1) + a)

using namespace std;

#define MAX_STEP 5

Vec3 skybox(const Ray &ray) {
    //* Render the background part.
    // Fix value range -1~1.
    // And there is the concept that standardize all normal vectors
    // of different lengths on the surface of the sphere.
    Vec3 unit_direction = unit_vector(ray.direction());
    // Fix value range 0~2 in the (), and fix value range 0~1 with multiple 0.5.
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * Vec3(1, 1, 1) + t * Vec3(0.5, 0.7, 1.0);
}

bool check_in_shadow(const Ray &ray, const vector<Sphere> scene, Vec3 &light_source, int self_index) {
    //* Find whether the shadow ray hit other object. And if there is
    //* intersection, it means that there are other obstacles between this point
    //* and the light source, which means that this point is now under the shadow
    //* of others, which means that the color of this point does not need to
    //* consider the effect of light from the light source. However, it is necessary
    //* to notice a situation: when this point (assuming p) is in line with the
    //* light source and other objects (assuming q), that is, q will be judged
    //* to be hit by shadow ray, but the light source is actually in p and
    //* between q, so p is not in the shadow of q. So we need to determine whether
    //* the distance between p and q is less than the distance between p and
    //* light source.
    //* self_index is the index of the sphere of current hit point.

    float current_cloest_t = FLT_EPSILON;
    float length_to_light_source = Vec3(light_source - ray.origin()).length();

    // Need to skip self surface or set a tmin, or, there will be noise in the surface.
    for (int i = 0; i < scene.size(); i++) {
        if (i == self_index) {
            continue;
        } else {
            if (scene[i].shadow_hit(ray, current_cloest_t)) {
                if (current_cloest_t < length_to_light_source) {
                    return true;
                }
            }
        }
    }

    return false;
}

Vec3 shading(Vec3 &light_source, Vec3 &light_intensity, hit_record record, const vector<Sphere> &scene) {
    //* Compute local color with shadow.
    //* record is the information about current hit point.

    // Calculate shadow ray.
    Vec3 N = record.normal;
    // L is the light direction that it need to point to light source.
    Vec3 light_direction = unit_vector(light_source - record.p);
    Ray shadow_ray(record.p, light_direction);
    
    // Find whether the shadow_ray hit other object.
    bool is_in_shadow = check_in_shadow(shadow_ray, scene, light_source, record.in_scene_index);

    // Surface is only illuminated if nothing blocks its view of the light.
    if (!is_in_shadow) {
        return record.material.get_kd() * light_intensity * std::max<float>(0.0, dot(N, light_direction));
    } else {
        return Vec3(0.0, 0.0, 0.0);
    }
}

bool intersect(const Ray &ray, const vector<Sphere> scene, float t_min, float t_max, hit_record &record, int self_index) {
    //* Find there is intersection or no, and record the closest intersection
    //* to 'record'.
    //* self_index is the index of the current sphere, and then need to skip
    //* self, or, it will be noise.

    hit_record current_cloest_record;
    bool has_hit = false;
    double current_cloest_t = t_max;

    for (int i = 0; i < scene.size(); i++) {
        if (i == self_index) continue;
        if (scene[i].hit(ray, t_min, current_cloest_t, current_cloest_record)) {
            has_hit = true;
            current_cloest_t = current_cloest_record.t;
            current_cloest_record.in_scene_index = i;
            record = current_cloest_record;
        }
    }

    return has_hit;
}

Vec3 trace(const Ray &ray, const vector<Sphere> &scene, int depth, int self_index = -1) {
    //* Deal with the color of the current pixel.
    //* If the pixel is not sphere, then it is skybox.
    //* self_index is the index of the current sphere, default -1 means that step is 0.
    //* When trace to find intersection, need to skip self, or, it will be noise.

    if (depth >= MAX_STEP) {
        return skybox(ray);
    }

    /* Spheres part */
    // Find intersection of this ray with which sphere in the scene and record the information of the intersection.
    hit_record cloest_record;
    float t_min = FLT_EPSILON;
    float t_max = FLT_MAX;
    bool has_intersection = intersect(ray, scene, t_min, t_max, cloest_record, self_index);
    if (has_intersection) {
        Vec3 light_source(-10, 10, 0);
        Vec3 light_intensity = Vec3(1.0, 1.0, 1.0); // intensity of lightsource.

        // Local color with shadow.
        Vec3 local_color = shading(light_source, light_intensity, cloest_record, scene);

        // Reflected color
        Vec3 reflected_direction = reflect(ray.direction(), cloest_record.normal);
        reflected_direction.make_unit_vector();
        Ray reflected_ray = Ray(cloest_record.p, reflected_direction);
        Vec3 reflected_color = trace(reflected_ray, scene, depth + 1, cloest_record.in_scene_index);

        // Transmitted color
        // assumes that is air to glass.
        float n_over_nt = 1 / 1.46;
        Vec3 refracted_direction = refract(ray.direction(), cloest_record.normal, n_over_nt);
        refracted_direction.make_unit_vector();
        // Ray transmitted_ray;
        // if (refracted_direction.length() == 0) {
        //     // Total internal reflection.
        //     transmitted_ray = Ray(cloest_record.p, reflected_direction);
        // } else {
        //     transmitted_ray = Ray(cloest_record.p, refracted_direction);
        // }
        Ray transmitted_ray = Ray(cloest_record.p, refracted_direction);
        Vec3 transmitted_color = trace(transmitted_ray, scene, depth + 1, cloest_record.in_scene_index);

        // Mix color.
        Vec3 color;
        if (fabsf(cloest_record.material.get_wr() - 0) < FLT_EPSILON && fabsf(cloest_record.material.get_wt() - 0)  < FLT_EPSILON) {
            // No reflection and refraction.
            color = local_color;
        } else if (fabsf(cloest_record.material.get_wt() - 0)  < FLT_EPSILON) {
            // Just reflection.
            color = (1.0 - cloest_record.material.get_wr()) * local_color + cloest_record.material.get_wr() * reflected_color;
        } else {
            // Need reflection and refraction.
            color = (1.0 - cloest_record.material.get_wt()) * ((1.0 - cloest_record.material.get_wr()) * local_color + cloest_record.material.get_wr() * reflected_color) +
                    cloest_record.material.get_wt() * transmitted_color;
            // Clamped color [0, 255].
            // color = (1.0 - cloest_record.material.get_wt()) * (local_color + cloest_record.material.get_wr() * reflected_color) +
            //         cloest_record.material.get_wt() * transmitted_color;
        }
        return color;
    } else {
        /* Skybox part */
        return skybox(ray);
    }
}

vector<Sphere> random_scene() {
    //* Generate the scene.
    //* There is a big sphere as ground.
    //* And there are three same size spheres in the center.
    //* And there are some randomly generated small spheres on the ground.

    vector<Sphere> hitable_list;

    // First sphere is the big sphere as ground.
    hitable_list.push_back(Sphere(Vec3(0.0, -100.5, -2.0), 100.0));

    // The three center spheres.
    hitable_list.push_back(Sphere(Vec3(0.0, 0.0, -2.0), 0.5, Material(Vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.9f)));
    hitable_list.push_back(Sphere(Vec3(1, 0, -1.75), 0.5, Material(Vec3(1.0f, 1.0f, 1.0f), 0.9f, 0.0f)));
    hitable_list.push_back(Sphere(Vec3(-1, 0, -2.25), 0.5, Material(Vec3(1.0f, 0.7f, 0.3f), 0.0f, 0.0f)));

    Vec3 colorlist[8] = {Vec3(0.8, 0.3, 0.3), Vec3(0.3, 0.8, 0.3), Vec3(0.3, 0.3, 0.8),
                         Vec3(0.8, 0.8, 0.3), Vec3(0.3, 0.8, 0.8), Vec3(0.8, 0.3, 0.8),
                         Vec3(0.8, 0.8, 0.8), Vec3(0.3, 0.3, 0.3)};

    // srand(time(NULL));
    srand(819);
    for (int i = 0; i < 48; i++) {
        float xr = ((float)rand() / (float)(RAND_MAX)) * 6.0f - 3.0f;
        float zr = ((float)rand() / (float)(RAND_MAX)) * 3.0f - 1.5f;
        int cindex = rand() % 8;
        float rand_reflec = ((float)rand() / (float)(RAND_MAX));
        // float rand_refrac = ((float)rand() / (float)(RAND_MAX));
        // hitable_list.push_back(Sphere(Vec3(xr, -0.4, zr - 2), 0.1, Material(colorlist[cindex], rand_reflec, rand_refrac)));
        hitable_list.push_back(Sphere(Vec3(xr, -0.4, zr - 2), 0.1, Material(colorlist[cindex], rand_reflec, 0.0)));
    }

    return hitable_list;
}

int main() {
    int width = 200;
    int height = 100;

    // For anti-aliasing.
    int anti_aliasing_times = 100;

    fstream ppm_file;
    ppm_file.open("ray_tracing_with_anti-alias.ppm", ios::out);

    // Construct the camera and projection plane.
    // Vec3 look_from(3.0, 3.0, 2.0);
    // Vec3 look_at(0.0, 0.0, -1.0);
    // Vec3 vup(0.0, 1.0, 0.0);
    // float vfov = 20;
    // float aspect = float(width) / float(height);
    // float dist_to_focus = 10.0;
    // float aperture = 0.1;
    // Camera camera(look_from, look_at, vup, vfov, aspect, aperture, dist_to_focus);
    Camera camera;

    // Construct spheres.
    vector<Sphere> scene = random_scene();

    ppm_file << "P3\n"
             << width << " " << height << "\n255\n";
    for (int row_index = height - 1; row_index >= 0; row_index--) {
        for (int column_index = 0; column_index < width; column_index++) {
            // Deal with anti-alias.
            // Same pixel calculate many times ray, then sum,
            // and then calculate average at last.
            Vec3 current_pixel_color(0.0, 0.0, 0.0);
            for (int times = 0; times < anti_aliasing_times; times++) {
                // u is the horizontal offset of the current point from the lower left corner.
                float u = float(column_index + float(random(0, 100)) / 100.0f) / float(width);
                // v is the vertical offset of the current point from the lower left corner.
                float v = float(row_index + float(random(0, 100)) / 100.0f) / float(height);
                Ray ray = camera.get_ray(u, v);
                current_pixel_color += trace(ray, scene, 0);
            }
            current_pixel_color /= float(anti_aliasing_times);
            //gamma correct
            // current_pixel_color = Vec3(sqrt(current_pixel_color.r()), sqrt(current_pixel_color.g()), sqrt(current_pixel_color.b()));

            ppm_file << int(current_pixel_color.r() * 255) << " " << int(current_pixel_color.g() * 255) << " " << int(current_pixel_color.b() * 255) << "\n";
        }
    }

    return 0;
}
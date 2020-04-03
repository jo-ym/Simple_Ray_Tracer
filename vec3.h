#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <stdlib.h>
#include <iostream>

class Vec3 {
    public:
        /* constructors */
        Vec3() {}
        Vec3(float e0, float e1, float e2) {
            e[0] = e0;
            e[1] = e1;
            e[2] = e2;
        }
        Vec3(float e[3]) {
            this->e[0] = e[0];
            this->e[1] = e[1];
            this->e[2] = e[2];
        }
        Vec3(const Vec3& vec) {
            e[0] = vec.x();
            e[1] = vec.y();
            e[2] = vec.z();
        }

        /* getter methods */
        inline float x() const {
            return e[0];
        }
        inline float y() const {
            return e[1];
        }
        inline float z() const {
            return e[2];
        }
        inline float r() const {
            return e[0];
        }
        inline float g() const {
            return e[1];
        }
        inline float b() const {
            return e[2];
        }

        /* overloading unary operator methods */
        inline Vec3 operator=(const Vec3 &vec) {
            e[0] = vec.x();
            e[1] = vec.y();
            e[2] = vec.z();
            return *this;
        }

        // Unary +
        inline const Vec3& operator+() const {
            return *this;
        }

        // Unary -
        inline Vec3 operator-() const {
            return Vec3(-e[0], -e[1], -e[2]);
        }

        inline float operator[](int i) const {
            return e[i];
        }

        inline float& operator[](int i) {
            return e[i];
        }

        inline Vec3& operator+=(const Vec3 &vec2) {
            e[0] += vec2.x();
            e[1] += vec2.y();
            e[2] += vec2.z();
            return *this;
        }

        inline Vec3& operator-=(const Vec3 &vec2) {
            e[0] -= vec2.x();
            e[1] -= vec2.y();
            e[2] -= vec2.z();
            return *this;
        }

        inline Vec3 &operator*=(const Vec3 &vec2) {
            e[0] *= vec2.x();
            e[1] *= vec2.y();
            e[2] *= vec2.z();
            return *this;
        }

        inline Vec3 &operator/=(const Vec3 &vec2) {
            e[0] /= vec2.x();
            e[1] /= vec2.y();
            e[2] /= vec2.z();
            return *this;
        }

        inline Vec3& operator*=(float t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        inline Vec3& operator/=(float t) {
            e[0] /= t;
            e[1] /= t;
            e[2] /= t;
            return *this;
        }

        /* other methods */
        inline float length() const {
            return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
        }

        inline float squared_length() const {
            return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
        }

        inline void make_unit_vector();

    private:
        float e[3];
};

inline std::ostream& operator<<(std::ostream &os, const Vec3 &vec) {
    os << "Vec3<" << vec.x() << ", " << vec.y() << ", " << vec.z() << ">";
    return os;
}

/* Binary operators */
inline Vec3 operator+(const Vec3 &vec1, const Vec3 &vec2) {
    return Vec3(
        vec1.x() + vec2.x(),
        vec1.y() + vec2.y(),
        vec1.z() + vec2.z()
    );
}

inline Vec3 operator-(const Vec3 &vec1, const Vec3 &vec2) {
    return Vec3(
        vec1.x() - vec2.x(),
        vec1.y() - vec2.y(),
        vec1.z() - vec2.z()
    );
}

inline Vec3 operator*(const Vec3 &vec1, float t) {
    return Vec3(
        vec1.x() * t,
        vec1.y() * t,
        vec1.z() * t
    );
}

inline Vec3 operator*(float t, const Vec3 &vec1) {
    return Vec3(
        t * vec1.x(),
        t * vec1.y(),
        t * vec1.z()
    );
}

inline Vec3 operator*(const Vec3 &vec1, const Vec3 &vec2) {
    return Vec3(
        vec1.x() * vec2.x(),
        vec1.y() * vec2.y(),
        vec1.z() * vec2.z()
    );
}

inline Vec3 operator/(const Vec3 &vec1, float t) {
    return Vec3(
        vec1.x() / t,
        vec1.y() / t,
        vec1.z() / t
    );
}

inline Vec3 operator/(const Vec3 &vec1, const Vec3 &vec2) {
    return Vec3(
        vec1.x() / vec2.x(),
        vec1.y() / vec2.y(),
        vec1.z() / vec2.z()
    );
}

/* other operations for vector */
inline void Vec3::make_unit_vector() {
    float length = Vec3::length();
    e[0] /= length;
    e[1] /= length;
    e[2] /= length;
}

inline float dot(const Vec3 &vec1, const Vec3 &vec2) {
    return vec1.x() * vec2.x() +
        vec1.y() * vec2.y() +
        vec1.z() * vec2.z();
}

inline Vec3 cross(const Vec3 &vec1, const Vec3 &vec2) {
    return Vec3(
        vec1.y() * vec2.z() - vec1.z() * vec2.y(),
        -(vec1.x() * vec2.z() - vec1.z() * vec2.x()),
        vec1.x() * vec2.y() - vec1.y() * vec2.x()
    );
}

inline Vec3 unit_vector(Vec3 vec) {
    return vec / vec.length();
}

#endif
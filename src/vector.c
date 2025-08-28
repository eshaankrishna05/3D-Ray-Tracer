#include "vector.h"
#include <math.h>

// Vector addition
Vec3 add(Vec3 v1, Vec3 v2) {
    Vec3 result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
    return result;
}

// Vector subtraction
Vec3 subtract(Vec3 v1, Vec3 v2) {
    Vec3 result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
    return result;
}

// Scalar multiplication
Vec3 scalarMultiply(float s, Vec3 v) {
    Vec3 result = { s * v.x, s * v.y, s * v.z };
    return result;
}

// Divide a vector by a scalar
Vec3 scalarDivide(Vec3 v, float d) {
    Vec3 result = {0.0, 0.0, 0.0};
    if (d != 0) {
        result.x = v.x / d;
        result.y = v.y / d;
        result.z = v.z / d;
    }
    return result;
}

// Vector normalization
Vec3 normalize(Vec3 v) {
    float len = length(v);
    return (len != 0) ? scalarDivide(v, len) : (Vec3) { 0, 0, 0 };
}

// Dot product
float dot(Vec3 v1, Vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

// Length squared
float length2(Vec3 v) {
    return dot(v, v);
}

// Vector length
float length(Vec3 v) {
    return sqrt(length2(v));
}

// Distance squared between vectors
float distance2(Vec3 v1, Vec3 v2) {
    return length2(subtract(v1, v2));
}

// Distance between vectors
float distance(Vec3 v1, Vec3 v2) {
    return sqrt(distance2(v1, v2));
}


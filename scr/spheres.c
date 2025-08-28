#include "spheres.h"
#include <stdlib.h>
#include <math.h>

void worldInit(World *world) {
    world->size = 0;
    world->capacity = 4;
    world->spheres = (Sphere **)malloc(world->capacity * sizeof(Sphere *));
}

void freeWorld(World *world) {
    for (int i = 0; i < world->size; i++) {
        free(world->spheres[i]);
    }
    free(world->spheres);
    world->size = 0;
    world->capacity = 0;
    world->spheres = NULL;
}

void addSphere(World *world, Sphere *sphere) {
    if (world->size >= world->capacity) {
        world->capacity *= 2;
        world->spheres = (Sphere **)realloc(world->spheres, world->capacity * sizeof(Sphere *));
    }
    world->spheres[world->size++] = sphere;
}

Sphere *createSphere(float radius, Vec3 position, Vec3 color) {
    Sphere *sphere = (Sphere *)malloc(sizeof(Sphere));
    sphere->r = radius;
    sphere->pos = position;
    sphere->color = color;
    return sphere;
}

int doesIntersect(const Sphere *sphere, Vec3 rayPos, Vec3 rayDir, float *t) {
    Vec3 oc = subtract(rayPos, sphere->pos);
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(oc, rayDir);
    float c = dot(oc, oc) - sphere->r * sphere->r;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return 0; // No intersection
    }

    // Calculate intersection t values
    float sqrtDisc = sqrt(discriminant);
    float t1 = (-b - sqrtDisc) / (2.0 * a);
    float t2 = (-b + sqrtDisc) / (2.0 * a);

    // Choose closest intersection in front of the ray
    *t = (t1 > 0) ? t1 : ((t2 > 0) ? t2 : -1);
    return (*t > 0) ? 1 : 0;
}


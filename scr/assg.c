#include "vector.h"
#include "spheres.h"
#include "color.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

Vec3 backgroundColor;
Vec3 sphereColor;  
World world;
Vec3 lightPos;
int width, height;
float viewportHeight; 

float lightBrightness, focalLength;

void renderImage(int width, int height, const char *outputPath);
void renderImage_color(int width, int height, const char *outputPath);
void parseInput(const char *inputFile, World *world);
void printOperationResults(FILE *outputFile, World *world);
void printVec3(FILE *outputFile, const char *initial, Vec3 v);

void parseInput(const char *inputFile, World *world) {
    FILE *file = fopen(inputFile, "r");
    if (!file) {
        fprintf(stderr, "Error opening input file.\n");
        exit(1);
        }

    sphereColor = (Vec3){1.0, 1.0, 1.0};
    backgroundColor = (Vec3){0.0, 0.0, 0.0};
    int colorCount, sphereCount, backgroundColorIndex;
    fscanf(file, "%d %d %f %f", &width, &height, &viewportHeight, &focalLength);
    fscanf(file, "%f %f %f %f", &lightPos.x, &lightPos.y, &lightPos.z, &lightBrightness);
    fscanf(file, "%d", &colorCount);

    // Allocate memory for colors
    unsigned int *colors = malloc(colorCount * sizeof(unsigned int));
    if (!colors) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        exit(1);
    }

    for (int i = 0; i < colorCount; i++) {
        fscanf(file, "%x", &colors[i]);
    }

    fscanf(file, "%d", &backgroundColorIndex);

    #ifdef FS
        qsort(colors, colorCount, sizeof(unsigned int), compareColor);
        backgroundColor = unpackRGB(colors[backgroundColorIndex]);
     #endif

    worldInit(world);
    fscanf(file, "%d", &sphereCount);
    for (int i = 0; i < sphereCount; i++) {
        Vec3 pos;
        float radius;
        int colorIndex;
        fscanf(file, "%f %f %f %f %d", &pos.x, &pos.y, &pos.z, &radius, &colorIndex);

        #ifdef FS
            sphereColor = unpackRGB(colors[colorIndex]);
        #endif

        addSphere(world, createSphere(radius, pos, sphereColor));
    }

    // Free colors after usage
    free(colors);
    fclose(file);
}

void renderImage(int width, int height, const char *outputPath) {
    FILE *ppmFile = fopen(outputPath, "w");
    if (!ppmFile) {  // Check if file was opened successfully
        fprintf(stderr, "Error opening output file: %s\n", outputPath);
        exit(1);  // Exit if file cannot be opened
    }
    
    fprintf(ppmFile, "P3\n%d %d\n255\n", width, height);

    float aspectRatio = (float)width / (float)height;
    float viewportWidth = viewportHeight * aspectRatio;
    Vec3 cameraPos = {0, 0, 0};

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            float px = (x + 0.5f) / width * viewportWidth - viewportWidth / 2;
            float py = (y + 0.5f) / height * viewportHeight - viewportHeight / 2;
            Vec3 rayDir = normalize((Vec3){px, py, -focalLength});
            Vec3 pixelColor = backgroundColor;
            float closestT = 1e30;
            for (int i = 0; i < world.size; i++) {
                float t;
                if (doesIntersect(world.spheres[i], cameraPos, rayDir, &t) && t < closestT) {
                    closestT = t;
                    Vec3 intersectionPoint = add(cameraPos, scalarMultiply(t, rayDir));
                    Vec3 normal = normalize(subtract(intersectionPoint, world.spheres[i]->pos));
                    Vec3 lightDir = normalize(subtract(lightPos, intersectionPoint));
                    float intensity = fmax(0, dot(normal, lightDir)) * lightBrightness / distance2(lightPos, intersectionPoint);
                    intensity = fmin(1, intensity);
                    int inShadow = 0;
                    Vec3 shadowRayStart = add(intersectionPoint, scalarMultiply(0.001f, normal));
                    for (int j = 0; j < world.size; j++) {
                        if (j != i) {
                            float shadowT;
                            if (doesIntersect(world.spheres[j], shadowRayStart, lightDir, &shadowT) && shadowT > 0 && shadowT < distance(lightPos, intersectionPoint)) {    
                                inShadow = 1;
                                break;
                            }
                        }
                    }
                    intensity *= inShadow ? 0.1 : 1.0;
                    pixelColor = scalarMultiply(intensity, (Vec3){1.0, 1.0, 1.0});
                }
            }
            writeColour(ppmFile, pixelColor);
        }
    }
    fclose(ppmFile);
}

void printVec3(FILE *outputFile, const char *initial, Vec3 v) {
    fprintf(outputFile, "%s(%.1f, %.1f, %.1f)", initial, v.x, v.y, v.z);
}

void printOperationResults(FILE *outputFile, World *world) {
    Vec3 result;
    float viewportWidth;
    // Basic Vector Operations with Background and Light Position
 
    viewportWidth = (viewportHeight * width) / height;
    result = add(backgroundColor, lightPos);
    printVec3(outputFile, "", backgroundColor);
    fprintf(outputFile, " + ");
    printVec3(outputFile, "", lightPos);
    fprintf(outputFile, " = ");
    printVec3(outputFile, "", result);
    fprintf(outputFile, "\n");

    result = subtract(backgroundColor, lightPos);
    printVec3(outputFile, "", backgroundColor);
    fprintf(outputFile, " - ");
    printVec3(outputFile, "", lightPos);
    fprintf(outputFile, " = ");
    printVec3(outputFile, "", result);
    fprintf(outputFile, "\n");

    result = scalarMultiply(viewportWidth, lightPos);
    fprintf(outputFile, "%.1f * ", viewportWidth);
    printVec3(outputFile, "", lightPos);
    fprintf(outputFile, " = ");
    printVec3(outputFile, "", result);
    fprintf(outputFile, "\n");

    result = normalize(lightPos);
    fprintf(outputFile, "normalize");
    printVec3(outputFile, "", lightPos);
    fprintf(outputFile, " = ");
    printVec3(outputFile, "", result);
    fprintf(outputFile, "\n");

    // Sphere-Specific Calculations
    for (int i = 0; i < world->size; i++) {
        Sphere *sphere = world->spheres[i];
        fprintf(outputFile, "\n");

        // Scalar Division of Sphere Color by Radius
        result = scalarDivide(sphere->color, sphere->r);
        printVec3(outputFile, "", sphere->color);
        fprintf(outputFile, " / %.1f = ", sphere->r);
        printVec3(outputFile, "", result);
        fprintf(outputFile, "\n");

        // Dot Product
        float dotProduct = dot(lightPos, sphere->pos);
        fprintf(outputFile, "dot(");
        printVec3(outputFile, "", lightPos);
        fprintf(outputFile, ", ");
        printVec3(outputFile, "", sphere->pos);
        fprintf(outputFile, ") = %.1f\n", dotProduct);

        // Distance
        float dist = distance(lightPos, sphere->pos);
        fprintf(outputFile, "distance(");
        printVec3(outputFile, "", lightPos);
        fprintf(outputFile, ", ");
        printVec3(outputFile, "", sphere->pos);
        fprintf(outputFile, ") = %.1f\n", dist);

        // Length
        float len = length(sphere->pos);
        fprintf(outputFile, "length");
        printVec3(outputFile, "", sphere->pos);
        fprintf(outputFile, " = %.1f\n", len);
    }
}

void renderImage_color(int width, int height, const char *outputPath) {

    FILE *ppmFile = fopen(outputPath, "w");
    if (!ppmFile) {  // Check if file was opened successfully
        fprintf(stderr, "Error opening output file: %s\n", outputPath);
        exit(1);  // Exit if file cannot be opened
    }
    
    fprintf(ppmFile, "P3\n%d %d\n255\n", width, height);

    float aspectRatio = (float)width / height;
    float viewportWidth = viewportHeight * aspectRatio;
    Vec3 cameraPos = {0, 0, 0};

    for (int y = height - 1; y >= 0; y--) {
        for (int x = 0; x < width; x++) {
            Vec3 finalColor = {0.0, 0.0, 0.0};
            for (int subY = 0; subY < 3; subY++) {
                for (int subX = 0; subX < 3; subX++) {
                    float px = (x + (subX + 0.5f) / 3) / width * viewportWidth - viewportWidth / 2;
                    float py = (y + (subY + 0.5f) / 3) / height * viewportHeight - viewportHeight / 2;
                    Vec3 rayDir = normalize((Vec3){px, py, -focalLength});

                    Vec3 pixelColor = backgroundColor;
                    float closestT = 1e30;

                    for (int i = 0; i < world.size; i++) {
                        float t;
                        if (doesIntersect(world.spheres[i], cameraPos, rayDir, &t) && t < closestT) {
                            closestT = t;
                            Vec3 intersectionPoint = add(cameraPos, scalarMultiply(t, rayDir));
                            Vec3 normal = normalize(subtract(intersectionPoint, world.spheres[i]->pos));
                            Vec3 lightDir = normalize(subtract(lightPos, intersectionPoint));
                            float intensity = fmax(0, dot(normal, lightDir)) * lightBrightness / distance2(lightPos, intersectionPoint);
                            intensity = fmin(1, intensity);

                            // Shadow calculation
                            int inShadow = 0;
                            Vec3 shadowRayStart = add(intersectionPoint, scalarMultiply(0.001f, normal));
                            for (int j = 0; j < world.size; j++) {
                                if (j != i) {
                                    float shadowT;
                                   
                                      if (doesIntersect(world.spheres[j], shadowRayStart, lightDir, &shadowT) && shadowT > 0 && shadowT < distance(lightPos, intersectionPoint)) {    
                                        inShadow = 1;
                                        break;
                                    }
                                }
                            }

                            intensity *= inShadow ? 0.1 : 1.0;
                            pixelColor = scalarMultiply(intensity, world.spheres[i]->color);
                        }
                    }

                    finalColor = add(finalColor, pixelColor);
                }
            }
            finalColor = scalarDivide(finalColor, 9.0); // Average the samples
            writeColour(ppmFile, finalColor);
        }
    }

    fclose(ppmFile);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }
    parseInput(argv[1], &world);

    #ifdef MS1
    FILE *outputFile = fopen(argv[2], "w");
    if (!outputFile) {
        perror("Error opening output file");
        freeWorld(&world);
        return 1;
    }
    printOperationResults(outputFile, &world);

    fflush(outputFile);
    fclose(outputFile);
    freeWorld(&world);
    #endif

    #ifdef MS2
    renderImage(width, height, argv[2]);
    freeWorld(&world);
    #endif

    #ifdef FS
    renderImage_color(width, height, argv[2]);
    freeWorld(&world);
    #endif

    return 0;
}

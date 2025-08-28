#include "color.h"
#include <math.h>

// Compare function for qsort to sort unsigned int colors
int compareColor(const void *a, const void *b) {
    int a1 = 0, b1 = 0;
    for (int i = 0; i < sizeof(int); i++) {
        a1 |= (*((unsigned char *)a + i) & 0x0F) << (i * 8);
        b1 |= (*((unsigned char *)b + i) & 0x0F) << (i * 8);
    }

    return (a1 < b1) ? -1 : (b1 < a1) ? 1 : 
           (*((int *)a) < *((int *)b)) ? -1 : 
           (*((int *)a) > *((int *)b)) ? 1 : 0;
}

// Unpacks a 24-bit RGB color into a Vec3
Vec3 unpackRGB(unsigned int packedRGB) {
    float r = ((packedRGB >> 16) & 0xFF) / 255.0f;
    float g = ((packedRGB >> 8) & 0xFF) / 255.0f;
    float b = (packedRGB & 0xFF) / 255.0f;
    return (Vec3){r, g, b};
}

void writeColour(FILE *ppmFile, Vec3 color) {
    // Clamp the color values to the range [0, 255]
    int r = (int)(fmin(fmax(color.x * 255, 0), 255));
    int g = (int)(fmin(fmax(color.y * 255, 0), 255));
    int b = (int)(fmin(fmax(color.z * 255, 0), 255));
    fprintf(ppmFile, "%d %d %d ", r, g, b);

}

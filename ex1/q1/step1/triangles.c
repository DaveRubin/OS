#include <printf.h>
#include <math.h>

//
// Created by David Rubin on 26/11/2016.
//
struct point {
    float x, y;
};

struct triangle {
    struct point vertices[3];
};

struct triangle getTriangleFromUser();

int getTrianglesCount();

struct triangle getTriangleFromUser();

void printPoint(struct point point);

void print1Decimal(float x);

void calc_perimeter(struct triangle triangle);

float calcDistance(struct point point, struct point point1);

void calc_area(struct triangle triangle);

void printTriangleInfo(struct triangle triangle);

int main() {
    int count = getTrianglesCount();
    for (int i = 0; i < count; ++i) {
        struct triangle tri = getTriangleFromUser();
        calc_perimeter(tri);
        calc_area(tri);
    }
    return 0;
}

void calc_area(struct triangle triangle) {
    struct point p1 = triangle.vertices[0];
    struct point p2 = triangle.vertices[1];
    struct point p3 = triangle.vertices[2];
    //according to Shoelace formula
    float area = 0.5f * fabsf(p1.x * p2.y + p2.x * p3.y + p3.x * p1.y - p2.x * p1.y - p3.x * p2.y - p1.x * p3.y);
    printTriangleInfo(triangle);
    printf("Area = ");
    print1Decimal(area);
    printf("\n");
}

/**
 * Print triangle info and permiter
 * Triangle {x1,y1} {x2,y2} {x3,y3} perimeter = P
 *
 * @param triangle
 */
void calc_perimeter(struct triangle triangle) {
    float perimeter = calcDistance(triangle.vertices[0], triangle.vertices[1]) +
                      calcDistance(triangle.vertices[1], triangle.vertices[2]) +
                      calcDistance(triangle.vertices[2], triangle.vertices[0]);
    printTriangleInfo(triangle);
    printf("Perimeter = ");
    print1Decimal(perimeter);
    printf("\n");
}

void printTriangleInfo(struct triangle triangle) {
    printf("Triangle ");
    printPoint(triangle.vertices[0]);
    printPoint(triangle.vertices[1]);
    printPoint(triangle.vertices[2]);
}

float calcDistance(struct point point1, struct point point2) {
    float deltaX = point1.x - point2.x;
    float deltaY = point1.y - point2.y;
    return (float) sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

void printPoint(struct point point) {
    printf("{");
    print1Decimal(point.x);
    printf(", ");
    print1Decimal(point.y);
    printf("} ");
}

/**
 * Get float and print it in X.X format
 * @param x
 */
void print1Decimal(float x) {
    int top = (int) floor(x);
    int decimal = (int) floor((x - top) * 10);
    printf("%d.%d", top, decimal);
}

/**
 * Get int from user
 * @return
 */
int getTrianglesCount() {
    printf("Please enter the number of the triangles: \n");
    int result;
    scanf("%d", &result);
    return result;
}

/**
 * Ask the user for input for one triangle
 * @return
 */
struct triangle getTriangleFromUser() {
    struct point vertices[3] = {{0, 0},
                                {0, 0},
                                {0, 0}};

    for (int i = 0; i < 3; ++i) {
        float x, y;
        printf("Enter x for point $%d :", i + 1);
        scanf("%f", &x);
        printf("Enter y for point $%d :", i + 1);
        scanf("%f", &y);

        struct point p = {x, y};
        vertices[i] = p;
        printf("\n");
    }

    struct triangle result = {
            {vertices[0], vertices[1], vertices[2]}
    };

    return result;
}
#include <printf.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h> 		// for fork, sleep etc.
#include <curses.h>

//
// Created by David Rubin on 26/11/2016.
//
struct point {
    float x, y;
} Point;

struct triangle {
    struct point vertices[3];
} Triangle;

struct triangle getTriangleFromUser();

int getTrianglesCount();

struct triangle getTriangleFromUser();

void printPoint(struct point point);

void print1Decimal(float x);

void calc_perimeter(struct triangle triangle);

float calcDistance(struct point point, struct point point1);

void calc_area(struct triangle triangle);

void printTriangleInfo(struct triangle triangle);

void DoReader(int pInt, int pInt1);

void DoPeri(int i);

void DoArea(int i);

char* processMessage = "%s pid %d processed %d triangles\n";

int main() {

    int readerPeriPipe[2];
    int readerAreaPipe[2];
    pipe(readerPeriPipe); //create pipes
    pipe(readerAreaPipe);
    int readerPid = fork();

    if (readerPid == 0 ) {
        //if Reader process
        //close reading for pipes
        close(readerPeriPipe[0]);
        close(readerAreaPipe[0]);
        DoReader(readerPeriPipe[1], readerAreaPipe[1]);
        exit(0);
    }
    else {
        //Still In parent process
        //close writing for pipes
        close(readerPeriPipe[1]);
        close(readerAreaPipe[1]);
        int periPid = fork();
        if (periPid == 0) {
            DoPeri(readerPeriPipe[0]);
            exit(0);
        }
        else {
            int areaPid = fork();
            if (areaPid == 0) {
                DoArea(readerAreaPipe[0]);
                exit(0);
            }

        }

        int status;
        while(wait(&status)>0);
        fprintf(stderr,"PARENT pid %d exiting\n",getpid());
        //Needed so that we won't return to shell before all of the child processes end
    }

    return 0;
}

/**
 * Execute Area functionality
 * @param readPipe
 */
void DoArea(int readPipe) {
    int trianglesProcessed = 0;
    while (TRUE) {
        struct triangle tri;
        int count = (int) read(readPipe, &tri, sizeof(struct triangle));
        if (count<sizeof(struct triangle)) {
            break;
        }
        else {
            calc_area(tri);
            trianglesProcessed++;
        }

    }
    close(readPipe);
    fprintf(stderr,processMessage,"AREA",getpid(),trianglesProcessed);
}

/**
 * Execute Peri functionality
 * @param readPipe
 */
void DoPeri(int readPipe) {
    int trianglesProcessed = 0;
    while (TRUE) {
        struct triangle tri;
        int count = (int) read(readPipe, &tri, sizeof(struct triangle));
        if (count<sizeof(struct triangle)) {
            break;
        }
        else {
            calc_perimeter(tri);
            trianglesProcessed++;
        }

    }
    close(readPipe);
    fprintf(stderr,processMessage,"PERI",getpid(),trianglesProcessed);
}

/**
 * Execute READER functionality
 */
void DoReader(int readerPeriWritePipe, int readerAreaWritePipe) {
    int count = getTrianglesCount();

    for (int i = 0; i < count; ++i) {
        struct triangle tri = getTriangleFromUser();
        write(readerAreaWritePipe,&tri, sizeof(struct triangle));
        write(readerPeriWritePipe,&tri, sizeof(struct triangle));
    }

    close(readerAreaWritePipe);
    close(readerPeriWritePipe);
    fprintf(stderr,processMessage,"READER",getpid(),count);
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
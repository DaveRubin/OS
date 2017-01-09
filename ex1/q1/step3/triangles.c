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

struct triangle getTriangleFromUser(FILE *i);

int getTrianglesCount(FILE *i);

struct triangle getTriangleFromUser(FILE *i);

void printPoint(struct point point, FILE *output);

void print1Decimal(float x, FILE *output);

void calc_perimeter(struct triangle triangle, FILE *output);

float calcDistance(struct point point, struct point point1);

void calc_area(struct triangle triangle, FILE *output);

void printTriangleInfo(FILE *output, struct triangle triangle);

void DoReader(int pInt, int pInt1, FILE *i);

void DoPeri(int i);

void DoArea(int i);

char *processMessage = "%s pid %d processed %d triangles\n";
FILE *errLog;

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("No input file argument found...\n");
        exit(0);
    }

    FILE *inputFD = fopen(argv[1], "r");
    errLog = fopen("err.log", "w");

    int readerPeriPipe[2];
    int readerAreaPipe[2];
    pipe(readerPeriPipe); //create pipes
    pipe(readerAreaPipe);
    int readerPid = fork();

    if (readerPid == 0) {
        //if Reader process
        //close reading for pipes
        close(readerPeriPipe[0]);
        close(readerAreaPipe[0]);
        DoReader(readerPeriPipe[1], readerAreaPipe[1], inputFD);
        exit(0);
    } else {
        //Still In parent process
        //close writing for pipes
        close(readerPeriPipe[1]);
        close(readerAreaPipe[1]);
        int periPid = fork();
        if (periPid == 0) {
            DoPeri(readerPeriPipe[0]);
            exit(0);
        } else {
            int areaPid = fork();
            if (areaPid == 0) {
                DoArea(readerAreaPipe[0]);
                exit(0);
            }

        }

        int status;
        while (wait(&status) > 0);
        fprintf(errLog, "PARENT pid %d exiting\n", getpid());
        fclose(errLog);
        //Needed so that we won't return to shell before all of the child processes end
    }

    return 0;
}

/**
 * Execute Area functionality
 * @param readPipe
 */
void DoArea(int readPipe) {
    FILE *outPut = fopen("area_out.log", "w");
    int trianglesProcessed = 0;
    while (TRUE) {
        struct triangle tri;
        int count = (int) read(readPipe, &tri, sizeof(struct triangle));
        if (count < sizeof(struct triangle)) {
            break;
        } else {
            calc_area(tri,outPut);
            trianglesProcessed++;
        }

    }
    fprintf(errLog, processMessage, "AREA", getpid(), trianglesProcessed);
    close(readPipe);
    fclose(outPut);
}

/**
 * Execute Peri functionality
 * @param readPipe
 */
void DoPeri(int readPipe) {
    FILE *outPut = fopen("peri_out.log", "w");
    int trianglesProcessed = 0;
    while (TRUE) {
        struct triangle tri;
        int count = (int) read(readPipe, &tri, sizeof(struct triangle));
        if (count < sizeof(struct triangle)) {
            break;
        } else {
            calc_perimeter(tri, outPut);
            trianglesProcessed++;
        }

    }
    fprintf(errLog, processMessage, "PERI", getpid(), trianglesProcessed);
    close(readPipe);
    fclose(outPut);
}

/**
 * Execute READER functionality
 */
void DoReader(int readerPeriWritePipe, int readerAreaWritePipe, FILE *inputPipe) {

    int count = getTrianglesCount(inputPipe);

    for (int i = 0; i < count; ++i) {
        struct triangle tri = getTriangleFromUser(inputPipe);
        write(readerAreaWritePipe, &tri, sizeof(struct triangle));
        write(readerPeriWritePipe, &tri, sizeof(struct triangle));
    }

    close(readerAreaWritePipe);
    close(readerPeriWritePipe);
    fclose(inputPipe);
    fprintf(errLog, processMessage, "READER", getpid(), count);
}

void calc_area(struct triangle triangle, FILE *output) {
    struct point p1 = triangle.vertices[0];
    struct point p2 = triangle.vertices[1];
    struct point p3 = triangle.vertices[2];
    //according to Shoelace formula
    float area = 0.5f * fabsf(p1.x * p2.y + p2.x * p3.y + p3.x * p1.y - p2.x * p1.y - p3.x * p2.y - p1.x * p3.y);
    printTriangleInfo(output, triangle);
    fprintf(output,"Area = ");
    print1Decimal(area,output);
    fprintf(output,"\n");
}

/**
 * Print triangle info and permiter
 * Triangle {x1,y1} {x2,y2} {x3,y3} perimeter = P
 *
 * @param triangle
 */
void calc_perimeter(struct triangle triangle, FILE *output) {
    float perimeter = calcDistance(triangle.vertices[0], triangle.vertices[1]) +
                      calcDistance(triangle.vertices[1], triangle.vertices[2]) +
                      calcDistance(triangle.vertices[2], triangle.vertices[0]);
    printTriangleInfo(output, triangle);
    fprintf(output, "Perimeter = ");
    print1Decimal(perimeter, output);
    fprintf(output, "\n");
}

void printTriangleInfo(FILE *output, struct triangle triangle) {
    fprintf(output,"Triangle ");
    printPoint(triangle.vertices[0], output);
    printPoint(triangle.vertices[1], output);
    printPoint(triangle.vertices[2], output);
}

float calcDistance(struct point point1, struct point point2) {
    float deltaX = point1.x - point2.x;
    float deltaY = point1.y - point2.y;
    return (float) sqrt((deltaX * deltaX) + (deltaY * deltaY));
}

void printPoint(struct point point, FILE *output) {
    fprintf(output, "{");
    print1Decimal(point.x, output);
    fprintf(output, ", ");
    print1Decimal(point.y, output);
    fprintf(output, "} ");
}

/**
 * Get float and print it in X.X format
 * @param x
 */
void print1Decimal(float x, FILE *output) {
    int top = (int) floor(x);
    int decimal = (int) floor((x - top) * 10);
    fprintf(output, "%d.%d", top, decimal);
}

/**
 * Get int from user
 * @return
 */
int getTrianglesCount(FILE *inputPipe) {
    int result;
    fscanf(inputPipe, "%d ", &result);
    return result;
}

/**
 * Ask the user for input for one triangle
 * @return
 */
struct triangle getTriangleFromUser(FILE *inputPipe) {
    struct point vertices[3] = {{0, 0},
                                {0, 0},
                                {0, 0}};
    for (int i = 0; i < 3; ++i) {
        float x, y;
        fscanf(inputPipe, "%f ", &x);
        fscanf(inputPipe, "%f ", &y);
        struct point p = {x, y};
        vertices[i] = p;
    }

    struct triangle result = {
            {vertices[0], vertices[1], vertices[2]}
    };

    return result;
}
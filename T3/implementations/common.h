//
// Created by varce on 5/30/26.
//

#ifndef TAREA_3_CC7515_COMMON_H
#define TAREA_3_CC7515_COMMON_H

typedef unsigned char ubyte;
typedef unsigned int uint;
typedef unsigned short ushort;

struct BenchmarkResult {
    double totalGPUTime;
    double kernelTime;

    BenchmarkResult() : totalGPUTime(0), kernelTime(0) {}
    BenchmarkResult(double total, double kernel) : totalGPUTime(total), kernelTime(kernel) {}
};


#endif //TAREA_3_CC7515_COMMON_H
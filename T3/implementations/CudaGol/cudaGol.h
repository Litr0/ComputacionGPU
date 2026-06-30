#ifndef CUDAGOL_H
#define CUDAGOL_H

#include "../common.h" //

// Función original de la Tarea 2
struct BenchmarkResult computeIterationCuda(ubyte*& data, ubyte*& resultData, const uint width, const uint height, const uint iterations, const int blockSize);

// NUEVAS FUNCIONES PARA TAREA 3
// Ejecuta un paso de simulación directamente sobre punteros que ya residen en la GPU
void runGameOfLifeStepCuda(ubyte* gpuData, ubyte* gpuResultData, const uint width, const uint height, const int blockSize);

// Convierte el estado binario (0 o 1) de la GPU en colores RGBA directamente en el buffer del PBO
void renderSimulationToPBO(const ubyte* gpuData, void* pboPtr, const uint width, const uint height);

#endif // CUDAGOL_H
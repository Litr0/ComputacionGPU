//
// Created by varce on 19/05/2026.
//
#include "./cudaGol.h"
#include <cassert>
#include <cstdlib>

__global__ void gameOfLifeKernel(const ubyte* data, const uint width, const uint height, ubyte* resultData) {
    uint sizeW = width * height;

    for (uint cellId = blockIdx.x * blockDim.x + threadIdx.x; cellId < sizeW; cellId += blockDim.x * gridDim.x) {
        uint x = cellId % width;
        uint yAbs = cellId - x;

        uint xL = (x + width - 1) % width;
        uint xR = (x + 1) % width;
        uint yAbsUp = (yAbs + sizeW - width) % sizeW;
        uint yAbsDown = (yAbs + width) % sizeW;

        uint alive = data[xL + yAbsUp] + data[x + yAbsUp]
            + data[xR + yAbsUp] + data[xL + yAbs] + data[xR + yAbs]
            + data[xL + yAbsDown] + data[x + yAbsDown] + data[xR + yAbsDown];

        resultData[x + yAbs] = alive == 3 || (alive == 2 && data[x + yAbs]) || (alive == 6 && !data[x + yAbs]) ? 1 : 0;
    }
}

struct BenchmarkResult computeIterationCuda(ubyte*& data, ubyte*& resultData, const uint width, const uint height, const uint iterations, const int blockSize) {
    ubyte* gpuData, *gpuResultData;

    size_t dataLength = width * height;

    cudaEvent_t startTotal, stopTotal, startKernel, stopKernel;
    cudaEventCreate(&startTotal);
    cudaEventCreate(&stopTotal);
    cudaEventCreate(&startKernel);
    cudaEventCreate(&stopKernel);

    cudaEventRecord(startTotal);
    cudaMalloc(&gpuData, dataLength * sizeof(ubyte));
    cudaMalloc(&gpuResultData, dataLength * sizeof(ubyte));

    cudaMemcpy(gpuData, data, dataLength * sizeof(ubyte), cudaMemcpyHostToDevice);

    size_t reqBlocksCount = (dataLength + blockSize - 1) / blockSize;
    
    cudaEventRecord(startKernel);
    for (int i = 0; i < iterations; ++i) {
        gameOfLifeKernel<<<reqBlocksCount, blockSize>>>(gpuData, width, height, gpuResultData);
        std::swap(gpuData, gpuResultData);
    }
    cudaEventRecord(stopKernel);
    cudaEventSynchronize(stopKernel);

    cudaMemcpy(data, gpuData, dataLength * sizeof(ubyte), cudaMemcpyDeviceToHost);
    cudaEventRecord(stopTotal);
    cudaEventSynchronize(stopTotal);

    cudaFree(gpuData);
    cudaFree(gpuResultData);

    float totalGPUTimeF, kernelTimeF;
    cudaEventElapsedTime(&kernelTimeF, startKernel, stopKernel);
    cudaEventElapsedTime(&totalGPUTimeF, startTotal, stopTotal);
    cudaEventDestroy(startTotal);
    cudaEventDestroy(stopTotal);
    cudaEventDestroy(startKernel);
    cudaEventDestroy(stopKernel);
    return BenchmarkResult(static_cast<double>(totalGPUTimeF), static_cast<double>(kernelTimeF));
}
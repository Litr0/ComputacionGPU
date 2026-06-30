//
// Created by varce on 19/05/2026.
//
#include "../../common.h"
#include <cassert>
#include <cstdlib>

#include <cuda_runtime.h>
#include <nvtx3/nvToolsExt.h>

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

void computeIterationCudaProfiler(ubyte*& data, ubyte*& resultData, const uint width, const uint height, const uint iterations, const int blockSize) {
    ubyte* gpuData, *gpuResultData;

    size_t dataLength = width * height;

    nvtxRangePushA("CUDA Game of Life");

    nvtxRangePushA("cudaMalloc");
    cudaMalloc(&gpuData, dataLength * sizeof(ubyte));
    cudaMalloc(&gpuResultData, dataLength * sizeof(ubyte));
    nvtxRangePop();

    nvtxRangePushA("Host to Device");
    cudaMemcpy(gpuData, data, dataLength * sizeof(ubyte), cudaMemcpyHostToDevice);
    nvtxRangePop();

    size_t reqBlocksCount = (dataLength + blockSize - 1) / blockSize;
    
    nvtxRangePushA("Kernel Execution");
    for (int i = 0; i < iterations; ++i) {
        gameOfLifeKernel<<<reqBlocksCount, blockSize>>>(gpuData, width, height, gpuResultData);
        std::swap(gpuData, gpuResultData);
    }
    cudaDeviceSynchronize();

    nvtxRangePop();

    nvtxRangePushA("Device to Host");
    cudaMemcpy(data, gpuData, dataLength * sizeof(ubyte), cudaMemcpyDeviceToHost);
    nvtxRangePop();

    nvtxRangePushA("cudaFree");
    cudaFree(gpuData);
    cudaFree(gpuResultData);
    nvtxRangePop();

    nvtxRangePop();
}
#include "cudaSharedMemGol.h"

constexpr int BLOCK_SIZE = 16;

__global__ void gameOfLifeSharedMemKernel(const ubyte* data, const uint width, const uint height, ubyte* resultData) {
    extern __shared__ ubyte sharedData[];

    uint sizeW = width * height;

    uint gid = blockIdx.x * blockDim.x + threadIdx.x;

    if (gid >= sizeW) {
        return;
    }

    sharedData[threadIdx.x] = data[gid];

    __syncthreads();

    uint blockStartId = blockIdx.x * blockDim.x;
    uint blockEndId = blockStartId + blockDim.x;

    uint x = gid % width;
    uint y = gid / width;
    
    uint cellsAlive = 0;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            uint neighborX = (x + dx + width) % width;
            uint neighborY = (y + dy + height) % height;
            uint neighborId = neighborY * width + neighborX;

            if (neighborId >= blockStartId && neighborId < blockEndId) {
                cellsAlive += sharedData[neighborId - blockStartId];
            } 
            else {
                cellsAlive += data[neighborId];
            }
        }
    }

    resultData[gid] = (cellsAlive == 3 || (cellsAlive == 2 && sharedData[threadIdx.x]) || (cellsAlive == 6 && !sharedData[threadIdx.x])) ? 1 : 0;
}

struct BenchmarkResult computeIterationCudaShared(ubyte*& data, ubyte*& resultData, const uint width, const uint height, const uint iterations, const int blockSize) {
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

    int gridSize = (dataLength + blockSize - 1) / blockSize;
    size_t sharedMemSize = blockSize * sizeof(ubyte);

    cudaEventRecord(startKernel);
    for (uint i = 0; i < iterations; ++i) {
        gameOfLifeSharedMemKernel<<<gridSize, blockSize, sharedMemSize>>>(gpuData, width, height, gpuResultData);
        std::swap(gpuData, gpuResultData);
    }
    cudaEventRecord(stopKernel);
    cudaEventSynchronize(stopKernel);

    cudaMemcpy(data, gpuData, dataLength * sizeof(ubyte), cudaMemcpyDeviceToHost);
    cudaEventRecord(stopTotal);
    cudaEventSynchronize(stopTotal);

    cudaFree(gpuData);
    cudaFree(gpuResultData);

    float totalGPUTime, kernelTime;
    cudaEventElapsedTime(&totalGPUTime, startTotal, stopTotal);
    cudaEventElapsedTime(&kernelTime, startKernel, stopKernel);
    cudaEventDestroy(startTotal);
    cudaEventDestroy(stopTotal);
    cudaEventDestroy(startKernel);
    cudaEventDestroy(stopKernel);
    return BenchmarkResult(totalGPUTime, kernelTime);
}

__global__ void gameOfLifeSharedMemKernelWithHalo(const ubyte* data, const uint width, const uint height, ubyte* resultData) {
    __shared__ ubyte tile[BLOCK_SIZE + 2][BLOCK_SIZE + 2];

    uint x = blockIdx.x * BLOCK_SIZE + threadIdx.x;
    uint y = blockIdx.y * BLOCK_SIZE + threadIdx.y;

    uint localX = threadIdx.x + 1;
    uint localY = threadIdx.y + 1;

    if (x < width && y < height) {
        tile[localY][localX] = data[y * width + x];
    } else {
        tile[localY][localX] = 0;
    }

    // Load halo cells
    if (threadIdx.x == 0) {
        // Left halo
        uint haloX = (x - 1 + width) % width;
        if (y < height) {
            tile[localY][0] = data[y * width + haloX];
        } else {
            tile[localY][0] = 0;
        }
    }

    if (threadIdx.x == BLOCK_SIZE - 1) {
        // Right halo
        uint haloX = (x + 1) % width;
        if (y < height) {
            tile[localY][BLOCK_SIZE + 1] = data[y * width + haloX];
        } else {
            tile[localY][BLOCK_SIZE + 1] = 0;
        }
    }

    if (threadIdx.y == 0) {
        // Top halo
        uint haloY = (y - 1 + height) % height;
        if (x < width) {
            tile[0][localX] = data[haloY * width + x];
        } else {
            tile[0][localX] = 0;
        }
    }

    if (threadIdx.y == BLOCK_SIZE - 1) {
        // Bottom halo
        uint haloY = (y + 1) % height;
        if (x < width) {
            tile[BLOCK_SIZE + 1][localX] = data[haloY * width + x];
        } else {
            tile[BLOCK_SIZE + 1][localX] = 0;
        }
    }

    // Faltan las esquinas del halo
    if (threadIdx.x == 0 && threadIdx.y == 0) {
        uint blockStartX = blockIdx.x * BLOCK_SIZE;
        uint blockStartY = blockIdx.y * BLOCK_SIZE;

        // Top left corner
        uint topLeftX = (blockStartX - 1 + width) % width;
        uint topLeftY = (blockStartY - 1 + height) % height;
        tile[0][0] = data[topLeftY * width + topLeftX];

        // Top right corner
        uint topRightX = (blockStartX + BLOCK_SIZE) % width;
        uint topRightY = (blockStartY - 1 + height) % height;
        tile[0][BLOCK_SIZE + 1] = data[topRightY * width + topRightX];

        // Bottom left corner
        uint bottomLeftX = (blockStartX - 1 + width) % width;
        uint bottomLeftY = (blockStartY + BLOCK_SIZE) % height;
        tile[BLOCK_SIZE + 1][0] = data[bottomLeftY * width + bottomLeftX];

        // Bottom right corner
        uint bottomRightX = (blockStartX + BLOCK_SIZE) % width;
        uint bottomRightY = (blockStartY + BLOCK_SIZE) % height;
        tile[BLOCK_SIZE + 1][BLOCK_SIZE + 1] = data[bottomRightY * width + bottomRightX];
    }

    __syncthreads();

    if (x >= width || y >= height) return;

    uint cellsAlive = 0;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            cellsAlive += tile[localY + dy][localX + dx];
        }
    }

    ubyte currentState = tile[localY][localX];
    resultData[y * width + x] = (cellsAlive == 3 || (cellsAlive == 2 && currentState) || (cellsAlive == 6 && !currentState)) ? 1 : 0;
}

void computeIterationCudaSharedWithHalo(ubyte*& data, ubyte*& resultData, const uint width, const uint height, const uint iterations) {
    dim3 blockSize(BLOCK_SIZE, BLOCK_SIZE);
    dim3 gridSize((width + BLOCK_SIZE - 1) / BLOCK_SIZE, (height + BLOCK_SIZE - 1) / BLOCK_SIZE);

    for (uint i = 0; i < iterations; ++i) {
        gameOfLifeSharedMemKernelWithHalo<<<gridSize, blockSize>>>(data, width, height, resultData);
        std::swap(data, resultData);
    }
}
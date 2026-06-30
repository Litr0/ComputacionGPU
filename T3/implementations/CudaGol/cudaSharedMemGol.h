#include "../common.h"

struct BenchmarkResult computeIterationCudaShared(ubyte*& data, ubyte*& resultData, const uint width, const uint height, const uint iterations, const int blockSize);

void computeIterationCudaSharedWithHalo(ubyte*& data, ubyte*& resultData, const uint width, const uint height, const uint iterations);
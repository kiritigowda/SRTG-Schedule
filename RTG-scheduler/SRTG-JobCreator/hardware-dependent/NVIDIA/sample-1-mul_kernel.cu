/*
 *      Author: Kiriti Nagesh Gowda
 */

#include <assert.h>
#include <cuda.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
cudaError_t cudaFuncSetCacheConfig(const char * func, enum cudaFuncCache
cacheConfig)
*/

// Functions
void Check_CUDA_Error(const char *);
void mul(float *, float *, float *, float, float, float);
void randomInit(float *, int);

// Kernel
__global__ void matrixmul(float *Md, float *Nd, float *Pd, float width,
                          float width_blk, float height_blk, float width_M,
                          float width_N, float height_M, int m, int n) {
  // Block Index
  int bx = blockIdx.x;
  int by = blockIdx.y;

  // Thread Index
  int tx = threadIdx.x;
  int ty = threadIdx.y;

  int Row = by * width_blk + ty;
  int Col = bx * height_blk + tx;

  float pValue = 0;

  if (Col < (int)width_N && Row < (int)height_M) {
    for (int i = 0; i < width; i++) {
      float Melement = Md[Row * (int)width_M + i];
      float Nelement = Nd[i * (int)width_N + Col];

      pValue += Melement * Nelement;
    }
    Pd[Row * (int)width_N + Col] = pValue;
  }
}

int main(void) {
  float *M, *N, *P, *K; // Host Matrices
  float *Md, *Nd, *Pd;  // Device Matrices

  float height_M, width_M; // height and width of matrix M
  float height_N, width_N; // height and width of matrix N

  srand(2006); // setting limit for rand function

  // printf("enter the height and width of matrix M\n");
  // scanf("%f %f",&height_M,&width_M);
  height_M = 32;
  width_M = 32;

  // printf("enter the heigth and width of matrix N\n");
  // scanf("%f %f", &height_N, &width_N);\

  height_N = 32;
  width_N = 32;

  float size_M, size_N, size_P;

  size_M = height_M * width_M;
  size_N = height_N * width_N;
  size_P = height_M * width_N;

  // check whether the given matrix sizes are compatable fr multiplication
  float width = width_M;

  if (width_M == height_N) {

    // Allocate space for the matrices in host memory
    M = (float *)malloc(size_M * sizeof(float));
    N = (float *)malloc(size_N * sizeof(float));
    P = (float *)malloc(size_P * sizeof(float));
    K = (float *)malloc(size_P * sizeof(float));

    // Initialize the data to the matrices  (some random data)
    randomInit(M, size_M);
    randomInit(N, size_N);

    // Allocate memory for the matrices on the GPU
    cudaMalloc((void **)&Md, size_M * sizeof(float));
    cudaMalloc((void **)&Nd, size_N * sizeof(float));
    cudaMalloc((void **)&Pd, size_P * sizeof(float));

    // Transfer Data From host to Device
    cudaMemcpy(Md, M, size_M * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(Nd, N, size_N * sizeof(float), cudaMemcpyHostToDevice);

    // block dimensions
    float height_blk, width_blk;

    // printf("enter the Dimensions of the block\n");
    // scanf("%f %f",&width_blk,&height_blk);

    height_blk = 16;
    width_blk = 16;

    dim3 threads(width_blk, height_blk);

    float x = ceil(width_N / threads.x);
    float y = ceil(height_M / threads.y);

    printf("x=%f\n y=%f\n", x, y);

    dim3 grid(1, 1);

    // Timer
    cudaEvent_t start, stop;
    float time;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // kernel Invocation
    cudaDeviceSetCacheConfig(cudaFuncCachePreferL1);
    // cudaDeviceSetCacheConfig( cudaFuncCachePreferShared );
    // cudaDeviceSetCacheConfig( cudaFuncCachePreferNone );

    matrixmul<<<grid, threads>>>(Md, Nd, Pd, width, width_blk, height_blk,
                                 width_M, width_N, height_M);
    Check_CUDA_Error("Kernel Execution Failed!");

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);

    printf("Processing time GPU: %f(ms) \n", time);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    // Transferring the result data from DEVICE to HOST
    cudaMemcpy(P, Pd, size_P * sizeof(float), cudaMemcpyDeviceToHost);

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    // multiplication on host
    mul(M, N, K, height_M, width_N, width_M);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&time, start, stop);

    printf("Processing time CPU: %f(ms) \n", time);

    /*
    for(int i=0; i<size_P;i++)
    {
      if(P[i]!= K[i])
      printf("Error at P[%d] = %.5f ------------- K[%d]=%.5f\n",i,P[i],i,K[i]);
    }
    */

    // Free Device Memory
    cudaFree(Md);
    cudaFree(Nd);
    cudaFree(Pd);

    // Free Host Memory
    free(M);
    free(N);
    free(P);
    free(K);
  } else {
    printf("the sizes of the matrix you entered are not compatable for "
           "multiplication\n");
  }
}

// Fucntions
void Check_CUDA_Error(const char *message) {
  cudaError_t error = cudaGetLastError();
  if (error != cudaSuccess) {
    // fprintf(stderr,"ERROR: %s: %s\n", message, cudaGetErrorString(error) );
    exit(-1);
  }
}

// Matrix multiplication on HOST
void mul(float *M, float *N, float *K, float height_M, float width_N,
         float width_M) {
  for (int i = 0; i < height_M; i++) {
    for (int j = 0; j < width_N; j++) {
      float sum = 0;

      for (int k = 0; k < width_M; k++) {
        float a = M[i * (int)width_M + k];
        float b = N[k * (int)width_N + j];
        sum += a * b;
      }
      K[i * (int)width_N + j] = sum;
    }
  }
}

// Initializing the matrices using rand function
void randomInit(float *data, int size) {
  for (int i = 0; i < size; ++i)
    data[i] = rand() / (float)RAND_MAX;
}

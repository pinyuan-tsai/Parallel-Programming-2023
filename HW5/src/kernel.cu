//kernel 0
// #include <cuda.h>
// #include <stdio.h>
// #include <stdlib.h>

// __global__ void mandelKernel() {
//     // To avoid error caused by the floating number, use the following pseudo code
//     //
//     // float x = lowerX + thisX * stepX;
//     // float y = lowerY + thisY * stepY;
// }

// // Host front-end function that allocates the memory and launches the GPU kernel
// void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
// {
//     float stepX = (upperX - lowerX) / resX;
//     float stepY = (upperY - lowerY) / resY;
// }

//kernel 1
// #include <cuda.h>
// #include <stdio.h>
// #include <stdlib.h>

// __device__ int mandel(float c_re, float c_im, int maxIteration)
// {
//   float z_re = c_re, z_im = c_im;
//   int i;
//   for (i = 0; i < maxIteration; ++i)
//   {

//     if (z_re * z_re + z_im * z_im > 4.f)
//       break;

//     float new_re = z_re * z_re - z_im * z_im;
//     float new_im = 2.f * z_re * z_im;
//     z_re = c_re + new_re;
//     z_im = c_im + new_im;
//   }

//   return i;
// }

// __global__ void mandelKernel(float lowerX, float lowerY, float stepX, float stepY, int *d_img, int resX, int resY, int maxIterations){
//     // To avoid error caused by the floating number, use the following pseudo code
//     //
//     // float x = lowerX + thisX * stepX;
//     // float y = lowerY + thisY * stepY;

//     int thisX = blockIdx.x * blockDim.x + threadIdx.x;
//     int thisY = blockIdx.y * blockDim.y + threadIdx.y;

//     if (thisX >= resX || thisY >= resY) return;

//     float x = lowerX + thisX * stepX;
//     float y = lowerY + thisY * stepY;
//     int index = thisY * resX + thisX;
//     d_img[index] = mandel(x, y, maxIterations);
    
// }

// // Host front-end function that allocates the memory and launches the GPU kernel
// void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
// {   
    
//     float stepX = (upperX - lowerX) / resX;
//     float stepY = (upperY - lowerY) / resY;

//     //initiate the memory
//     dim3 threadsofblock(16,16);
//     dim3 totalblocks(ceil(resX / threadsofblock.x), ceil(resY / threadsofblock.y));

//     //not allowed to use the image input as the host memory directly
//     int size = resX * resY * sizeof(int);
//     int *d_img;
//     int *host = (int*)malloc(size);
//     cudaMalloc((void**)&d_img, size);

//     //launch the kernel
//     mandelKernel<<<totalblocks, threadsofblock>>>(lowerX, lowerY, stepX, stepY, d_img, resX, resY, maxIterations);

//     cudaMemcpy(host, d_img, size, cudaMemcpyDeviceToHost);
//     memcpy(img, host, size);

//     //free the memory
//     cudaFree(d_img);
//     free(host);
    
// }

//kernel2
// #include <cuda.h>
// #include <stdio.h>
// #include <stdlib.h>

// __device__ int mandel(float c_re, float c_im, int maxIteration)
// {
//   float z_re = c_re, z_im = c_im;
//   int i;
//   for (i = 0; i < maxIteration; ++i)
//   {

//     if (z_re * z_re + z_im * z_im > 4.f)
//       break;

//     float new_re = z_re * z_re - z_im * z_im;
//     float new_im = 2.f * z_re * z_im;
//     z_re = c_re + new_re;
//     z_im = c_im + new_im;
//   }

//   return i;
// }

// __global__ void mandelKernel(float lowerX, float lowerY, float stepX, float stepY, int *d_img, int resX, int resY, int maxIterations){
//     // To avoid error caused by the floating number, use the following pseudo code
//     //
//     // float x = lowerX + thisX * stepX;
//     // float y = lowerY + thisY * stepY;

//     int thisX = blockIdx.x * blockDim.x + threadIdx.x;
//     int thisY = blockIdx.y * blockDim.y + threadIdx.y;

//     if (thisX >= resX || thisY >= resY) return;

//     float x = lowerX + thisX * stepX;
//     float y = lowerY + thisY * stepY;
//     int index = thisY * resX + thisX;
//     d_img[index] = mandel(x, y, maxIterations);
    
// }

// // Host front-end function that allocates the memory and launches the GPU kernel
// void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
// {   
    
//     float stepX = (upperX - lowerX) / resX;
//     float stepY = (upperY - lowerY) / resY;

//     //initiate the memory
//     dim3 threadsofblock(16,16);
//     dim3 totalblocks(ceil(resX / threadsofblock.x), ceil(resY / threadsofblock.y));

//     //Use cudaHostAlloc to allocate the host memory, and use cudaMallocPitch to allocate GPU memory
//     int size = resX * resY * sizeof(int);
//     int *d_img;
//     int *host;
//     size_t pitch = 0;
//     // Allocate pinned host memory using cudaHostAlloc
//     cudaHostAlloc(&host, size, cudaHostAllocDefault);
//     // Allocate device memory using cudaMallocPitch
//     cudaMallocPitch(&d_img, &pitch, resX * sizeof(int), resY);

//     //launch the kernel
//     mandelKernel<<<totalblocks, threadsofblock>>>(lowerX, lowerY, stepX, stepY, d_img, resX, resY, maxIterations);

//     cudaDeviceSynchronize();
    
//     //copy the memory back
//     //printf("%d\n", pitch);
//     //cudaMemcpy2D(host, resX * sizeof(int), d_img, pitch, resX * sizeof(int), resY, cudaMemcpyDeviceToHost);
//     cudaMemcpy(host,d_img, size, cudaMemcpyDeviceToHost);
//     memcpy(img, host, size);

//     //free the memory
//     cudaFree(d_img);
//     cudaFreeHost(host);
    
// }


//kernel3
//---5.794ms---
// #include <cuda.h>
// #include <stdio.h>
// #include <stdlib.h>

// __device__ int mandel(float c_re, float c_im, int maxIteration)
// {
//   float z_re = c_re, z_im = c_im;
//   int i;
//   for (i = 0; i < maxIteration; ++i)
//   {

//     if (z_re * z_re + z_im * z_im > 4.f)
//       break;

//     float new_re = z_re * z_re - z_im * z_im;
//     float new_im = 2.f * z_re * z_im;
//     z_re = c_re + new_re;
//     z_im = c_im + new_im;
//   }

//   return i;
// }

// __global__ void mandelKernel(float lowerX, float lowerY, float stepX, float stepY, int *d_img, int resX, int resY, int maxIterations, int group_x, int group_y){
//     // To avoid error caused by the floating number, use the following pseudo code
//     //
//     // float x = lowerX + thisX * stepX;
//     // float y = lowerY + thisY * stepY;

//     int thisX = (blockIdx.x * blockDim.x + threadIdx.x) * group_x;
//     int thisY = (blockIdx.y * blockDim.y + threadIdx.y) * group_y;

//     if (thisX >= resX || thisY >= resY) return;

//     for(int i = 0; i < group_y; i++){
//       if(thisY + i >= resY) return;
//       for(int j = 0; j < group_x ; j++){
//         if(thisX + j >= resX) return;
//         float x = lowerX + (thisX + j) * stepX;
//         float y = lowerY + (thisY + i) * stepY;
//         int index = (thisY + i) * resX + (thisX + j);
//         d_img[index] = mandel(x, y, maxIterations);
//       }
//     }

//     // float x = lowerX + thisX * stepX;
//     // float y = lowerY + thisY * stepY;
//     // int index = thisY * resX + thisX;
//     // d_img[index] = mandel(x, y, maxIterations);
    
// }

// // Host front-end function that allocates the memory and launches the GPU kernel
// void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
// {   
    
//     float stepX = (upperX - lowerX) / resX;
//     float stepY = (upperY - lowerY) / resY;
//     int group_x = 2, group_y = 2;
//     int thread = 16;
//     //initiate the memory
//     dim3 threadsofblock(thread / group_x ,thread / group_y);
//     dim3 totalblocks(ceil(resX / thread), ceil(resY / thread));

//     //Use cudaHostAlloc to allocate the host memory, and use cudaMallocPitch to allocate GPU memory
//     int size = resX * resY * sizeof(int);
//     int *d_img;
//     int *host;
//     size_t pitch = 0;
//     // Allocate pinned host memory using cudaHostAlloc
//     cudaHostAlloc(&host, size, cudaHostAllocDefault);
//     // Allocate device memory using cudaMallocPitch
//     cudaMallocPitch(&d_img, &pitch, resX * sizeof(int), resY);

//     //launch the kernel
//     mandelKernel<<<totalblocks, threadsofblock>>>(lowerX, lowerY, stepX, stepY, d_img, resX, resY, maxIterations, group_x,group_y);

//     cudaDeviceSynchronize();
    
//     //copy the memory back
//     //printf("%d\n", pitch);
//     //cudaMemcpy2D(host, resX * sizeof(int), d_img, pitch, resX * sizeof(int), resY, cudaMemcpyDeviceToHost);
//     cudaMemcpy(host,d_img, size, cudaMemcpyDeviceToHost);
//     memcpy(img, host, size);

//     //free the memory
//     cudaFree(d_img);
//     cudaFreeHost(host);
    
// }

//kernel4
//---3.792ms---
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

__device__ int mandel(float c_re, float c_im, int maxIteration)
{
  float z_re = c_re, z_im = c_im;
  int i;
  for (i = 0; i < maxIteration; ++i)
  {

    if (z_re * z_re + z_im * z_im > 4.f)
      break;

    float new_re = z_re * z_re - z_im * z_im;
    float new_im = 2.f * z_re * z_im;
    z_re = c_re + new_re;
    z_im = c_im + new_im;
  }

  return i;
}

__global__ void mandelKernel(float lowerX, float lowerY, float stepX, float stepY, int *d_img, int resX, int resY, int maxIterations){
    // To avoid error caused by the floating number, use the following pseudo code
    //
    // float x = lowerX + thisX * stepX;
    // float y = lowerY + thisY * stepY;

    int thisX = blockIdx.x * blockDim.x + threadIdx.x;
    int thisY = blockIdx.y * blockDim.y + threadIdx.y;

    if (thisX < resX && thisY < resY){

		float x = lowerX + thisX * stepX;
		float y = lowerY + thisY * stepY;
		int index = thisY * resX + thisX;
		d_img[index] = mandel(x, y, maxIterations);
	}
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{   
    
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;

    //initiate the memory
    dim3 threadsofblock(16,16);
    dim3 totalblocks(ceil(resX / threadsofblock.x), ceil(resY / threadsofblock.y));

    //not allowed to use the image input as the host memory directly
    int size = resX * resY * sizeof(int);
    int *d_img;
    //int *host = (int*)malloc(size);
    cudaMalloc((void**)&d_img, size);

    //launch the kernel
    mandelKernel<<<totalblocks, threadsofblock>>>(lowerX, lowerY, stepX, stepY, d_img, resX, resY, maxIterations);

    cudaMemcpy(img, d_img, size, cudaMemcpyDeviceToHost);
    //memcpy(img, host, size);

    //free the memory
    cudaFree(d_img);
    //free(host);
    
}
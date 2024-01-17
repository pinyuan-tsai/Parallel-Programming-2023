#include <stdio.h>
#include <stdlib.h>
#include "hostFE.h"
#include "helper.h"




void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage, cl_device_id *device,
            cl_context *context, cl_program *program)
{
    cl_int status;
    int filterSize = filterWidth * filterWidth;

    // Create command queue
    cl_command_queue commandQueue = clCreateCommandQueue(*context, *device, 0, &status);
    if (status != CL_SUCCESS) {
        printf("Error creating command queue\n");
        return;
    }

    // Create memory buffers
    cl_mem inputBuffer = clCreateBuffer(*context, CL_MEM_USE_HOST_PTR, imageHeight * imageWidth * sizeof(float), inputImage, &status);
    cl_mem outputBuffer = clCreateBuffer(*context, CL_MEM_WRITE_ONLY, imageHeight * imageWidth * sizeof(float), NULL, &status);
    cl_mem filterBuffer = clCreateBuffer(*context, CL_MEM_USE_HOST_PTR, filterSize * sizeof(float), filter, &status);
    if (status != CL_SUCCESS) {
        printf("Error creating memory buffers\n");
        return;
    }

    // Create kernel
    cl_kernel kernel = clCreateKernel(*program, "convolution", &status);
    if (status != CL_SUCCESS) {
        printf("Error creating kernel\n");
        return;
    }

    // Set kernel arguments
    status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    status |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
    status |= clSetKernelArg(kernel, 2, sizeof(cl_int), &imageWidth);
    status |= clSetKernelArg(kernel, 3, sizeof(cl_int), &imageHeight);
    status |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &filterBuffer);
    status |= clSetKernelArg(kernel, 5, sizeof(cl_int), &filterWidth);
    if (status != CL_SUCCESS) {
        printf("Error setting kernel arguments\n");
        return;
    }

    // Enqueue kernel for execution
    size_t global_size = imageHeight * imageWidth;
    size_t local_size = 64;
    status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, 0, &global_size, &local_size, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        printf("Error enqueueing kernel\n");
        return;
    }

    // Read output data from memory buffer
    status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, imageHeight * imageWidth * sizeof(float), outputImage, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
        printf("Error reading output data\n");
        return;
    }

    // Clean up
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
    clReleaseMemObject(filterBuffer);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commandQueue);
}
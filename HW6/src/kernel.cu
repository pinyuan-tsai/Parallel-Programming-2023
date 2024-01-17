#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "CycleTimer.h"
#include "helper.h"
#include "bmpfuncs.h"
#include <cuda.h>
typedef unsigned char uchar;

void storeImage(float *imageOut, const char *filename, int rows, int cols,
                const char *refFilename)
{

   FILE *ifp, *ofp;
   unsigned char tmp;
   int offset;
   unsigned char *buffer;
   int i, j;

   int bytes;

   int height, width;

   ifp = fopen(refFilename, "rb");
   if (ifp == NULL)
   {
      perror(filename);
      exit(-1);
   }

   fseek(ifp, 10, SEEK_SET);
   fread(&offset, 4, 1, ifp);

   fseek(ifp, 18, SEEK_SET);
   fread(&width, 4, 1, ifp);
   fread(&height, 4, 1, ifp);

   fseek(ifp, 0, SEEK_SET);

   buffer = (unsigned char *)malloc(offset);
   if (buffer == NULL)
   {
      perror("malloc");
      exit(-1);
   }

   fread(buffer, 1, offset, ifp);

   printf("Writing output image to %s\n", filename);
   ofp = fopen(filename, "wb");
   if (ofp == NULL)
   {
      perror("opening output file");
      exit(-1);
   }
   bytes = fwrite(buffer, 1, offset, ofp);
   if (bytes != offset)
   {
      printf("error writing header!\n");
      exit(-1);
   }

   // NOTE bmp formats store data in reverse raster order (see comment in
   // readImage function), so we need to flip it upside down here.
   int mod = width % 4;
   if (mod != 0)
   {
      mod = 4 - mod;
   }
   //   printf("mod = %d\n", mod);
   for (i = height - 1; i >= 0; i--)
   {
      for (j = 0; j < width; j++)
      {
         tmp = (unsigned char)imageOut[i * cols + j];
         fwrite(&tmp, sizeof(char), 1, ofp);
      }
      // In bmp format, rows must be a multiple of 4-bytes.
      // So if we're not at a multiple of 4, add junk padding.
      for (j = 0; j < mod; j++)
      {
         fwrite(&tmp, sizeof(char), 1, ofp);
      }
   }

   fclose(ofp);
   fclose(ifp);

   free(buffer);
}

/*
 * Read bmp image and convert to byte array. Also output the width and height
 */
float *readImage(const char *filename, int *widthOut, int *heightOut)
{

   uchar *imageData;

   int height, width;
   uchar tmp;
   int offset;
   int i, j;

   printf("Reading input image from %s\n", filename);
   FILE *fp = fopen(filename, "rb");
   if (fp == NULL)
   {
      perror(filename);
      exit(-1);
   }

   fseek(fp, 10, SEEK_SET);
   fread(&offset, 4, 1, fp);

   fseek(fp, 18, SEEK_SET);
   fread(&width, 4, 1, fp);
   fread(&height, 4, 1, fp);

   printf("width = %d\n", width);
   printf("height = %d\n", height);

   *widthOut = width;
   *heightOut = height;

   imageData = (uchar *)malloc(width * height);
   if (imageData == NULL)
   {
      perror("malloc");
      exit(-1);
   }

   fseek(fp, offset, SEEK_SET);
   fflush(NULL);

   int mod = width % 4;
   if (mod != 0)
   {
      mod = 4 - mod;
   }

   // NOTE bitmaps are stored in upside-down raster order.  So we begin
   // reading from the bottom left pixel, then going from left-to-right,
   // read from the bottom to the top of the image.  For image analysis,
   // we want the image to be right-side up, so we'll modify it here.

   // First we read the image in upside-down

   // Read in the actual image
   for (i = 0; i < height; i++)
   {

      // add actual data to the image
      for (j = 0; j < width; j++)
      {
         fread(&tmp, sizeof(char), 1, fp);
         imageData[i * width + j] = tmp;
      }
      // For the bmp format, each row has to be a multiple of 4,
      // so I need to read in the junk data and throw it away
      for (j = 0; j < mod; j++)
      {
         fread(&tmp, sizeof(char), 1, fp);
      }
   }

   // Then we flip it over
   int flipRow;
   for (i = 0; i < height / 2; i++)
   {
      flipRow = height - (i + 1);
      for (j = 0; j < width; j++)
      {
         tmp = imageData[i * width + j];
         imageData[i * width + j] = imageData[flipRow * width + j];
         imageData[flipRow * width + j] = tmp;
      }
   }

   fclose(fp);

   // Input image on the host
   float *floatImage = NULL;
   floatImage = (float *)malloc(sizeof(float) * width * height);
   if (floatImage == NULL)
   {
      perror("malloc");
      exit(-1);
   }

   // Convert the BMP image to float (not required)
   for (i = 0; i < height; i++)
   {
      for (j = 0; j < width; j++)
      {
         floatImage[i * width + j] = (float)imageData[i * width + j];
      }
   }

   free(imageData);
   return floatImage;
}

//#include "serialConv.h"
char *readSource(char *kernelPath)
{
    cl_int status;
    FILE *fp;
    char *source;
    long int size;

    printf("Program file is: %s\n", kernelPath);

    fp = fopen(kernelPath, "rb");
    if (!fp)
    {
        printf("Could not open kernel file\n");
        exit(-1);
    }
    status = fseek(fp, 0, SEEK_END);
    if (status != 0)
    {
        printf("Error seeking to end of file\n");
        exit(-1);
    }
    size = ftell(fp);
    if (size < 0)
    {
        printf("Error getting file position\n");
        exit(-1);
    }

    rewind(fp);

    source = (char *)malloc(size + 1);

    int i;
    for (i = 0; i < size + 1; i++)
    {
        source[i] = '\0';
    }

    if (source == NULL)
    {
        printf("Error allocating space for the kernel source\n");
        exit(-1);
    }

    fread(source, 1, size, fp);
    source[size] = '\0';

    return source;
}
float *readFilter(const char *filename, int *filterWidth)
{
    printf("Reading filter data from %s\n", filename);

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("Could not open filter file\n");
        exit(-1);
    }

    fscanf(fp, "%d", filterWidth);

    float *filter = (float *)malloc(*filterWidth * *filterWidth * sizeof(int));

    float tmp;
    for (int i = 0; i < *filterWidth * *filterWidth; i++)
    {
        fscanf(fp, "%f", &tmp);
        filter[i] = tmp;
    }

    printf("Filter width: %d\n", *filterWidth);

    fclose(fp);
    return filter;
}

void usage(const char *progname)
{
   printf("Usage: %s [options]\n", progname);
   printf("Program Options:\n");
   printf("  -i  --input   <String> Input image\n");
   printf("  -f  --filter  <INT>    Use which filter (0, 1, 2)\n");
   printf("  -?  --help             This message\n");
}

int compare(const void *a, const void *b)
{
   double *x = (double *)a;
   double *y = (double *)b;
   if (*x < *y)
      return -1;
   else if (*x > *y)
      return 1;
   return 0;
}
__global__ void convolution(const float *img, float *out, const int width, 
                            const int height, float *filter, const int filterWidth)
{
   int index = blockIdx.x * blockDim.x + threadIdx.x;
   int col = index % width;
   int row = index / width;
   int half_filter = filterWidth / 2;
   float sum = 0;

   for(int k = -half_filter; k <= half_filter; k++) 
   {
        if(row + k < 0 || row + k >= height)
            continue;
        for(int l = -half_filter; l <= half_filter; l++)
        {
            
            if (col + l >= 0 && col + l < width)
            {
                sum += img[(row + k) * width + col + l] * filter[(k + half_filter) * filterWidth + l + half_filter];
            }
        }
   }
   
   out[row * width + col] = sum;
}


void hostFE(int filterWidth, float *filter, int imageHeight, int imageWidth,
            float *inputImage, float *outputImage)
{
    int filterSize = filterWidth * filterWidth;

    // Allocate device memory
    float *d_inputImage, *d_outputImage, *d_filter;
    cudaMalloc((void**)&d_inputImage, imageHeight * imageWidth * sizeof(float));
    cudaMalloc((void**)&d_outputImage, imageHeight * imageWidth * sizeof(float));
    cudaMalloc((void**)&d_filter, filterSize * sizeof(float));

    // Copy input data from host to device
    cudaMemcpy(d_inputImage, inputImage, imageHeight * imageWidth * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_filter, filter, filterSize * sizeof(float), cudaMemcpyHostToDevice);

    // Set grid and block dimensions
    dim3 blockSize(16, 16);
    dim3 gridSize((imageWidth + blockSize.x - 1) / blockSize.x, (imageHeight + blockSize.y - 1) / blockSize.y);

    // Launch kernel
    convolution<<<gridSize, blockSize>>>(d_inputImage, d_outputImage, imageWidth, imageHeight, d_filter, filterWidth);

    // Copy output data from device to host
    cudaMemcpy(outputImage, d_outputImage, imageHeight * imageWidth * sizeof(float), cudaMemcpyDeviceToHost);

    // Clean up
    cudaFree(d_inputImage);
    cudaFree(d_outputImage);
    cudaFree(d_filter);
}

int main(int argc, char **argv)
{
   int i, j;

   // Rows and columns in the input image
   int imageHeight;
   int imageWidth;

   double start_time, end_time;

   char *inputFile = "input.bmp";
   const char *outputFile = "output.bmp";
   //const char *refFile = "ref.bmp";
   char *filterFile = "filter1.csv";

   // parse commandline options ////////////////////////////////////////////
   int opt;
   static struct option long_options[] = {
       {"filter", 1, 0, 'f'},
       {"input", 1, 0, 'i'},
       {"help", 0, 0, '?'},
       {0, 0, 0, 0}};

   while ((opt = getopt_long(argc, argv, "i:f:?", long_options, NULL)) != EOF)
   {

      switch (opt)
      {
      case 'i':
      {
         inputFile = optarg;

         break;
      }
      case 'f':
      {
         int idx = atoi(optarg);
         if (idx == 2)
            filterFile = "filter2.csv";
         else if (idx == 3)
            filterFile = "filter3.csv";

         break;
      }
      case '?':
      default:
         usage(argv[0]);
         return 1;
      }
   }
   // end parsing of commandline options

   // read filter data
   int filterWidth;
   float *filter = readFilter(filterFile, &filterWidth);

   // Homegrown function to read a BMP from file
   float *inputImage = readImage(inputFile, &imageWidth, &imageHeight);
   // Size of the input and output images on the host
   int dataSize = imageHeight * imageWidth * sizeof(float);
   // Output image on the host
   float *outputImage = (float *)malloc(dataSize);

   // helper init CL
   //cl_program program;
   //cl_device_id device;
   //cl_context context;
   //initCL(&device, &context, &program);

   double minThread = 0;
   double recordThread[10] = {0};
   for (int i = 0; i < 10; ++i)
   {
      memset(outputImage, 0, dataSize);
      start_time = currentSeconds();
      // Run the host to execute the kernel
      hostFE(filterWidth, filter, imageHeight, imageWidth, inputImage, outputImage);
      end_time = currentSeconds();
      recordThread[i] = end_time - start_time;
   }
   qsort(recordThread, 10, sizeof(double), compare);
   for (int i = 3; i < 7; ++i)
   {
      minThread += recordThread[i];
   }
   minThread /= 4;

   printf("\n[conv opencl]:\t\t[%.3f] ms\n\n", minThread * 1000);

   // Write the output image to file
   storeImage(outputImage, outputFile, imageHeight, imageWidth, inputFile);

   // Output image of reference on the host
//    float *refImage = NULL;
//    refImage = (float *)malloc(dataSize);
//    memset(refImage, 0, dataSize);

//    double minSerial = 0;
//    double recordSerial[10] = {0};
//    for (int i = 0; i < 10; ++i)
//    {
//       memset(refImage, 0, dataSize);
//       start_time = currentSeconds();
//       serialConv(filterWidth, filter, imageHeight, imageWidth, inputImage, refImage);
//       end_time = currentSeconds();
//       recordSerial[i] = end_time - start_time;
//    }
//    qsort(recordSerial, 10, sizeof(double), compare);
//    for (int i = 3; i < 7; ++i)
//    {
//       minSerial += recordSerial[i];
//    }
//    minSerial /= 4;

//    printf("\n[conv serial]:\t\t[%.3f] ms\n\n", minSerial * 1000);

//    storeImage(refImage, refFile, imageHeight, imageWidth, inputFile);

//    int diff_counter = 0;
//    for (i = 0; i < imageHeight; i++)
//    {
//       for (j = 0; j < imageWidth; j++)
//       {
//          if (abs(outputImage[i * imageWidth + j] - refImage[i * imageWidth + j]) > 10)
//          {
//             diff_counter += 1;
//          }
//       }
//    }

//    float diff_ratio = (float)diff_counter / (imageHeight * imageWidth);
//    printf("Diff ratio: %f\n", diff_ratio);

//    if (diff_ratio > 0.1)
//    {
//       printf("\n\033[31mFAILED:\tResults are incorrect!\033[0m\n");
//       return -1;
//    }
//    else
//    {
//       printf("\n\033[32mPASS:\t(%.2fx speedup over the serial version)\033[0m\n", minSerial / minThread);
//    }

   return 0;
}

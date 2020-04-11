#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "clutil.c"
#include <CL/cl.h>
#include <vector>
#include <memory>
#include <algorithm>
#include<stdio.h>

#define TS 16
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

using namespace cv;
using namespace std;


int main(){

    string image_path    = "lenna.png";
    char* kernelstring;
    int tamanhoString;
    const int tamanho = 3;
    Mat img = imread(image_path, IMREAD_GRAYSCALE);
    unsigned char* data;
    unsigned char* result;
    

    //pega vetor organizado row-by-row
    data = img.data;
    int altura = img.rows;
    int largura = img.cols;
    int tamanhoTotal = altura*largura;
    int canais = img.channels();

    printf("dados sobre a imagem:\naltura\t%d\nlargura\t%d\ncanais\t%d\n", altura, largura, canais);
    
    //aloca memoria para o resultado
    result = (unsigned char *) malloc(largura*altura*sizeof(unsigned char));


    // Configure the OpenCL environment
    printf(">>> Initializing OpenCL...\n");

    //plataformas
    cl_platform_id platform = 0;
    clGetPlatformIDs(1, &platform, NULL);
    
    //device
    cl_device_id device = 0;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    
    //contexto
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    
    //fila de comando
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, NULL, NULL);
    
    //nome do device
    char deviceName[1024];
    clGetDeviceInfo(device, CL_DEVICE_NAME, 1024, deviceName, NULL);
    printf("%s\n", deviceName);
    cl_event event = NULL;

    // Lê da string
    readProgram("blur.cl", &kernelstring, &tamanhoString);
    
    //compila o programa
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&kernelstring, NULL, NULL);
    clBuildProgram(program, 0, NULL, "", NULL, NULL);

    // Check for compilation errors
    size_t logSize;
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
    char* messages = (char*)malloc((1+logSize)*sizeof(char));
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, messages, NULL);
    messages[logSize] = '\0';
    if (logSize > 10) { printf(">>> Compiler message: %s\n", messages); }
    free(messages);

    // Prepare OpenCL memory objects
    cl_mem bufIn = clCreateBuffer(context, CL_MEM_READ_ONLY,  tamanhoTotal*sizeof(unsigned char), NULL, NULL);
    cl_mem bufOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, tamanhoTotal*sizeof(unsigned char), NULL, NULL);
    

    // Copy matrices to the GPU
    clEnqueueWriteBuffer(queue, bufIn, CL_TRUE, 0, tamanhoTotal*sizeof(unsigned char), data, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufOut, CL_TRUE, 0, tamanhoTotal*sizeof(unsigned char), result, 0, NULL, NULL);
    

    // configura os parametros
    cl_kernel kernel = clCreateKernel(program, "blur", NULL);
    clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&bufIn);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&bufOut);
    clSetKernelArg(kernel, 2, sizeof(int), (void*)&altura);
    clSetKernelArg(kernel, 3, sizeof(int), (void*)&largura);
    clSetKernelArg(kernel, 4, sizeof(int), (void*)&tamanho);

    printf(">>> Starting blur run...\n");

    //const size_t local[2] = {TS, TS};
    const size_t global[2] = {altura, largura};
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL, &event);
    // Wait for calculations to be finished
    clWaitForEvents(1, &event);
    printf(">>> Done\n");
    clEnqueueReadBuffer(queue, bufOut, CL_TRUE, 0, tamanhoTotal*sizeof(unsigned char), result, 0, NULL, NULL);
    
    // Free the OpenCL memory objects
    clReleaseMemObject(bufIn);
    clReleaseMemObject(bufOut);

    // Clean-up OpenCL 
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
    clReleaseProgram(program);
    clReleaseKernel(kernel);

    Mat resultMat = img.clone();
    resultMat.data = result;
    
    namedWindow( "Lenna", cv::WINDOW_AUTOSIZE );
    imshow("Lenna", img);

    namedWindow( "Lenna Blur", cv::WINDOW_AUTOSIZE );
    imshow("Lenna Blur", resultMat);


    waitKey(0);
}
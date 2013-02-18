#include <engine/epsilon.hpp>

using namespace std;

Epsilon::Epsilon(cl_device_id device)
{
    /* Save the device! */
    this->device = device;
    cl_int error = 0;

    /* Create the OpenCL context. */
    context = clCreateContext(0, 1, &device, 0, 0, &error);
    if (context == 0) throw runtime_error(Error(E_CTX, error));

    /* Now, create the command queue. */
    queue = clCreateCommandQueue(context, device, 0, &error);
    if (queue == 0) throw runtime_error(Error(E_QUEUE, error));

    /* Acquire the local work group size for the device. */
    error = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
                            sizeof(size_t), &localGroupSize, 0);
    if (error != CL_SUCCESS) throw runtime_error(Error(E_INFO, error));

    /* Now, prepare to load the OpenCL program. */
    char* source = (char*)("#include <cl/epsilon.cl>");
    program = clCreateProgramWithSource(context, 1, (const char**)&source,
                                        0, 0);
    if (program == 0) throw runtime_error(Error(E_PROG, error));

    /* Try and build the program. */
    error = clBuildProgram(program, 0, 0, "-I cl/", 0, 0);
    if (error != CL_SUCCESS)
    {
        size_t logSize;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
                              0, 0, &logSize);

        char *log = (char*)malloc(logSize + 1);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              logSize + 1, log, 0);

        ofstream logFile;
        logFile.open("cl_log");
        logFile << log;
        logFile.close();

        throw runtime_error(Error(E_BUILD, error));
    }

    /* Now, create the kernel. */
    kernel = clCreateKernel(program, "clmain", 0);
    if (kernel == 0) throw runtime_error(Error(E_KER, error));
}

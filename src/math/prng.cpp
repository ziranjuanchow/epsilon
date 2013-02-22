#include <math/prng.hpp>

/* Device-side representation. */
struct cl_prng { cl_ulong4 seed; };

bool PRNG::IsActive() { return true; }

void PRNG::Initialize()
{
    cl_int error;
    this->buffer = cl::Buffer(params.context, CL_MEM_READ_ONLY,
                              sizeof(cl_prng), nullptr, &error);
    Error::Check(Error::Memory, error);

    this->seed = 0;
}

void PRNG::Bind(cl::Kernel kernel, cl_uint index)
{
    Error::Check(Error::Bind, kernel.setArg(index, this->buffer));
}

void PRNG::Update(size_t index)
{
    cl_int error = params.queue.enqueueWriteBuffer(this->buffer, CL_FALSE, 0,
                                                   sizeof(uint64_t),
                                                   &this->seed);
    Error::Check(Error::CLIO, error);
    this->seed++;
}

void* PRNG::Query(size_t query)
{
    return nullptr;
}

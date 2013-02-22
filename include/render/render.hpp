#pragma once

#include <common/common.hpp>
#include <engine/architecture.hpp>

/** @file render.hpp
  * @brief Pixel buffer manipulation
  *
  * This file contains the definition of a kernel object responsible for
  * managing a pixel buffer, used by the renderer to draw and accumulate
  * colors into.
**/

/* This is pixel, which contains a three-color tristimulus value of any color *
 * space (XYZ or RGB) and a fourth format-dependent value, generally defining *
 * the pixel's radiance. The formats are respectively XYZr and RGB. */
struct Pixel
{
    private:
        float x, y, z, r;
    public:
        /* Default constructor, sets the pixel to black by convention. */
        Pixel() { this->x = 0; this->y = 0; this->z = 0; this->r = 0; }

        /* Returns the pixel's XYZ or RGB value. */
        Vector XYZ() { return Vector(x, y, z); }
        Vector RGB() { return Vector(x, y, z); }

        /* Returns the pixel's luminance. Valid for RGB format only. */
        float Luminance()
        {
            return (this->x * 0.2645 +
                    this->y * 0.7243 + 
                    this->z * 0.0085);
        }

		/* Gamma-corrects the pixel. Valid for RGB format only. */
		void GammaCorrect()
		{
			if (this->x >= 0.018) this->x = (1.099 * pow(this->x, 0.45)) - 0.099;
        	else this->x *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;
        	if (this->y >= 0.018) this->y = (1.099 * pow(this->y, 0.45)) - 0.099;
        	else this->y *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;
        	if (this->z >= 0.018) this->z = (1.099 * pow(this->z, 0.45)) - 0.099;
        	else this->z *= ((1.099 * pow(0.018, 0.45)) - 0.099) / 0.018;
		}

        /* Converts the pixel from XYZr format to RGB format. */
        void XYZToRGB()
        {
            /* Normalize the XYZ color. */
            float sum = this->x + this->y + this->z;
            if (sum >= EPSILON)
            {
                this->x /= sum;
                this->y /= sum;
                this->z /= sum;
            }

            /* Uses the CIE XYZ -> RGB forward conversion matrix. */
            float R = +2.2878400 * x - 0.8333680 * y - 0.454471 * z;
            float G = -0.5116510 * x + 1.4227600 * y + 0.088893 * z;
            float B = +0.0057204 * x - 0.0159068 * y + 1.010190 * z;

            /* Constrain the new RGB color within the RGB gamut. */
            float w = std::min(0.0f, std::min(R, std::min(G, B)));
            R -= w; G -= w; B -= w;
            
            /* Multiply by radiance. */
            this->x = R * this->r; 
            this->y = G * this->r; 
            this->z = B * this->r;
            this->r = 0.0f;
        }

        /* This will tonemap the pixel based on a log-average luminance and an
         * exposure key. This is vaid for RGBr format only. */
        void Tonemap(float logAvg, float exposure)
        {
            float ky = exposure / logAvg;
            float mu = ky / (1.0f + this->Luminance() * ky);
            this->x *= mu;
            this->y *= mu;
            this->z *= mu;
        }

        /* This will convert the pixel's RGB values into byte range. */
        void ByteRGB(int *r, int *g, int *b)
        {
            *r = (int)(std::min(std::max(this->x, 0.0f), 1.0f) * 255);
            *g = (int)(std::min(std::max(this->y, 0.0f), 1.0f) * 255);
            *b = (int)(std::min(std::max(this->z, 0.0f), 1.0f) * 255);
        }
};



/** @class DeviceParams
  * @brief Device-side engine params
  *
  * This kernel object uploads relevant engine parameters to the kernel, this
  * includes render width and height. This is required for the kernel to work.
  *
  * It is not active, since it uploads constant data.
  *
  * This kernel object handles no queries.
**/
class DeviceParams : public KernelObject
{
    private:
        cl::Buffer buffer;

    public:
        DeviceParams(const EngineParams& params) : KernelObject(params) { }
        ~DeviceParams() { }

        bool IsActive();
        void Initialize();
        void Bind(cl::Kernel kernel, cl_uint slot);
        void Update(size_t index);
        void* Query(size_t query);
};

/** @class PixelBuffer
  * @brief Device-side pixel buffer
  *
  * This kernel object is responsible for managing a pixel buffer, which is a
  * raster array of pixels which the kernel can read from and write to. This
  * kernel object is passive, given that it only performs work during its
  * initialization and cleanup (to save the final pixel buffer to file).
  *
  * This kernel object handles no queries.
**/
class PixelBuffer : public KernelObject
{
    private:
        size_t width, height, size;
        cl::Buffer buffer;
        Pixel *pixels;
        size_t index;

        void Acquire(const EngineParams& params);
        void Upload(const EngineParams& params);

        void ConvertToRGB();
        void Tonemap();
        void GammaCorrect();

        void WriteToFile(std::string path);
    public:
        PixelBuffer(const EngineParams& params) : KernelObject(params) { }
        ~PixelBuffer();

        bool IsActive();
        void Initialize();
        void Bind(cl::Kernel kernel, cl_uint slot);
        void Update(size_t index);
        void* Query(size_t query);
};

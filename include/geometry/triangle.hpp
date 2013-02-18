#pragma once

#include <common/common.hpp>

/* This is the OpenCL triangle structure which will be sent to the device. It *
 * does not contain as much information as the host-side implementation since *
 * things like the triangle's centroid, or bounding box are not necessary for *
 * rendering, but only for building the bounding volume hierarchy, host-side. */
struct cl_triangle
{
    cl_float4 p; /* The main triangle vertex. */
    cl_float4 x; /* The "left" triangle edge. */
    cl_float4 y; /* The other triangle edge.  */
    cl_float4 n; /* The triangle's normal.    */
	cl_int mat;
};

class Triangle
{
    public:
        /* The vertices... */
        Vector p1, p2, p3;
		int material;

        /* Precomputed... */
        Vector x, y, n, c;

        /* Creates the triangle from three points. */
        Triangle(Vector p1, Vector p2, Vector p3, int material);

        /* Outputs the equivalent OpenCL triangle structure. */
        void CL(cl_triangle *out);
};

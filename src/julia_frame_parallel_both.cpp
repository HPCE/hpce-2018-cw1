#include "julia.hpp"

void juliaFrameRender_ParallelBoth(
        unsigned width,     //! Number of pixels across
        unsigned height,    //! Number of rows of pixels
        complex_t c,        //! Constant to use in z=z^2+c calculation
        unsigned maxIter,   //! When to give up on a pixel
        unsigned *pDest     //! Array of width*height pixels, with pixel (x,y) at pDest[width*y+x]
    )
{
    fprintf(stderr, "No implementation of parallel both!\n");
    exit(1);
}

#ifndef julia_hpp
#define julia_hpp

#include <complex>

//! We will use single-precision complex numbers
typedef std::complex<float> complex_t;


//! Perform a julia iteration starting at the point z_0, for offset c.
/*! The overall calculation is:

     z_{i+1} = z_{i}^2 + c

 The point has "escaped" when |z_{i}| > 2.
*/
unsigned juliaPixelIteration(
    complex_t z_0,      //! Location within the fractal (maps to a pixel location)
    complex_t c,        //! c Offset of the overall fractal
    unsigned maxIter    //! maxIter The number of times to iterate before stopping
);

//! This is an "abstract" rendering function. It allows us to easily switch rendering engine.
typedef void (*julia_frame_render_proc_t)(
        unsigned width,     //! Number of pixels across
        unsigned height,    //! Number of rows of pixels
        complex_t c,        //! Constant to use in z=z^2+c calculation
        unsigned maxIter,   //! When to give up on a pixel
        unsigned *pDest     //! Array of width*height pixels, with pixel (x,y) at pDest[width*y+x]
);

//! Reference implementation for sequential rendering
void juliaFrameRender_Reference(
        unsigned width,     //! Number of pixels across
        unsigned height,    //! Number of rows of pixels
        complex_t c,        //! Constant to use in z=z^2+c calculation
        unsigned maxIter,   //! When to give up on a pixel
        unsigned *pDest     //! Array of width*height pixels, with pixel (x,y) at pDest[width*y+x]
    );

//! Parallel renderer using TBB on the inner loop
void juliaFrameRender_ParallelInner(
        unsigned width,     //! Number of pixels across
        unsigned height,    //! Number of rows of pixels
        complex_t c,        //! Constant to use in z=z^2+c calculation
        unsigned maxIter,   //! When to give up on a pixel
        unsigned *pDest     //! Array of width*height pixels, with pixel (x,y) at pDest[width*y+x]
    );
    
//! Parallel renderer using TBB on the outer loop
void juliaFrameRender_ParallelOuter(
        unsigned width,     //! Number of pixels across
        unsigned height,    //! Number of rows of pixels
        complex_t c,        //! Constant to use in z=z^2+c calculation
        unsigned maxIter,   //! When to give up on a pixel
        unsigned *pDest     //! Array of width*height pixels, with pixel (x,y) at pDest[width*y+x]
    );


//! Parallel renderer using TBB on the outer loop
void juliaFrameRender_ParallelBoth(
        unsigned width,     //! Number of pixels across
        unsigned height,    //! Number of rows of pixels
        complex_t c,        //! Constant to use in z=z^2+c calculation
        unsigned maxIter,   //! When to give up on a pixel
        unsigned *pDest     //! Array of width*height pixels, with pixel (x,y) at pDest[width*y+x]
    );


#endif

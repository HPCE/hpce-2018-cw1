#include "julia.hpp"

void juliaFrameRender_Reference(
        unsigned width,     //! Number of pixels across
        unsigned height,    //! Number of rows of pixels
        complex_t c,        //! Constant to use in z=z^2+c calculation
        unsigned maxIter,   //! When to give up on a pixel
        unsigned *pDest     //! Array of width*height pixels, with pixel (x,y) at pDest[width*y+x]
    )
{
    float dx=3.0f/width, dy=3.0f/height;

    for(unsigned y=0; y<height; y++){
        for(unsigned x=0; x<width; x++){
            // Map pixel to z_0
            complex_t z(-1.5f+x*dx, -1.5f+y*dy);

            //Perform a julia iteration starting at the point z_0, for offset c.
            //   z_{i+1} = z_{i}^2 + c
            // The point escapes for the first i where |z_{i}| > 2.

            unsigned iter=0;
            while(iter<maxIter){
                if(abs(z) > 2){
                    break;
                }
                z = z*z + c;
                ++iter;
            }
            pDest[y*width+x] = iter;
        }
    }
}

#include "julia.hpp"

#include <vector>
#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <utility>
#include <string.h>

#include <chrono>
#include <thread>

#if !(defined(_WIN32) || defined(_WIN64))
#include <unistd.h>
void set_binary_io()
{}
#else
// http://stackoverflow.com/questions/341817/is-there-a-replacement-for-unistd-h-for-windows-visual-c
// http://stackoverflow.com/questions/13198627/using-file-descriptors-in-visual-studio-2010-and-windows

#include <io.h>
#include <fcntl.h>

#define read _read
#define write _write
#define STDIN_FILENO 0
#define STDOUT_FILENO 1

void set_binary_io()
{
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
}
#endif


const unsigned greyLevels=9; // Ignore blank
const char *greyChars=".:-=+*#%@";

char toGrey(unsigned v, unsigned maxIter)
{
    if(v>=maxIter){
        return ' ';
    }else{
        return greyChars[v%greyLevels];
    }
}


complex_t chooseC(float t)
{
    return 0.9f*exp(-float(t)*0.2f*complex_t(0,1));
}

void RenderNonProgressive(julia_frame_render_proc_t julia, unsigned width, unsigned height, complex_t z, unsigned maxIter)
{
    std::vector<unsigned> buffer(width*height,UINT_MAX);

    julia(width, height, z, maxIter, &buffer[0]);

    std::stringstream acc;

    for(unsigned y=0;y<height;y++){
        for(unsigned x=0;x<width;x++){
            acc<<toGrey(buffer[y*width+x], maxIter);
        }
        acc<<"\n";
    }
    std::cout<<acc.str();
    std::cout.flush();
}

void RenderProgressive(julia_frame_render_proc_t julia, unsigned width, unsigned height, complex_t z, unsigned maxIter)
{
    std::vector<unsigned> buffer(width*height,UINT_MAX);

    std::thread render([&](){
        julia(width, height, z, maxIter, &buffer[0]);
    });

    for(unsigned y=0;y<height;y++){
        for(unsigned x=0;x<width;x++){
            while(buffer[y*width+x]==UINT_MAX){
                std::cout.flush();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            std::cout<<toGrey(buffer[y*width+x], maxIter);
        }
        std::cout<<std::endl;
    }
    std::cout.flush();

    render.join();
}

void RenderAnimation(julia_frame_render_proc_t julia, unsigned width, unsigned height, unsigned maxIter, unsigned fps, unsigned maxFrames)
{
    auto start = std::chrono::system_clock::now();

    double dt= fps ? 1.0/fps : 1.0/25;

    unsigned frames=0;
    double t=0.0;
    while(maxFrames==0 || (frames<maxFrames)){
        auto now = std::chrono::system_clock::now();
        t= fps ? std::chrono::duration<double>(now-start).count() : t+dt;

        complex_t c=chooseC(t);

        RenderNonProgressive(julia, width, height, c, maxIter);

        if(fps){
            std::this_thread::sleep_until(now+std::chrono::milliseconds((int)(dt)));
        }

        frames++;
    }
}


void indexToRGB32(uint8_t *rgb, unsigned index, unsigned maxIter)
{
    rgb[0]=0;
    rgb[1]=0;
    rgb[2]=0;
    rgb[3]=0;

    if(index>=maxIter)
        return;

    index=index%12;
    unsigned color=index%3;
    unsigned level=index/3;

    rgb[color]=level*64+63; // map to 63, 127, 191, 254
}

void RenderVideo(julia_frame_render_proc_t julia, unsigned width, unsigned height, unsigned maxIter, unsigned fps, unsigned maxFrames)
{
    set_binary_io();

    auto start = std::chrono::system_clock::now();

    double dt= fps==0 ? 1.0/25 : 1.0/fps;

    std::vector<unsigned> buffer(width*height,UINT_MAX);
    std::vector<uint8_t> pixels(width*height*4);

    unsigned frames=0;
    double t=0.0;
    while(maxFrames==0 || (frames<maxFrames)){
        auto now = std::chrono::system_clock::now();
        t= fps ? std::chrono::duration<double>(now-start).count() : t+dt;

        complex_t c=chooseC(t);

        julia(width, height, c, maxIter, &buffer[0]);

        for(unsigned i=0; i<width*height; i++){
            indexToRGB32(&pixels[4*i], buffer[i], maxIter);
        }

        int todo=pixels.size();
        int done=0;
        while(todo>0){
            int written=write(STDOUT_FILENO, &pixels[done],todo);
            if(written<=0){
                fprintf(stderr, "Failed to write all pixels.\n");
                exit(1);
            }
            todo-=written;
            done+=written;
        }

        fprintf(stderr, "  Done %d\n", frames);

        if(fps){
            std::this_thread::sleep_until(now+std::chrono::milliseconds((int)(dt)));
        }

        frames++;
    }
}

void printUsage()
{
    std::cout<<"pipe_driver [options]\n";
    std::cout<<"  -progressive            : Show characters as that are rendered\n";
    std::cout<<"  -non-progressive        : Render frame once complete\n";
    std::cout<<"  -animation              : Simulate animated text\n";
    std::cout<<"  -video                 : Send raw pixels down stdout (Warning!)\n";
    std::cout<<"  -engine engineName      : Name of engine to use\n";
    std::cout<<"  -width width            : Width in characters or pixels\n";
    std::cout<<"  -height height          : Height in characters or pixels\n";
    std::cout<<"  -max-iter maxIterations : How deep into image to go\n";
    std::cout<<"  -fps framesPerSecond    : Target frame rate\n";
    std::cout<<"  -max-frames n           : Do this many frames then quit.\n";
    std::cout<<"\n";
    std::cout<<"Engines:\n";
    std::cout<<"  reference\n";
    std::cout<<"  parallel_inner\n";
    std::cout<<"  parallel_outer\n";
    std::cout<<"  parallel_both\n";
}

int main(int argc, char *argv[])
{
    auto juliaEngine=juliaFrameRender_Reference;

    unsigned width=60, height=30;
    complex_t z(-0.505f,0.505f);
    unsigned maxIter=100000;
    unsigned maxFrames=0;

    unsigned fps=0;

    enum{
        Progressive,
        NonProgressive,
        Animation,
        Video
    } mode = Progressive;

    int ai=1;
    while(ai<argc){
        if(!strcmp(argv[ai], "-help")){
            printUsage();
            exit(0);
        }else if(!strcmp(argv[ai],"-progressive")){
            mode=Progressive;
            ai++;
        }else if(!strcmp(argv[ai],"-non-progressive")){
            mode=NonProgressive;
            ai++;
        }else if(!strcmp(argv[ai],"-animation")){
            mode=Animation;
            ai++;
        }else if(!strcmp(argv[ai],"-video")){
            mode=Video;
            ai++;
        }else if(!strcmp(argv[ai],"-width")){
            if(ai+1 >= argc){
                fprintf(stderr, "Missing argument to width.");
                exit(1);
            }
            width=atoi(argv[ai+1]);
            ai+=2;
        }else if(!strcmp(argv[ai],"-height")){
            if(ai+1 >= argc){
                fprintf(stderr, "Missing argument to height.");
                exit(1);
            }
            height=atoi(argv[ai+1]);
            ai+=2;
        }else if(!strcmp(argv[ai],"-max-iter")){
            if(ai+1 >= argc){
                fprintf(stderr, "Missing argument to max-iter.");
                exit(1);
            }
            maxIter=atoi(argv[ai+1]);
            ai+=2;
        }else if(!strcmp(argv[ai],"-max-frames")){
            if(ai+1 >= argc){
                fprintf(stderr, "Missing argument to max-frames.");
                exit(1);
            }
            maxFrames=atoi(argv[ai+1]);
            ai+=2;
        }else if(!strcmp(argv[ai],"-engine")){
            if(ai+1 >= argc){
                fprintf(stderr, "Missing argument to engine.");
                exit(1);
            }
            if(!strcmp(argv[ai+1], "reference")){
                juliaEngine=juliaFrameRender_Reference;
            }else if(!strcmp(argv[ai+1], "parallel_inner")){
                juliaEngine=juliaFrameRender_ParallelInner;
            }else if(!strcmp(argv[ai+1], "parallel_outer")){
                juliaEngine=juliaFrameRender_ParallelOuter;
                }else if(!strcmp(argv[ai+1], "parallel_both")){
                juliaEngine=juliaFrameRender_ParallelBoth;
            }else{
                fprintf(stderr, "Uknown argument to -engine : '%s'\n", argv[ai+1]);
                exit(1);
            }
            ai+=2;
        }else if(!strcmp(argv[ai],"-fps")){
            if(ai+1 >= argc){
                fprintf(stderr, "Missing argument to fps.");
                exit(1);
            }
            fps=atoi(argv[ai+1]);
            ai+=2;
        }else{
            fprintf(stderr, "Unknown option %s\n", argv[ai]);
            exit(1);
        }
    }

    if(mode==Progressive){
        RenderProgressive(juliaEngine, width, height, z, maxIter);
    }else if(mode==NonProgressive){
        RenderNonProgressive(juliaEngine, width, height, z, maxIter);
    }else if(mode==Animation){
        RenderAnimation(juliaEngine, width, height, maxIter, fps, maxFrames);
    }else if(mode==Video){
        RenderVideo(juliaEngine, width, height, maxIter, fps, maxFrames);
    }else{
        fprintf(stderr, "Error, no mode selected.\n");
    }

    return 0;
}


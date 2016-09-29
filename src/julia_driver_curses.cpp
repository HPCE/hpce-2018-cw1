#include "julia.hpp"

#include <vector>
#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <utility>
#include <numeric>

#include <chrono>
#include <thread>

#include <curses.h>

const unsigned greyLevels=9; // Ignore blank
const char *greyChars=".:-=+*#%@";

char toGrey(unsigned v)
{
    if(v==UINT_MAX){
        return ' ';
    }else{
        return greyChars[v%greyLevels];
    }
}


void RenderCurses()
{
    WINDOW *win=initscr();
    try{
        cbreak(); // character oriented input
        noecho(); // Don't show characters
        nodelay(win, true); // don't wait for a character
        keypad(win, true); // we want arrow keys
        
        unsigned width, height;
        
        unsigned numStatus=2;
        
        struct {
            julia_frame_render_proc_t engine;
            const char *name;
        } engines [] = {
            { juliaFrameRender_Reference, "Reference" },
            { juliaFrameRender_Parallel, "Parallel" }
        };
        unsigned engineCount=sizeof(engines)/sizeof(engines[0]);
        unsigned engineIndex=0;
        unsigned maxIter=64;
        
        int color=has_colors();
        if(color){
            start_color();
            init_pair(1, COLOR_RED, COLOR_BLACK);
            init_pair(2, COLOR_GREEN, COLOR_BLACK);
            init_pair(3, COLOR_YELLOW, COLOR_BLACK);
            init_pair(4, COLOR_BLUE, COLOR_BLACK);
            init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
            init_pair(6, COLOR_CYAN, COLOR_BLACK);
            init_pair(7, COLOR_WHITE, COLOR_BLACK);
            
            init_pair(8, COLOR_BLACK, COLOR_WHITE);
        }
        
        
        auto start = std::chrono::system_clock::now();
        
        double targetFPS=10;
        
        std::vector<double> times;
        
        while(1){
                
            getmaxyx(win, height, width);
            
            while(1){
                int ch=getch();
                if(ch==ERR)
                    break;
                switch(ch){
                case KEY_UP:
                case 'w':
                    engineIndex=(engineIndex+engineCount-1)%engineCount;
                    break;
                case KEY_DOWN:
                case 's':
                    engineIndex=(engineIndex+1)%engineCount;
                    break;
                case KEY_LEFT:
                case 'a':
                    maxIter=std::max(2u, maxIter*3/4);
                    break;
                case KEY_RIGHT:
                case 'd':
                    maxIter=std::max(3u, maxIter*4/3);
                    break;
                default:
                    break;
                }
            }
            
            auto engine=engines[engineIndex].engine;
            
            std::vector<unsigned> buffer(width*(height-numStatus),UINT_MAX);
            
            auto now = std::chrono::system_clock::now();
            float t=std::chrono::duration<float>(now-start).count();
            
            float scale=1.0f-(cos(t*0.1f)+1)/2.5;
            complex_t c=scale*exp(-t*0.2f*complex_t(0,1));
            
            auto renderBegin = std::chrono::high_resolution_clock::now();
            engine(width, height-numStatus, c, maxIter, &buffer[0]);
            auto renderEnd = std::chrono::high_resolution_clock::now();
            double renderTime=std::chrono::duration<double>(renderEnd-renderBegin).count();
            
            times.push_back(renderTime);
            while(times.size()>5){
                times.erase(times.begin());
            }
            
            clear();
            for(unsigned y=0; y<height-numStatus; y++){
                move(y+numStatus,0);
                for(unsigned x=0; x<width; x++){
                    if(color){
                        if(buffer[y*width+x]==maxIter){
                            addch(' ');
                        }else{
                            int lev=buffer[y*width+x]%14;
                            addch('+' | COLOR_PAIR(1+(lev%7)) | (lev>7 ? A_BOLD : 0));
                        }
                    }else{
                        if(buffer[y*width+x]==maxIter){
                            addch(' ');
                        }else{
                            addch(toGrey(buffer[y*width+x]) | A_NORMAL);
                        }
                    }
                }
            }
            
            int cx=(int)(width*(c.real()+1.0f)/2.0f);
            int cy=(int)(height*(c.imag()+1.0f)/2.0f);
            
            move(cy+numStatus,cx);
            addch('0' | COLOR_PAIR(8) | A_BOLD);
                   
            move(0,0);
            printw("Engine = %s, maxIter = %d", engines[engineIndex].name, maxIter);
            
            assert(times.size()>0);
            double avgTime=std::accumulate(times.begin(),times.end(),0.0)/times.size();
            double maxTime=*std::max_element(times.begin(), times.end());
            move(1,0);
            printw("Last %d frames : avgTime = %.6f, maxTime=%.6f, lasTime=%.6f", times.size(), avgTime, maxTime, times.back());
            
            refresh();
            
            std::this_thread::sleep_until(now+std::chrono::milliseconds((int)(1000.0/targetFPS)));
        }
    }catch(...){
        endwin();
        throw;
    }
}

int main()
{
    RenderCurses();
    
    return 0;
}


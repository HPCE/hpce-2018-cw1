#ifndef two_thread_for_hpp
#define two_thread_for_hpp

#include <thread>

//! Applies a function over a 1D iteration space using two threads
/*! Given a function f and a range [begin,end), this will
    call f(i) for begin<=i<end
*/
template<class T>
void two_thread_for(
    unsigned begin,     //! Beginning of range (inclusive)
    unsigned end,       //! End of range (exclusive)
    const T &f          //! Any function or object where `f(i)` is valid
){
    // Split the range into [begin,mid), and [mid,end)
    unsigned mid = (begin+end)/2;

    // This is a function that does the first range
    auto top = [](){
        for(unsigned i=begin; i<mid; i++){
            f(i);
        }
    };

    // This will do the second range
    auto bottom = [](){
        for(unsigned i=begin; i<mid; i++){
            f(i);
        }
    };

    // at this point neither top nor bottom have executed yet

    // Start top on a different thread
    std::thread topThread(top);

    // Run bottom on this thread
    bottom();

    // Make sure top has finished before we return
    topThread.join();
}

#endif

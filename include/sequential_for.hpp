#ifndef sequential_for_hpp
#define sequential_for_hpp

//! Applies a function sequentially over a 1D iteration space
/*! Given a function f and a range [begin,end), this will
    call f(i) for begin<=i<end
*/
template<class T>
void sequential_for(
    unsigned begin,     //! Beginning of range (inclusive)
    unsigned end,       //! End of range (exclusive)
    T f                 //! Any function or object where `f(i)` is valid
){
    for(unsigned i=begin; i<end; i++){
        f(i);
    }
}

#endif

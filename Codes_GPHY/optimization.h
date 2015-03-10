#ifndef OPTIMIZATION_H
    #define	OPTIMIZATION_H

    #ifdef __GNUC__
        #define likely(expr)    __builtin_expect((expr), !0)
        #define unlikely(expr)  __builtin_expect((expr), 0)
    #else
        #define likely(expr) (expr)
        #define unlikely(expr) (expr)
    #endif

    #define _GLIBCXX_PARALLEL
    #ifdef _GLIBCXX_ALGORITHM
        #include <parallel/algorithm>
    #endif


#endif	/* OPTIMIZATION_H */


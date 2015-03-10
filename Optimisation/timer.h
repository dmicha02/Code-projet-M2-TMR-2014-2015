#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <opencv2/core/core.hpp>
#include <stdio.h>

#ifndef timer__             
#define timer__(data)   for (int64 blockTime = -1; (blockTime == -1 ? (blockTime = getTickCount()) != -1 : false); printf(data " : %.3fms\n", /*(double)*/ 100*((getTickCount() - blockTime) / getTickFrequency())), fflush(stdout))
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */


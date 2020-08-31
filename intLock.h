#ifndef _INTLOCK_H_
#define _INTLOCK_H_

#include "Thread.h"

// mozda ipak ne ovako ?????
extern int mutex_glb;
void mutex_glb_wait();

#define intLock mutex_glb_wait();
#define intUnlock mutex_glb = 1;

#endif

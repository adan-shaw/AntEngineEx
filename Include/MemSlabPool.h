#ifndef APP_MEMSLAB_H
#define APP_MEMSLAB_H

#include "Nocopy.h"
#include "Spinlock.h"

namespace app {

struct MemPage {
    usz mSlab;
    MemPage* mNext;
    usz mPrev;
};


struct MemStat {
    usz mTotal;
    usz mUsed;
    usz mRequests;
    usz mFails;
};

//鍘熷
#include "CheckSum.h"
namespace app {


inline u32 AppCheckOverflow(u32 it) {
    it = (it >> 16) + (it & 0xFFFF);
    /*if(it > 0xFFFF) {
        it -= 0xFFFF;
    }
    return it;
    */
    return (it >> 16) + (it & 0xFFFF);
}


void CheckSum::add(const void* iData, s32 iSize) {
    if (iSize <= 0) {
        return;
    }
    register const u16* buffer = (const u16*)iData;
    s32 byte_swapped = 0;
    register s32 mlen = 0;

    //leftover byte of last buffer
    if (mHaveTail) {
        *(((u8*)&mLeftover) + 1) = *(const u8*)buffer;
        mSum += mLeftover;
        mHaveTail = false;
        buffer = (const u16*)(((const u8*)buffer) + 1);
        mlen = iSize - 1;
    } else {
        mlen = iSize;
    }
    /*
    鎸囬拡闈炲唴瀛樺
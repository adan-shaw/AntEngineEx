/***************************************************************************************************
 * MIT License
 *
 * Copyright (c) 2021 antmuse@live.cn/antmuse@qq.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
***************************************************************************************************/


/**
*@file CheckSum.h
*@brief CheckSum generates and validates checksums.
*/

#ifndef APP_CHECKSUM_H
#define APP_CHECKSUM_H

#include "Config.h"


namespace app {

/**
* @brief Checksum for TCP/IP head.
* Checksum routine for Internet Protocol family headers (Portable Version).
*
* This routine is very heavily used in the network
* code and should be modified for each CPU to be as fast as possible.
*/
class CheckSum {
public:
    CheckSum() :
        mHaveTail(false),
        mSum(0),
        mLeftover(0) {
    }

    CheckSum(const CheckSum& other) :
        mHaveTail(other.mHaveTail),
        mSum(other.mSum),
        mLeftover(other.mLeftover) {
    }

    CheckSum& operator=(const CheckSum& other) {
        mSum = other.mSum;
        mLeftover = other.mLeftover;
        mHaveTail = other.mHaveTail;
        return *this;
    }

    /**
    * @brief Clear this checksum and reuse it.
    */
    void clear() {
        mSum = 0;
        mLeftover = 0;
        mHaveTail = false;
    }

    /**
    * @brief Append buffer into this checksum.
    * @param buffer The buffer.
    * @param iSize The buffer length.
    */
    void add(const void* buffer, s32 iSize);

    /**
    * @brief Get result, and still can append buffers into this checksum in future.
    * @return The result.
    */
    u16 get()const;

    /**
    * @brief Get result, and should not append buffers into this checksum in future.
    * @return The result.
    */
    u16 finish();

private:
    u32 mSum;
    mutable u16 mLeftover;
    bool  mHaveTail;
};

}//namespace app
#endif //APP_CHECKSUM_H

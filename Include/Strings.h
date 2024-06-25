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


#ifndef APP_STRINGS_H
#define	APP_STRINGS_H

#include <string.h>
#include <string>
#include "StrConverter.h"
#include "TAllocator.h"


namespace app {

DFINLINE s16 App2Char2S16(const s8* it) {
    return *reinterpret_cast<const s16*>(it);
}

DFINLINE s32 App4Char2S32(const s8* it) {
    return *reinterpret_cast<const s32*>(it);
}

template <typename T>
DFINLINE static T App2Lower(T x) {
    return x >= 'A' && x <= 'Z' ? x | 32 : x;
}

template <typename T>
DFINLINE static T App2Upper(T x) {
    return x >= 'a' && x <= 'z' ? x & (~32) : x;
}

template <typename T>
static void AppStr2Upper(T* str, usz len) {
    for (; len > 0; --len) {
        if (*(str) >= 'a' && *(str) <= 'z') {
            *(str) &= (~32);
        }
        ++str;
    }
}

template <typename T>
static void AppStr2Lower(T* str, usz len) {
    for (; len > 0; --len) {
        if (*(str) >= 'A' && *(str) <= 'Z') {
            *(str) |= 32;
        }
        ++str;
    }
}


/**
* @brief 瀛楃
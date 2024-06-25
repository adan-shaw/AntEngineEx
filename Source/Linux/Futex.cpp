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

#include "Futex.h"

#if defined(DOS_ANDROID) || defined(DOS_LINUX)
#include <stdlib.h>
#include <stdio.h>
#include <linux/futex.h>
#include <sys/time.h>    //for struct timespec
#include <sys/syscall.h> //for SYS_futex
#include <unistd.h>      //for syscall
#include <errno.h>

namespace app {

static s32 futex(s32* uaddr, s32 futex_op, s32 val, const struct timespec* timeout, s32* uaddr2, s32 val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}


Futex::Futex() : mValue(0) {
}

Futex::~Futex() {
}

void Futex::lock() {
    s32 val = 0;
    while (!mValue.compare_exchange_strong(val, 1)) {
        s32 ret = futex(reinterpret_cast<s32*>(&mValue), FUTEX_WAIT, 1, NULL, NULL, 0);
        // 鍒ゆ柇琚
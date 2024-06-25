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


#ifndef APP_LOGGER_H
#define	APP_LOGGER_H

#include <stdarg.h>
#include <mutex>
#include "TVector.h"
#include "Strings.h"

#if defined(DOS_WINDOWS)
#define DBFILE(N) (strrchr(N,'\\')?strrchr(N,'\\')+1:N)
#define DLOG(LV, FMT, ...) Logger::log(LV, "%s:%d " FMT, DBFILE(__FILE__), __LINE__, ##__VA_ARGS__)
#else
#define DLOG(LV, FMT, ...) Logger::log(LV, __BASE_FILE__ ":%d, " FMT, __LINE__, ##__VA_ARGS__)
#endif

#if defined(DDEBUG)
#define DDLOG(LV, FMT, ...) Logger::log(LV, FMT, ##__VA_ARGS__)
#else
#define DDLOG(LV, FMT, ...)
#endif



namespace app {

//error code of app
enum EErrorCode {
    EE_OK = 0,
    EE_NO_OPEN = -1,         //鏈
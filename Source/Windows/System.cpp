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


#include "System.h"
#if defined(DOS_WINDOWS)
#include <winsock2.h>
#include <io.h>
#include <tchar.h>
#include "Logger.h"
#include "Windows/WinAPI.h"
#include "Net/Socket.h"

namespace app {

class FileFinder {
public:
    FileFinder() : mHandle(-1) {
        mFormat[0] = 0;
    }

    ~FileFinder() {
        close();
    }

    _tfinddata64_t* open(const tchar* fmt) {
        close();
        _tcscpy_s(mFormat, fmt);
        mHandle = _tfindfirst64(mFormat, &mDat);
        return mHandle != -1 ? &mDat : nullptr;
    }

    _tfinddata64_t* next() {
        return (mHandle != -1 && 0 == _tfindnext64(mHandle, &mDat)) ? &mDat : nullptr;
    }

    void close() {
        if (mHandle != -1) {
            _findclose(mHandle);
            mHandle = -1;
        }
    }

    const tchar* getFormat() const {
        return mFormat;
    }

private:
    intptr_t mHandle;
    _tfinddata64_t mDat;
    tchar mFormat[_MAX_PATH];
};


//@param disk NULL琛ㄧず绋嬪簭褰撳墠纾佺洏
static u32 AppGetDiskSectorSize(const tchar* disk = nullptr /*DSTR("C:\\")*/) {
    DWORD sectorsPerCluster;     // 纾佺洏涓€涓
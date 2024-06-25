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


#include "Net/HTTP/HttpMsg.h"
#include "Logger.h"
#include "Net/HTTP/HttpLayer.h"

namespace app {
namespace net {
static const s32 G_MAX_BODY = 1024 * 1024 * 10;

HttpMsg::HttpMsg(HttpLayer* it) :
    mStationID(0),
    mRespStatus(0),
    mLayer(it),
    mEvent(nullptr),
    mFlags(0),
    mMethod(HTTP_GET),
    mType(EHTTP_BOTH),
    mStatusCode(HTTP_STATUS_OK){
    //mBrief.setLen(0);
    mCacheIn.init();
    mCacheOut.init();
    if (it) {
        it->grab();
    }
}


HttpMsg::~HttpMsg() {
    setEvent(nullptr);
    if (mLayer) {
        mLayer->drop();
        mLayer = nullptr;
    }
}


void HttpMsg::writeStatus(s32 val, const s8* str) {
    s8 tmp[64]; //len=17="HTTP/1.1 200 OK\r\n"
    mStatusCode = val;
    usz tsz = snprintf(tmp, sizeof(tmp), "HTTP/1.1 %d %s\r\n", val, str);
    if (tsz >= sizeof(tmp)) {
        tsz = sizeof(tmp);
        tmp[tsz - 2] = '\r';
        tmp[tsz - 1] = '\n';
    }
    mCacheOut.write(tmp, static_cast<s32>(tsz));
}


void HttpMsg::writeOutBody(const void* buf, usz bsz) {
    mCacheOut.write(buf, bsz);
}


void HttpMsg::writeOutHead(const s8* name, const s8* value) {
    if (name && value) {
        usz klen = strlen(name);
        usz vlen = strlen(value);
        if (klen > 0) {
            mCacheOut.write(name, klen);
            mCacheOut.write(":", 1);
            mCacheOut.write(value, vlen);
            mCacheOut.write("\r\n", 2);
        }
    }
}


const StringView HttpMsg::getMimeType(const s8* filename, usz iLen) {
    DASSERT(filename && iLen > 0);
    /*
    *鍩轰簬鏂囦欢鍚庣紑鐨刴ime type琛ㄣ€
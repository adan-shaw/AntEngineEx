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


#include "Net/HTTP/HttpLayer.h"
#include "Net/HTTP/Website.h"
#include "Net/Acceptor.h"
#include "Loop.h"
#include "Timer.h"

namespace app {
namespace net {


HttpLayer::HttpLayer(EHttpParserType tp) :
    mPType(tp),
    mWebsite(nullptr),
    mMsg(nullptr),
    mHttpError(HPE_OK),
    mHTTPS(true) {
    clear();
}


HttpLayer::~HttpLayer() {
    //onClose();
}


s32 HttpLayer::get(const String& gurl) {
    mMsg->clear();
    if (EE_OK != mMsg->writeGet(gurl)) {
        return EE_ERROR;
    }

    mHTTPS = mMsg->getURL().isHttps();
    if (!mHTTPS) {
        mTCP.getHandleTCP().setClose(EHT_TCP_LINK, HttpLayer::funcOnClose, this);
        mTCP.getHandleTCP().setTime(HttpLayer::funcOnTime, 20 * 1000, 30 * 1000, -1);
    } else {
        mTCP.setClose(EHT_TCP_LINK, HttpLayer::funcOnClose, this);
        mTCP.setTime(HttpLayer::funcOnTime, 20 * 1000, 30 * 1000, -1);
    }

    StringView host = mMsg->getURL().getHost();
    NetAddress addr(mMsg->getURL().getPort());
    s8 shost[256];  //255, Maximum host name defined in RFC 1035
    snprintf(shost, sizeof(shost), "%.*s", (s32)(host.mLen), host.mData);
    addr.setDomain(shost);

    RequestFD* nd = RequestFD::newRequest(4 * 1024);
    nd->mUser = this;
    nd->mCall = HttpLayer::funcOnConnect;
    s32 ret = mHTTPS ? mTCP.open(addr, nd) : mTCP.getHandleTCP().open(addr, nd);
    if (EE_OK != ret) {
        RequestFD::delRequest(nd);
        return ret;
    }
    return EE_OK;
}

s32 HttpLayer::post(const String& gurl) {
    return EE_ERROR;
}


void HttpLayer::msgBegin() {
    if (mMsg) {
        mHttpError = HPE_CB_MsgBegin;
        postClose();
        return; //error
    }

    mMsg = new HttpMsg(this);
    mMsg->mStationID = ES_INIT;
    msgStep();
}

void HttpLayer::msgEnd() {
    DASSERT(mMsg);
    if (mMsg) {
        mMsg->mStationID = ES_BODY_DONE;
        msgStep();
        mMsg->drop();
        mMsg = nullptr;
    }
}


void HttpLayer::msgPath() {
    DASSERT(mMsg);
    if (mMsg) {
        mMsg->mType = (EHttpParserType)mType;
        mMsg->mFlags = mFlags;
        mMsg->mStationID = ES_PATH;
        msgStep();
    }
}

s32 HttpLayer::headDone() {
    DASSERT(mMsg);
    if (mMsg) {
        mMsg->mFlags = mFlags;
        mMsg->mStationID = ES_HEAD;
        msgStep();
    }
    return 0;
}

void HttpLayer::chunkHeadDone() {
    DASSERT(mMsg);
    if (mMsg) {
        mMsg->mFlags = mFlags;
        mMsg->mStationID = ES_HEAD;
        msgStep();
    }
}

void HttpLayer::msgStep() {
    if (EE_OK != mWebsite->stepMsg(mMsg)) {
        postClose();
    }
}

void HttpLayer::msgBody() {
    if (mMsg) {
        mMsg->mStationID = ES_BODY;
        msgStep();
    }
}

void HttpLayer::msgError() {
    if (mMsg) {
        mMsg->mStationID = ES_ERROR;
        msgStep();
    }
}

void HttpLayer::chunkMsg() {
    if (mMsg) {
        mMsg->mStationID = ES_BODY;
        msgStep();
    }
}

void HttpLayer::chunkDone() {
    if (mMsg) {
        mMsg->mStationID = ES_BODY_DONE;
        mWebsite->stepMsg(mMsg);
        mMsg->drop();
        mMsg = nullptr;
    }
}


bool HttpLayer::sendReq() {
    RingBuffer& bufs = mMsg->getCacheOut();
    if (bufs.getSize() > 0) {
        RequestFD* nd = RequestFD::newRequest(0);
        nd->mUser = mMsg;
        nd->mCall = HttpLayer::funcOnWrite;
        StringView msg = bufs.peekHead();
        nd->mData = msg.mData;
        nd->mAllocated = (u32)msg.mLen;
        nd->mUsed = (u32)msg.mLen;
        s32 ret = writeIF(nd);
        if (0 != ret) {
            RequestFD::delRequest(nd);
            return false;
        }
    }
    return true;
}

bool HttpLayer::sendResp(HttpMsg* msg) {
    DASSERT(msg);

    if (msg->getRespStatus() > 0) {
        return true;
    }

    RingBuffer& bufs = msg->getCacheOut();
    RequestFD* nd = RequestFD::newRequest(0);
    nd->mUser = msg;
    nd->mCall = HttpLayer::funcOnWrite;
    StringView pack = bufs.peekHead();
    nd->mData = pack.mData;
    nd->mAllocated = (u32)pack.mLen;
    nd->mUsed = (u32)pack.mLen;
    s32 ret = writeIF(nd);
    if (EE_OK != ret) {
        RequestFD::delRequest(nd);
        return false;
    }
    msg->grab();
    msg->setRespStatus(1);
    return true;
}



#ifdef DDEBUG
s32 TestHttpReceive(HttpLayer& mMsg) {
    usz tlen;
    usz used;

    /*const s8* str0 =
        "POST /fs/upload HTTP/1.1\r\n"
        "Con";

    tlen = strlen(str0);
    used = parseBuf(str0, tlen);

    const s8* str1 = "Content-Length: 8";
    tlen = strlen(str1);
    used = parseBuf(str1, tlen);
    const s8* str2 = "Content-Length: 8\r\n";

    tlen = strlen(str2);
    used = parseBuf(str2, tlen);
    const s8* str3 =
        "Connection: Keep-Alive\r\n"
        "\r\n"
        "abcd";

    tlen = strlen(str3);
    used = parseBuf(str3, tlen);
    const s8* str4 = "len5";

    tlen = strlen(str4);
    used = parseBuf(str4, tlen);*/

    const s8* test =
        "POST /fs/upload HTTP/1.1\r\n"
        "Content-Length: 18\r\n"
        "Content-Type: multipart/form-data; boundary=vksoun\r\n"
        "Connection: Keep-Alive\r\n"
        "Cookie: Mailbox=yyyy@qq.com\r\n"
        "\r\n"
        "--vksoun\r\n" //boundary
        "Content-Disposition: form-data; name=\"fieldName\"; filename=\"filename.txt\"\r\n"
        "\r\n"
        "msgPart1"
        "\r\n"
        "--vksoun\r\n" //boundary
        "Content-Type: text/plain\r\n"
        "\r\n"
        "--vksoun-"
        "\r\n\r\n"
        "--vksoun--"  //boundary tail
        ;
    tlen = strlen(test);
    used = mMsg.parseBuf(test, tlen);
    const s8* laststr = test + used;

    return 0;
}
#endif



s32 HttpLayer::onTimeout(HandleTime& it) {
    return shouldKeepAlive() ? EE_OK : EE_ERROR;
}


void HttpLayer::onClose(Handle* it) {
#ifdef DDEBUG
    if (mHTTPS) {
        DASSERT((&mTCP == it) && "HttpLayer::onClose https handle?");
    } else {
        DASSERT((&mTCP.getHandleTCP() == it) && "HttpLayer::onClose http handle?");
    }
#endif

    if (mMsg) {
        mMsg->mStationID = ES_CLOSE;
        mWebsite->stepMsg(mMsg);
        mMsg->drop();
        mMsg = nullptr;
    }
    if (mWebsite) {
        Website* site = mWebsite;
        mWebsite = nullptr;
        site->unbind(this);
    } else {
        drop();
        Logger::log(ELL_ERROR, "HttpLayer::onClose>>null website");
    }
}


bool HttpLayer::onLink(RequestFD* it) {
    DASSERT(nullptr == mWebsite);
    net::Acceptor* accp = (net::Acceptor*)(it->mUser);
    RequestAccept& req = *(RequestAccept*)it;
    mWebsite = reinterpret_cast<Website*>(accp->getUser());
    DASSERT(mWebsite);
    mHTTPS = 1 == mWebsite->getConfig().mType;
    if (mHTTPS) {
        mTCP.setClose(EHT_TCP_LINK, HttpLayer::funcOnClose, this);
        mTCP.setTime(HttpLayer::funcOnTime, 20 * 1000, 30 * 1000, -1);
    } else {
        mTCP.getHandleTCP().setClose(EHT_TCP_LINK, HttpLayer::funcOnClose, this);
        mTCP.getHandleTCP().setTime(HttpLayer::funcOnTime, 20 * 1000, 30 * 1000, -1);
    }
    RequestFD* nd = RequestFD::newRequest(4 * 1024);
    nd->mUser = this;
    nd->mCall = HttpLayer::funcOnRead;
    s32 ret = mHTTPS ? mTCP.open(req, nd, &mWebsite->getTlsContext()) : mTCP.getHandleTCP().open(req, nd);
    if (0 == ret) {
        mWebsite->bind(this);
        Logger::log(ELL_INFO, "HttpLayer::onLink>> [%s->%s]", mTCP.getRemote().getStr(), mTCP.getLocal().getStr());
    } else {
        mWebsite = nullptr;
        RequestFD::delRequest(nd);
        Logger::log(ELL_ERROR, "HttpLayer::onLink>> [%s->%s], ecode=%d", mTCP.getRemote().getStr(), mTCP.getLocal().getStr(), ret);
    }
    return EE_OK == ret;
}

void HttpLayer::onConnect(RequestFD* it) {
    if (0 == it->mError && sendReq()) {
        it->mCall = HttpLayer::funcOnRead;
        if (EE_OK == readIF(it)) {
            return;
        }
    }
    RequestFD::delRequest(it);
}

void HttpLayer::onWrite(RequestFD* it, HttpMsg* msg) {
    if (EE_OK != it->mError) {
        Logger::log(ELL_ERROR, "HttpLayer::onWrite>>size=%u, ecode=%d", it->mUsed, it->mError);
    } else {
        msg->setRespStatus(0);
        msg->getCacheOut().commitHead(static_cast<s32>(it->mUsed));
        if (msg->getCacheOut().getSize() > 0) {
            sendResp(msg);
        } else {
            if (EE_OK != mWebsite->stepMsg(msg)) {
                postClose();
            }
        }
    }
    RequestFD::delRequest(it);
    msg->drop();
}


void HttpLayer::onRead(RequestFD* it) {
    const s8* dat = it->getBuf();
    ssz datsz = it->mUsed;
    if (0 == datsz) {
        parseBuf(dat, 0); //make the http-msg-finish callback
    } else {
        ssz parsed = 0;
        ssz stepsz;
        while (datsz > 0 && 0 == mHttpError) {
            stepsz = parseBuf(dat + parsed, datsz);
            parsed += stepsz;
            if (stepsz < datsz) {
                break; //leftover
            }
            datsz -= stepsz;
        }
        it->clearData((u32)parsed);

        if (0 == it->getWriteSize()) {
            //鍙
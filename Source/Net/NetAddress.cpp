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


#include "Net/NetAddress.h"
#include "Converter.h"
//#include <cstring>

#if defined(DOS_WINDOWS)
#include <winsock2.h>
#include <WS2tcpip.h>
#elif defined(DOS_LINUX) || defined(DOS_ANDROID)
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#endif  //DOS_WINDOWS

#define DNET_ANY_IP "0.0.0.0"
#define DNET_ANY_IP6 "[::]"
#define DNET_ANY_PORT 0


namespace app {
namespace net {
constexpr static u8 GIPV6_SIZE = 28;
constexpr static u8 GIPV4_SIZE = 16;

NetAddress::NetAddress(bool ipv6) {
    clear();
    snprintf(mName, sizeof(mName), "%s:0", ipv6 ? DNET_ANY_IP6 : DNET_ANY_IP);
    init(ipv6);
    initIP();
    initPort();
}


NetAddress::NetAddress(const s8* ipPort) {
    setIPort(ipPort);
}


NetAddress::NetAddress(const String& ipPort) {
    setIPort(ipPort.c_str());
}


NetAddress::NetAddress(u16 port, bool ipv6) {
    clear();
    mPort = port;
    snprintf(mName, sizeof(mName), "%s:%d", ipv6 ? DNET_ANY_IP6 : DNET_ANY_IP, port);
    init(ipv6);
    initIP();
    initPort();
}


NetAddress::NetAddress(const s8* ip, u16 port, bool ipv6) {
    clear();
    if(ip) {
        snprintf(mName, sizeof(mName), "%s:%d", ip, port);
    } else {
        snprintf(mName, sizeof(mName), "%s:%d", ipv6 ? DNET_ANY_IP6 : DNET_ANY_IP, port);
    }
    mPort = port;
    init(ipv6);
    initIP();
    initPort();
}

NetAddress::NetAddress(const String& ip, u16 port, bool ipv6) :
    NetAddress(ip.c_str(), port, ipv6) {
}

NetAddress::NetAddress(const NetAddress& it) {
    memcpy(this, &it, sizeof(it));
}


NetAddress::~NetAddress() {
}


void NetAddress::clear() {
    memset(this, 0, sizeof(*this));
}


DINLINE void NetAddress::init(bool ipv6) {
    if(ipv6) {
        mSize = sizeof(sockaddr_in6);
        getAddress6()->sin6_family = AF_INET6;
    } else {
        mSize = sizeof(sockaddr_in);
        getAddress4()->sin_family = AF_INET;
    }
}


DINLINE void NetAddress::initIP() {
    if(isIPV6()) {
        inet_pton(getAddress6()->sin6_family, mName, &(getAddress6()->sin6_addr));
    } else {
        inet_pton(getAddress4()->sin_family, mName, &(getAddress4()->sin_addr));
    }
}


DINLINE void NetAddress::initPort() {
    if(isIPV6()) {
        getAddress6()->sin6_port = htons(mPort);
    } else {
        getAddress4()->sin_port = htons(mPort);
    }
}


NetAddress& NetAddress::operator=(const NetAddress& it) {
    if(this != &it) {
        memcpy(this, &it, sizeof(it));
    }
    return *this;
}


bool NetAddress::operator==(const NetAddress& it) const {
    if(mSize == it.mSize && mPort == it.mPort) {
        return 0 == memcmp(mAddress, it.mAddress, mSize);
    }
    return false;
}


bool NetAddress::operator!=(const NetAddress& it) const {
    if(mSize == it.mSize && mPort == it.mPort) {
        return 0 != memcmp(mAddress, it.mAddress, mSize);
    }
    return true;
}


u16 NetAddress::getFamily()const {
    return (*(u16*)mAddress);
}


void NetAddress::setIP(const s8* ip, bool ipv6) {
    if(ip && '[' != ip[0]) {
        if(ipv6) {
            mSize = sizeof(sockaddr_in6);
            getAddress6()->sin6_family = AF_INET6;
            inet_pton(AF_INET6, ip, &(getAddress6()->sin6_addr));
            snprintf(mName, sizeof(mName), "[%s]:%d", ip, mPort);
        } else {
            mSize = sizeof(sockaddr_in);
            getAddress4()->sin_family = AF_INET;
            inet_pton(AF_INET, ip, &(getAddress4()->sin_addr));
            snprintf(mName, sizeof(mName), "%s:%d", ip, mPort);
        }
    }
}


bool NetAddress::setIP(bool ipv6) {
    clear();
    s8 host_name[256];
    if(::gethostname(host_name, sizeof(host_name) - 1) < 0) {
        return false;
    }
    struct addrinfo* head = 0;
    struct addrinfo hints;
    hints.ai_family = ipv6 ? AF_INET6 : AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;    // IPPROTO_TCP; //0锛岄粯璁
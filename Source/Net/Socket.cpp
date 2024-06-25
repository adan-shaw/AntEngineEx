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


#include "Net/Socket.h"

#if defined(DOS_WINDOWS)
#include <winsock2.h>
#include <mstcpip.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <afunix.h>
#include "Windows/Request.h"
#elif defined(DOS_LINUX) || defined(DOS_ANDROID)
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#endif  //DOS_WINDOWS
#include "System.h"


namespace app {
namespace net {

enum EShutFlag {
    ESHUT_RECEIVE,
    ESHUT_SEND,
    ESHUT_BOTH
};

Socket::Socket(netsocket sock) :
    mSocket(sock) {
    DASSERT(sizeof(socklen_t) == sizeof(u32));
}


Socket::Socket() :
    mSocket(DINVALID_SOCKET) {
#if defined(DOS_WINDOWS)
    DASSERT(INVALID_SOCKET == DINVALID_SOCKET);
#elif defined(DOS_LINUX) || defined(DOS_ANDROID)
    //DASSERT(-1 == DINVALID_SOCKET);
#endif
}


Socket::~Socket() {
}


bool Socket::close() {
    if (DINVALID_SOCKET == mSocket) {
        return false;
    }

#if defined(DOS_WINDOWS)
    if (::closesocket(mSocket)) {
        return false;
    }
#elif defined(DOS_LINUX) || defined(DOS_ANDROID)
    if (::close(mSocket)) {
        return false;
    }
#endif  //DOS_WINDOWS

    mSocket = DINVALID_SOCKET;
    return true;
}


bool Socket::closeReceive() {
    return 0 == ::shutdown(mSocket, ESHUT_RECEIVE);
}

bool Socket::closeSend() {
    return 0 == ::shutdown(mSocket, ESHUT_SEND);
}

bool Socket::closeBoth() {
    return 0 == ::shutdown(mSocket, ESHUT_BOTH);
}


bool Socket::isOpen()const {
    return DINVALID_SOCKET != mSocket;
}

s32 Socket::getError()const {
    s32 err = 0;
    socklen_t len = sizeof(err);
    if (0 != getsockopt(mSocket, SOL_SOCKET, SO_ERROR, (s8*)&err, &len)) {
        err = -1;
    }
    return err;
}

void Socket::getLocalAddress(NetAddress& it) {
    socklen_t len = it.getAddrSize();
    ::getsockname(mSocket, (struct sockaddr*)it.getAddress6(), &len);
    it.reverse();
}


void Socket::getRemoteAddress(NetAddress& it) {
    socklen_t len = it.getAddrSize();
    ::getpeername(mSocket, (struct sockaddr*)it.getAddress6(), &len);
    it.reverse();
}


s32 Socket::setReceiveOvertime(u32 it) {
#if defined(DOS_WINDOWS)
    return ::setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (s8*)&it, sizeof(it));
#else
    struct timeval time;
    time.tv_sec = it / 1000;                     //seconds
    time.tv_usec = 1000 * (it % 1000);         //microseconds
    return ::setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (s8*)&time, sizeof(time));
#endif
}


s32 Socket::setSendOvertime(u32 it) {
#if defined(DOS_WINDOWS)
    return ::setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (s8*)&it, sizeof(it));
#else
    struct timeval time;
    time.tv_sec = it / 1000;                     //seconds
    time.tv_usec = 1000 * (it % 1000);           //microseconds
    return ::setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (s8*)&time, sizeof(time));
#endif
}


s32 Socket::setCustomIPHead(bool on) {
    s32 opt = on ? 1 : 0;
    return ::setsockopt(mSocket, IPPROTO_IP, IP_HDRINCL, (s8*)&opt, sizeof(opt));
}


s32 Socket::setReceiveAll(bool on) {
#if defined(DOS_WINDOWS)
    u32 dwBufferLen[10];
    u32 dwBufferInLen = on ? RCVALL_ON : RCVALL_OFF;
    DWORD dwBytesReturned = 0;
    //::ioctlsocket(mScoketListener.getValue(), SIO_RCVALL, &dwBufferInLen);
    return (::WSAIoctl(mSocket, SIO_RCVALL,
        &dwBufferInLen, sizeof(dwBufferInLen),
        &dwBufferLen, sizeof(dwBufferLen),
        &dwBytesReturned, 0, 0));
#elif defined(DOS_LINUX) || defined(DOS_ANDROID)
    ifreq iface;
    const char* device = "eth0";
    DASSERT(IFNAMSIZ > ::strlen(device));

    s32 setopt_result = ::setsockopt(mSocket, SOL_SOCKET,
        SO_BINDTODEVICE, device, ::strlen(device));

    ::strcpy(iface.ifr_name, device);
    if (::ioctl(mSocket, SIOCGIFFLAGS, &iface) < 0) {
        return -1;
    }
    if (!(iface.ifr_flags & IFF_RUNNING)) {
        //printf("eth link down\n");
        return -1;
    }
    if (on) {
        iface.ifr_flags |= IFF_PROMISC;
    } else {
        iface.ifr_flags &= ~IFF_PROMISC;
    }
    return ::ioctl(mSocket, SIOCSIFFLAGS, &iface);
#endif
}


s32 Socket::keepAlive(bool on, s32 idleTime, s32 timeInterval, s32 maxTick) {
#if defined(DOS_WINDOWS)
    //struct tcp_keepalive {
    //    ULONG onoff;   // 鏄
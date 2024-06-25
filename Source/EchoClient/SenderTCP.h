#ifndef APP_SENDERTCP_H
#define	APP_SENDERTCP_H

#include "Engine.h"
#include "Packet.h"
#include "Net/HandleTLS.h"

namespace app {


class SenderTCP {
public:
    SenderTCP();

    ~SenderTCP();

    //@param cnt 鍙戦€佹
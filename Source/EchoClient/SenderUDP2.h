#ifndef APP_SENDERUDP2_H
#define	APP_SENDERUDP2_H

#include "Engine.h"
#include "Packet.h"
#include "Net/HandleUDP.h"
#include "Net/KCProtocal.h"

namespace app {


class SenderUDP2 {
public:
    SenderUDP2();

    ~SenderUDP2();

    //@param cnt 鍙戦€佹
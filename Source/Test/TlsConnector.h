#ifndef APP_TLSCONNECTOR_H
#define	APP_TLSCONNECTOR_H

#include "Engine.h"
#include "Packet.h"
#include "Net/HandleTLS.h"

namespace app {


class TlsConnector {
public:
    TlsConnector();

    ~TlsConnector();

    //@param cnt 鍙戦€佹
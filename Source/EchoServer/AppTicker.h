#ifndef APP_APPTICKER_H
#define	APP_APPTICKER_H

#include "MsgHeader.h"
#include "Loop.h"


namespace app {
namespace net {

enum EPackType {
    EPT_RESP_BIT = 0x8000,

    EPT_ACTIVE = 0x0001,
    EPT_ACTIVE_RESP = EPT_RESP_BIT| EPT_ACTIVE,
    EPT_SUBMIT = 0x0002,
    EPT_SUBMIT_RESP = EPT_RESP_BIT | EPT_SUBMIT,

    EPT_VERSION = 0xF001
};

//蹇冭烦鍖
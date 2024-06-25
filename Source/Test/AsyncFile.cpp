#include "AsyncFile.h"
#include "System.h"
#include "Engine.h"

namespace app {

AsyncFile::AsyncFile() :
    mLoop(&Engine::getInstance().getLoop())
    , mPack(1024)
    , mAsyncSize(0) {
    mHandleFile.setClose(EHT_FILE, AsyncFile::funcOnClose, this);
    //35绉掑唴娌″
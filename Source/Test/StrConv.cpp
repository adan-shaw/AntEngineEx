#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <vector>
#include "Timer.h"
#include "TVector.h"
#include "TList.h"
#include "Converter.h"
#include "Strings.h"
#include "Spinlock.h"
#include "CodecBase64.h"
#include "EncoderMD5.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Packet.h"
#include "Net/HTTP/HttpMsg.h"
#include "gzip/EncoderGzip.h"
#include "gzip/DecoderGzip.h"

namespace app {

void AppTestStrConv() {
    s8 tmp[32] = "aBcv";
    StringView str(tmp, 3);
    printf("str = %.*s\n", (s32)str.mLen, str.mData);
    str.toLower();
    printf("str = %.*s\n", (s32)str.mLen, str.mData);
    str.toUpper();
    printf("str = %.*s\n", (s32)str.mLen, str.mData);

    const wchar_t* src = L"use鏋滄眮";
    const s8* src8 = u8"use鏋滄眮";
    s8 str8[128];
    wchar_t wstr[128];
    usz len = AppWcharToUTF8(src, str8, sizeof(str8));
    s32 cmp = memcmp(src8, str8, len);
    printf("cmp = %d, len=%llu\n", cmp, len);
    len = AppUTF8ToWchar(src8, wstr, sizeof(wstr));



    Spinlock spi;
    spi.lock();

    bool trlk = spi.tryLock();
    spi.unlock();
    trlk = spi.tryLock();
    trlk = spi.tryLock();
}

void AppTestBase64() {
    // base64 = "YmFzZTY0dGVzdA=="
    const s8* src = "base64test";
    const s32 strsize = (s32)strlen(src);
    s32 esize = CodecBase64::getEncodeCacheSize(strsize);
    s8* dest = new s8[esize];
    esize = CodecBase64::encode((app::u8*)src, strsize, dest);
    printf("src=%s/%d, encode=%s/%d\n", src, strsize, dest, esize);

    s32 dsize = CodecBase64::getDecodeCacheSize(esize);
    s8* decode = new s8[dsize];
    dsize = CodecBase64::decode(dest, esize, (app::u8*)decode);
    printf("decode=%.*s/%d, encode=%s/%d\n", dsize, decode, dsize, dest, esize);
    delete[] dest;
    delete[] decode;
}

void AppTestMD5(s32 argc, s8** argv) {
    EncoderMD5 md5;
    // md5 = "9567E4269E9974E8B28C2F903037F501"
    const s8* src = "base64test";
    const s32 strsize = (s32)strlen(src);
    md5.add(src, strsize);
    ID128 resID;
    md5.finish(resID);
    u8* res = (u8*)&resID;
    printf("str[%d] = %s\n", strsize, src);
    printf("buf = %lld,%lld\nmd5 = 0x", resID.mLow, resID.mHigh);
    for (s32 i = 0; i < EncoderMD5::GMD5_LENGTH; ++i) {
        printf("%.2X", res[i]);
    }
    printf("\n");

    s8 hex[EncoderMD5::GMD5_LENGTH * 2 + 1];
    AppBufToHex(&resID, EncoderMD5::GMD5_LENGTH, hex, sizeof(hex));
    printf("AppBufToHex = 0x%s\n", hex);
    resID.clear();
    AppHexToBuf(hex, sizeof(hex) - 1, &resID, sizeof(resID));
    printf("re_buf = %lld,%lld\n", resID.mLow, resID.mHigh);
}

// String::find()鎬ц兘娴嬭瘯
void AppTestStrFind(s32 cnt) {
    const s32 mx = 2;
    ssz pos[mx];
    String stdsrc[mx] = {
        "鍥哄疄鍘嬬缉锛氭妸瑕佸帇缂╃殑瑙嗕负鍚屼竴涓
#include "Config.h"
#include <string.h>
#include <stdio.h>
#include <memory>
#include "StrConverter.h"
#include "System.h"
#include "Engine.h"

namespace app {

void AppStrNormalizeGBK(s8* text, u32 options);
void AppStrNormalizeUTF8(s8* text, u32 options);

s32 AppTestStrConvGBKU8(s32 argc, s8** argv) {
    const s8* utf8 = u8"鎴戞槸utf-8瀛楃
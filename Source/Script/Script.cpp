#include "Script/Script.h"
#include "Script/HLua.h"
#include "Script/LuaFunc.h"
#include "Logger.h"
#include "Engine.h"
#include "FileReader.h"
#include "Script/ScriptManager.h"

namespace app {
namespace script {

Script::Script() {
}

Script::~Script() {
}

bool Script::isLoaded(lua_State* vm, const String& iName) {
    lua_getglobal(vm, iName.c_str());
    bool ret = !lua_isnil(vm, -1);
    lua_pop(vm, 1);
    return ret;
}

bool Script::getLoaded(lua_State* vm, const String& iName) {
    lua_getglobal(vm, iName.c_str());
    return !lua_isnil(vm, -1);
}

bool Script::loadBuf(lua_State* vm, const String& iName, const s8* buf, usz bsz, bool reload, bool pop) {
    if (!reload) {
        if (getLoaded(vm, iName)) {
            if (pop) {
                lua_pop(vm, 1);
            }
            return true;
        }
        lua_pop(vm, 1);
    }
    bool ret = compile(vm, iName, buf, bsz, pop);
    mName = iName;
    return ret;
}

bool Script::load(lua_State* vm, const String& iPath, const String& iName, bool reload, bool pop) {
    if (!reload) {
        if (getLoaded(vm, iName)) {
            if (pop) {
                lua_pop(vm, 1);
            }
            return true;
        }
        lua_pop(vm, 1);
    }
    FileReader file;
    if (!file.openFile(iPath + iName)) {
        Logger::logError("Script::load, open fail, script = %s", iName.c_str());
        return false;
    }
    usz bufsz = file.getFileSize();
    s8* buf = new s8[bufsz + 1];
    if (bufsz != file.read(buf, bufsz)) {
        Logger::logError("Script::load, read fail, script = %s", iName.c_str());
        // file.close();
        delete[] buf;
        return false;
    }
    buf[bufsz] = '\0';
    bool ret = compile(vm, iName, buf, bufsz, pop);
    mName = iName;
    delete[] buf;
    return ret;
}

void Script::unload(lua_State* vm) {
    if (vm) {
        lua_pushnil(vm);
        lua_setglobal(vm, mName.c_str());
    }
}

bool Script::compile(lua_State* vm, const String& iName, const s8* buf, usz bufsz, bool pop) {
    if (!vm || !buf || 0 == bufsz || 0 == iName.getLen()) {
        return false;
    }
    u32 cnt = lua_gettop(vm);
    if (0 != luaL_loadbuffer(vm, buf, bufsz, iName.c_str())) {
        Logger::logError("Script::compile, load err = %s, name=%s\n", lua_tostring(vm, -1), iName.c_str());
        lua_pop(vm, 1); // pop lua err
        return false;
    }
    cnt = lua_gettop(vm);
    if (!pop) {
        lua_pushvalue(vm, -1);
    }
    lua_setglobal(vm, iName.c_str()); // set and pop 1
    cnt = lua_gettop(vm);
    Logger::logInfo("Script::compile, success script = %s", iName.c_str());
    return true;
}

bool Script::exec(lua_State* vm, const s8* func, s32 nargs, s32 nresults, s32 errfunc) {
    const s32 cnt = lua_gettop(vm);
    if (!getLoaded(vm, mName)) {
        lua_pop(vm, 1);
        return false;
    }

    if (func) {
        lua_getglobal(vm, func);
    }

    /**
     * lua_pcall鍙傛暟:
     * 浼犲叆nargs涓
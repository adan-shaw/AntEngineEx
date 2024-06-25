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


#ifndef APP_THREADPOOL_H
#define APP_THREADPOOL_H

#include "Config.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace app {

using FuncVoid = void (*)();
using FuncTask = void (*)(void*);
using FuncTaskClass = void (*)(const void*, void*);


class TaskNode {
public:
    TaskNode* mNext;
    FuncTask mCall;
    const void* mThis;
    void* mData;
    TaskNode() : mNext(nullptr), mCall(nullptr), mThis(nullptr), mData(nullptr) {
    }

    ~TaskNode() {
    }

    void clear() {
        mNext = nullptr;
        mCall = nullptr;
        mThis = nullptr;
        mData = nullptr;
    }

    template <class P>
    void pack(void (*func)(P*), P* dat) {
        mCall = func;
        mThis = nullptr;
        mData = dat;
    }

    template <class T, class P>
    void pack(void (T::*func)(P*), const void* it, P* dat) {
        void* fff = reinterpret_cast<void*>(&func);
        mCall = *(FuncTask*)fff;
        mThis = it;
        mData = dat;
    }

    void operator()() {
        if (mThis) {
            ((FuncTaskClass)mCall)(mThis, mData);
        } else {
            mCall(mData);
        }
    }
};


class ThreadPool {
public:
    ThreadPool() :
        mRunning(false), mTaskCount(0), mAllocated(0), mMaxAllocated(1000), mAllTask(nullptr), mIdleTask(nullptr),
        mThreadInit(nullptr), mThreadUninit(nullptr) {
    }

    ThreadPool(u32 cnt, FuncVoid init = nullptr, FuncVoid uninit = nullptr) :
        mRunning(false), mTaskCount(0), mAllocated(0), mMaxAllocated(1000), mAllTask(nullptr), mIdleTask(nullptr),
        mThreadInit(init), mThreadUninit(uninit) {
        start(cnt);
    }

    ~ThreadPool() {
        stop();
        clear();
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(const ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&&) = delete;

    /**
     * @brief 鍦═hreadPool::start鍓嶈
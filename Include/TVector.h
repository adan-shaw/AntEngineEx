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


#ifndef APP_VECTOR_H
#define	APP_VECTOR_H


#include "Config.h"
#include "FuncSort.h"
#include "TAllocator.h"

namespace app {


/**
 * @brief Self reallocating template TVector (like stl vector) with additional features.
 * Some features are: Heap sorting, binary search methods, easier debugging.
 */
template <class T, typename TAlloc = TAllocator<T> >
class TVector {
public:

    TVector()
        : mData(nullptr)
        , mAllocated(0)
        , mUsed(0)
        , mStrategy(E_STRATEGY_DOUBLE)
        , mSorted(true) {
    }


    /**
     * @param start_count 棰勭暀瀹归噺 */
    TVector(usz start_count)
        : mData(nullptr)
        , mAllocated(0)
        , mUsed(0)
        , mStrategy(E_STRATEGY_DOUBLE)
        , mSorted(true) {
        reallocate(start_count);
    }


    TVector(const TVector<T, TAlloc>& other)
        : mData(nullptr), mAllocated(0), mUsed(0) {
        *this = other;
    }

    TVector(TVector<T, TAlloc>&& it)noexcept : mData(it.mData)
        , mAllocated(it.mAllocated)
        , mUsed(it.mUsed)
        , mStrategy(it.mStrategy)
        , mSorted(it.mSorted) {
        it.mUsed = 0;
        it.mAllocated = 0;
        it.mData = nullptr;
    }

    ~TVector() {
        clearAll();
    }


    void reallocate(usz new_size, bool canShrink = true) {
        if (mAllocated == new_size) {
            return;
        }
        if (!canShrink && (new_size < mAllocated)) {
            return;
        }

        T* old_data = mData;
        mData = mAllocator.allocate(new_size); //new T[new_size];
        mAllocated = new_size;

        // copy old data
        ssz end = mUsed < new_size ? mUsed : new_size;
        for (ssz i = 0; i < end; ++i) {
            mAllocator.construct(&mData[i], std::move(old_data[i]));
        }

        // destruct old mData
        for (usz j = 0; j < mUsed; ++j) {
            mAllocator.destruct(&old_data[j]);
        }
        mAllocator.deallocate(old_data);

        if (mAllocated < mUsed) {
            mUsed = mAllocated;
        }
    }


    /**
     * @brief 璁剧疆鍐呭瓨鐢宠
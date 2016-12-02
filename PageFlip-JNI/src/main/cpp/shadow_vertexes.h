/*
 * Copyright (C) 2016 eschao <esc.chao@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_PAGEFLIP_SHADOWVERTEXES_H
#define ANDROID_PAGEFLIP_SHADOWVERTEXES_H


#include "ShadowColor.h"

class ShadowVertexProgram;

class ShadowVertexes {

protected:
    int mCount;
    int mCapacity;
    int mSpaceOfFrontRear;
    int mBackward;
    int mForward;
    int mMaxBackward;

    float mVertexZ;
    float *mVertexes;
    ShadowColor mColor;

public:
    ShadowVertexes(int spaceOfFrontRear,
                   float startColor, float startAlpha,
                   float endColor, float endAlpha);
    ~ShadowVertexes();

    void release();
    void set(int capacity);
    ShadowVertexes& setVertexes(int offset, float startX, float startY, float endX, float endY);
    ShadowVertexes& addVertexesBackward(float startX, float startY, float endX, float endY);
    ShadowVertexes& addVertexesForward(float startX, float startY, float endX, float endY);
    void draw(ShadowVertexProgram& program);

    // inline
    inline void reset()
    {
        mVertexZ = 0;
        mBackward = mMaxBackward;
        mForward = mMaxBackward + (mSpaceOfFrontRear << 2);
    }

    inline ShadowVertexes& addVertexes(bool isForward,
                                       float startX, float startY,
                                       float endX, float endY)
    {
        return isForward ?
               addVertexesForward(startX, startY, endX, endY) :
               addVertexesBackward(startX, startY, endX, endY);
    }
};


#endif //ANDROID_PAGEFLIP_SHADOWVERTEXES_H

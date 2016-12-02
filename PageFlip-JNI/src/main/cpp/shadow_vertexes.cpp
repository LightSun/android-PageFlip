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

#include "ShadowVertexes.h"
#include "ShadowVertexProgram.h"
#include "VertexProgram.h"

ShadowVertexes::ShadowVertexes(int spaceOfFrontRear,
                               float startColor, float startAlpha,
                               float endColor, float endAlpha)
        : mSpaceOfFrontRear(spaceOfFrontRear),
          mBackward(0),
          mForward(0),
          mMaxBackward(0),
          mVertexes(NULL)
{
    mColor.set(startColor, startAlpha, endColor, endAlpha);
}

ShadowVertexes::~ShadowVertexes()
{
    release();
}

void ShadowVertexes::set(int capacity) {
    mMaxBackward = capacity << 3;
    mCapacity = (capacity << 4) + (mSpaceOfFrontRear << 2);
    mVertexes = new float[mCapacity];
    reset();
}

void ShadowVertexes::release()
{
    if (mVertexes)
    {
        delete[] mVertexes;
    }

    mBackward = 0;
    mForward = 0;
    mMaxBackward = 0;
    mSpaceOfFrontRear = 0;
    mCapacity = 0;
}

ShadowVertexes& ShadowVertexes::setVertexes(int offset,
                                            float startX, float startY,
                                            float endX, float endY)
{
    mVertexes[offset++] = startX;
    mVertexes[offset++] = startY;
    mVertexes[offset++] = mColor.startColor;
    mVertexes[offset++] = mColor.startAlpha;
    mVertexes[offset++] = endX;
    mVertexes[offset++] = endY;
    mVertexes[offset++] = mColor.endColor;
    mVertexes[offset] = mColor.endAlpha;
    return *this;
}

ShadowVertexes& ShadowVertexes::addVertexesBackward(float startX, float startY,
                                                    float endX, float endY)
{
    mVertexes[--mBackward] = startX;
    mVertexes[--mBackward] = startY;
    mVertexes[--mBackward] = mColor.startColor;
    mVertexes[--mBackward] = mColor.startAlpha;
    mVertexes[--mBackward] = endX;
    mVertexes[--mBackward] = endY;
    mVertexes[--mBackward] = mColor.endColor;
    mVertexes[--mBackward] = mColor.endAlpha;
    return *this;
}

ShadowVertexes& ShadowVertexes::addVertexesForward(float startX, float startY,
                                                   float endX, float endY)
{
    mVertexes[mForward++] = startX;
    mVertexes[mForward++] = startY;
    mVertexes[mForward++] = mColor.startColor;
    mVertexes[mForward++] = mColor.startAlpha;
    mVertexes[mForward++] = endX;
    mVertexes[mForward++] = endY;
    mVertexes[mForward++] = mColor.endColor;
    mVertexes[mForward++] = mColor.endAlpha;
    return *this;
}

void ShadowVertexes::draw(ShadowVertexProgram &program)
{
    if (mCount > 0)
    {
        glUniformMatrix4fv(program.getMVPMatrixLoc(), 1, GL_FALSE, VertexProgram::MVPMatrix);
        glUniform1f(program.getVertexZ, mVertexZ);

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glVertexAttribPointer(program.getVertexPositionLoc(), 4, GL_FLOAT, GL_FALSE, 0, mVertexes);
        glEnableVertexAttribArray(program.getVertexPositionLoc());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, mCount);

        glDisable(GL_BLEND);
    }
}

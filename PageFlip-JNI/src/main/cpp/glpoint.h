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

#ifndef ANDROID_PAGEFLIP_GLPOINT_H
#define ANDROID_PAGEFLIP_GLPOINT_H

/**
 * OpenGL point includes (x, y, z) coordinate in 3D system and corresponding texture coordinate
 * (x, y)
 */
class GLPoint
{
public:
    float x;
    float y;
    float z;

    float tX;
    float tY;

    GLPoint(float x, float y, float z, float tx, float ty)
            : x(x), y(y), z(z), tX(tx), tY(ty)
    { }

    inline void set(float x, float y, float z, float tx, float ty)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->tX = tx;
        this->tY = ty;
    }
};
#endif //ANDROID_PAGEFLIP_GLPOINT_H

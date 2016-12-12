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

namespace eschao {

/**
 * OpenGL point includes (x, y, z) coordinate in 3D system and corresponding texture coordinate
 * (x, y)
 */
struct GLPoint {

public:
    GLPoint() : x(0), y(0), z(0), tex_x(0), tex_y(0) { }

    GLPoint(float x, float y, float z, float tex_x, float tex_y)
            : x(x), y(y), z(z), tex_x(tex_x), tex_y(tex_y) { }

    inline void set(float x, float y, float z, float tex_x, float tex_y)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->tex_x = tex_x;
        this->tex_y = tex_y;
    }

public:
    float x;
    float y;
    float z;

    float tex_x;
    float tex_y;
};

}
#endif //ANDROID_PAGEFLIP_GLPOINT_H

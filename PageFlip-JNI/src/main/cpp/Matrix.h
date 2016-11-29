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

#ifndef ANDROID_PAGEFLIP_MATRIX_H
#define ANDROID_PAGEFLIP_MATRIX_H

#include <math.h>

#define normalize(x, y, z) \
{ \
    float norm = 1.0f / sqrt(x*x+y*y+z*z);\
    x *= norm; y *= norm; z *= norm; \
}

class Matrix {

public:
    static void orthoM(float m[16],
                       float left, float right, float bottom, float top,
                       float near, float far);
    static void setIdentityM(float m[16]);
    static void setLookAtM(float m[16],
                           float eyeX, float eyeY, float eyeZ,
                           float centerX, float centerY, float centerZ,
                           float upX, float upY, float upZ);
    static void multiplyMM(float m[16], float lhs[16], float rhs[16]);
    static void translateM(float m[16], float x, float y, float z);

};


#endif //ANDROID_PAGEFLIP_MATRIX_H

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

#ifndef ANDROID_PAGEFLIP_GLVIEWRECT_H
#define ANDROID_PAGEFLIP_GLVIEWRECT_H

namespace eschao {

class GLViewRect {

public:
    GLViewRect();

    GLViewRect(float surface_width, float surface_height,
               float margin_left, float margin_right);

    void set(float surface_width, float surface_height,
             float margin_left, float margin_right);

    inline void set(float surfaceWidth, float surfaceHeight) {
        set(surfaceWidth, surfaceHeight, margin_left, margin_right);
    }

    inline void set_margin(float margin_left, float margin_right) {
        set(surface_width, surface_height, margin_left, margin_right);
    }

    inline float min_of_width_height() {
        return width > height ? width : height;
    }

    inline float to_opengl_x(float x) {
        return x - half_width;
    }

    inline float to_opengl_y(float y) {
        return half_height - y;
    }

public:
    float left;
    float right;
    float top;
    float bottom;
    float width;
    float height;
    float half_width;
    float half_height;

    float margin_left;
    float margin_right;
    float surface_width;
    float surface_height;
};

}
#endif //ANDROID_PAGEFLIP_GLVIEWRECT_H

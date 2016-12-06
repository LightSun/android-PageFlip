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

#ifndef ANDROID_PAGEFLIP_SHADOWCOLOR_H
#define ANDROID_PAGEFLIP_SHADOWCOLOR_H

#include "error.h"
#include "pageflip_exception.h"

namespace eschao {

class ShadowColor {

public:

    ShadowColor() : start_color(0), start_alpha(0), end_color(0),
                    end_alpha(0) { }

    inline void set(float start_color, float start_alpha,
                    float end_color, float end_alpha) {
        if (start_color < 0 || start_color > 1 ||
            start_alpha < 0 || start_alpha > 1 ||
            end_color < 0 || end_color > 1 ||
            end_alpha < 0 || end_alpha > 1) {
            throw PageFlipException(Error::ERR_INVALID_PARAMETER);
        }

        this->start_color = start_color;
        this->start_alpha = start_alpha;
        this->end_color = end_color;
        this->end_alpha = end_alpha;
    }

public:
    float start_color;
    float start_alpha;
    float end_color;
    float end_alpha;
};

}
#endif //ANDROID_PAGEFLIP_SHADOWCOLOR_H

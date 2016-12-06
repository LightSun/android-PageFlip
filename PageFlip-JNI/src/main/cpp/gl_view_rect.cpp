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

#include "gl_view_rect.h"

namespace eschao {

GLViewRect::GLViewRect()
        : left(0), right(0), top(0), bottom(0), width(0), height(0),
          half_width(0), half_height(0), margin_left(0), margin_right(0),
          surface_width(0), surface_height(0)
{ }

GLViewRect::GLViewRect() (float surface_width, float surface_height,
                          float margin_left, float margin_right)
{
    set(surface_width, surface_height, margin_left, margin_right);
}

void GLViewRect::set(float surface_width, float surface_height,
                     float margin_left, float margin_right)
{
    this->surface_width = surface_width;
    this->surface_height = surface_height;
    this->margin_left = margin_left;
    this->margin_right = margin_right;

    width = surface_width - margin_left - margin_right;
    height = surface_height;
    half_width = width * 0.5f;
    half_height = height * 0.5f;
    left = -half_width + margin_left;
    right = half_width - margin_right;
    top = half_height;
    bottom = -half_height;
}

}

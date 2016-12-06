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

#ifndef ANDROID_PAGEFLIP_BACKOFFOLDVERTEXES_H
#define ANDROID_PAGEFLIP_BACKOFFOLDVERTEXES_H

#include "vertexes.h"
#include "error.h"

namespace eschao {

class Page;

class BackOfFoldVertexProgram;

class BackOfFoldVertexes : public Vertexes {

public:
    BackOfFoldVertexes();

    ~BackOfFoldVertexes();

    void draw(BackOfFoldVertexProgram &program, Page &page,
              bool has_second_page, GLuint gradient_light_id);

    //inline
    inline void set(int mesh_count) {
        Vertexes::set(mesh_count << 1, 4, true);
    }

    inline int set_mask_alpha(int alpha) {
        if (alpha < 0 || alpha > 255) {
            return Error::ERR_INVALID_PARAMETER;
        }

        m_mask_alpha = alpha / 255.0f;
        return Error::OK;
    }

    inline int set_mask_alpha(float alpha) {
        if (alpha < 0 || alpha > 255) {
            return Error::ERR_INVALID_PARAMETER;
        }

        m_mask_alpha = alpha;
        return Error::OK;
    }

protected:
    float m_mask_alpha;

};

}
#endif //ANDROID_PAGEFLIP_BACKOFFOLDVERTEXES_H

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

#ifndef ANDROID_PAGEFLIP_BACKOFFOLDVERTEXPROGRAM_H
#define ANDROID_PAGEFLIP_BACKOFFOLDVERTEXPROGRAM_H

#include "vertex_program.h"

class BackOfFoldVertexProgram : public VertexProgram {

public:
    BackOfFoldVertexProgram();
    ~BackOfFoldVertexProgram();

    void clean();
    int init();

    // inline
    inline GLint get_shadow_loc()
    {
        return m_shadow_loc;
    }

    inline GLint get_mask_color_loc()
    {
        return m_mask_color_loc;
    }

    inline GLint get_tex_x_offset_loc()
    {
        return m_tex_x_offset_loc;
    }

protected:
    void get_vars_location();

protected:
    static const char *VAR_TEXTURE_OFFSET   = "u_texXOffset";
    static const char *VAR_MASK_COLOR       = "u_maskColor";
    static const char *VAR_SHADOW_TEXTURE   = "u_shadow";

    GLint m_shadow_loc;
    GLint m_mask_color_loc;
    GLint m_tex_x_offset_loc;
};

#endif //ANDROID_PAGEFLIP_BACKOFFOLDVERTEXPROGRAM_H

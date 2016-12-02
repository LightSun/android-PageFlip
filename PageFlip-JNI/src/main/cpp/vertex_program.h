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

#ifndef ANDROID_PAGEFLIP_VERTEXPROGRAM_H
#define ANDROID_PAGEFLIP_VERTEXPROGRAM_H


#include "gl_program.h"

class VertexProgram : public GLProgram {

public:
    VertexProgram();
    ~VertexProgram();

    void clean();
    int init();
    void init_matrix(float left, float right, float bottom, float top);

    // inline
    inline GLint get_MVP_matrix_loc()
    {
        return MVP_matrix_loc;
    }

    inline GLint get_vertex_pos_loc()
    {
        return vertex_pos_loc;
    }

    inline GLint get_tex_coord_loc()
    {
        return tex_coord_loc;
    }

    inline GLint get_texture_loc()
    {
        return texture_loc;
    }

protected:
    void get_vars_location();

public:
    static float mv_matrix[16];
    static float mvp_matrix[16];

protected:
    static const char *VAR_MVP_MATRIX       = "u_MVPMatrix";
    static const char *VAR_VERTEX_POS       = "a_vexPosition";
    static const char *VAR_TEXTURE_COORD    = "a_texCoord";
    static const char *VAR_TEXTURE          = "u_texture";

    GLint mvp_matrix_loc;
    GLint vertex_pos_loc;
    GLint tex_coord_loc;
    GLint texture_loc;
};

#endif //ANDROID_PAGEFLIP_VERTEXPROGRAM_H

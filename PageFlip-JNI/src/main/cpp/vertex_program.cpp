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

#include "vertex_program.h"
#include "constant.h"
#include "matrix.h"

namespace eschao {

static const auto g_vertex_shader =
        "uniform mat4 u_MVPMatrix;\n"
                "attribute vec4 a_vexPosition;\n"
                "attribute vec2 a_texCoord;\n"
                "varying vec2 v_texCoord;\n"
                "\n"
                "void main() {\n"
                "    gl_Position = u_MVPMatrix * a_vexPosition;\n"
                "    v_texCoord = a_texCoord;\n"
                "}";

static const auto g_fragment_shader =
        "uniform sampler2D u_texture;\n"
                "varying vec2 v_texCoord;\n"
                "\n"
                "void main() {\n"
                "    gl_FragColor = texture2D(u_texture, v_texCoord);\n"
                "}";

VertexProgram::VertexProgram()
        : m_mvp_matrix_loc(Constant::kGlInValidLocation),
          m_vertex_pos_loc(Constant::kGlInValidLocation),
          m_tex_coord_loc(Constant::kGlInValidLocation),
          m_texture_loc(Constant::kGlInValidLocation) {
}

VertexProgram::~VertexProgram() {
    clean();
}

void VertexProgram::clean() {
    m_texture_loc = Constant::kGlInValidLocation;
    m_mvp_matrix_loc = Constant::kGlInValidLocation;
    m_tex_coord_loc = Constant::kGlInValidLocation;
    m_vertex_pos_loc = Constant::kGlInValidLocation;

    GLProgram::clean();
}

void VertexProgram::init() {
    GLProgram::init(g_vertex_shader, g_fragment_shader);
}

void VertexProgram::init_matrix(float left, float right,
                                float bottom, float top) {
    float projectMatrix[16];
    Matrix::ortho(projectMatrix, left, right, bottom, top, 0, 6000);
    Matrix::set_identity(mv_matrix);
    Matrix::set_look_at(mv_matrix, 0, 0, 3000, 0, 0, 0, 0, 1, 0);
    Matrix::set_identity(mvp_matrix);
    Matrix::multiply_mm(mvp_matrix, projectMatrix, mv_matrix);
}

void VertexProgram::get_vars_location() {
    m_texture_loc = glGetUniformLocation(m_program_ref, VAR_TEXTURE);
    m_mvp_matrix_loc = glGetUniformLocation(m_program_ref, VAR_MVP_MATRIX);
    m_tex_coord_loc = glGetAttribLocation(m_program_ref, VAR_TEXTURE_COORD);
    m_vertex_pos_loc = glGetAttribLocation(m_program_ref, VAR_VERTEX_POS);
}

}

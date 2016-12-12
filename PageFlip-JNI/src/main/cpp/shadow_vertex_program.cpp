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

#include "shadow_vertex_program.h"
#include "constant.h"

namespace eschao {

static const auto g_vertex_shader =
        "precision mediump float;\n"
        "uniform mat4 u_MVPMatrix;\n"
        "uniform float u_vexZ;\n"
        "attribute vec4 a_vexPosition;\n"
        "varying vec4 v_texColor;\n"
        "\n"
        "void main() {\n"
        "    vec4 vexPos = vec4(a_vexPosition.xy, u_vexZ, 1.0);\n"
        "    v_texColor = vec4(a_vexPosition.z, a_vexPosition.z, a_vexPosition.z, a_vexPosition.w);\n"
        "    gl_Position = u_MVPMatrix * vexPos;\n"
        "}";

static const auto g_fragment_shader =
        "precision mediump float;\n"
        "varying vec4 v_texColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = v_texColor;\n"
        "}";

static const char *VAR_MVP_MATRIX = "u_MVPMatrix";
static const char *VAR_VERTEX_Z = "u_vexZ";
static const char *VAR_VERTEX_POS = "a_vexPosition";

ShadowVertexProgram::ShadowVertexProgram()
        : m_mvp_matrix_loc(Constant::kGlInValidLocation),
          m_vertex_z_loc(Constant::kGlInValidLocation),
          m_vertex_pos_loc(Constant::kGlInValidLocation)
{
}

ShadowVertexProgram::~ShadowVertexProgram()
{
    clean();
}

void ShadowVertexProgram::clean()
{
    m_mvp_matrix_loc = Constant::kGlInValidLocation;
    m_vertex_z_loc = Constant::kGlInValidLocation;
    m_vertex_pos_loc = Constant::kGlInValidLocation;

    GLProgram::clean();
}

int ShadowVertexProgram::init()
{
    return GLProgram::init(g_vertex_shader, g_fragment_shader);
}

void ShadowVertexProgram::get_vars_location()
{
    m_vertex_z_loc = glGetUniformLocation(m_program_ref, VAR_VERTEX_Z);
    m_mvp_matrix_loc = glGetUniformLocation(m_program_ref, VAR_MVP_MATRIX);
    m_vertex_pos_loc = glGetAttribLocation(m_program_ref, VAR_VERTEX_POS);
}

}

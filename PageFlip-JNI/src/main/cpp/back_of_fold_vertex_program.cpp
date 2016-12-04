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

#include "back_of_fold_vertex_program.h"
#include "constant.h"

static const auto g_vertex_shader =
        "uniform mat4 u_MVPMatrix;\n"
        "uniform float u_texXOffset;\n"
        "attribute vec4 a_vexPosition;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "varying float v_shadowX;\n"
        "\n"
        "void main() {\n"
        "    v_texCoord = vec2(abs(a_texCoord.x - u_texXOffset), a_texCoord.y);\n"
        "    v_shadowX = clamp(abs(a_vexPosition.w), 0.01, 1.0);\n"
        "    vec4 vertex = vec4(a_vexPosition.xyz, 1);\n"
        "    gl_Position = u_MVPMatrix * vertex;\n"
        "}";

static const auto g_fragment_shader =
        "uniform sampler2D u_texture;\n"
        "uniform sampler2D u_shadow;\n"
        "uniform vec4 u_maskColor;\n"
        "varying vec2 v_texCoord;\n"
        "varying float v_shadowX;\n"
        "\n"
        "void main() {\n"
        "    vec4 texture = texture2D(u_texture, v_texCoord);\n"
        "    vec2 shadowCoord = vec2(v_shadowX, 0);\n"
        "    vec4 shadow = texture2D(u_shadow, shadowCoord);\n"
        "    vec4 maskedTexture = vec4(mix(texture.rgb, u_maskColor.rgb, u_maskColor.a), 1.0);\n"
        "    gl_FragColor = vec4(maskedTexture.rgb * (1.0 - shadow.a) + shadow.rgb, maskedTexture.a);\n"
        "}";

BackOfFoldVertexProgram::BackOfFoldVertexProgram()
        : m_shadow_loc(Constant::kGlInValidLocation),
          m_mask_color_loc(Constant::kGlInValidLocation),
          m_tex_x_offset_loc(Constant::kGlInValidLocation)
{
}

BackOfFoldVertexProgram::~BackOfFoldVertexProgram()
{
    clean();
}

void BackOfFoldVertexProgram::clean()
{
    m_shadow_loc = Constant::kGlInValidLocation;
    m_mask_color_loc = Constant::kGlInValidLocation;
    m_tex_x_offset_loc = Constant::kGlInValidLocation;

    GLProgram::clean();
}

int BackOfFoldVertexProgram::init()
{
    return GLProgram::(g_vertex_shader, g_fragment_shader);
}

void BackOfFoldVertexProgram::get_vars_location()
{
    m_shadow_loc = glGetUniformLocation(m_program_ref, VAR_SHADOW_TEXTURE);
    m_mask_color_loc = glGetUniformLocation(m_program_ref, VAR_MASK_COLOR);
    m_tex_x_offset_loc = glGetUniformLocation(m_program_ref, VAR_TEXTURE_OFFSET);
}

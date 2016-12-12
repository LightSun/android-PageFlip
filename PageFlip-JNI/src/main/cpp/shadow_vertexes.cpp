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

#include "shadow_vertexes.h"
#include "shadow_vertex_program.h"
#include "vertex_program.h"

namespace eschao {

ShadowVertexes::ShadowVertexes(int space_of_front_rear,
                               float start_color, float start_alpha,
                               float end_color, float end_alpha)
        : m_space_of_front_rear(space_of_front_rear),
          m_backward(0),
          m_forward(0),
          m_max_backward(0),
          m_vertexes(NULL)
{
    color.set(start_color, start_alpha, end_color, end_alpha);
}

ShadowVertexes::~ShadowVertexes()
{
    release();
}

void ShadowVertexes::set(int mesh_count)
{
    m_max_backward = mesh_count << 3;
    m_capacity = (mesh_count << 4) + (m_space_of_front_rear << 2);
    m_vertexes = new float[m_capacity];
    reset();
}

void ShadowVertexes::release()
{
    if (m_vertexes) {
        delete[] m_vertexes;
    }

    m_backward = 0;
    m_forward = 0;
    m_max_backward = 0;
    m_space_of_front_rear = 0;
    m_capacity = 0;
}

ShadowVertexes& ShadowVertexes::set_vertexes(int offset,
                                             float start_x, float start_y,
                                             float end_x, float end_y)
{
    m_vertexes[offset++] = start_x;
    m_vertexes[offset++] = start_y;
    m_vertexes[offset++] = color.start_color;
    m_vertexes[offset++] = color.start_alpha;
    m_vertexes[offset++] = end_x;
    m_vertexes[offset++] = end_y;
    m_vertexes[offset++] = color.end_color;
    m_vertexes[offset] = color.end_alpha;
    return *this;
}

ShadowVertexes& ShadowVertexes::add_vertexes_backward(float start_x,
                                                      float start_y,
                                                      float end_x,
                                                      float end_y)
{
    m_vertexes[--m_backward] = start_x;
    m_vertexes[--m_backward] = start_y;
    m_vertexes[--m_backward] = color.start_color;
    m_vertexes[--m_backward] = color.start_alpha;
    m_vertexes[--m_backward] = end_x;
    m_vertexes[--m_backward] = end_y;
    m_vertexes[--m_backward] = color.end_color;
    m_vertexes[--m_backward] = color.end_alpha;
    return *this;
}

ShadowVertexes& ShadowVertexes::add_vertexes_forward(float start_x,
                                                     float start_y,
                                                     float end_x,
                                                     float end_y)
{
    m_vertexes[m_forward++] = start_x;
    m_vertexes[m_forward++] = start_y;
    m_vertexes[m_forward++] = color.start_color;
    m_vertexes[m_forward++] = color.start_alpha;
    m_vertexes[m_forward++] = end_x;
    m_vertexes[m_forward++] = end_y;
    m_vertexes[m_forward++] = color.end_color;
    m_vertexes[m_forward++] = color.end_alpha;
    return *this;
}

void ShadowVertexes::draw(ShadowVertexProgram &program)
{
    int count = (m_forward - m_backward) >> 2;
    if (count > 0) {
        glUniformMatrix4fv(program.mvp_matrix_loc(), 1, GL_FALSE,
                           VertexProgram::mvp_matrix);
        glUniform1f(program.vertex_z_loc(), m_vertex_z);

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glVertexAttribPointer(program.vertex_pos_loc(), 4, GL_FLOAT, GL_FALSE,
                              0, m_vertexes);
        glEnableVertexAttribArray(program.vertex_pos_loc());
        glDrawArrays(GL_TRIANGLE_STRIP, 0, count);

        glDisable(GL_BLEND);
    }
}

}

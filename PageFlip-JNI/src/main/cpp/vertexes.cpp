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

#include "vertexes.h"
#include "error.h"

namespace eschao {

Vertexes::Vertexes()
        : m_next(0),
          m_capacity(0),
          m_size_of_per_vex(0),
          m_vertexes(NULL),
          m_tex_coords(NULL) {
}

Vertexes::Vertexes(int capacity, int size_per_vex, bool has_texture) {
    set(capacity, size_per_vex, has_texture);
}

Vertexes::~Vertexes() {
    release();
}

void Vertexes::release() {
    if (m_vertexes) {
        delete m_vertexes[m_capacity * m_size_of_per_vex];
    }

    if (m_tex_coords) {
        delete m_tex_coords[m_capacity << 1];
    }

    m_next = 0;
    m_capacity = 0;
    m_size_of_per_vex = 0;
}

void Vertexes::set(int capacity, int size_of_per_vex, bool has_texture) {
    if (size_of_per_vex < 2) {
        throw PageFlipException(Error::ERR_INVALID_PARAMETER);
    }

    release();
    this->m_capacity = capacity;
    this->m_size_of_per_vex = size_of_per_vex;
    m_vertexes = new float[capacity * size_of_per_vex];

    if (has_texture) {
        m_tex_coords = new float[capacity << 1];
    }
}

Vertexes &Vertexes::add_vertex(float x, float y, float z) {
    m_vertexes[m_next++] = x;
    m_vertexes[m_next++] = y;
    m_vertexes[m_next++] = z;
    return *this;
}

Vertexes &Vertexes::add_vertex(float x, float y, float z,
                               float tx, float ty) {
    int j = m_next / m_size_of_per_vex * 2;
    m_vertexes[m_next++] = x;
    m_vertexes[m_next++] = y;
    m_vertexes[m_next++] = z;

    m_tex_coords[j++] = tx;
    m_tex_coords[j] = ty;
    return *this;
}

Vertexes &Vertexes::add_vertex(float x, float y, float z, float w) {
    m_vertexes[m_next++] = x;
    m_vertexes[m_next++] = y;
    m_vertexes[m_next++] = z;
    m_vertexes[m_next++] = w;
    return *this;
}

Vertexes &Vertexes::add_vertex(float x, float y, float z, float w,
                               float tx, float ty) {
    int j = m_next / m_size_of_per_vex * 2;
    m_vertexes[m_next++] = x;
    m_vertexes[m_next++] = y;
    m_vertexes[m_next++] = z;
    m_vertexes[m_next++] = w;

    m_tex_coords[j++] = tx;
    m_tex_coords[j] = ty;
    return *this;
}

Vertexes &Vertexes::add_vertex(GLPoint &p) {
    int j = m_next / m_size_of_per_vex * 2;
    m_vertexes[m_next++] = p.x;
    m_vertexes[m_next++] = p.y;
    m_vertexes[m_next++] = p.z;

    m_tex_coords[j++] = p.tex_x;
    m_tex_coords[j] = p.tex_y;
    return *this;
}

void Vertexes::draw_with(GLenum type, GLint vertex_pos_loc,
                         GLint tex_coord_loc) {
    glVertexAttribPointer(vertex_pos_loc, m_size_of_per_vex, GL_FLOAT,
                          GL_FALSE, 0, m_vertexes);
    glEnableVertexAttribArray(vertex_pos_loc);

    glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT, GL_FALSE,
                          0, m_tex_coords);
    glEnableVertexAttribArray(tex_coord_loc);

    glDrawArrays(type, 0, count());
}

void Vertexes::draw_with(GLenum type, GLint vertex_pos_loc, GLint tex_coord_loc,
                         int offset, int length) {
    glVertexAttribPointer(vertex_pos_loc, m_size_of_per_vex, GL_FLOAT,
                          GL_FALSE, 0, m_vertexes);
    glEnableVertexAttribArray(vertex_pos_loc);

    glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT, GL_FALSE,
                          0, m_tex_coords);
    glEnableVertexAttribArray(tex_coord_loc);

    glDrawArrays(type, offset, length);
}

}

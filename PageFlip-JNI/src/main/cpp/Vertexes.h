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

#ifndef ANDROID_PAGEFLIP_VERTEXES_H
#define ANDROID_PAGEFLIP_VERTEXES_H

#include <GLES2/gl2.h>
#include "gl_point.h"

class Vertexes {

public:
    Vertexes();
    Vertexes(int capacity, int size_of_per_vex, bool has_texture = false);
    virtual ~Vertexes();

    void release();
    int set(int capacity, int size_of_per_vex, bool has_texture = false);
    Vertexes& add_vertex(float x, float y, float z);
    Vertexes& add_vertex(float x, float y, float z, float w);
    Vertexes& add_vertex(float x, float y, float z, float tx, float ty);
    Vertexes& add_vertex(float x, float y, float z, float w,
                         float tx, float ty);
    Vertexes& add_vertex(GLPoint &p);

    void draw_with(GLenum type, GLint vertex_pos_loc, GLint tex_coord_loc);
    void draw_with(GLenum type, GLint vertex_pos_loc, GLint tex_coord_loc,
                   int offset, int length);

    // inline
    inline int capacity()
    {
        return m_capacity;
    };

    inline int count()
    {
        return m_next / m_size_of_per_vex;
    }

    inline int size_of_per_vex()
    {
        return m_size_of_per_vex;
    }

    inline void reset()
    {
        m_next = 0;
    }

    inline float float_at(int index)
    {
        return (index >= 0 && index < m_next) ? m_vertexes[index] : 0;
    }

protected:
    int m_size_of_per_vex;
    int m_capacity;
    int m_next;

    float *m_vertexes;
    float *m_tex_coords;
};

#endif //ANDROID_PAGEFLIP_VERTEXES_H

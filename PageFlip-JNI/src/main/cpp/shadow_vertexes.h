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

#ifndef ANDROID_PAGEFLIP_SHADOWVERTEXES_H
#define ANDROID_PAGEFLIP_SHADOWVERTEXES_H


#include "shadow_color.h"

class ShadowVertexProgram;

class ShadowVertexes {

public:
    ShadowVertexes(int space_of_front_rear,
                   float start_color, float start_alpha,
                   float end_color, float end_alpha);
    ~ShadowVertexes();

    void release();
    void set(int capacity);
    ShadowVertexes& set_vertexes(int offset,
                                 float start_x, float start_y,
                                 float end_x, float end_y);
    ShadowVertexes& add_vertexes_backward(float start_x, float start_y,
                                          float end_x, float end_y);
    ShadowVertexes& add_vertexes_forward(float start_x, float start_y,
                                         float end_x, float end_y);
    void draw(ShadowVertexProgram& program);

    // inline
    inline void reset()
    {
        m_vertex_z = 0;
        m_backward = m_max_backward;
        m_forward = m_max_backward + (m_space_of_front_rear << 2);
    }

    inline int max_backward()
    {
        return m_max_backward;
    }

    inline void set_vertex_z(float z)
    {
        m_vertex_z = z;
    }

    inline ShadowVertexes& add_vertexes(bool is_forward,
                                        float start_x, float start_y,
                                        float end_x, float end_y)
    {
        return is_forward ?
               add_vertexes_forward(start_x, start_y, end_x, end_y) :
               add_vertexes_backward(start_x, start_y, end_x, end_y);
    }

public:
    ShadowColor color;

protected:
    int m_capacity;
    int m_space_of_front_rear;
    int m_backward;
    int m_forward;
    int m_max_backward;

    float m_vertex_z;
    float *m_vertexes;
};


#endif //ANDROID_PAGEFLIP_SHADOWVERTEXES_H

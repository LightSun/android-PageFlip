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

#include <algorithm>
#include <math.h>
#include <android/bitmap.h>
#include "page.h"
#include "utility.h"

using namespace std;

namespace eschao {

static const size_t kSizeOf3Float = sizeof(float) * 3;

void Textures::set_first_texture_with_second()
{
    if (m_is_used[FIRST_TEXTURE_ID]) {
        m_unused_ids[m_unused_ids_size++] = m_used_ids[FIRST_TEXTURE_ID];
    }

    memcpy(mask_color[FIRST_TEXTURE_ID], mask_color[SECOND_TEXTURE_ID],
           kSizeOf3Float);
    m_used_ids[FIRST_TEXTURE_ID] = m_used_ids[SECOND_TEXTURE_ID];
    m_is_used[SECOND_TEXTURE_ID] = false;
}

void Textures::set_second_texture_with_first()
{
    if (m_is_used[SECOND_TEXTURE_ID]) {
        m_unused_ids[m_unused_ids_size++] = m_used_ids[SECOND_TEXTURE_ID];
    }

    memcpy(mask_color[SECOND_TEXTURE_ID], mask_color[FIRST_TEXTURE_ID],
           kSizeOf3Float);
    m_used_ids[SECOND_TEXTURE_ID] = m_used_ids[FIRST_TEXTURE_ID];
    m_is_used[FIRST_TEXTURE_ID] = false;
}

void Textures::swap_textures_with(Textures &rhs)
{
    m_unused_ids[m_unused_ids_size++] = m_used_ids[SECOND_TEXTURE_ID];
    m_used_ids[SECOND_TEXTURE_ID] = m_used_ids[FIRST_TEXTURE_ID];

    m_unused_ids[m_unused_ids_size++] = m_used_ids[BACK_TEXTURE_ID];
    m_used_ids[BACK_TEXTURE_ID] = rhs.m_used_ids[FIRST_TEXTURE_ID];

    m_used_ids[FIRST_TEXTURE_ID] = rhs.m_used_ids[BACK_TEXTURE_ID];
    rhs.m_is_used[BACK_TEXTURE_ID] = false;

    rhs.m_used_ids[FIRST_TEXTURE_ID] = rhs.m_used_ids[SECOND_TEXTURE_ID];
    rhs.m_is_used[SECOND_TEXTURE_ID] = false;
}

int Textures::set_texture(int index, AndroidBitmapInfo& info, GLvoid* data)
{
    int color = compute_average_color(info, data, 30);
    mask_color[index][0] = RED(color) / 255.0f;
    mask_color[index][1] = GREEN(color) / 255.0f;
    mask_color[index][2] = BLUE(color) / 255.0f;

    GLint format;
    GLenum type;
    if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        format = GL_RGB;
        type = GL_UNSIGNED_SHORT_5_6_5;
    }
    else if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
    }
    else {
        return Error::ERR_UNSUPPORT_BITMAP_FORMAT;
    }

    GLuint id;
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, info.width, info.height, 0, format,
                 type, data);
    m_used_ids[index] = id;
    m_is_used[index] = true;
    return Error::OK;
}

const int Page::m_page_apex_orders[][4] = {
        {0, 1, 2, 3}, // for case A
        {1, 0, 3, 2}, // for case B
        {2, 3, 0, 1}, // for case C
        {3, 2, 1, 0}, // for case D
};

const int Page::m_fold_vex_orders[][5] = {
        {4, 3, 1, 2, 0}, // Case A
        {3, 3, 2, 0, 1}, // Case B
        {3, 2, 1, 3, 0}, // Case C
        {2, 2, 3, 1, 0}, // Case D
        {1, 0, 1, 3, 2}, // Case E
};

Page::Page()
{
    init(0, 0, 0, 0);
}

Page::Page(float left, float right, float top, float bottom)
{
    init(left, right, top, bottom);
}

void Page::init(float left, float right, float top, float bottom)
{
    m_top = top;
    m_left = left;
    m_right = right;
    m_bottom = bottom;

    m_width = right - left;
    m_height = top - bottom;

    m_tex_width = m_width;
    m_tex_height = m_height;

    m_front_vertex_count = 0;
    m_apex_order_index = 0;

    build_vertexes_of_full_page();
}

void Page::compute_index_of_apex_order()
{
    m_apex_order_index = 0;
    if (m_origin_p.x < m_right && m_origin_p.y < 0) {
        m_apex_order_index = 3;
    }
    else {
        if (m_origin_p.y > 0) {
            m_apex_order_index++;
        }
        if (m_origin_p.x < m_right) {
            m_apex_order_index++;
        }
    }
}

void Page::set_origin_diagonal_points(bool has_second_page, bool is_top_area)
{
    if (has_second_page && m_left < 0) {
        m_origin_p.x = m_left;
        m_diagonal_p.x = m_right;
    }
    else {
        m_origin_p.x = m_right;
        m_diagonal_p.x = m_left;
    }

    if (is_top_area) {
        m_origin_p.y = m_bottom;
        m_diagonal_p.y = m_top;
    }
    else {
        m_origin_p.y = m_top;
        m_diagonal_p.y = m_bottom;
    }

    compute_index_of_apex_order();

    m_origin_p.tex_x = (m_origin_p.x - m_left) / m_tex_width;
    m_origin_p.tex_y = (m_top - m_origin_p.y) / m_tex_height;
    m_diagonal_p.tex_x = (m_diagonal_p.x - m_left) / m_tex_width;
    m_diagonal_p.tex_y = (m_top - m_diagonal_p.y) / m_tex_height;
}

void Page::invert_y_of_origin_p()
{
    swap(m_origin_p.y, m_diagonal_p.y);
    swap(m_origin_p.tex_y, m_diagonal_p.tex_y);
    compute_index_of_apex_order();
}

void Page::draw_front_page(VertexProgram &program, Vertexes &vertexes)
{
    // 1. draw unfold part and curled part with the first texture
    glUniformMatrix4fv(program.mvp_matrix_loc(), 1, GL_FALSE,
                       VertexProgram::mvp_matrix);
    glBindTexture(GL_TEXTURE_2D, textures.m_used_ids[FIRST_TEXTURE_ID]);
    glUniform1i(program.texture_loc(), 0);
    vertexes.draw_with(GL_TRIANGLE_STRIP,
                       program.vertex_pos_loc(), program.tex_coord_loc(),
                       0, m_front_vertex_count);

    // 2. draw the second texture
    glBindTexture(GL_TEXTURE_2D, textures.m_used_ids[SECOND_TEXTURE_ID]);
    glUniform1i(program.texture_loc(), 0);
    glDrawArrays(GL_TRIANGLE_STRIP, m_front_vertex_count,
                 vertexes.count() - m_front_vertex_count);
}

void Page::draw_full_page(VertexProgram &program, GLuint texture_id)
{
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glUniform1i(program.texture_loc(), 0);

    glVertexAttribPointer(program.vertex_pos_loc(), 3, GL_FLOAT, GL_FALSE, 0,
                          m_apexes);
    glEnableVertexAttribArray(program.vertex_pos_loc());

    glVertexAttribPointer(program.tex_coord_loc(), 2, GL_FLOAT, GL_FALSE, 0,
                          m_apex_tex_coords);
    glEnableVertexAttribArray(program.tex_coord_loc());

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Page::build_vertexes_of_page_when_vertical(Vertexes &front_vertexes,
                                                PointF &x_fold_p1)
{
    // if xFoldX and yFoldY are both outside the page, use the last vertex
    // order to draw page
    int index = 4;

    // compute xFoldX and yFoldY points
    if (!is_x_outside_page(x_fold_p1.x)) {
        // use the case B of vertex order to draw page
        index = 1;
        float cx = texture_x(x_fold_p1.x);
        m_x_fold_p.set(x_fold_p1.x, m_origin_p.y, 0, cx, m_origin_p.tex_y);
        m_y_fold_p.set(x_fold_p1.x, m_diagonal_p.y, 0, cx, m_diagonal_p.tex_y);
    }

    // get apex order and fold vertex order
    const int *apex_order = m_page_apex_orders[m_apex_order_index];
    const int *vex_order = m_fold_vex_orders[index];

    // need to draw first texture, add xFoldX and yFoldY first. Remember
    // the adding order of vertex in float buffer is X point prior to Y
    // point
    if (vex_order[0] > 1) {
        front_vertexes.add_vertex(m_x_fold_p).add_vertex(m_y_fold_p);
    }

    // add the leftover m_vertexes for the first texture
    for (int i = 1; i < vex_order[0]; ++i) {
        int k = apex_order[vex_order[i]];
        int m = k * 3;
        int n = k << 1;
        front_vertexes.add_vertex(m_apexes[m], m_apexes[m + 1], 0,
                                  m_apex_tex_coords[n],
                                  m_apex_tex_coords[n + 1]);
    }

    // the vertex size for drawing front of fold page and first texture
    m_front_vertex_count = front_vertexes.count();

    // if xFoldX and yFoldY are in the page, need add them for drawing the
    // second texture
    if (vex_order[0] > 1) {
        m_x_fold_p.z = m_y_fold_p.z = -1;
        front_vertexes.add_vertex(m_x_fold_p).add_vertex(m_y_fold_p);
    }

    // add the remaining m_vertexes for the second texture
    for (int i = vex_order[0]; i < VEX_ORDER_LEN; ++i) {
        int k = apex_order[vex_order[i]];
        int m = k * 3;
        int n = k << 1;
        front_vertexes.add_vertex(m_apexes[m], m_apexes[m + 1], -1,
                                  m_apex_tex_coords[n],
                                  m_apex_tex_coords[n + 1]);
    }
}

void Page::build_vertexes_of_page_when_slope(Vertexes &front_vertexes,
                                             PointF &x_fold_p1,
                                             PointF &y_fold_p1,
                                             float k_value)
{
    // compute xFoldX point
    float half_h = m_height * 0.5f;
    int index = 0;
    m_x_fold_p.set(x_fold_p1.x, m_origin_p.y, 0,
                   texture_x(x_fold_p1.x), m_origin_p.tex_y);
    if (is_x_outside_page(x_fold_p1.x)) {
        index = 2;
        m_x_fold_p.x = m_diagonal_p.x;
        m_x_fold_p.y = m_origin_p.y + (m_x_fold_p.x - m_diagonal_p.x) / k_value;
        m_x_fold_p.tex_x = m_diagonal_p.tex_x;
        m_x_fold_p.tex_y = texture_y(m_x_fold_p.y);
    }

    // compute yFoldY point
    m_y_fold_p.set(m_origin_p.x, y_fold_p1.y, 0, m_origin_p.tex_x,
                   texture_y(y_fold_p1.y));
    if (fabs(y_fold_p1.y) > half_h) {
        index++;
        m_y_fold_p.x = m_origin_p.x + k_value * (y_fold_p1.y - m_diagonal_p.y);
        if (is_x_outside_page(m_y_fold_p.x)) {
            index++;
        }
        else {
            m_y_fold_p.y = m_diagonal_p.y;
            m_y_fold_p.tex_x = texture_x(m_y_fold_p.x);
            m_y_fold_p.tex_y = m_diagonal_p.tex_y;
        }
    }

    // get apex order and fold vertex order
    const int* const apex_order = m_page_apex_orders[m_apex_order_index];
    const int* const vex_order = m_fold_vex_orders[index];

    // need to draw first texture, add xFoldX and yFoldY first. Remember
    // the adding order of vertex in float buffer is X point prior to Y
    // point
    if (vex_order[0] > 1) {
        front_vertexes.add_vertex(m_x_fold_p).add_vertex(m_y_fold_p);
    }

    // add the leftover m_vertexes for the first texture
    for (int i = 1; i < vex_order[0]; ++i) {
        int k = apex_order[vex_order[i]];
        int m = k * 3;
        int n = k << 1;
        front_vertexes.add_vertex(m_apexes[m], m_apexes[m + 1], 0,
                                  m_apex_tex_coords[n],
                                  m_apex_tex_coords[n + 1]);
    }

    // the vertex size for drawing front of fold page and first texture
    m_front_vertex_count = front_vertexes.count();

    // if xFoldX and yFoldY are in the page, need add them for drawing the
    // second texture
    if (vex_order[0] > 1) {
        m_x_fold_p.z = m_y_fold_p.z = -1;
        front_vertexes.add_vertex(m_x_fold_p).add_vertex(m_y_fold_p);
    }

    // add the remaining m_vertexes for the second texture
    for (int i = vex_order[0]; i < VEX_ORDER_LEN; ++i) {
        int k = apex_order[vex_order[i]];
        int m = k * 3;
        int n = k << 1;
        front_vertexes.add_vertex(m_apexes[m], m_apexes[m + 1], -1,
                                  m_apex_tex_coords[n],
                                  m_apex_tex_coords[n + 1]);
    }
}

void Page::build_vertexes_of_full_page()
{
    int i = 0;
    int j = 0;

    m_apexes[i++] = m_right;
    m_apexes[i++] = m_bottom;
    m_apexes[i++] = 0;
    m_apex_tex_coords[j++] = texture_x(m_right);
    m_apex_tex_coords[j++] = texture_y(m_bottom);

    m_apexes[i++] = m_right;
    m_apexes[i++] = m_top;
    m_apexes[i++] = 0;
    m_apex_tex_coords[j++] = texture_x(m_right);
    m_apex_tex_coords[j++] = texture_y(m_top);

    m_apexes[i++] = m_left;
    m_apexes[i++] = m_top;
    m_apexes[i++] = 0;
    m_apex_tex_coords[j++] = texture_x(m_left);
    m_apex_tex_coords[j++] = texture_y(m_top);

    m_apexes[i++] = m_left;
    m_apexes[i++] = m_bottom;
    m_apexes[i] = 0;
    m_apex_tex_coords[j++] = texture_x(m_left);
    m_apex_tex_coords[j] = texture_y(m_bottom);
}

}

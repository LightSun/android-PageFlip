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

#ifndef ANDROID_PAGEFLIP_PAGE_H
#define ANDROID_PAGEFLIP_PAGE_H

#include <GLES2/gl2.h>
#include <string.h>
#include <jni.h>
#include "gl_point.h"
#include "vertex_program.h"
#include "vertexes.h"
#include "pointf.h"

#define FIRST_TEXTURE_ID    0
#define SECOND_TEXTURE_ID   1
#define BACK_TEXTURE_ID     2
#define TEXTURE_SIZE        3

class PageFlip;

/**
 * Class Textures
 */
class Textures {

public:
    Textures::Textures() : unused_ids_size(0)
    {
        memset(is_used, false, sizeof(bool)*TEXTURE_SIZE);
    }

    void set_first_texture_with_second();
    void set_second_texture_with_first();
    void swap_textures_with(Textures& rhs);
    void set_first_texture(jobject bitmap);
    void set_second_texture(jobject bitmap);
    void set_back_texture(jobject bitmap);

    inline GLuint back_texture_id()
    {
        return is_used[BACK_TEXTURE_ID] ?
               used_ids[BACK_TEXTURE_ID] :
               used_ids[FIRST_TEXTURE_ID];
    }

    inline bool is_first_texture_set()
    {
        return is_used[FIRST_TEXTURE_ID];
    }

    inline bool is_second_texture_set()
    {
        return is_used[SECOND_TEXTURE_ID];
    }

    inline bool is_back_texture_set()
    {
        return is_used[BACK_TEXTURE_ID];
    }

    inline void delete_unused_textures()
    {
        if (unused_ids_size > 0) {
            glDeleteTextures(unused_ids_size, unused_ids);
            unused_ids_size = 0;
        }
    }

    inline void delete_all()
    {
        glDeleteTextures(TEXTURE_SIZE, used_ids);
        memset(is_used, false, sizeof(bool)*TEXTURE_SIZE);
    }

public:
    float mask_color[TEXTURE_SIZE][3];

private:
    GLuint used_ids[TEXTURE_SIZE];
    GLuint unused_ids[TEXTURE_SIZE];
    bool is_used[TEXTURE_SIZE];
    int unused_ids_size;

    friend class Page;
};

/**
 * Class Page
 */
class Page {

public:
    Page();
    Page(float left, float right, float top, float bottom);

    void set_origin_diagonal_points(bool has_second_page, bool is_top_area);
    void invert_y_of_origin_p();
    void draw_front_page(VertexProgram& program, Vertexes& vertexes);
    void build_vertexes_of_page_when_veritcal(Vertexes& front_vertexes,
                                              PointF& x_fold_p1);
    void build_vertexes_of_page_when_slope(Vertexes& front_vertexes,
                                           PointF& x_fold_p1,
                                           PointF& y_fold_p1,
                                           float k_value);
    void build_vertexes_of_full_page();

    inline float width()
    {
        return m_width;
    }

    inline float height()
    {
        return m_height;
    }

    inline bool is_left_page()
    {
        return m_right <= 0;
    }

    inline bool is_right_page()
    {
        return m_left >= 0;
    }

    inline bool contains(float x, float y)
    {
        return m_left < m_right && m_bottom < m_top &&
               m_left <= x && x < m_right &&
               m_bottom <= y && y < m_top;
    }

    inline bool is_x_in_range(float x, float ratio)
    {
        const float w = m_width * ratio;
        return (m_origin_p.x < 0) ? x < (m_origin_p.x + w) : x > (m_origin_p.x - w);
    }

    inline bool is_x_outside_page(float x)
    {
        return (m_origin_p.x < 0) ? x > m_diagonal_p.x : x < m_diagonal_p.x;
    }

    inline float texture_x(float x)
    {
        return (x - m_left) ? m_tex_width;
    }

    inline float texture_y(float y)
    {
        return (m_top - y ) / m_tex_height;
    }

    inline void draw_full_page(VertexProgram& program, bool is_first)
    {
        is_first ?
            draw_full_page(program, textures.used_ids[FIRST_TEXTURE_ID]) :
            draw_full_page(program, textures.used_ids[SECOND_TEXTURE_ID]);
    }

private:
    void init(float left, float right, float top, float bottom);
    void compute_index_of_apex_order();
    void draw_full_page(VertexProgram& program, GLuint texture_id);

public:
    Textures textures;

private:

    /**
     * <p>
     * 4 apexes of page has different permutation order according to original
     * point since original point will be changed when user click to curl page
     * from different direction. There are 4 kinds of order:
     * </p><pre>
     *   A           B           C           D
     * 2    1      3    0      0    3      1    2
     * +----+      +----+      +----+      +----+
     * |    |      |    |      |    |      |    |
     * +----+      +----+      +----+      +----+
     * 3    0      2    1      1    2      0    3
     *             From A      From A      From A
     *             0 <-> 1     0 <-> 2     0 <-> 3
     *             3 <-> 2     3 <-> 1     1 <-> 2
     * </pre>
     * <ul>
     *      <li>0 always represents the origin point, accordingly 2 is diagonal
     *      point</li>
     *      <li>Case A is default order: 0 -> 1 -> 2 -> 3</li>
     *      <li>Every apex data is stored in mApexes following the case A order
     *      and never changed</li>
     *      <li>This array is mapping apex order (case A - D) to real apex data
     *      stored in mApexes. For example:
     *      <ul>
     *          <li>Case A has same order with storing sequence of apex data in
     *          mApexes</li>
     *          <li>Case B: the 0 apex is stored in 1 position in mApexes</li>
     *      </ul></li>
     *  </ul>
     */
    static const int m_page_apex_orders[][4] = {
            {0, 1, 2, 3}, // for case A
            {1, 0, 3, 2}, // for case B
            {2, 3, 0, 1}, // for case C
            {3, 2, 1, 0}, // for case D
    };

    /**
     * <p>When page is curled, there are 4 kinds of m_vertexes orders for drawing
     * first texture and second texture with TRIANGLE_STRIP way</p><pre>
     *     A             B              C              D
     * 2       1     2     X 1      2 X     1      2       1
     * +-------+     +-----.-+      +-.-----+      +-------+
     * |       |     | F  /  |      |/      |      |   F   |
     * |   F   .Y    |   /   |     Y.   S   |     X.-------.Y
     * |      /|     |  /    |      |       |      |   S   |
     * +-----.-+     +-.-----+      +-------+      +-------+
     * 3    X  0     3 Y     0      3       0      3       0
     * </pre>
     * <ul>
     *      <li>All cases are based on the apex order case A(0 -> 1 -> 2 -> 3)
     *      </li>
     *      <li>F means the first texture area, S means the second texture area
     *      </li>
     *      <li>X is xFoldX point, Y is yFoldY point</li>
     *      <li>Case A means: xFoldX and yFoldY are both in page</li>
     *      <li>Case B means: xFoldX is in page, but yFoldY is the intersecting
     *      point with line 1->2 since yFoldY is outside the page</li>
     *      <li>Case C means: xFoldX and yFoldY are both outside the page</li>
     *      <li>Case D means: xFoldX outside page but yFoldY is in the page</li>
     *      <li>Combining {@link #mPageApexOrders} with this array, we can get
     *      the right apex data from mApexes array which will help us quickly
     *      organizing triangle data for openGL drawing</li>
     *      <li>The last array(Case E) in this array means: xFoldX and yFoldY
     *      are both outside the page and the whole page will be draw with
     *      second texture</li>
     * </ul>
     */
    static const int m_fold_vex_orders[][5] = {
            {4, 3, 1, 2, 0}, // Case A
            {3, 3, 2, 0, 1}, // Case B
            {3, 2, 1, 3, 0}, // Case C
            {2, 2, 3, 1, 0}, // Case D
            {1, 0, 1, 3, 2}, // Case E
    };

    static const int VEX_ORDER_LEN = sizeof(m_fold_vex_orders[0]) / sizeof(int);

    // page size
    float m_left;
    float m_right;
    float m_top;
    float m_bottom;
    float m_width;
    float m_height;

    // texture size for rendering page, normally they are same with page width
    // and height
    float m_tex_width;
    float m_tex_height;

    /**
     * <p>origin point and diagonal point</p>
     * <pre>
     * 0-----+
     * |     |
     * |     |
     * +-----1
     * </pre>
     * <p>if origin(x, y) is 1, the diagonal(x, y) is 0</p>
     */
    GLPoint m_origin_p;
    GLPoint m_diagonal_p;

    GLPoint m_x_fold_p;
    GLPoint m_y_fold_p;

    // storing 4 apexes data of page
    float m_apexes[12];
    // texture coordinates for page apex
    float m_apex_tex_coords[8];
    // vertex size of front of fold page and unfold page
    int m_front_vertex_count;
    // index of apex order array for current original point
    int m_apex_order_index;

    // mask color of back texture
    float m_mask_color[3][3];

    friend class PageFlip;
};

#endif //ANDROID_PAGEFLIP_PAGE_H

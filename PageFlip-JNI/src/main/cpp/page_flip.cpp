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

#include <math.h>
#include <GLES2/gl2.h>
#include <algorithm>
#include "page.h"
#include "page_flip.h"

PageFlip::PageFlip()
        : m_flip_state(END_FLIP),
          m_is_vertical(false),
          m_pixels_of_mesh(kMeshVertexPixels),
          m_semi_perimeter_ratio(0.8f),
          m_is_click_to_flip(true),
          m_width_ratio_of_click_to_flip(kWidthRatioOfClickToFlip),
          m_page_mode(SINGLE_PAGE_MODE),
          m_first_page(NULL),
          m_second_page(NULL),
          m_fold_edge_shadow_width(5, 30, 0.25f),
          m_fold_base_shadow_width(2, 40, 0.4f),
          m_fold_edge_shadow_vertexes(kFoldTopEdgeShadowVexCount,
                                      kFoldEdgeShadowStartColor,
                                      kFoldEdgeShadowStartAlpha,
                                      kFoldEdgeShadowEndColor,
                                      kFoldEdgeShadowEndAlpha),
          m_fold_base_shadow_vertexes(0,
                                      kFoldBaseShadowStartColor,
                                      kFoldBaseShadowStartAlpha,
                                      kFoldBaseShadowEndColor,
                                      kFoldBaseShadowEndAlpha)
{
}

PageFlip::~PageFlip()
{
    if (m_first_page) {
        delete m_first_page;
    }

    if (m_second_page) {
        delete m_second_page;
    }
}

bool PageFlip::enable_auto_page(bool is_auto)
{
    PageMode new_mode = is_auto ? AUTO_PAGE_MODE : SINGLE_PAGE_MODE;
    if (m_page_mode != new_mode) {
        m_page_mode = new_mode;

        if ((new_mode == AUTO_PAGE_MODE &&
             m_view_rect.surface_width > m_view_rect.surface_height &&
             m_second_page == NULL) ||
            (new_mode == SINGLE_PAGE_MODE && m_second_page != NULL)) {
            create_pages();
            return true;
        }
    }

    return false;
}

void PageFlip::on_surface_created()
{
    glClearColor(0, 0, 0, 1f);
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);

    m_vertex_prog.init();
    m_shadow_vertex_prog.init();
    m_back_of_fold_vertex_prog.init();
}

void PageFlip::on_surface_changed(int width, int height)
{
    m_view_rect.set(width, height);
    glViewport(0, 0, width, height);
    m_vertex_prog.init_matrix(-m_view_rect.half_width,
                              m_view_rect.half_width,
                              -m_view_rect.half_height,
                              m_view_rect.half_height);
    compute_max_mesh_count();
    create_pages();
}

void PageFlip::create_pages()
{
    if (m_first_page) {
        m_first_page->textures.delete_all();
    }

    if (m_second_page) {
        m_second_page->textures.delete_all();
    }

    if (m_page_mode == AUTO_PAGE_MODE &&
        m_view_rect.surface_width > m_view_rect.surface_height) {
        m_first_page = new Page(m_view_rect.left, 0, m_view_rect.top,
                                m_view_rect.bottom);
        m_second_page = new Page(0, m_view_rect.right, m_view_rect.top,
                                 m_view_rect.bottom);
    }
    else {
        m_first_page = new Page(m_view_rect.left, m_view_rect.right,
                                m_view_rect.top, m_view_rect.bottom);
        m_second_page = NULL;
    }
}

void PageFlip::on_finger_down(float x, float y)
{
    x = m_view_rect.to_opengl_x(x);
    y = m_view_rect.to_opengl_y(y);

    bool is_contained = m_first_page->contains(x, y);
    if (!is_contained && m_second_page &&
        m_second_page->contains(x, y)) {
        is_contained = true;
        std::swap(m_first_page, m_second_page);
    }

    if (is_contained) {
        m_max_t2o_tan_a = 0;
        m_max_t2d_tan_a = 0;
        m_last_touch_p.set(x, y);
        m_start_touch_p.set(x, y);
        m_touch_p.set(x, y);
        m_flip_state = BEGIN_FLIP;
    }
}

bool PageFlip::on_finger_move(float x, float y)
{
    x = m_view_rect.to_opengl_x(x);
    y = m_view_rect.to_opengl_y(y);

    float dy = (y - m_start_touch_p.y);
    float dx = (x - m_start_touch_p.x);

    Page& page = *m_first_page;
    const GLPoint& origin_p = page.m_origin_p;
    const GLPoint& diagonal_p = page.m_diagonal_p;

    if (m_flip_state == BEGIN_FLIP && (fabs(dx) > m_view_rect.half_width)) {
        page.set_origin_diagonal_points(m_second_page != NULL, dy > 0);

        // compute max degree between X axis and line from TouchP to OriginP
        // and max degree between X axis and line from TouchP to
        // (OriginP.x, DiagonalP.Y)
        float y2o = (float)fabs(m_start_touch_p.y - origin_p.y);
        float y2d = (float)fabs(m_start_touch_p.y - diagonal_p.y);
        m_max_t2o_tan_a = compute_tan_of_curl_angle(y2o);
        m_max_t2d_tan_a = compute_tan_of_curl_angle(y2d);

        // moving at the top and bottom screen have different tan value of
        // angle
        if ((origin_p.y < 0 && page.m_right > 0) ||
            (origin_p.y > 0 && page.m_right <= 0)) {
            m_max_t2o_tan_a = -m_max_t2o_tan_a;
        }
        else {
            m_max_t2d_tan_a = -m_max_t2d_tan_a;
        }

        // determine if it is moving m_backward or m_forward
        if (m_second_page == NULL &&
            dx > 0)
            //mListener != null &&
            //mListener.canFlipBackward())
            {
            m_start_touch_p.x = origin_p.x;
            dx = (x - m_start_touch_p.x);
            m_flip_state = BACKWARD_FLIP;
        }
        else if (//mListener != null &&
                 //mListener.canFlipForward() &&
                 (dx < 0 && origin_p.x > 0 || dx > 0 && origin_p.x < 0)) {
            m_flip_state = FORWARD_FLIP;
        }
    }

    // in moving, compute the TouchXY
    if (m_flip_state == FORWARD_FLIP ||
        m_flip_state == BACKWARD_FLIP ||
        m_flip_state == RESTORE_FLIP) {

        // check if page is flipping vertically
        m_is_vertical = fabs(dy) <= 1f;

        // multiply a factor to make sure the touch point is always head of
        // finger point
        if (FORWARD_FLIP == m_flip_state) {
            dx *= 1.2f;
        }
        else {
            dx *= 1.1f;
        }

        // moving direction is changed:
        // 1. invert max curling angle
        // 2. invert Y of original point and diagonal point
        if ((dy < 0 && origin_p.y < 0) || (dy > 0 && origin_p.y > 0)) {
            std::swap(m_max_t2o_tan_a, m_max_t2d_tan_a);
            //float t = mMaxT2DAngleTan;
            //mMaxT2DAngleTan = mMaxT2OAngleTan;
            //mMaxT2OAngleTan = t;
            page.invert_y_of_origin_p();
        }

        // compute new TouchP.y
        float max_y = dx * m_max_t2o_tan_a;
        if (fabs(dy) > fabs(max_y)) {
            dy = max_y;
        }

        // check if XFoldX1 is outside page width, if yes, recompute new
        // TouchP.y to assure the XFoldX1 is in page width
        float t2o_k = dy / dx;
        float x_touch_x = dx + dy * t2o_k;
        float x_ratio = (1 + m_semi_perimeter_ratio) * 0.5f;
        float x_fold_x1 = x_ratio * x_touch_x;
        if (fabs(x_fold_x1) + 2 >= page.m_width) {
            float dy2 = ((diagonal_p.x - origin_p.x) / x_ratio - dx) * dx;
            // ignore current moving if we can't get a valid dy, for example
            // , in double pages mode, when finger is moving from the one
            // page to another page, the dy2 is negative and should be
            // ignored
            if (dy2 < 0) {
                return false;
            }

            double t = sqrt(dy2);
            if (origin_p.y > 0) {
                t = -t;
                dy = (int)ceil(t);
            }
            else {
                dy = (int)floor(t);
            }
        }

        // set touchP(x, y) and middleP(x, y)
        m_last_touch_p.set(x, y);
        m_touch_p.set(dx + origin_p.x, dy + origin_p.y);
        m_middle_p.x = (m_touch_p.x + origin_p.x) * 0.5f;
        m_middle_p.y = (m_touch_p.y + origin_p.y) * 0.5f;

        // continue to compute points to drawing flip
        compute_vertexes_build_page();
        return true;
    }

    return false;
    }
}

bool PageFlip::on_finger_up(float x, float y, int duration) {
    x = m_view_rect.to_opengl_x(x);
    y = m_view_rect.to_opengl_y(y);

    Page& page = *m_first_page;
    const GLPoint& origin_p = page.m_origin_p;
    const GLPoint& diagonal_p = page.m_diagonal_p;
    PointF start(m_touch_p);
    PointF end(0, 0);

    // m_forward flipping
    if (m_flip_state == FORWARD_FLIP) {
        // can't going m_forward, restore current page
        if (page.is_x_in_range(x, kWidthOfRatioOfRestoreFlip)) {
            end.x = origin_p.x;
            m_flip_state = RESTORE_FLIP;
        }
        else if (m_second_page && origin_p.x < 0) {
            end.x = diagonal_p.x + page.m_width;
        }
        else {
            end.x = diagonal_p.x - page.m_width;
        }
        end.y = origin_p.y;
    }
    // m_backward flipping
    else if (m_flip_state == BACKWARD_FLIP) {
        // if not over middle x, change from m_backward to m_forward to restore
        if (!page.is_x_in_range(x, 0.5f)) {
            m_flip_state = FORWARD_FLIP;
            end.set(diagonal_p.x - page.m_width, origin_p.y);
        }
        else {
            m_max_t2o_tan_a = (m_touch_p.y - origin_p.y) /
                                 (m_touch_p.x - origin_p.x);
            end.set((int) origin_p.x, (int) origin_p.y);
        }
    }
    // ready to flip
    else if (m_flip_state == BEGIN_FLIP) {
        m_is_vertical = false;
        m_flip_state = END_FLIP;
        page.set_origin_diagonal_points(m_second_page != NULL, -y > 0);

        // if enable clicking to flip, compute scroller points for animation
        if (m_is_click_to_flip && fabs(x - m_start_touch_p.x) < 2) {
            compute_scroll_points_for_clicking_flip(x, start, end);
        }
    }

    // start scroller for animating
    if (m_flip_state == FORWARD_FLIP ||
        m_flip_state == BACKWARD_FLIP ||
        m_flip_state == RESTORE_FLIP) {
        //mScroller.startScroll(start.x, start.y,
        //                      end.x - start.x, end.y - start.y,
        //                      duration);
        return true;
    }

    return false;
}

bool PageFlip::can_animate(float x, float y) {
    return (m_flip_state == FORWARD_FLIP &&
            !m_first_page->contains(m_view_rect.to_opengl_x(x),
                                   m_view_rect.to_opengl_y(y)));
}

void PageFlip::compute_scroll_points_for_clicking_flip(float x,
                                                       PointF& start,
                                                       PointF& end) {
    Page& page = *m_first_page;
    GLPoint& origin_p = page.m_origin_p;
    GLPoint& diagonal_p = page.m_diagonal_p;
    bool has_second_page = m_second_page != NULL;

    // m_forward and m_backward flip have different degree
    float tan_of_forward_angle = kMaxTanOfForwardFlip;
    float tan_of_backward_angle = kMaxTanOfBackwardFlip;
    if ((origin_p.y < 0 && origin_p.x > 0) ||
        (origin_p.y > 0 && origin_p.x < 0)) {
        tan_of_forward_angle = -tan_of_forward_angle;
        tan_of_backward_angle = -tan_of_backward_angle;
    }

    // m_backward flip
    if (!has_second_page &&
        x < diagonal_p.x + page.m_width * m_width_ratio_of_click_to_flip) //&&
        //mListener != null &&
        //mListener.canFlipBackward()) {
        {
        m_flip_state = BACKWARD_FLIP;
        m_k_value = tan_of_backward_angle;
        start.set(diagonal_p.x,
                  (origin_p.y + (start.x - origin_p.x) * m_k_value));
        end.set(origin_p.x - 5, origin_p.y);
    }
    // m_forward flip
    else if (//mListener != null &&
             //mListener.canFlipForward() &&
             page.is_x_in_range(x, kWidthRatioOfClickToFlip)) {
        m_flip_state = FORWARD_FLIP;
        m_k_value = tan_of_forward_angle;

        // compute start.x
        if (origin_p.x < 0) {
            start.x = (int)(origin_p.x + page.m_width * 0.25f);
        }
        else {
            start.x = (int)(origin_p.x - page.m_width * 0.25f);
        }

        // compute start.y
        start.y = origin_p.y + (start.x - origin_p.x) * m_k_value);

        // compute end.x
        // left page in double page mode
        if (has_second_page && origin_p.x < 0) {
            end.x = diagonal_p.x + page.m_width;
        }
        // right page in double page mode
        else {
            end.x = diagonal_p.x - page.m_width;
        }
        end.y = origin_p.y;
    }
}

/**
 * Compute animating and check if it can continue
 *
 * @return true animating is continue or it is stopped
 */
bool PageFlip::animating()
{
    Page& page = *m_first_page;
    const GLPoint& origin_p = page.m_origin_p;
    const GLPoint& diagonal_p = page.m_diagonal_p;

    // is to end animating?
    bool is_animating = true;//!mScroller.isFinished();
    if (is_animating) {
        // get new (x, y)
        //mScroller.computeScrollOffset();
        //m_touch_p.set(mScroller.getCurrX(), mScroller.getCurrY());

        // for m_backward and restore flip, compute x to check if it can
        // continue to flip
        if (m_flip_state == BACKWARD_FLIP ||
            m_flip_state == RESTORE_FLIP) {
            m_touch_p.y = (m_touch_p.x - origin_p.x) * m_k_value + origin_p.y;
            is_animating = fabs(m_touch_p.x - origin_p.x) > 10;
        }
        // check if flip is vertical
        else {
            m_is_vertical = fabs(m_touch_p.y - origin_p.y) < 1f;
        }

        // compute middle point
        m_middle_p.set((m_touch_p.x + origin_p.x) * 0.5f,
                      (m_touch_p.y + origin_p.y) * 0.5f);

        // compute key points
        if (m_is_vertical) {
            compute_key_vertexes_when_vertical();
        }
        else {
            compute_key_vertexes_when_slope();
        }

        // in double page mode
        if (m_second_page) {
            // if the xFoldP1.x is outside page width, need to limit
            // xFoldP1.x is in page.width and recompute new key points so
            // that the page flip is still going m_forward
            if (page.is_x_outside_page(m_x_fold_p1.x)) {
                m_x_fold_p1.x = diagonal_p.x;
                float cos_A = (m_touch_p.x - origin_p.x) / m_len_of_t2o;
                float ratio = (float)(1 - page.m_width * fabs(cos_A)
                                          / m_len_of_t2o);
                m_radius = (float)(m_len_of_t2o * (1 - 2 * ratio) / M_PI);
                m_x_fold_p0.x = m_len_of_t2o * ratio / cos_A + origin_p.x;

                if (m_is_vertical) {
                    m_y_fold_p0.x = m_x_fold_p0.x;
                    m_y_fold_p1.x = m_x_fold_p1.x;
                }
                else {
                    m_y_fold_p1.y = origin_p.y + (m_x_fold_p1.x - origin_p.x)
                                                / m_k_value;
                    m_y_fold_p0.y = origin_p.y + (m_x_fold_p0.x - origin_p.x)
                                                / m_k_value;
                }

                // re-compute mesh count
                float len = (float)fabs(m_middle_p.x - m_x_fold_p0.x);
                if (m_mesh_count > len) {
                    m_mesh_count = (int)len;
                }
                is_animating = m_mesh_count > 0 &&
                               fabs(m_x_fold_p0.x - diagonal_p.x) >= 2;
            }
        }
        // in single page mode, check if the whole fold page is outside the
        // screen and animating should be stopped
        else if (m_flip_state == FORWARD_FLIP) {
            float r = (float)(m_len_of_t2o * m_semi_perimeter_ratio / M_PI);
            float x = (m_y_fold_p1.y - diagonal_p.y) * m_k_value + r;
            is_animating = x > (diagonal_p.x - origin_p.x);
        }
    }

    // animation is stopped
    if (!is_animating) {
        abort_animating();
    }
    // continue animation and compute m_vertexes
    else if (m_is_vertical) {
        compute_vertexes_when_vertical();
    }
    else {
        compute_vertexes_when_slope();
    }

    return is_animating;
}

/**
 * Abort animating
 */
void PageFlip::abort_animating()
{
    //mScroller.abortAnimation();
    if (m_flip_state == FORWARD_FLIP) {
        m_flip_state = END_WITH_FORWARD;
    }
    else if (m_flip_state == BACKWARD_FLIP) {
        m_flip_state = END_WITH_BACKWARD;
    }
    else if (m_flip_state == RESTORE_FLIP) {
        m_flip_state = END_WITH_RESTORE;
    }
}

/**
 * Draw flipping frame
 */
void PageFlip::draw_flip_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. draw back of fold page
    glUseProgram(m_back_of_fold_vertex_prog.program_ref());
    glActiveTexture(GL_TEXTURE0);
    m_back_of_fold_vertexes.draw(m_back_of_fold_vertex_prog, *m_first_page,
                                m_second_page != NULL, m_gradient_light_tid);

    // 2. draw unfold page and front of fold page
    glUseProgram(m_vertex_prog.program_ref());
    glActiveTexture(GL_TEXTURE0);
    m_first_page->draw_front_page(m_vertex_prog, m_fold_front_vertexes);
    if (m_second_page) {
        m_second_page->draw_full_page(m_vertex_prog, true);
    }

    // 3. draw edge and base shadow of fold parts
    glUseProgram(m_shadow_vertex_prog.program_ref());
    m_fold_base_shadow_vertexes.draw(m_shadow_vertex_prog);
    m_fold_edge_shadow_vertexes.draw(m_shadow_vertex_prog);
}

/**
 * Draw frame with full page
 */
void PageFlip::draw_page_frame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_vertex_prog.program_ref());
    glUniformMatrix4fv(m_vertex_prog.mvp_matrix_loc(), 1, GL_FALSE,
                       m_vertex_prog.mvp_matrix);
    glActiveTexture(GL_TEXTURE0);

    // 1. draw first page
    m_first_page->draw_full_page(m_vertex_prog, true);

    // 2. draw second page if have
    if (m_second_page) {
        m_second_page->draw_full_page(m_vertex_prog, true);
    }
}

/**
 * Compute max mesh count and allocate m_vertexes buffer
 */
void PageFlip::compute_max_mesh_count()
{
    // compute max mesh count
    int max_mesh_cnt = (int)m_view_rect.min_of_width_height()
                       / m_pixels_of_mesh;

    // make sure the vertex count is even number
    if (max_mesh_cnt % 2 != 0) {
        max_mesh_cnt++;
    }

    // init m_vertexes buffers
    m_back_of_fold_vertexes.set(max_mesh_cnt + 2);
    m_fold_front_vertexes.set((max_mesh_cnt << 1) + 8, 3, true);
    m_fold_edge_shadow_vertexes.set(max_mesh_cnt + 2);
    m_fold_base_shadow_vertexes.set(max_mesh_cnt + 2);
}

/**
 * Create gradient shadow texture for lighting effect
 */
void PageFlip::create_gradient_light_texture()
{
    /*
    int textureIDs[] = new int[1];
    glGenTextures(1, textureIDs, 0);
    glActiveTexture(GL_TEXTURE0);
    mGradientShadowTextureID = textureIDs[0];

    // gradient shadow texture
    Bitmap shadow = PageFlipUtils.createGradientBitmap();
    glBindTexture(GL_TEXTURE_2D, mGradientShadowTextureID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLUtils.texImage2D(GL_TEXTURE_2D, 0, shadow, 0);
    shadow.recycle();
     */
}

/**
 * Compute m_vertexes of page
 */
void PageFlip::compute_vertexes_build_page()
{
    if (m_is_vertical) {
        compute_key_vertexes_when_vertical();
        compute_vertexes_when_vertical();
    }
    else {
        compute_key_vertexes_when_slope();
        compute_vertexes_when_slope();
    }
}

/**
 * Compute key m_vertexes when page flip is vertical
 */
void PageFlip::compute_key_vertexes_when_vertical()
{
    const float o_x = m_first_page->m_origin_p.x ;
    const float o_y = m_first_page->m_origin_p.y;
    const float d_y = m_first_page->m_diagonal_p.y;

    m_touch_p.y = o_y;
    m_middle_p.y = o_y;

    // set key point on X axis
    float r0 = 1 - m_semi_perimeter_ratio;
    float r1 = 1 + m_semi_perimeter_ratio;
    m_x_fold_p.set(m_middle_p.x, o_y);
    m_x_fold_p0.set(o_x + (m_x_fold_p.x - o_x) * r0, m_x_fold_p.y);
    m_x_fold_p1.set(o_x + r1 * (m_x_fold_p.x - o_x), m_x_fold_p.y);

    // set key point on Y axis
    m_y_fold_p.set(m_middle_p.x, d_y);
    m_y_fold_p0.set(m_x_fold_p0.x, m_y_fold_p.y);
    m_y_fold_p1.set(m_x_fold_p1.x, m_y_fold_p.y);

    // line length from mTouchP to originP
    m_len_of_t2o = (float)fabs(m_touch_p.x - o_x);
    m_radius = (float)(m_len_of_t2o * m_semi_perimeter_ratio / M_PI);

    // compute mesh count
    compute_max_mesh_count();
}

/**
 * Compute all m_vertexes when page flip is vertical
 */
void PageFlip::compute_vertexes_when_vertical()
{
    float x = m_middle_p.x;
    const float step_x = (m_middle_p.x - m_x_fold_p0.x) / m_mesh_count;

    Page& page = *m_first_page;
    const float o_y = page.m_origin_p.y;
    const float d_y = page.m_diagonal_p.y;
    const float d_ty = page.m_diagonal_p.tex_y;
    const float o_ty = page.m_origin_p.tex_y;
    const float o_tx = page.m_origin_p.tex_x;

    // compute the point on back page half cylinder
    m_back_of_fold_vertexes.reset();

    for (int i = 0; i <= m_mesh_count; ++i, x -= step_x) {
        // compute rad of x point
        float x2t = x - m_x_fold_p1.x;
        float rad = x2t / m_radius;
        float sin_r = (float)sin(rad);
        float tex_x = page.texture_x(x);
        float fx = m_x_fold_p1.x + m_radius * sin_r;
        float fz = (float) (m_radius * (1 - cos(rad)));

        // compute vertex when it is curled
        m_back_of_fold_vertexes.add_vertex(fx, d_y, fz, sin_r, tex_x, d_ty)
                               .add_vertex(fx, o_y, fz, sin_r, tex_x, o_ty);
    }

    float tp_x = m_touch_p.x;
    m_back_of_fold_vertexes.add_vertex(tp_x, d_y, 1, 0, o_tx, d_ty)
                           .add_vertex(tp_x, o_y, 1, 0, o_tx, o_ty);

    // compute shadow width
    float sw = -m_fold_edge_shadow_width.width(m_radius);
    float bw = m_fold_base_shadow_width.width(m_radius);
    if (page.m_origin_p.x < 0) {
        sw = -sw;
        bw = -bw;
    }

    // fold base shadow
    float bx0 = m_back_of_fold_vertexes.float_at(0);
    m_fold_base_shadow_vertexes.set_vertexes(0, bx0, o_y, bx0 + bw, o_y)
                              .set_vertexes(8, bx0, d_y, bx0 + bw, d_y);

    // fold edge shadow
    m_fold_edge_shadow_vertexes.set_vertexes(0, tp_x, o_y, tp_x + sw, o_y)
                              .set_vertexes(8, tp_x, d_y, tp_x + sw, d_y);

    // fold front
    m_fold_front_vertexes.reset();
    page.build_vertexes_of_page_when_veritcal(m_fold_front_vertexes,
                                              m_x_fold_p1);
}

/**
 * Compute key m_vertexes when page flip is slope
 */
void PageFlip::compute_key_vertexes_when_slope()
{
    const float o_x = m_first_page->m_origin_p.x;
    const float o_y = m_first_page->m_origin_p.y;

    const float dx = m_middle_p.x - o_x;
    const float dy = m_middle_p.y - o_y;

    // compute key points on X axis
    float r0 = 1 - m_semi_perimeter_ratio;
    float r1 = 1 + m_semi_perimeter_ratio;
    m_x_fold_p.set(m_middle_p.x + dy * dy / dx, o_y);
    m_x_fold_p0.set(o_x + (m_x_fold_p.x - o_x) * r0, m_x_fold_p.y);
    m_x_fold_p1.set(o_x + r1 * (m_x_fold_p.x - o_x), m_x_fold_p.y);

    // compute key points on Y axis
    m_y_fold_p.set(o_x, m_middle_p.y + dx * dx / dy);
    m_y_fold_p0.set(m_y_fold_p.x, o_y + (m_y_fold_p.y - o_y) * r0);
    m_y_fold_p1.set(m_y_fold_p.x, o_y + r1 * (m_y_fold_p.y - o_y));

    // line length from TouchXY to OriginalXY
    m_len_of_t2o = (float)hypot(m_touch_p.x - o_x, m_touch_p.y - o_y);

    // cylinder radius
    m_radius = (float)(m_len_of_t2o * m_semi_perimeter_ratio / M_PI);

    // compute line slope
    m_k_value = (m_touch_p.y - o_y) / (m_touch_p.x - o_x);

    // compute mesh count
    compute_max_mesh_count();
}

/**
 * Compute back vertex and edge shadow vertex of fold page
 * <p>
 * In 2D coordinate system, for every vertex on fold page, we will follow
 * the below steps to compute its 3D point (x,y,z) on curled page(cylinder):
 * </p>
 * <ul>
 *     <li>deem originP as (0, 0) to simplify the m_next computing steps</li>
 *     <li>translate point(x, y) to new coordinate system
 *     (originP is (0, 0))</li>
 *     <li>rotate point(x, y) with curling angle A in clockwise</li>
 *     <li>compute 3d point (x, y, z) for 2d point(x, y), at this time, the
 *     cylinder is vertical in new coordinate system which will help us
 *     compute point</li>
 *     <li>rotate 3d point (x, y, z) with -A to restore</li>
 *     <li>translate 3d point (x, y, z) to original coordinate system</li>
 * </ul>
 *
 * <p>For point of edge shadow, the most computing steps are same but:</p>
 * <ul>
 *     <li>shadow point is following the page point except different x
 *     coordinate</li>
 *     <li>shadow point has same z coordinate with the page point</li>
 * </ul>
 *
 * @param isX is vertex for x point on x axis or y point on y axis?
 * @param x0 x of point on axis
 * @param y0 y of point on axis
 * @param sx0 x of edge shadow point
 * @param sy0 y of edge shadow point
 * @param tX x of xFoldP1 point in rotated coordinate system
 * @param sinA sin value of page curling angle
 * @param cosA cos value of page curling angel
 * @param tex_x x of texture coordinate
 * @param coordY y of texture coordinate
 * @param oX x of originate point
 * @param oY y of originate point
 */
void PageFlip::compute_back_vertex(bool is_x, float x0, float y0, float sx0,
                                   float sy0, float xfx, float sin_a,
                                   float cos_a, float tex_x, float tex_y,
                                   float o_x, float o_y)
{
    // rotate degree A
    float x = x0 * cos_a - y0 * sin_a;
    float y = x0 * sin_a + y0 * cos_a;

    // rotate degree A for m_vertexes of fold edge shadow
    float sx = sx0 * cos_a - sy0 * sin_a;
    float sy = sx0 * sin_a + sy0 * cos_a;

    // compute mapping point on cylinder
    float rad = (x - xfx) / m_radius;
    double sin_r = sin(rad);
    x = (float) (xfx + m_radius * sin_r);
    float cz = (float) (m_radius * (1 - cos(rad)));

    // rotate degree -A, sin(-A) = -sin(A), cos(-A) = cos(A)
    float cx = x * cos_a + y * sin_a + o_x;
    float cy = y * cos_a - x * sin_a + o_y;
    m_back_of_fold_vertexes.add_vertex(cx, cy, cz, (float)sin_r, tex_x, tex_y);

    // compute coordinates of fold shadow edge
    float s_rad = (sx - xfx) / m_radius;
    sx = (float)(xfx + m_radius * sin(s_rad));
    m_fold_edge_shadow_vertexes.add_vertexes(is_x, cx, cy,
                                             sx * cos_a + sy * sin_a + o_x,
                                             sy * cos_a - sx * sin_a + o_y);
}

/**
 * Compute back vertex of fold page
 * <p>
 * Almost same with another compute_back_vertex function except expunging the
 * shadow point part
 * </p>
 *
 * @param x0 x of point on axis
 * @param y0 y of point on axis
 * @param tX x of xFoldP1 point in rotated coordinate system
 * @param sinA sin value of page curling angle
 * @param cosA cos value of page curling angel
 * @param tex_x x of texture coordinate
 * @param coordY y of texture coordinate
 * @param oX x of originate point
 * @param oY y of originate point
 */
void PageFlip::compute_back_vertex(float x0, float y0, float xfx,
                                   float sin_a, float cos_a, float tex_x,
                                   float tex_y, float o_x, float o_y)
{
    // rotate degree A
    float x = x0 * cos_a - y0 * sin_a;
    float y = x0 * sin_a + y0 * cos_a;

    // compute mapping point on cylinder
    float rad = (x - xfx) / m_radius;
    double sin_r = sin(rad);
    x = (float) (xfx + m_radius * sin_r);
    float cz = (float) (m_radius * (1 - cos(rad)));

    // rotate degree -A, sin(-A) = -sin(A), cos(-A) = cos(A)
    float cx = x * cos_a + y * sin_a + o_x;
    float cy = y * cos_a - x * sin_a + o_y;
    m_back_of_fold_vertexes.add_vertex(cx, cy, cz, (float)sin_r, tex_x, tex_y);
}

/**
 * Compute front vertex and base shadow vertex of fold page
 * <p>The computing principle is almost same with
 * {@link #compute_back_vertex(boolean, float, float, float, float, float,
 * float, float, float, float, float, float)}</p>
 *
 * @param isX is vertex for x point on x axis or y point on y axis?
 * @param x0 x of point on axis
 * @param y0 y of point on axis
 * @param tX x of xFoldP1 point in rotated coordinate system
 * @param sinA sin value of page curling angle
 * @param cosA cos value of page curling angel
 * @param baseWcosA base shadow width * cosA
 * @param baseWsinA base shadow width * sinA
 * @param tex_x x of texture coordinate
 * @param coordY y of texture coordinate
 * @param oX x of originate point
 * @param oY y of originate point
 */
void PageFlip::compute_front_vertex(bool is_x, float x0, float y0, float xfx,
                                    float sin_a, float cos_a,
                                    float base_w_cos_a, float base_w_sin_a,
                                    float tex_x, float tex_y,
                                    float o_x, float o_y, float d_y)
{
    // rotate degree A
    float x = x0 * cos_a - y0 * sin_a;
    float y = x0 * sin_a + y0 * cos_a;

    // compute mapping point on cylinder
    float rad = (x - xfx)/ m_radius;
    x = (float)(xfx + m_radius * sin(rad));
    float cz = (float)(m_radius * (1 - cos(rad)));

    // rotate degree -A, sin(-A) = -sin(A), cos(-A) = cos(A)
    float cx = x * cos_a + y * sin_a + o_x;
    float cy = y * cos_a - x * sin_a + o_y;
    m_fold_front_vertexes.add_vertex(cx, cy, cz, tex_x, tex_y);
    m_fold_base_shadow_vertexes.add_vertexes(is_x, cx, cy,
                                             cx + base_w_cos_a,
                                             cy - base_w_sin_a);
    //String s = isX ? "[X]" : "[Y]";
    //Log.d(TAG, s+" v: "+cx+", "+cy+", "+cz);
}

/**
 * Compute front vertex
 * <p>The difference with another
 * {@link #compute_front_vertex(boolean, float, float, float, float, float,
 * float, float, float, float, float, float, float)} is that it won't
 * compute base shadow vertex</p>
 *
 * @param x0 x of point on axis
 * @param y0 y of point on axis
 * @param tX x of xFoldP1 point in rotated coordinate system
 * @param sinA sin value of page curling angle
 * @param cosA cos value of page curling angel
 * @param tex_x x of texture coordinate
 * @param coordY y of texture coordinate
 * @param oX x of originate point
 * @param oY y of originate point
 */
void PageFlip::compute_front_vertex(float x0, float y0, float xfx,
                                    float sin_a, float cos_a,
                                    float tex_x, float tex_y,
                                    float o_x, float o_y)
{
    // rotate degree A
    float x = x0 * cos_a - y0 * sin_a;
    float y = x0 * sin_a + y0 * cos_a;

    // compute mapping point on cylinder
    float rad = (x - xfx)/ m_radius;
    x = (float)(xfx + m_radius * sin(rad));
    float cz = (float)(m_radius * (1 - cos(rad)));

    // rotate degree -A, sin(-A) = -sin(A), cos(-A) = cos(A)
    float cx = x * cos_a + y * sin_a + o_x;
    float cy = y * cos_a - x * sin_a + o_y;
    m_fold_front_vertexes.add_vertex(cx, cy, cz, tex_x, tex_y);
}

/**
 * Compute last vertex of base shadow(m_backward direction)
 * <p>
 * The m_vertexes of base shadow are composed by two part: m_forward and
 * m_backward part. Forward m_vertexes are computed from XFold points and
 * m_backward m_vertexes are computed from YFold points. The reason why we use
 * m_forward and m_backward is because how to change float buffer index when we
 * add a new vertex to buffer. Backward means the index is declined from
 * buffer middle position to the head, in contrast, the m_forward is
 * increasing index from middle to the tail. This design will help keep
 * float buffer consecutive and to be draw at a time.
 * </p><p>
 * Sometimes, the whole or part of YFold points will be outside page, that
 * means their Y coordinate are greater than page height(diagonal.y). In
 * this case, we have to crop them like cropping line on 2D coordinate
 * system. If delve further, we can conclude that we only need to compute
 * the first start/end m_vertexes which is falling on the border line of
 * diagonal.y since other m_backward m_vertexes must be outside page and could
 * not be seen, and then combine these m_vertexes with m_forward m_vertexes to
 * render base shadow.
 * </p><p>
 * This function is just used to compute the couple m_vertexes.
 * </p>
 *
 * @param x0 x of point on axis
 * @param y0 y of point on axis
 * @param tX x of xFoldP1 point in rotated coordinate system
 * @param sinA sin value of page curling angle
 * @param cosA cos value of page curling angel
 * @param baseWcosA base shadow width * cosA
 * @param baseWsinA base shadow width * sinA
 * @param oX x of originate point
 * @param oY y of originate point
 * @param dY y of diagonal point
 */
void PageFlip::compute_base_shadow_last_vertex(float x0, float y0, float xfs,
                                               float sin_a, float cos_a,
                                               float base_w_cos_a,
                                               float base_w_sin_a,
                                               float o_x, float o_y, float d_y)
{
    // like computing front vertex, we firstly compute the mapping vertex
    // on fold cylinder for point (x0, y0) which also is last vertex of
    // base shadow(m_backward direction)
    float x = x0 * cos_a - y0 * sin_a;
    float y = x0 * sin_a + y0 * cos_a;

    // compute mapping point on cylinder
    float rad = (x - xfs)/ m_radius;
    x = (float)(xfs + m_radius * sin(rad));

    float cx1 = x * cos_a + y * sin_a + o_x;
    float cy1 = y * cos_a - x * sin_a + o_y;

    // now, we have start vertex(cx1, cy1), compute end vertex(cx2, cy2)
    // which is translated based on start vertex(cx1, cy1)
    float cx2 = cx1 + base_w_cos_a;
    float cy2 = cy1 - base_w_sin_a;

    // as we know, this function is only used to compute last vertex of
    // base shadow(m_backward) when the YFold points are outside page height,
    // that means the (cx1, cy1) and (cx2, cy2) we computed above normally
    // is outside page, so we need to compute their projection points on page
    // border as rendering vertex of base shadow
    float bx1 = cx1 + m_k_value * (cy1 - d_y);
    float bx2 = cx2 + m_k_value * (cy2 - d_y);

    // add start/end vertex into base shadow buffer, it will be linked with
    // m_forward m_vertexes to draw base shadow
    m_fold_base_shadow_vertexes.add_vertexes(false, bx1, d_y, bx2, d_y);
}

/**
 * Compute m_vertexes when page flip is slope
 */
void PageFlip::compute_vertexes_when_slope()
{
    Page& page = *m_first_page;
    const float o_x = page.m_origin_p.x;
    const float o_y = page.m_origin_p.y;
    const float d_y = page.m_diagonal_p.y;
    const float t_ox = page.m_origin_p.tex_x;
    const float t_oy = page.m_origin_p.tex_y;
    const float t_dy = page.m_diagonal_p.tex_y;
    const float height = page.m_height;
    const float d2o_y = d_y - o_y;

    // compute radius and sin/cos of angle
    const float sin_a = (m_touch_p.y - o_y) / m_len_of_t2o;
    const float cos_a = (o_x - m_touch_p.x) / m_len_of_t2o;

    // need to translate before rotate, and then translate back
    float x_fp1 = (m_x_fold_p1.x - o_x) * cos_a;
    float edge_w = m_fold_edge_shadow_width.width(m_radius);
    float base_w = m_fold_base_shadow_width.width(m_radius);
    float base_w_cos_a = base_w * cos_a;
    float base_w_sin_a = base_w * sin_a;
    float edge_y = o_y > 0 ? edge_w : -edge_w;
    float edge_x = o_x > 0 ? edge_w : -edge_w;
    float step_sy = edge_y / m_mesh_count;
    float step_sx = edge_x / m_mesh_count;

    // reset m_vertexes buffer counter
    m_fold_edge_shadow_vertexes.reset();
    m_fold_base_shadow_vertexes.reset();
    m_fold_front_vertexes.reset();
    m_back_of_fold_vertexes.reset();

    // add the first 3 float numbers is fold triangle
    m_back_of_fold_vertexes.add_vertex(m_touch_p.x, m_touch_p.y, 1, 0,
                                       t_ox, t_oy);

    // compute m_vertexes for fold back part
    float step_x = (m_x_fold_p0.x - m_x_fold_p.x) / m_mesh_count;
    float step_y = (m_y_fold_p0.y - m_y_fold_p.y) / m_mesh_count;
    float x = m_x_fold_p0.x - o_x;
    float y = m_y_fold_p0.y - o_y;
    float sx = edge_x;
    float sy = edge_y;

    // compute point of back of fold page
    // Case 1: y coordinate of point YFP0 -> YFP is < diagonalP.y
    //
    //   <---- Flip
    // +-------------+ diagonalP
    // |             |
    // |             + YFP
    // |            /|
    // |           / |
    // |          /  |
    // |         /   |
    // |        /    + YFP0
    // |       / p  /|
    // +------+--.-+-+ originP
    //      XFP   XFP0
    //
    // 1. XFP -> XFP0 -> originP -> YFP0 ->YFP is back of fold page
    // 2. XFP -> XFP0 -> YFP0 -> YFP is a half of cylinder when page is
    //    curled
    // 3. P point will be computed
    //
    // compute points within the page
    int i = 0;
    for (; i <= m_mesh_count && fabs(y) < height;
          ++i, x -= step_x, y -= step_y, sy -= step_sy, sx -= step_sx) {
        compute_back_vertex(true, x, 0, x, sy, x_fp1, sin_a, cos_a,
                            page.texture_x(x + o_x), t_oy, o_x, o_y);
        compute_back_vertex(false, 0, y, sx, y, x_fp1, sin_a, cos_a, t_ox,
                            page.texture_y(y + o_y), o_x, o_y);
    }

    // If y coordinate of point on YFP0 -> YFP is > diagonalP
    // There are two cases:
    //                      <---- Flip
    //     Case 2                               Case 3
    //          YFP                               YFP   YFP0
    // +---------+---+ diagonalP          +--------+-----+--+ diagonalP
    // |        /    |                    |       /     /   |
    // |       /     + YFP0               |      /     /    |
    // |      /     /|                    |     /     /     |
    // |     /     / |                    |    /     /      |
    // |    /     /  |                    |   /     /       |
    // |   / p   /   |                    |  / p   /        |
    // +--+--.--+----+ originalP          +-+--.--+---------+ originalP
    //   XFP   XFP0                        XFP   XFP0
    //
    // compute points outside the page
    if (i <= m_mesh_count) {
        if (fabs(y) != height) {
            // case 3: compute mapping point of diagonalP
            if (fabs(m_y_fold_p0.y - o_y) > height) {
                float tx = o_x + 2 * m_k_value * (m_y_fold_p.y - d_y);
                float ty = d_y + m_k_value * (tx - o_x);
                m_back_of_fold_vertexes.add_vertex(tx, ty, 1, 0, t_ox, t_dy);

                float tsx = tx - sx;
                float tsy = d_y + m_k_value * (tsx - o_x);
                m_fold_edge_shadow_vertexes.add_vertexes(false, tx, ty,
                                                        tsx, tsy);
            }
                // case 2: compute mapping point of diagonalP
            else {
                float x1 = m_k_value* d2o_y;
                compute_back_vertex(true, x1, 0, x1, sy, x_fp1, sin_a, cos_a,
                                    page.texture_x(x1 + o_x), t_oy, o_x, o_y);
                compute_back_vertex(false, 0, d2o_y, sx, d2o_y, x_fp1, sin_a,
                                    cos_a, t_ox, t_dy, o_x, o_y);
            }
        }

        // compute the remaining points
        for (; i <= m_mesh_count;
               ++i, x -= step_x, y -= step_y, sy -= step_sy, sx -= step_sx) {
            compute_back_vertex(true, x, 0, x, sy, x_fp1, sin_a, cos_a,
                                page.texture_x(x + o_x), t_oy, o_x, o_y);

            // since the origin Y is beyond page, we need to compute its
            // projection point on page border and then compute mapping
            // point on curled cylinder
            float x1 = m_k_value * (y + o_y - d_y);
            compute_back_vertex(x1, d2o_y, x_fp1, sin_a, cos_a,
                                page.texture_x(x1 + o_x), t_dy, o_x, o_y);
        }
    }

    // Like above computation, the below steps are computing m_vertexes of
    // front of fold page
    // Case 1: y coordinate of point YFP -> YFP1 is < diagonalP.y
    //
    //     <---- Flip
    // +----------------+ diagonalP
    // |                |
    // |                + YFP1
    // |               /|
    // |              / |
    // |             /  |
    // |            /   |
    // |           /    + YFP
    // |          /    /|
    // |         /    / |
    // |        /    /  + YFP0
    // |       /    /  /|
    // |      / p  /  / |
    // +-----+--.-+--+--+ originP
    //    XFP1  XFP  XFP0
    //
    // 1. XFP -> YFP -> YFP1 ->XFP1 is front of fold page and a half of
    //    cylinder when page is curled.
    // 2. YFP->XFP is joint line of front and back of fold page
    // 3. P point will be computed
    //
    // compute points within the page
    step_x = (m_x_fold_p.x - m_x_fold_p1.x) / m_mesh_count;
    step_y = (m_y_fold_p.y - m_y_fold_p1.y) / m_mesh_count;
    x = m_x_fold_p.x - o_x - step_x;
    y = m_y_fold_p.y - o_y - step_y;
    int j = 0;
    //Log.d(TAG, "===========================");
    for (; j < m_mesh_count && fabs(y) < height;
           ++j, x -= step_x, y -= step_y) {
        compute_front_vertex(true, x, 0, x_fp1, sin_a, cos_a,
                             base_w_cos_a, base_w_sin_a,
                             page.texture_x(x + o_x), t_oy, o_x, o_y, d_y);
        compute_front_vertex(false, 0, y, x_fp1, sin_a, cos_a,
                             base_w_cos_a, base_w_sin_a,
                             t_ox, page.texture_y(y + o_y), o_x, o_y, d_y);
    }

    // compute points outside the page
    if (j < m_mesh_count) {
        // compute mapping point of diagonalP
        if (fabs(y) != height && j > 0) {
            float y1 = (d_y - o_y);
            float x1 = m_k_value * y1;
            compute_front_vertex(true, x1, 0, x_fp1, sin_a, cos_a,
                                 base_w_cos_a, base_w_sin_a,
                                 page.texture_x(x1 + o_x), t_oy, o_x, o_y, d_y);

            compute_front_vertex(0, y1, x_fp1, sin_a, cos_a, t_ox,
                                 page.texture_y(y1+o_y), o_x, o_y) ;
        }

        // compute last pair of m_vertexes of base shadow
        compute_base_shadow_last_vertex(0, y, x_fp1, sin_a, cos_a,
                                        base_w_cos_a, base_w_sin_a,
                                        o_x, o_y, d_y);

        // compute the remaining points
        for (; j < m_mesh_count; ++j, x -= step_x, y -= step_y) {
            compute_front_vertex(true, x, 0, x_fp1, sin_a, cos_a,
                                 base_w_cos_a, base_w_sin_a,
                                 page.texture_x(x + o_x), t_oy, o_x, o_y, d_y);

            float x1 = m_k_value * (y + o_y - d_y);
            compute_front_vertex(x1, d2o_y, x_fp1, sin_a, cos_a,
                                 page.texture_x(x1 + o_x), t_dy, o_x, o_y);
        }

    }

    // set uniform Z value for shadow m_vertexes
    m_fold_edge_shadow_vertexes.set_vertex_z(m_fold_front_vertexes.float_at(2));
    m_fold_base_shadow_vertexes.set_vertex_z(-0.5f);

    // add two m_vertexes to connect with the unfold front page
    page.build_vertexes_of_page_when_slope(m_fold_front_vertexes, m_x_fold_p1,
                                           m_y_fold_p1, m_k_value);

    // compute m_vertexes of fold edge shadow
    compute_vertexes_of_fold_top_edge_shadow(m_touch_p.x, m_touch_p.y,
                                             sin_a, cos_a,
                                             -edge_x, edge_y);
}

/**
 * Compute m_vertexes of fold top edge shadow
 * <p>Top edge shadow of fold page is a quarter circle</p>
 *
 * @param x0 X of touch point
 * @param y0 Y of touch point
 * @param sinA Sin value of page curling angle
 * @param cosA Cos value of page curling angle
 * @param sx Shadow width on X axis
 * @param sy Shadow width on Y axis
 */
void PageFlip::compute_vertexes_of_fold_top_edge_shadow(float x0, float y0,
                                                        float sin_a, float cos_a,
                                                        float sx, float sy)
{
    float sin_2a = 2 * sin_a * cos_a;
    float cos_2a = (float)(1 - 2 * pow(sin_a, 2));
    float r = 0;
    float dr = (float)(M_PI / (kFoldTopEdgeShadowVexCount - 2));
    int size = kFoldTopEdgeShadowVexCount / 2;
    int j = m_fold_edge_shadow_vertexes.max_backward();

    //                 ^ Y                             __ |
    //      TouchP+    |                             /    |
    //             \   |                            |     |
    //              \  |                             \    |
    //               \ |              X <--------------+--+- OriginP
    //                \|                                 /|
    // X <----------+--+- OriginP                       / |
    //             /   |                               /  |
    //             |   |                              /   |
    //              \__+ Top edge              TouchP+    |
    //                 |                                  v Y
    // 1. compute quarter circle at origin point
    // 2. rotate quarter circle to touch point direction
    // 3. move quarter circle to touch point as top edge shadow
    for (int i = 0; i < size; ++i, r += dr, j += 8) {
        float x = (float)(sx * cos(r));
        float y = (float)(sy * sin(r));

        // rotate -2A and then translate to touchP
        m_fold_edge_shadow_vertexes.set_vertexes(j, x0, y0,
                                                 x * cos_2a + y * sin_2a + x0,
                                                 y * cos_2a - x * sin_2a + y0);
    }
}

/**
 * Compute mesh count for page flip
 */
void PageFlip::compute_mesh_count()
{
    float dx = (float)fabs(m_x_fold_p0.x - m_x_fold_p1.x);
    float dy = (float)fabs(m_y_fold_p0.y - m_y_fold_p1.y);
    int len = m_is_vertical ? (int)dx : (int)std::min(dx, dy);
    m_mesh_count = 0;

    // make sure mesh count is greater than threshold, if less than it,
    // the page maybe is drawn unsmoothly
    for (int i = m_pixels_of_mesh;
         i >= 1 && m_mesh_count < kMeshCountThreshold;
         i >>= 1) {
        m_mesh_count = len / i;
    }

    // keep count is even
    if (m_mesh_count % 2 != 0) {
        m_mesh_count++;
    }

    // half count for fold page
    m_mesh_count >>= 1;
}

/**
 * Compute tan value of curling angle
 *
 * @param dy the diff value between touchP.y and originP.y
 * @return tan value of max curl angle
 */
float PageFlip::compute_tan_of_curl_angle(float dy)
{
    float ratio = dy / m_view_rect.half_height;
    if (ratio <= 1 - kMaxPageCurlAngleRatio) {
        return kMaxPageCurlTanOfAngle;
    }

    float degree = kMaxPageCurlAngle - kPageCurlAngleDiff * ratio;
    if (degree < kMinPageCurlAngle) {
        return kMinPageCurlTanOfAngle;
    }
    else {
        return (float)tan(M_PI * degree / 180);
    }
}


/**
 * Debug information
 */
/*
private void debugInfo() {
    final GPoint originP = mPages[FIRST_PAGE].originP;
    final GPoint diagonalP = mPages[FIRST_PAGE].diagonalP;

    Log.d(TAG, "************************************");
    Log.d(TAG, " Mesh Count:    " + mMeshCount);
    Log.d(TAG, " Mesh Pixels:   " + mPixelsOfMesh);
    Log.d(TAG, " Origin:        " + originP.x + ", " + originP.y);
    Log.d(TAG, " Diagonal:      " + diagonalP.x + ", " + diagonalP.y);
    Log.d(TAG, " OriginTouchP:  " + mStartTouchP.x + ", "
               + mStartTouchP.y);
    Log.d(TAG, " TouchP:        " + mTouchP.x + ", " + mTouchP.y);
    Log.d(TAG, " MiddleP:       " + mMiddleP.x + ", " + mMiddleP.y);
    Log.d(TAG, " XFoldP:        " + mXFoldP.x + ", " + mXFoldP.y);
    Log.d(TAG, " XFoldP0:       " + mXFoldP0.x + ", " + mXFoldP0.y);
    Log.d(TAG, " XFoldP1:       " + mXFoldP1.x + ", " + mXFoldP1.y);
    Log.d(TAG, " YFoldP:        " + mYFoldP.x + ", " + mYFoldP.y);
    Log.d(TAG, " YFoldP0:       " + mYFoldP0.x + ", " + mYFoldP0.y);
    Log.d(TAG, " YFoldP1:       " + mYFoldP1.x + ", " + mYFoldP1.y);
    Log.d(TAG, " LengthT->O:    " + mLenOfTouchOrigin);
}
*/

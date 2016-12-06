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

#ifndef ANDROID_PAGEFLIP_PAGE_FLIP_H
#define ANDROID_PAGEFLIP_PAGE_FLIP_H

#include <math.h>
#include "page.h"
#include "pointf.h"
#include "gl_point.h"
#include "gl_view_rect.h"
#include "scroller.h"
#include "shadow_width.h"
#include "vertexes.h"
#include "shadow_vertexes.h"
#include "back_of_fold_vertexes.h"
#include "vertex_program.h"
#include "shadow_vertex_program.h"
#include "back_of_fold_vertex_program.h"

namespace eschao {

// default pixels of mesh vertex
static const int kMeshVertexPixels = 10;
static const int kMeshCountThreshold = 20;

// The min page curl angle (5 degree)
static const int kMinPageCurlAngle = 5;
// The max page curl angle (5 degree)
static const int kMaxPageCurlAngle = 65;
static const int kPageCurlAngleDiff = (kMaxPageCurlAngle - kMinPageCurlAngle);
static const float kMinPageCurlRadian = (float) (M_PI * kMinPageCurlAngle /
                                                 180);
static const float kMaxPageCurlRadian = (float) (M_PI * kMaxPageCurlAngle /
                                                 180);
static const float kMinPageCurlTanOfAngle = (float) tan(kMinPageCurlRadian);
static const float kMaxPageCurlTanOfAngle = (float) tan(kMaxPageCurlRadian);
static const float kMaxPageCurlAngleRatio = kMaxPageCurlAngle / 90f;
static const float kMaxTanOfForwardFlip = (float) tan(M_PI / 6);
static const float kMaxTanOfBackwardFlip = (float) tan(M_PI / 20);

// width m_ratio of clicking to flip
static const float kWidthRatioOfClickToFlip = 0.5f;

// width m_ratio of triggering restore flip
static const float kWidthOfRatioOfRestoreFlip = 0.4f;

// folder page shadow color buffer size
static const int kFoldTopEdgeShadowVexCount = 22;

// fold edge shadow color
static const float kFoldEdgeShadowStartColor = 0.1f;
static const float kFoldEdgeShadowStartAlpha = 0.25f;
static const float kFoldEdgeShadowEndColor = 0.3f;
static const float kFoldEdgeShadowEndAlpha = 0f;

// fold base shadow color
static const float kFoldBaseShadowStartColor = 0.05f;
static const float kFoldBaseShadowStartAlpha = 0.4f;
static const float kFoldBaseShadowEndColor = 0.3f;
static const float kFoldBaseShadowEndAlpha = 0f;

enum PageNo {
    FIRST_PAGE = 0,
    SECOND_PAGE,
    PAGE_SIZE,
};

enum PageMode {
    SINGLE_PAGE_MODE = 0,
    AUTO_PAGE_MODE,
};

enum PageFlipState {
    BEGIN_FLIP,
    FORWARD_FLIP,
    BACKWARD_FLIP,
    RESTORE_FLIP,
    END_FLIP,
    END_WITH_FORWARD,
    END_WITH_BACKWARD,
    END_WITH_RESTORE,
};

class PageFlip {

public:
    PageFlip();

    ~PageFlip();

    bool enable_auto_page(bool is_auto);

    void on_surface_created();

    void on_surface_changed(int width, int height);

    void on_finger_down(float x, float y);

    bool on_finger_move(float x, float y);

    bool on_finger_up(float x, float y, int duration);

    bool can_animate(float x, float y);

    bool animating();

    void abort_animating();

    void draw_flip_frame();

    void draw_page_frame();

    inline bool is_auto_page_enabled() {
        return m_page_mode == AUTO_PAGE_MODE;
    }

    inline void enable_click_to_flip(bool is_enable) {
        m_is_click_to_flip = is_enable;
    }

    inline bool set_width_ratio_of_click_to_flip(float ratio) {
        if (ratio <= 0 || ratio > 0.5f) {
            return false;
        }

        m_width_ratio_of_click_to_flip = ratio;
        return true;
    }

    inline void set_pixels_of_mesh(int pixels) {
        m_pixels_of_mesh = pixels > 0 ? m_pixels_of_mesh : kMeshVertexPixels;
    }

    inline bool set_semi_perimeter_ratio(float ratio) {
        if (ratio <= 0 || ratio > 1) {
            return false;
        }

        m_semi_perimeter_ratio = ratio;
        return true;
    }

    inline void set_mask_alpha_of_fold(int alpha) {
        m_back_of_fold_vertexes.set_mask_alpha(alpha);
    }

    inline void set_shadow_color_of_fold_edges(float start_color,
                                               float start_alpha,
                                               float end_color,
                                               float end_alpha) {
        m_fold_edge_shadow_vertexes.color.set(start_color, start_alpha,
                                              end_color, end_alpha);
    }

    inline void set_shadow_color_of_fold_base(float start_color,
                                              float start_alpha,
                                              float end_color,
                                              float end_alpha) {
        m_fold_base_shadow_vertexes.color.set(start_color, start_alpha,
                                              end_color, end_alpha);
    }

    inline void set_shadow_width_of_fold_edges(float min,
                                               float max,
                                               float ratio) {
        m_fold_edge_shadow_width.set(min, max, ratio);
    }

    inline void set_shadow_width_of_fold_base(float min,
                                              float max,
                                              float ratio) {
        m_fold_base_shadow_width.set(min, max, ratio);
    }

    inline int surface_width() {
        return (int) m_view_rect.surface_width;
    }

    inline int surface_height() {
        return (int) m_view_rect.surface_height;
    }

    inline PageFlipState flip_state() {
        return m_flip_state;
    }

    inline bool isAnimating() {
        return true;//!mScroller.isFinished();
    }

    inline bool is_started_flip() {
        return m_flip_state == BACKWARD_FLIP ||
               m_flip_state == FORWARD_FLIP ||
               m_flip_state == RESTORE_FLIP;
    }

    inline bool is_ended_flip() {
        return m_flip_state == END_FLIP ||
               m_flip_state == END_WITH_RESTORE ||
               m_flip_state == END_WITH_BACKWARD ||
               m_flip_state == END_WITH_FORWARD;
    }

    inline void delete_unused_textures() {
        m_first_page->textures.delete_unused_textures();
        if (m_second_page) {
            m_second_page->textures.delete_unused_textures();
        }
    }

private:
    void create_pages();

    void compute_scroll_points_for_clicking_flip(float x,
                                                 PointF &start,
                                                 PointF &end);

    void compute_max_mesh_count();

    void create_gradient_light_texture();

    void compute_vertexes_build_page();

    void compute_key_vertexes_when_vertical();

    void compute_vertexes_when_vertical();

    void compute_key_vertexes_when_slope();

    void compute_vertexes_when_slope();

    void compute_back_vertex(bool is_x, float x0, float y0, float sx0,
                             float sy0, float xfx, float sin_a,
                             float cos_a, float tex_x, float tex_y,
                             float o_x, float o_y);

    void compute_back_vertex(float x0, float y0, float xfx,
                             float sin_a, float cos_a, float tex_x,
                             float tex_y, float o_x, float o_y);

    void compute_front_vertex(bool is_x, float x0, float y0, float xfx,
                              float sin_a, float cos_a, float base_w_cos_a,
                              float base_w_sin_a, float tex_x, float tex_y,
                              float o_x, float o_y);

    void compute_front_vertex(float x0, float y0, float xfx,
                              float sin_a, float cos_a,
                              float tex_x, float tex_y,
                              float o_x, float o_y);

    void compute_base_shadow_last_vertex(float x0, float y0, float xfs,
                                         float sin_a, float cos_a,
                                         float base_w_cos_a, float base_w_sin_a,
                                         float o_x, float o_y, float d_y);

    void compute_vertexes_of_fold_top_edge_shadow(float x0, float y0,
                                                  float sin_a, float cos_a,
                                                  float sx, float sy);

    void compute_mesh_count();

    float compute_tan_of_curl_angle(float dy);

private:
    // view size
    GLViewRect m_view_rect;

    // the pixel size for each mesh
    int m_pixels_of_mesh;

    // gradient shadow texture id
    int m_gradient_light_tid;

    // touch point and last touch point
    PointF m_touch_p;
    // the last touch point (could be deleted?)
    PointF m_last_touch_p;
    // the first touch point when finger down on the screen
    PointF m_start_touch_p;
    // the middle point between touch point and origin point
    PointF m_middle_p;

    // from 2D perspective, the line will intersect Y axis and X axis that being
    // through middle point and perpendicular to the line which is from touch
    // point to origin point, The point on Y axis is mYFoldP, the mXFoldP is on
    // X axis. The mY{X}FoldP1 is up mY{X}FoldP, The mY{X}FoldP0 is under
    // mY{X}FoldP
    //
    //        <----- Flip
    //                          ^ Y
    //                          |
    //                          + mYFoldP1
    //                        / |
    //                       /  |
    //                      /   |
    //                     /    |
    //                    /     |
    //                   /      |
    //                  /       + mYFoldP
    //    mTouchP      /      / |
    //       .        /      /  |
    //               /      /   |
    //              /      /    |
    //             /      /     |
    //            /   .  /      + mYFoldP0
    //           /      /      /|
    //          /      /      / |
    //         /      /      /  |
    //X <-----+------+------+---+ originP
    //   mXFoldP1 mXFoldP mXFoldP0
    //
    PointF m_y_fold_p;
    PointF m_y_fold_p0;
    PointF m_y_fold_p1;
    PointF m_x_fold_p;
    PointF m_x_fold_p0;
    PointF m_x_fold_p1;

    //            ^ Y
    //   mTouchP  |
    //        +   |
    //         \  |
    //          \ |
    //       A ( \|
    // X <--------+ originP
    //
    // A is angle between X axis and line from mTouchP to originP
    // the max curling angle between line from touchP to originP and X axis
    float m_max_t2o_tan_a;
    // another max curling angle when finger moving causes the originP change
    // from (x, y) to (x, -y) which means mirror based on Y axis.
    float m_max_t2d_tan_a;
    // the tan value of current curling angle
    // mKValue = (touchP.y - originP.y) / (touchP.x - originP.x)
    float m_k_value;
    // the length of line from mTouchP to originP
    float m_len_of_t2o;
    // the cylinder radius
    float m_radius;
    // the perimeter m_ratio of semi-cylinder based on mLenOfTouchOrigin;
    float m_semi_perimeter_ratio;
    // Mesh count
    int m_mesh_count;

    // edges shadow width of back of fold page
    ShadowWidth m_fold_edge_shadow_width;
    // base shadow width of front of fold page
    ShadowWidth m_fold_base_shadow_width;

    // fold page and shadow m_vertexes
    Vertexes m_fold_front_vertexes;
    BackOfFoldVertexes m_back_of_fold_vertexes;
    ShadowVertexes m_fold_edge_shadow_vertexes;
    ShadowVertexes m_fold_base_shadow_vertexes;

    // Shader program for openGL drawing
    VertexProgram m_vertex_prog;
    BackOfFoldVertexProgram m_back_of_fold_vertex_prog;
    ShadowVertexProgram m_shadow_vertex_prog;

    // is vertical page flip
    bool m_is_vertical;
    PageFlipState m_flip_state;

    // use for flip animation
    Scroller m_scroller;

    // pages and page mode
    // in single page mode, there is only one page in the index 0
    // in double pages mode, there are two pages, the first one is always active
    // page which is receiving finger events, for example: finger down/move/up
    Page *m_first_page;
    Page *m_second_page;
    PageMode m_page_mode;

    // is clicking to flip page
    bool m_is_click_to_flip;
    // width ration of clicking to flip
    float m_width_ratio_of_click_to_flip;

    // listener for page flipping
    //private OnPageFlipListener mListener;
};

}
#endif //ANDROID_PAGEFLIP_PAGE_FLIP_H

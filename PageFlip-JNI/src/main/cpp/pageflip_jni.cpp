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

#include <android/log.h>
#include <android/bitmap.h>
#include "pageflip.h"
#include "utility.h"
#include "pageflip_jni.h"

using namespace eschao;

static PageFlip *g_pageflip = NULL;

static const char *g_class_name =
        "com/eschao/android/widget/pageflip/jni/PageFlipLib";

static JNINativeMethod g_methods_tbl[] = {
        { "init", "()Z", (void *)jni_init },
        { "cleanup", "()Z", (void *)jni_cleanup },
        { "enableAutoPage", "(Z)I", (void *)jni_enable_auto_page },
        { "isAutoPageEnabled", "()Z", (void *)jni_is_auto_page_enabled },
        { "enableClickToFlip", "(Z)I", (void *)jni_enable_click_to_flip },
        { "setWidthRatioOfClickToFlip", "(F)I",
          (void *)jni_set_width_ratio_of_click_to_flip },
        { "setPixelsOfMesh", "(F)I", (void *)jni_set_pixels_of_mesh },
        { "setSemiPerimeterRatio", "(F)I",
          (void *)jni_set_semi_perimeter_ratio },
        { "setMaskAlphaOfFold", "(I)I", (void *)jni_set_mask_alpha_of_fold },
        { "setShadowColorOfFoldEdges", "(FFFF)I",
          (void *)jni_set_shadow_color_of_fold_edges },
        { "setShadowColorOfFoldBase", "(FFFF)I",
          (void *)jni_set_shadow_color_of_fold_base },
        { "setShadowWidthOfFoldEdges", "(FFF)I",
          (void *)jni_set_shadow_width_of_fold_edges },
        { "setShadowWidthOfFoldBase", "(FFF)I",
          (void *)jni_set_shadow_width_of_fold_base },
        { "getPixelsOfMesh", "()I", (void *)jni_get_pixels_of_mesh },
        { "getSurfaceWidth", "()I", (void *)jni_get_surface_width },
        { "getSurfaceHeight", "()I", (void *)jni_get_surface_height },
        { "onSurfaceCreated", "()Z", (void *)jni_on_surface_created },
        { "onSurfaceChanged", "(II)Z", (void *)jni_on_surface_changed },
        { "onFingerDown", "(FF)Z", (void *)jni_on_finger_down },
        { "animating", "()V", (void *)jni_animating },
        { "isAnimating", "()Z", (void *)jni_is_animating },
        { "abortAnimating", "()V", (void *)jni_abort_animating },
        { "drawFlipFrame", "()V", (void *)jni_draw_flip_frame },
        { "drawPageFrame", "()V", (void *)jni_draw_page_frame },
        { "getFlipState", "()I", (void *)jni_get_flip_state },
        { "hasSecondPage", "()Z", (void *)jni_has_second_page },
        { "isFirstTextureSet", "(Z)Z", (void *)jni_is_first_texture_set },
        { "isSecondTextureSet", "(Z)Z", (void *)jni_is_second_texture_set },
        { "isBackTextureSet", "(Z)Z", (void *)jni_is_back_texture_set },
        { "setFirstTexture", "(ZLandroid/graphics/Bitmap;)I",
          (void *)jni_set_first_texture },
        { "setSecondTexture", "(ZLandroid/graphics/Bitmap;)I",
          (void *)jni_set_second_texture },
        { "setBackTexture", "(ZLandroid/graphics/Bitmap;)I",
          (void *)jni_set_back_texture },
        { "setGradientLightTexture", "(Landroid/graphics/Bitmap;)I",
          (void *)jni_set_gradient_light_texture },
        { "setFirstTextureWithSecond", "(Z)I",
           (void *)jni_set_first_texture_with_second },
        { "setSecondTextureWithFirst", "(Z)I",
           (void *)jni_set_second_texture_with_first },
        { "swapTexturesWith", "(Z)I", (void *)jni_swap_textures_with },
        { "deleteUnusedTextures", "(Z)I", (void *)jni_delete_unused_textures },
        { "onFingerMove", "(FFZZ)Z", (void *)jni_on_finger_move },
        { "onFingerUp", "(FFIZZ)Z", (void *)jni_on_finger_up },
};

static bool register_natives(JNIEnv *env)
{
    jclass cls;
    cls = env->FindClass(g_class_name);
    if (cls == NULL) {
        return JNI_FALSE;
    }

    int size = sizeof(g_methods_tbl) / sizeof(g_methods_tbl[0]);
    jint ret = env->RegisterNatives(cls, g_methods_tbl, size);
    if (ret < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    if (!register_natives(env)) {
        return JNI_ERR;
    }

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL jni_init(JNIEnv* env)
{
   if (g_pageflip == NULL) {
       g_pageflip = new PageFlip();
   }
}

JNIEXPORT void JNICALL jni_cleanup(JNIEnv* env)
{
    if (g_pageflip) {
        delete g_pageflip;
        g_pageflip = NULL;
    }
}

JNIEXPORT jint JNICALL jni_enable_auto_page(JNIEnv* env, jboolean is_auto)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->enable_auto_page(is_auto);
        return JNI_OK;
    }
    else {
        LOGE("jni_enable_auto_page",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jboolean JNICALL jni_is_auto_page_enabled(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->is_auto_page_enabled();
    }
    else {
        LOGE("jni_is_auto_page_enabled",
             "PageFlip object is null, please call init() first!");
    }

    g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    return JNI_FALSE;
}

JNIEXPORT jint JNICALL jni_enable_click_to_flip(JNIEnv* env,
                                                jboolean enable)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->enable_click_to_flip(enable);
        return Error::OK;
    }
    else {
        LOGE("jni_enable_click_to_flip",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_width_ratio_of_click_to_flip(JNIEnv* env,
                                                            jfloat ratio)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->set_width_ratio_of_click_to_flip(ratio);
    }
    else {
        LOGE("jni_set_width_ratio_of_click_to_flip",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_pixels_of_mesh(JNIEnv* env, jint pixels)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->set_pixels_of_mesh(pixels);
        return Error::OK;
    }
    else {
        LOGE("jni_set_pixels_of_mesh",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_semi_perimeter_ratio(JNIEnv* env, jfloat ratio)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->set_semi_perimeter_ratio(ratio);
    }
    else {
        LOGE("jni_set_semi_perimeter_ratio",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_mask_alpha_of_fold(JNIEnv* env, jint alpha)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->set_mask_alpha_of_fold(alpha);
    }
    else {
        LOGE("jni_set_mask_alpha_of_fold",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_shadow_color_of_fold_edges(JNIEnv* env,
                                                          jfloat s_color,
                                                          jfloat s_alpha,
                                                          jfloat e_color,
                                                          jfloat e_alpha)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->set_shadow_color_of_fold_edges(s_color, s_alpha,
                                                          e_color, e_alpha);
    }
    else {
        LOGE("jni_set_shadow_color_of_fold_edges",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_shadow_color_of_fold_base(JNIEnv* env,
                                                         jfloat s_color,
                                                         jfloat s_alpha,
                                                         jfloat e_color,
                                                         jfloat e_alpha)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->set_shadow_color_of_fold_base(s_color, s_alpha,
                                                         e_color, e_alpha);
    }
    else {
        LOGE("jni_set_shadow_color_of_fold_base",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_shadow_width_of_fold_edges(JNIEnv* env,
                                                          jfloat min,
                                                          jfloat max,
                                                          jfloat ratio)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->set_shadow_width_of_fold_edges(min, max, ratio);
    }
    else {
        LOGE("jni_set_shadow_width_of_fold_edges",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_set_shadow_width_of_fold_base(JNIEnv* env,
                                                         jfloat min,
                                                         jfloat max,
                                                         jfloat ratio)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->set_shadow_width_of_fold_base(min, max, ratio);
    }
    else {
        LOGE("jni_set_shadow_width_of_fold_base",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_get_pixels_of_mesh(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return (jint)g_pageflip->pixels_of_mesh();
    }
    else {
        LOGE("jni_get_pixels_of_mesh",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_get_surface_width(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->surface_width();
    }
    else {
        LOGE("jni_get_surface_width",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_get_surface_height(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return (jint)g_pageflip->surface_height();
    }
    else {
        LOGE("jni_get_surface_height",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_on_surface_created(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->on_surface_created();
        return Error::OK;
    }
    else {
        LOGE("jni_on_surface_created",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_on_surface_changed(JNIEnv* env,
                                              jint width,
                                              jint height)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->on_surface_changed(width, height);
        return Error::OK;
    }
    else {
        LOGE("jni_on_surface_changed",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jboolean JNICALL jni_on_finger_down(JNIEnv* env, jfloat x, jfloat y)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->on_finger_down(x, y);
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_on_finger_down",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL jni_on_finger_move(JNIEnv* env,
                                              jfloat x,
                                              jfloat y,
                                              jboolean can_forward,
                                              jboolean can_backward)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->on_finger_move(x, y,
                                                    can_forward, can_backward);
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_on_finger_move",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL jni_on_finger_up(JNIEnv* env,
                                            jfloat x,
                                            jfloat y,
                                            jint duration,
                                            jboolean can_forward,
                                            jboolean can_backward)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->on_finger_up(x, y, duration,
                                                  can_forward, can_backward);
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_on_finger_up",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL jni_animating(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->animating();
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_animating",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL jni_is_animating(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean) g_pageflip->is_animating();
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_is_animating",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jint JNICALL jni_abort_animating(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->abort_animating();
        return Error::OK;
    }
    else {
        LOGE("jni_abort_animating",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_draw_flip_frame(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->draw_flip_frame();
        return Error::OK;
    }
    else {
        LOGE("jni_draw_flip_frame",
             "PageFlip object is null, please call init() first!");
    }
    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_draw_page_frame(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->draw_page_frame();
        return Error::OK;
    }
    else {
        LOGE("jni_draw_page_frame",
             "PageFlip object is null, please call init() first!");
    }
    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jint JNICALL jni_get_flip_state(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->flip_state();
    }
    else {
        LOGE("jni_get_flip_state",
             "PageFlip object is null, please call init() first!");
    }

    return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
}

JNIEXPORT jboolean JNICALL jni_has_second_page(JNIEnv* env)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->has_second_page();
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_has_second_page",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL jni_is_first_texture_set(JNIEnv* env,
                                                    jboolean is_first_page)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->is_first_texture_set(is_first_page);
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_is_first_texture_set",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL jni_is_second_texture_set(JNIEnv* env,
                                                     jboolean is_first_page)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->is_second_texture_set(is_first_page);
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_is_second_texture_set",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jboolean JNICALL jni_is_back_texture_set(JNIEnv* env,
                                                   jboolean is_first_page)
{
    g_error.reset();
    if (g_pageflip) {
        return (jboolean)g_pageflip->is_back_texture_set(is_first_page);
    }
    else {
        g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
        LOGE("jni_is_back_texture_set",
             "PageFlip object is null, please call init() first!");
    }

    return JNI_FALSE;
}

JNIEXPORT jint JNICALL jni_set_first_texture(JNIEnv* env,
                                             jboolean is_first_page,
                                             jobject jobj)
{
    g_error.reset();
    if (jobj == NULL) {
        LOGE("jni_set_first_texture",
             "Can't set first texture with null Bitmap object!");
        return g_error.set(Error::ERR_NULL_PARAMETER);
    }
    else if (g_pageflip) {
        int ret;
        AndroidBitmapInfo info;
        GLvoid *data;
        if ((ret = AndroidBitmap_getInfo(env, jobj, &info)) < 0) {
            return Error::ERR_GET_BITMAP_INFO;
        }

        if ((ret = AndroidBitmap_lockPixels(env, jobj, &data)) < 0) {
            return Error::ERR_GET_BITMAP_DATA;
        }

        ret = g_pageflip->set_first_texture(is_first_page, info, data);
        AndroidBitmap_unlockPixels(env, jobj);
        return ret;
    }
    else {
        LOGE("jni_set_first_texture",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

JNIEXPORT jint JNICALL jni_set_second_texture(JNIEnv* env,
                                              jboolean is_first_page,
                                              jobject jobj)
{
    g_error.reset();
    if (jobj == NULL) {
        LOGE("jni_set_second_texture",
             "Can't set second texture with null Bitmap object!");
        return g_error.set(Error::ERR_NULL_PARAMETER);
    }
    else if (g_pageflip) {
        int ret;
        AndroidBitmapInfo info;
        GLvoid *data;
        if ((ret = AndroidBitmap_getInfo(env, jobj, &info)) < 0) {
            return Error::ERR_GET_BITMAP_INFO;
        }

        if ((ret = AndroidBitmap_lockPixels(env, jobj, &data)) < 0) {
            return Error::ERR_GET_BITMAP_DATA;
        }

        ret = g_pageflip->set_second_texture(is_first_page, info, data);
        AndroidBitmap_unlockPixels(env, jobj);
        return ret;
    }
    else {
        LOGE("jni_set_second_texture",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

JNIEXPORT jint JNICALL jni_set_back_texture(JNIEnv* env,
                                            jboolean is_first_page,
                                            jobject jobj)
{
    g_error.reset();
    AndroidBitmapInfo info;
    if (jobj == NULL) {
        return g_pageflip->set_second_texture(is_first_page, info, NULL);
    }
    else if (g_pageflip) {
        int ret;
        GLvoid *data;
        if ((ret = AndroidBitmap_getInfo(env, jobj, &info)) < 0) {
            return Error::ERR_GET_BITMAP_INFO;
        }

        if ((ret = AndroidBitmap_lockPixels(env, jobj, &data)) < 0) {
            return Error::ERR_GET_BITMAP_DATA;
        }

        ret = g_pageflip->set_back_texture(is_first_page, info, data);
        AndroidBitmap_unlockPixels(env, jobj);
        return ret;
    }
    else {
        LOGE("jni_set_back_texture",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

JNIEXPORT jint JNICALL jni_set_gradient_light_texture(JNIEnv* env, jobject jobj)
{
    g_error.reset();
    AndroidBitmapInfo info;
    if (jobj == NULL) {
        LOGE("jni_set_gradient_light_texture",
             "Can't set gradient light texture with null Bitmap object!");
        return g_error.set(Error::ERR_NULL_PARAMETER);
    }
    else if (g_pageflip) {
        int ret;
        GLvoid *data;
        if ((ret = AndroidBitmap_getInfo(env, jobj, &info)) < 0) {
            return Error::ERR_GET_BITMAP_INFO;
        }

        if ((ret = AndroidBitmap_lockPixels(env, jobj, &data)) < 0) {
            return Error::ERR_GET_BITMAP_DATA;
        }

        ret = g_pageflip->set_gradient_light_texture(info, data);
        AndroidBitmap_unlockPixels(env, jobj);
        return ret;
    }
    else {
        LOGE("jni_set_gradient_light_texture",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

JNIEXPORT jint JNICALL jni_set_first_texture_with_second(JNIEnv* env,
                                                        jboolean is_first_page)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->set_first_texture_with_second(is_first_page);
        return Error::OK;
    }
    else {
        LOGE("jni_set_first_texture_with_second",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

JNIEXPORT jint JNICALL jni_set_second_texture_with_first(JNIEnv* env,
                                                         jboolean is_first_page)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->set_second_texture_with_first(is_first_page);
        return Error::OK;
    }
    else {
        LOGE("jni_set_first_texture_with_second",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

JNIEXPORT jint JNICALL jni_swap_textures_with(JNIEnv* env,
                                              jboolean is_first_page)
{
    g_error.reset();
    if (g_pageflip) {
        return g_pageflip->swap_textures_with(is_first_page);
    }
    else {
        LOGE("jni_set_first_texture_with_second",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

JNIEXPORT jint JNICALL jni_delete_unused_textures(JNIEnv* env,
                                                  jboolean is_first_page)
{
    g_error.reset();
    if (g_pageflip) {
        g_pageflip->delete_unused_textures();
        return Error::OK;
    }
    else {
        LOGE("jni_set_first_texture_with_second",
             "PageFlip object is null, please call init() first!");
        return g_error.set(Error::ERR_PAGE_FLIP_UNINIT);
    }
}

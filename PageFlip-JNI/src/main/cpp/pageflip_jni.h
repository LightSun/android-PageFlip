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

#ifndef ANDROID_PAGEFLIP_PAGEFLIP_JNI_H
#define ANDROID_PAGEFLIP_PAGEFLIP_JNI_H

#include <jni.h>

extern JNIEXPORT void JNICALL jni_init(JNIEnv* env);
extern JNIEXPORT void JNICALL jni_cleanup(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_enable_auto_page(JNIEnv* env,
                                                   jboolean is_auto);
extern JNIEXPORT jboolean JNICALL jni_is_auto_page_enabled(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_enable_click_to_flip(JNIEnv* env,
                                                       jboolean enable);
extern JNIEXPORT jint JNICALL jni_set_width_ratio_of_click_to_flip(JNIEnv* env,
                                                                   jfloat ratio);
extern JNIEXPORT jint JNICALL jni_set_pixels_of_mesh(JNIEnv* env, jint pixels);
extern JNIEXPORT jint JNICALL jni_set_semi_perimeter_ratio(JNIEnv* env,
                                                           jfloat ratio);
extern JNIEXPORT jint JNICALL jni_set_mask_alpha_of_fold(JNIEnv* env,
                                                         jint alpha);
extern JNIEXPORT jint JNICALL jni_set_shadow_color_of_fold_edges(
                                                            JNIEnv* env,
                                                            jfloat s_color,
                                                            jfloat s_alpha,
                                                            jfloat e_color,
                                                            jfloat e_alpha);
extern JNIEXPORT jint JNICALL jni_set_shadow_color_of_fold_base(
                                                            JNIEnv* env,
                                                            jfloat s_color,
                                                            jfloat s_alpha,
                                                            jfloat e_color,
                                                            jfloat e_alpha);
extern JNIEXPORT jint JNICALL jni_set_shadow_width_of_fold_edges(JNIEnv* env,
                                                                 jfloat min,
                                                                 jfloat max,
                                                                 jfloat ratio);
extern JNIEXPORT jint JNICALL jni_set_shadow_width_of_fold_edges(JNIEnv* env,
                                                                 jfloat min,
                                                                 jfloat max,
                                                                 jfloat ratio);
extern JNIEXPORT jint JNICALL jni_set_shadow_width_of_fold_base(JNIEnv* env,
                                                                jfloat min,
                                                                jfloat max,
                                                                jfloat ratio);
extern JNIEXPORT jint JNICALL jni_get_pixels_of_mesh(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_get_surface_width(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_get_surface_height(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_on_surface_created(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_on_surface_changed(JNIEnv* env,
                                                     jint width,
                                                     jint height);
extern JNIEXPORT jboolean JNICALL jni_on_finger_down(JNIEnv* env,
                                                     jfloat x,
                                                     jfloat y);
extern JNIEXPORT jboolean JNICALL jni_on_finger_move(JNIEnv* env,
                                                     jfloat x,
                                                     jfloat y,
                                                     jboolean can_forward,
                                                     jboolean can_backward);
extern JNIEXPORT jboolean JNICALL jni_on_finger_up(JNIEnv* env,
                                                   jfloat x,
                                                   jfloat y,
                                                   jint duration,
                                                   jboolean can_forward,
                                                   jboolean can_backward);
extern JNIEXPORT jboolean JNICALL jni_animating(JNIEnv* env);
extern JNIEXPORT jboolean JNICALL jni_is_animating(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_abort_animating(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_draw_flip_frame(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_draw_page_frame(JNIEnv* env);
extern JNIEXPORT jint JNICALL jni_get_flip_state(JNIEnv* env);
extern JNIEXPORT jboolean JNICALL jni_has_second_page(JNIEnv* env);
extern JNIEXPORT jboolean JNICALL jni_is_first_texture_set(
                                                    JNIEnv* env,
                                                    jboolean is_first_page);
extern JNIEXPORT jboolean JNICALL jni_is_second_texture_set(
                                                    JNIEnv* env,
                                                    jboolean is_first_page);
extern JNIEXPORT jboolean JNICALL jni_is_back_texture_set(
                                                    JNIEnv* env,
                                                    jboolean is_first_page);
extern JNIEXPORT jint JNICALL jni_set_first_texture(JNIEnv* env,
                                                    jboolean is_first_page,
                                                    jobject jobj);
extern JNIEXPORT jint JNICALL jni_set_second_texture(JNIEnv* env,
                                                     jboolean is_first_page,
                                                     jobject jobj);
extern JNIEXPORT jint JNICALL jni_set_back_texture(JNIEnv* env,
                                                   jboolean is_first_page,
                                                   jobject jobj);
extern JNIEXPORT jint JNICALL jni_set_gradient_light_texture(JNIEnv* env,
                                                             jobject jobj);
extern JNIEXPORT jint JNICALL jni_set_first_texture_with_second(
                                                    JNIEnv* env,
                                                    jboolean is_first_page);
extern JNIEXPORT jint JNICALL jni_set_second_texture_with_first(
                                                    JNIEnv* env,
                                                    jboolean is_first_page);
extern JNIEXPORT jint JNICALL jni_swap_textures_with(JNIEnv* env,
                                                     jboolean is_first_page);
extern JNIEXPORT jint JNICALL jni_delete_unused_textures(JNIEnv* env,
                                                         jboolean is_first_page);

#endif //ANDROID_PAGEFLIP_PAGEFLIP_JNI_H

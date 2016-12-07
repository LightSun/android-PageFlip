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

#include "pageflip_jni.h"
#include "pageflip.h"

using namespace eschao;

static PageFlip *g_pageflip = NULL;

static const char *g_class_name =
        "com/eschao/android/widget/pageflip/jni/PageFlipLib";

static JNINativeMethod g_methods_tbl[] = {
        { "init", "()Z", (void *)jni_init },
        { "onSurfaceCreated", "()Z", (void *)jni_on_surface_created },
        { "onSurfaceChanged", "(II)Z", (void *)jni_on_surface_changed },
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

JNIEXPORT void JNICALL jni_init()
{
   if (g_pageflip == NULL) {
       g_pageflip = new PageFlip();
   }
}

JNIEXPORT jboolean JNICALL jni_on_surface_created()
{
    if (g_pageflip) {
        g_pageflip->on_surface_created();
    }
}

JNIEXPORT jboolean JNICALL jni_on_surface_changed(jint width, jint height)
{
    if (g_pageflip) {
        g_pageflip->on_surface_changed(width, height);
    }
}


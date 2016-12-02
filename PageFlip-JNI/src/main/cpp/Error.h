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

#ifndef ANDROID_PAGEFLIP_ERROR_H
#define ANDROID_PAGEFLIP_ERROR_H

#define MAX_DESC_LENGTH 1023

enum Error {
    OK = 0,
    ERROR,
    ERR_NULL_PARAMETER,
    ERR_INVALID_PARAMETER,
    ERR_GL_COMPILE_SHADER,
    ERR_GL_LINK_PROGRAM,
    ERR_GL_CREATE_SHADER_REF,
    ERR_GL_CREATE_PROGRAM_REF,
    ERR_GL_ATTACH_SHADER,
    ERR_GL_ATTACH_FRAGMENT,
};

extern char err_desc[MAX_DESC_LENGTH + 1];

extern int check_err_desc_len(int lne);

#endif //ANDROID_PAGEFLIP_ERROR_H

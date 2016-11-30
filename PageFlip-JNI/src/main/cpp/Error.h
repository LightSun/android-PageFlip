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

class Error
{
private:
    static const int _ERROR = 10;
    static const int _ERR_GL = 1000;

public:
    static const int OK     = 0;
    static const int ERROR  = 1;
    static const int ERR_NULL_PARAMETER             = _ERROR + 0;
    static const int ERR_INVALID_PARAMETER          = _ERROR + 1;

    static const int ERR_GL_COMPILE_SHADER          = _ERR_GL + 0;
    static const int ERR_GL_LINK_PROGRAM            = _ERR_GL + 1;
    static const int ERR_GL_CREATE_SHADER_HANDLE    = _ERR_GL + 2;
    static const int ERR_GL_CREATE_PROGRAM_HANDLE   = _ERR_GL + 3;
    static const int ERR_GL_ATTACH_SHADER           = _ERR_GL + 4;
    static const int ERR_GL_ATTACH_FRAGMENT         = _ERR_GL + 5;

public:
    static const int MAX_DESC_LENGTH = 1023;
    static char desc[MAX_DESC_LENGTH + 1];

    static int checkDescLenth(int len)
    {
        if (len > MAX_DESC_LENGTH) {
            len = MAX_DESC_LENGTH;
        }
        desc[len + 1] = '\0';
        return len;
    }
};

#endif //ANDROID_PAGEFLIP_ERROR_H

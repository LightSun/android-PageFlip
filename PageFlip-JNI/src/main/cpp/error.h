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

#include <iostream>

using namespace std;

namespace eschao {

class Error {
public:
    Error();

    void set_desc(const char* desc);
    int check_gl_error(const char* desc = NULL);

    inline void reset()
    {
        err_code = OK;
        err_desc[0] = '\0';
    }

    inline int code() const
    {
        return err_code;
    }

    inline const char* desc() const
    {
        return err_desc;
    }

    inline const char* end_desc(int index)
    {
        if (index >= 0 && index <= MAX_ERR_DESC_LENGTH) {
            err_desc[index] = '\0';
        }

        return err_desc;
    }

    inline int set(int code, const char* desc = NULL)
    {
        set_desc(desc);
        return err_code = code;
    }

public:
    static const int MAX_ERR_DESC_LENGTH            = 1023;
    static const int OK                             = 0;
    static const int ERROR                          = OK - 1;
    static const int ERR_PAGE_FLIP_UNINIT           = OK - 2;
    static const int ERR_NULL_PARAMETER             = OK - 3;
    static const int ERR_INVALID_PARAMETER          = OK - 4;
    static const int ERR_GL_ERROR                   = OK - 5;
    static const int ERR_GL_COMPILE_SHADER          = OK - 6;
    static const int ERR_GL_LINK_PROGRAM            = OK - 7;
    static const int ERR_GL_CREATE_SHADER_REF       = OK - 8;
    static const int ERR_GL_CREATE_PROGRAM_REF      = OK - 9;
    static const int ERR_GL_ATTACH_SHADER           = OK - 10;
    static const int ERR_GL_ATTACH_FRAGMENT         = OK - 11;
    static const int ERR_UNSUPPORT_BITMAP_FORMAT    = OK - 12;
    static const int ERR_GET_BITMAP_INFO            = OK - 13;
    static const int ERR_GET_BITMAP_DATA            = OK - 14;
    static const int ERR_NO_TWO_PAGES               = OK - 15;

private:
    int err_code;
    char err_desc[MAX_ERR_DESC_LENGTH + 1];

};

extern Error g_error;

}
#endif //ANDROID_PAGEFLIP_ERROR_H

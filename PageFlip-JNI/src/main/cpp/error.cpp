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

#include <GLES2/gl2.h>
#include <string>
#include "error.h"

namespace eschao {

Error g_error;

Error::Error()
{
    err_code = OK;
    err_desc[0] = '\0';
}

void Error::set_desc(const char* desc)
{
    if (desc) {
        size_t len = strlen(desc);
        if (len > MAX_ERR_DESC_LENGTH) {
            len = MAX_ERR_DESC_LENGTH;
        }

        strncpy(err_desc, desc, len);
        err_desc[len + 1] = '\0';
    }
}

int Error::check_gl_error(const char* desc)
{
    if (desc == NULL) {
        desc = "";
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        sprintf(err_desc, "%s, glGetError() return 0x%x", err_desc, err);
        return Error::ERR_GL_ERROR;
    }

    return Error::OK;
}

}

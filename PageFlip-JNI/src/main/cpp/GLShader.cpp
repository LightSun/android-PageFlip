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

#include "GLShader.h"
#include "Constant.h"
#include "Error.h"

GLShader::GLShader()
        : hShader(Constant::INVALID_HANDLE)

{
}

GLShader::~GLShader()
{
    clean();
}

int GLShader::load(GLenum type, const char *shader)
{
    clean();

    hShader = glCreateShader(type);
    if (hShader == Constant::INVALID_HANDLE)
    {
        return Error::ERR_GL_CREATE_SHADER_HANDLE;
    }

    glShaderSource(hShader, 1, &shader, NULL);
    glCompileShader(hShader);

    GLint compiled = 0;
    glGetShaderiv(hShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(hShader, GL_INFO_LOG_LENGTH, &infoLen);

        if (infoLen)
        {
            infoLen = Error::checkDescLenth(infoLen);
            glGetShaderInfoLog(hShader, infoLen, NULL, Error::desc);
            glDeleteShader(hShader);
            hShader = Constant::INVALID_HANDLE;
        }

        return Error::ERR_GL_COMPILE_SHADER;
    }

    return Error::OK;
}

void GLShader::clean()
{
    if (hShader != Constant::INVALID_HANDLE) {
        glDeleteShader(hShader);
        hShader = 0;
    }
}

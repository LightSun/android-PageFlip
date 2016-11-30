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

#include <iostream>
#include "GLProgram.h"
#include "GLShader.h"
#include "Error.h"
#include "Constant.h"

using namespace std;

GLProgram::GLProgram()
{
    hProgram = Constant::GL_INVALID_HANDLE;
    mShader = new GLShader();
    mFragment = new GLShader();
}

GLProgram::~GLProgram()
{
    clean();
}

void GLProgram::clean()
{
    mShader->clean();
    mFragment->clean();

    if (hProgram != Constant::GL_INVALID_HANDLE)
    {
        glDeleteProgram(hProgram);
        hProgram = Constant::GL_INVALID_HANDLE;
    }
}

int GLProgram::init(const char *shader, const char *fragment)
{
    if (shader == NULL || fragment == NULL)
    {
        return Error::ERR_NULL_PARAMETER;
    }

    int error = mShader->load(GL_VERTEX_SHADER, shader);
    if (error != Error::OK)
    {
        return error;
    }

    error = mFragment->load(GL_FRAGMENT_SHADER, fragment);
    if (error != Error::OK)
    {
        return error;
    }

    hProgram = glCreateProgram();
    if (hProgram == Constant::GL_INVALID_HANDLE)
    {
        mShader->clean();
        mFragment->clean();
        return Error::ERR_GL_CREATE_PROGRAM_HANDLE;
    }

    glAttachShader(hProgram, mShader->getHandle());
    error = checkGlError("When attach shader(glAttachShader)");
    if (error != Error::OK)
    {
        clean();
        return Error::ERR_GL_ATTACH_SHADER;
    }

    glAttachShader(hProgram, mFragment->getHandle());
    error = checkGlError("When attach fragment(glAttachShader");
    if (error != Error::OK)
    {
        clean();
        return Error::ERR_GL_ATTACH_FRAGMENT;
    }

    glLinkProgram(hProgram);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(hProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
    {
        GLint infoLen = 0;
        glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen)
        {
            infoLen = Error::checkDescLenth(infoLen);
            glGetProgramInfoLog(hProgram, infoLen, NULL, Error::desc);
        }

        clean();
        return Error::ERR_GL_LINK_PROGRAM;
    }

    return Error::OK;
}

int GLProgram::checkGlError(const char* desc)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        sprintf(Error::desc, "%s, glGetError() return 0x%x", desc, err);
        return Error::ERROR;
    }

    return Error::OK;
}

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
#include "gl_program.h"
#include "gl_shader.h"
#include "error.h"
#include "constant.h"

using namespace std;

GLProgram::GLProgram()
{
    program_ref = Constant::kGlInvalidRef;
    shader = new GLShader();
    fragment = new GLShader();
}

GLProgram::~GLProgram()
{
    clean();
}

void GLProgram::clean()
{
    shader->clean();
    fragment->clean();

    if (program_ref != Constant::kGlInvalidRef)
    {
        glDeleteProgram(program_ref);
        program_ref = Constant::kGlInvalidRef;
    }
}

int GLProgram::init(const char *shader, const char *fragment)
{
    if (shader == NULL || fragment == NULL)
    {
        return Error::ERR_NULL_PARAMETER;
    }

    int error = shader->load(GL_VERTEX_SHADER, shader);
    if (error != Error::OK)
    {
        return error;
    }

    error = fragment->load(GL_FRAGMENT_SHADER, fragment);
    if (error != Error::OK)
    {
        return error;
    }

    program_ref = glCreateProgram();
    if (program_ref == Constant::kGlInvalidRef)
    {
        shader->clean();
        fragment->clean();
        return Error::ERR_GL_CREATE_PROGRAM_REF;
    }

    glAttachShader(program_ref, shader->getHandle());
    error = check_gl_error("When attach shader(glAttachShader)");
    if (error != Error::OK)
    {
        clean();
        return Error::ERR_GL_ATTACH_SHADER;
    }

    glAttachShader(program_ref, fragment->getHandle());
    error = check_gl_error("When attach fragment(glAttachShader");
    if (error != Error::OK)
    {
        clean();
        return Error::ERR_GL_ATTACH_FRAGMENT;
    }

    glLinkProgram(program_ref);
    GLint link_status = GL_FALSE;
    glGetProgramiv(program_ref, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE)
    {
        GLint info_len = 0;
        glGetProgramiv(program_ref, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len)
        {
            info_len = check_err_desc_len(info_len);
            glGetProgramInfoLog(program_ref, info_len, NULL, err_desc);
        }

        clean();
        return Error::ERR_GL_LINK_PROGRAM;
    }

    return Error::OK;
}

int GLProgram::check_gl_error(const char* desc)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        sprintf(err_desc, "%s, glGetError() return 0x%x", desc, err);
        return Error::ERROR;
    }

    return Error::OK;
}

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
#include "error.h"
#include "constant.h"

using namespace std;

namespace eschao {

GLProgram::GLProgram()
{
    m_program_ref = Constant::kGlInvalidRef;
}

GLProgram::~GLProgram()
{
    clean();
}

void GLProgram::clean()
{
    m_shader.clean();
    m_fragment.clean();

    if (m_program_ref != Constant::kGlInvalidRef) {
        glDeleteProgram(m_program_ref);
        m_program_ref = Constant::kGlInvalidRef;
    }
}

int GLProgram::init(const char* shader_glsl, const char* fragment_glsl)
{
    if (shader_glsl == NULL || fragment_glsl == NULL) {
        return g_error.set(Error::ERR_NULL_PARAMETER);
    }

    if (Error::OK != m_shader.load(GL_VERTEX_SHADER, shader_glsl)) {
        return g_error.code();
    }
    if (Error::OK != m_fragment.load(GL_FRAGMENT_SHADER, fragment_glsl)) {
        return g_error.code();
    }

    m_program_ref = glCreateProgram();
    if (m_program_ref == Constant::kGlInvalidRef) {
        m_shader.clean();
        m_fragment.clean();
        return g_error.set(Error::ERR_GL_CREATE_PROGRAM_REF);
    }

    glAttachShader(m_program_ref, m_shader.shader_ref());
    int error = g_error.check_gl_error("When attach m_shader(glAttachShader)");
    if (error != Error::OK) {
        clean();
        return g_error.set(Error::ERR_GL_ATTACH_SHADER);
    }

    glAttachShader(m_program_ref, m_fragment.shader_ref());
    error = g_error.check_gl_error("When attach m_fragment(glAttachShader");
    if (error != Error::OK) {
        clean();
        return g_error.set(Error::ERR_GL_ATTACH_FRAGMENT);
    }

    glLinkProgram(m_program_ref);
    GLint link_status = GL_FALSE;
    glGetProgramiv(m_program_ref, GL_LINK_STATUS, &link_status);

    if (link_status != GL_TRUE) {
        g_error.set(Error::ERR_GL_LINK_PROGRAM);
        GLint info_len = 0;
        glGetProgramiv(m_program_ref, GL_INFO_LOG_LENGTH, &info_len);

        if (info_len) {
            if (info_len > Error::MAX_ERR_DESC_LENGTH) {
                info_len = Error::MAX_ERR_DESC_LENGTH;
            }
            glGetProgramInfoLog(m_program_ref, info_len, NULL,
                                const_cast<char*>(g_error.desc()));
            g_error.end_desc(info_len + 1);
        }

        clean();
        return g_error.code();
    }

    return Error::OK;
}

}

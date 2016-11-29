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

#include "VertexProgram.h"
#include "Constant.h"
#include "Matrix.h"

static const auto gVertexShader =
        "uniform mat4 u_MVPMatrix;\n"
        "attribute vec4 a_vexPosition;\n"
        "attribute vec2 a_texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    gl_Position = u_MVPMatrix * a_vexPosition;\n"
        "    v_texCoord = a_texCoord;\n"
        "}";

static const auto gVertexFragment =
        "uniform sampler2D u_texture;\n"
        "varying vec2 v_texCoord;\n"
        "\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(u_texture, v_texCoord);\n"
        "}";

VertexProgram::VertexProgram()
        : hMVPMatrix(Constant::INVALID_HANDLE),
          hVertexPostion(Constant::INVALID_HANDLE),
          hTextureCoord(Constant::INVALID_HANDLE),
          hTexture(Constant::INVALID_HANDLE)
{
}

VertexProgram::~VertexProgram()
{
    clean();
}

void VertexProgram::clean()
{
    GLProgram::clean();

    hTexture = Constant::INVALID_HANDLE;
    hMVPMatrix = Constant::INVALID_HANDLE;
    hTextureCoord = Constant::INVALID_HANDLE;
    hVertexPostion = Constant::INVALID_HANDLE;
}

int VertexProgram::init()
{
    return GLProgram::init(gVertexShader, gVertexFragment);
}

void VertexProgram::initMatrix(float left, float right, float bottom, float top)
{
    float projectMatrix[16];
    Matrix::orthoM(projectMatrix, left, right, bottom, top, 0, 6000);
    Matrix::setIdentityM(MVMatrix);
    Matrix::setLookAtM(MVMatrix, 0, 0, 3000, 0, 0, 0, 0, 1, 0);
    Matrix::setIdentityM(MVPMatrix);
    Matrix::multiplyMM(MVPMatrix, projectMatrix, MVMatrix);
}

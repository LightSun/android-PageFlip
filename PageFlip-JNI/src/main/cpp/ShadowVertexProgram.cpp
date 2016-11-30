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

#include "ShadowVertexProgram.h"
#include "Constant.h"

static const auto gVertexShader =
        "uniform mat4 u_MVPMatrix;\n"
        "uniform float u_vexZ;\n"
        "attribute vec4 a_vexPosition;\n"
        "varying vec4 v_texColor;\n"
        "\n"
        "void main() {\n"
        "    vec4 vexPos = vec4(a_vexPosition.xy, u_vexZ, 1.0);\n"
        "    v_texColor = vec4(a_vexPosition.z, a_vexPosition.z, a_vexPosition.z, a_vexPosition.w);\n"
        "    gl_Position = u_MVPMatrix * vexPos;\n"
        "}";

static const auto gFragmentShader =
        "varying vec4 v_texColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = v_texColor;\n"
        "}";

ShadowVertexProgram::ShadowVertexProgram()
        : hMVPMatrix(Constant::GL_INVALID_LOCATION),
          hVertexZ(Constant::GL_INVALID_LOCATION),
          hVertexPosition(Constant::GL_INVALID_LOCATION)
{
}

ShadowVertexProgram::~ShadowVertexProgram()
{
    clean();
}

void ShadowVertexProgram::clean()
{
    hMVPMatrix = Constant::GL_INVALID_LOCATION;
    hVertexZ = Constant::GL_INVALID_LOCATION;
    hVertexPosition = Constant::GL_INVALID_LOCATION;

    GLProgram::clean();
}

int ShadowVertexProgram::init()
{
    return GLProgram::init(gVertexShader, gFragmentShader);
}

void ShadowVertexProgram::getVarsLocation()
{
    hVertexZ = glGetUniformLocation(hProgram, VAR_VERTEX_Z);
    hMVPMatrix = glGetAttribLocation(hProgram, VAR_VERTEX_POS);
    hVertexPosition = glGetUniformLocation(hProgram, VAR_VERTEX_POS);
}

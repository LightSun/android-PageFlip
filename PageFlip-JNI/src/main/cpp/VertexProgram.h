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

#ifndef ANDROID_PAGEFLIP_VERTEXPROGRAM_H
#define ANDROID_PAGEFLIP_VERTEXPROGRAM_H


#include "GLProgram.h"

class VertexProgram : public GLProgram
{
public:
    static float MVMatrix[16];
    static float MVPMatrix[16];

protected:
    static const char *VAR_MVP_MATRIX       = "u_MVPMatrix";
    static const char *VAR_VERTEX_POS       = "a_vexPosition";
    static const char *VAR_TEXTURE_COORD    = "a_texCoord";
    static const char *VAR_TEXTURE          = "u_texture";

    GLuint hMVPMatrix;
    GLuint hVertexPostion;
    GLuint hTextureCoord;
    GLuint hTexture;

public:
    VertexProgram();
    ~VertexProgram();

    void clean();
    int init();
    void initMatrix(float left,
                    float right,
                    float bottom,
                    float top);

protected:
    void getVarsLocation();
};


#endif //ANDROID_PAGEFLIP_VERTEXPROGRAM_H

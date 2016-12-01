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

#include "BackOfFoldVertexes.h"
#include "BackOfFoldVertexProgram.h"
#include "Page.h"

void BackOfFoldVertexes::draw(BackOfFoldVertexProgram &program,
                              Page &page,
                              bool hasSecondPage,
                              int gradientLightID)
{
    glUniformMatrix4fv(program.getMaskColorLoc(), 1, GL_FALSE, VertexProgram::MVPMatrix);
    glBindTexture(GL_TEXTURE_2D, page.getBackTextureID());
    glUniform1i(program.getTextureLoc(), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gradientLightID);
    glUniform1i(program.getShadowLoc(), 1);

    glUniform1f(program.getTexXOffsetLoc(), hasSecondPage ? 1.0f : 0);

    glUniform4f(program.getMaskColorLoc(),
                page.maskColor[0][0],
                page.maskColor[0][1],
                page.maskColor[0][2],
                hasSecondPage ? 0 : mMaskAlpha);

    drawWith(GL_TRIANGLE_STRIP,
             program.getVertexPositionLoc(),
             program.getTexCoordLoc());
}

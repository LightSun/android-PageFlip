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

#include "Vertexes.h"

void Vertexes::drawWith(GLenum type, GLint hVertexPos, GLint hTexCoord)
{
    glVertexAttribPointer(hVertexPos, mSizeOfPerVex, GL_FLOAT, GL_FALSE, 0, mVertexes);
    glEnableVertexAttribArray(hVertexPos);

    glVertexAttribPointer(hTexCoord, 2, GL_FLOAT, GL_FALSE, 0, mTexCoords);
    glEnableVertexAttribArray(hTexCoord);

    glDrawArrays(type, 0, count());
}

void Vertexes::drawWith(GLenum type, GLint hVertexPos, GLint hTexCoord,
                        int offset, int length)
{
    glVertexAttribPointer(hVertexPos, mSizeOfPerVex, GL_FLOAT, GL_FALSE, 0, mVertexes);
    glEnableVertexAttribArray(hVertexPos);

    glVertexAttribPointer(hTexCoord, 2, GL_FLOAT, GL_FALSE, 0, mTexCoords);
    glEnableVertexAttribArray(hTexCoord);

    glDrawArrays(type, offset, length);
}

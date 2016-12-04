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

#include "back_of_fold_vertexes.h"
#include "back_of_fold_vertex_program.h"
#include "page.h"

BackOfFoldVertexes::BackOfFoldVertexes()
{

}

BackOfFoldVertexes::~BackOfFoldVertexes()
{

}

void BackOfFoldVertexes::draw(BackOfFoldVertexProgram &program,
                              Page &page,
                              bool has_second_page,
                              GLuint gradient_light_id)
{
    glUniformMatrix4fv(program.get_mask_color_loc(), 1, GL_FALSE,
                       VertexProgram::mvp_matrix);
    glBindTexture(GL_TEXTURE_2D, page.textures.back_texture_id());
    glUniform1i(program.texture_loc(), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gradient_light_id);
    glUniform1i(program.get_shadow_loc(), 1);

    glUniform1f(program.get_tex_x_offset_loc(), has_second_page ? 1.0f : 0);

    const float *mask_color = page.textures.mask_color[0];
    glUniform4f(program.get_mask_color_loc(),
                mask_color[0], mask_color[1], mask_color[2],
                has_second_page ? 0 : m_mask_alpha);

    draw_with(GL_TRIANGLE_STRIP,
              program.vertex_pos_loc(),
              program.tex_coord_loc());
}

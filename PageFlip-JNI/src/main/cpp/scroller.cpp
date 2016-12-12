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

#include "scroller.h"

namespace eschao {

Scroller::Scroller()
        : m_finished(true)
{
    m_interpolator = new LinearInterpolator();
}

Scroller::Scroller(Interpolator *interpolator)
        : m_finished(true), m_interpolator(interpolator)
{
}

Scroller::~Scroller()
{
    if (m_interpolator) {
        delete m_interpolator;
    }
}

bool Scroller::compute_scroll_offset()
{
    if (m_finished) {
        return false;
    }

    long time_passed = get_system_clock() - m_start_time;
    if (time_passed < m_duration) {
        const float x = m_interpolator->interpolate(time_passed * m_duration);
        m_curr_x = (float) (m_start_x + round(x * m_delta_x));
        m_curr_y = (float) (m_start_y + round(x * m_delta_y));
    }
    else {
        m_curr_x = m_final_x;
        m_curr_y = m_final_y;
        m_finished = true;
    }

    return true;
}

void Scroller::start_scroll(float start_x, float start_y, float dx, float dy,
                            int duration)
{
    m_finished = false;
    m_duration = duration;
    m_start_time = get_system_clock();

    m_start_x = start_x;
    m_start_y = start_y;
    m_final_x = start_x + dx;
    m_final_y = start_y + dy;
    m_delta_x = dx;
    m_delta_y = dy;
}

}

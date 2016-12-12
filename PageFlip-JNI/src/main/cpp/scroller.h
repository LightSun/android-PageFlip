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

#ifndef ANDROID_PAGEFLIP_SCROLLER_H
#define ANDROID_PAGEFLIP_SCROLLER_H

#include <math.h>
#include <time.h>

#define DEFAULT_DURATION 250

namespace eschao {

class Interpolator {

public:
    virtual ~Interpolator() { };

    virtual float interpolate(float input) = 0;

protected:
    Interpolator() { }
};

class AccelerateInterpolator : public Interpolator {

public:
    explicit AccelerateInterpolator(float factor = 1.0f)
            : m_factor(factor), m_double_factor(factor * 2) { }

    virtual float interpolate(float input)
    {
        if (m_factor == 1.0f) {
            return input * input;
        }
        else {
            return (float) pow(input, m_double_factor);
        }
    }

private:
    const float m_factor;
    const float m_double_factor;
};

class LinearInterpolator : public Interpolator {

public:
    virtual float interpolate(float input)
    {
        return input;
    }
};

/**
 * Copy from Android Scroller.java &
 * frameworks/base/+/master/libs/hwui/Interpolator.h
 */
class Scroller {

public:
    Scroller();
    Scroller(Interpolator *interpolator);
    ~Scroller();

    bool compute_scroll_offset();
    void start_scroll(float start_x, float start_y, float dx, float dy,
                      int duration = DEFAULT_DURATION);

    inline void set_interpolator(Interpolator *interpolator)
    {
        if (m_interpolator) {
            delete m_interpolator;
        }
        m_interpolator = interpolator;
    }

    inline bool is_finished()
    {
        return m_finished;
    }

    inline void set_finished(bool finished)
    {
        m_finished = finished;
    }

    inline int duration()
    {
        return m_duration;
    }

    inline float curr_x()
    {
        return m_curr_x;
    }

    inline float curr_y()
    {
        return m_curr_y;
    }

    inline float start_x()
    {
        return m_start_x;
    }

    inline float start_y()
    {
        return m_start_y;
    }

    inline float final_x()
    {
        return m_final_x;
    }

    inline float final_y()
    {
        return m_final_y;
    }

    inline void abortAnimation()
    {
        m_curr_x = m_final_x;
        m_curr_y = m_final_y;
        m_finished = true;
    }

private:
    inline long get_system_clock()
    {
        return clock() / CLOCKS_PER_SEC * 1000;
    }

private:
    Interpolator *m_interpolator;

    float m_start_x;
    float m_start_y;
    float m_final_x;
    float m_final_y;
    float m_delta_x;
    float m_delta_y;

    int m_duration;
    long m_start_time;
    float m_curr_x;
    float m_curr_y;
    bool m_finished;
};

}
#endif //ANDROID_PAGEFLIP_SCROLLER_H

#pragma once

#include <vector>

namespace GW
{
        namespace AUDIO
        {
                class GSound;
        }
} // namespace GW


#define SOUND_COLOR_TYPE(color, type) E_SOUND_TYPE::TYPE_COUNT* color + type


struct E_SOUND_TYPE
{
        enum
        {
                SPLINE_COLLECT_0,
                SPLINE_COLLECT_1,
                SPLINE_COLLECT_2,
                TYPE_COUNT,
        };

        // static constexpr unsigned int variations[E_SOUND_TYPE::TYPE_COUNT] = {1, 1, 5, 1, 1, 1};
        static constexpr unsigned int variations[E_SOUND_TYPE::TYPE_COUNT] = {5, 5, 5};
        /* static constexpr char         Literal[E_SOUND_TYPE::TYPE_COUNT][20] = {"ORB_COLLECT",
                                                                        "ORB_AMBIENT",
                                                                        "SPLINE_COLLECT_0",
                                                                        "SPLINE_COLLECT_1",
                                                                        "SPLINE_COLLECT_2",
                                                                        "SPLINE_AMBIENT",
                                                                        "GOAL_COLLECT",
                                                                        "GOAL_AMBIENT"};*/
        static constexpr char Literal[E_SOUND_TYPE::TYPE_COUNT][20] = {"SPLINE_COLLECT_0",
                                                                       "SPLINE_COLLECT_1",
                                                                       "SPLINE_COLLECT_2"};
};

struct E_SOUND_POOL_SIZES
{
        enum
        {
                SPLINE_COLLECT_0 = 5,
                SPLINE_COLLECT_1 = 5,
                SPLINE_COLLECT_2 = 5,
        };

        /*enum
        {
                ORB_COLLECT    = 3,
                ORB_AMBIENT    = 6,
                SPLINE_COLLECT = 2,
                SPLINE_AMBIENT = 2,
                GOAL_COLLECT   = 2,
                GOAL_AMBIENT   = 2,
        };*/


        /* static constexpr unsigned int arr[E_SOUND_TYPE::TYPE_COUNT] =
             {ORB_COLLECT, ORB_AMBIENT, SPLINE_COLLECT, SPLINE_AMBIENT, GOAL_COLLECT, GOAL_AMBIENT};*/

        static constexpr unsigned int arr[E_SOUND_TYPE::TYPE_COUNT] = {SPLINE_COLLECT_0, SPLINE_COLLECT_1, SPLINE_COLLECT_2};
};


struct SoundPoolElement
{
        GW::AUDIO::GSound* gwSound;
};
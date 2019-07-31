#pragma once

namespace GW
{
        namespace AUDIO
        {
                class GSound;
        }
} // namespace GW


#define SOUND_COLOR_TYPE(color, type) 6 * color + type

struct E_SOUND_POOL_SIZES
{
        enum
        {
                ORB_COLLECT = 3,
                ORB_AMBIENT = 6,
                SPLINE_COLLECT = 10,
                SPLINE_AMBIENT = 3,
                GOAL_COLLECT = 2,
                GOAL_AMBIENT = 2,
        };
};

struct E_SOUND_TYPE
{
        enum
        {
                ORB_COLLECT,
                ORB_AMBIENT,
                SPLINE_COLLECT,
                SPLINE_AMBIENT,
                GOAL_COLLECT,
                GOAL_AMBIENT,
				TYPE_COUNT,
                RED_ORB_COLLECT = 0,
                RED_ORB_AMBIENT,
                RED_SPLINE_COLLECT,
                RED_SPLINE_AMBIENT,
                RED_GOAL_COLLECT,
                RED_GOAL_AMBIENT,
                GREEN_ORB_COLLECT,
                GREEN_ORB_AMBIENT,
                GREEN_SPLINE_COLLECT,
                GREEN_SPLINE_AMBIENT,
                GREEN_GOAL_COLLECT,
                GREEN_GOAL_AMBIENT,
                BLUE_ORB_COLLECT,
                BLUE_ORB_AMBIENT,
                BLUE_SPLINE_COLLECT,
                BLUE_SPLINE_AMBIENT,
                BLUE_GOAL_COLLECT,
                BLUE_GOAL_AMBIENT,
                TOTAL_TYPE_COUNT,
        };
};


constexpr unsigned int E_SOUND_POOL_SIZES[E_SOUND_TYPE::TYPE_COUNT] = {3, 6, 10, 3, 2, 2};

struct SoundPoolElement
{
        GW::AUDIO::GSound* gwSound;
};
#pragma once

struct E_LevelStateEvents
{
        enum
        {
			//Tutorial transitions
                TUTORIAL_LEVEL_TO_TUTORIAL_LEVEL = 0,
                TUTORIAL_LEVEL_TO_LEVEL_01,
                TUTORIAL_LEVEL_TO_LEVEL_02,
                TUTORIAL_LEVEL_TO_LEVEL_03,
                // Level_01 transitions
                LEVEL_01_TO_TUTORIAL_LEVEL,
                LEVEL_01_TO_LEVEL_01,
                LEVEL_01_TO_LEVEL_02,
                LEVEL_01_TO_LEVEL_03,
                // Level_02 transitions
                LEVEL_02_TO_TUTORIAL_LEVEL,
                LEVEL_02_TO_LEVEL_01,
                LEVEL_02_TO_LEVEL_02,
                LEVEL_02_TO_LEVEL_03,
                // Level_03 transitions
                LEVEL_03_TO_TUTORIAL_LEVEL,
                LEVEL_03_TO_LEVEL_01,
                LEVEL_03_TO_LEVEL_02,
                LEVEL_03_TO_LEVEL_03
        };
};
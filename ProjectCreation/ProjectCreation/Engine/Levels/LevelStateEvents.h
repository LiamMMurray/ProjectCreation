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
                TUTORIAL_LEVEL_TO_LEVEL_04,
                TUTORIAL_LEVEL_TO_CREDITS_LEVEL,
                // Level_01 transitions
                LEVEL_01_TO_TUTORIAL_LEVEL,
                LEVEL_01_TO_LEVEL_01,
                LEVEL_01_TO_LEVEL_02,
                LEVEL_01_TO_LEVEL_03,
                LEVEL_01_TO_LEVEL_04,
                LEVEL_01_TO_CREDITS_LEVEL,
                // Level_02 transitions
                LEVEL_02_TO_TUTORIAL_LEVEL,
                LEVEL_02_TO_LEVEL_01,
                LEVEL_02_TO_LEVEL_02,
                LEVEL_02_TO_LEVEL_03,
                LEVEL_02_TO_LEVEL_04,
                LEVEL_02_TO_CREDITS_LEVEL,
                // Level_03 transitions
                LEVEL_03_TO_TUTORIAL_LEVEL,
                LEVEL_03_TO_LEVEL_01,
                LEVEL_03_TO_LEVEL_02,
                LEVEL_03_TO_LEVEL_03,
                LEVEL_03_TO_LEVEL_04,
                LEVEL_03_TO_CREDITS_LEVEL,
                // Level_04 transitions
                LEVEL_04_TO_TUTORIAL_LEVEL,
                LEVEL_04_TO_LEVEL_01,
                LEVEL_04_TO_LEVEL_02,
                LEVEL_04_TO_LEVEL_03,
                LEVEL_04_TO_LEVEL_04,
                LEVEL_04_TO_CREDITS_LEVEL,
                // Credits_Level transitions
                CREDITS_LEVEL_TO_TUTORIAL_LEVEL,
                CREDITS_LEVEL_TO_LEVEL_01,
                CREDITS_LEVEL_TO_LEVEL_02,
                CREDITS_LEVEL_TO_LEVEL_03,
                CREDITS_LEVEL_TO_LEVEL_04,
                CREDITS_LEVEL_TO_CREDITS_LEVEL,
        };
};
#pragma once
#include "Unit.h"
class CollisionGrid
{

    public:
        CollisionGrid();
        static const int NumCell  = 10;
        static const int CellSize = 20;
        static void      AddUnit(Unit* unit);
        static void      HandleMelee();
        static void      handleCell(Unit* unit);
    private:
         static Unit* cells[NumCell][NumCell];
};
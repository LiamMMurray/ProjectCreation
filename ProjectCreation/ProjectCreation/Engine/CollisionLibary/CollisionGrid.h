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
        static void      HandleCell(Unit* unit);
        static void      Move(Unit* unit, double x, double y);

    private:
         static Unit* cells[NumCell][NumCell];
};
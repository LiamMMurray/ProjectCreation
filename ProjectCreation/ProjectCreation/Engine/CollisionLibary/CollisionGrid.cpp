#include "CollisionGrid.h"

Unit* CollisionGrid::cells[NumCell][NumCell] = {};

CollisionGrid::CollisionGrid()
{
        // Clear the grid.
        for (int x = 0; x < NumCell; x++)
        {
                for (int y = 0; y < NumCell; y++)
                {
                        cells[x][y] = nullptr;
                }
        }
}

void CollisionGrid::AddUnit(Unit* unit)
{
        // Determine which grid cell it's in.
        int cellX = (int)(unit->RowX / CollisionGrid::CellSize);
        int cellY = (int)(unit->ColY / CollisionGrid::CellSize);

        // Add to the front of list for the cell it's in.
        unit->prev          = nullptr;
        unit->next          = cells[cellX][cellY];
        cells[cellX][cellY] = unit;

        if (unit->next != nullptr)
        {
                unit->next->prev = unit;
        }
}

void CollisionGrid::HandleMelee()
{
        for (int x = 0; x < NumCell; x++)
        {
                for (int y = 0; y < NumCell; y++)
                {
                        handleCell(cells[x][y]);
                }
        }
}

void CollisionGrid::handleCell(Unit* unit)
{
        while (unit != nullptr)
        {
                Unit* other = unit->next;
                while (other != nullptr)
                {
                        if (unit->RowX == other->RowX && unit->ColY == other->ColY)
                        {
                                // handleAttack(unit, other);
                        }
                        other = other->next;
                }

                unit = unit->next;
        }
}

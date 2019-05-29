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
                        HandleCell(cells[x][y]);
                }
        }
}

void CollisionGrid::HandleCell(Unit* unit)
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

void CollisionGrid::Move(Unit* unit, double x, double y)
{
        // See which cell it was in.
        int oldCellX = (int)(unit->RowX / CollisionGrid::CellSize);
        int oldCellY = (int)(unit->ColY / CollisionGrid::CellSize);

        // See which cell it's moving to.
        int cellX = (int)(x / CollisionGrid::CellSize);
        int cellY = (int)(y / CollisionGrid::CellSize);

        unit->RowX = x;
        unit->ColY = y;

        // If it didn't change cells, we're done.
        if (oldCellX == cellX && oldCellY == cellY)
                return;

        // Unlink it from the list of its old cell.
        if (unit->prev != nullptr)
        {
                unit->prev->next = unit->next;
        }

        if (unit->next != nullptr)
        {
                unit->next->prev = unit->prev;
        }

        // If it's the head of a list, remove it.
        if (cells[oldCellX][oldCellY] == unit)
        {
                cells[oldCellX][oldCellY] = unit->next;
        }

        // Add it back to the grid at its new cell.
        AddUnit(unit);
}

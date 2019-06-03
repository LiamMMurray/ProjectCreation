#pragma once
class Unit
{
        friend class CollisionGrid;


    private:
        double         RowX;
        double         ColY;
        CollisionGrid* grid_;
        Unit*          prev;
        Unit*          next;

    public:
        Unit(CollisionGrid* grid, double x, double y) : grid_(grid), RowX(x), ColY(y), prev(nullptr), next(nullptr)
        {
                Unit::grid_->AddUnit(this);
        }
};
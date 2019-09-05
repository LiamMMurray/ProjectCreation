#pragma once
#include <DirectXMath.h>
#include <vector>
class MazeGenerator
{
    public:
        struct MazePoint
        {
                int x, y;

                inline bool operator==(MazePoint other)
                {
                        return x == other.x && y == other.y;
                }

                inline bool operator!=(MazePoint other)
                {
                        return !(other == *this);
                }

                inline MazePoint Move(int _x, int _y)
                {
                        return MazePoint{x + _x, y + _y};
                }
        };

        std::vector<DirectX::XMVECTOR> GeneratePath(unsigned int rows,
                                                    unsigned int cols,
                                                    int          startX,
                                                    int          startY,
                                                    int          endX,
                                                    int          endY);

    private:
        std::vector<MazePoint>     SimplifyPath(const std::vector<MazePoint>& points);
        std::vector<MazePoint>     GenerateMaze(unsigned int width, unsigned int height, MazePoint start, MazePoint end);
        static constexpr MazePoint InvalidPoint = {-1, -1};

        MazePoint GetRandomNeighbor(MazePoint point, unsigned width, unsigned height, bool** visited);
        bool      IsValidTile(MazePoint point, unsigned width, unsigned height, bool** visited);
};
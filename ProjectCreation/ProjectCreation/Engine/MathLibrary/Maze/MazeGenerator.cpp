#include "MazeGenerator.h"
#include <algorithm>
#include <stack>
#include <vector>

MazeGenerator::MazePoint MazeGenerator::GetRandomNeighbor(MazePoint point, unsigned rows, unsigned cols, bool** visited)
{
        std::vector<MazePoint> neigh(4);
        neigh[0] = point.Move(1, 0);
        neigh[1] = point.Move(-1, 0);
        neigh[2] = point.Move(0, 1);
        neigh[3] = point.Move(0, -1);
        std::random_shuffle(neigh.begin(), neigh.end() - 1);

        for (auto& n : neigh)
        {
                if (IsValidTile(n, rows, cols, visited))
                        return n;
        }

        return InvalidPoint;
}

bool MazeGenerator::IsValidTile(MazePoint point, unsigned rows, unsigned cols, bool** visited)
{
        if (point.x < 0 || point.y < 0 || point.x >= (int)cols || point.y >= (int)rows)
                return false;

        return !visited[point.y][point.x];
}

std::vector<DirectX::XMVECTOR> MazeGenerator::GeneratePath(unsigned int rows,
                                                           unsigned int cols,
                                                           int          startX,
                                                           int          startY,
                                                           int          endX,
                                                           int          endY)
{
        using namespace DirectX;

        std::vector<MazePoint> base       = GenerateMaze(rows, cols, MazePoint{startX, startY}, MazePoint{endX, endY});
        std::vector<MazePoint> simplified = SimplifyPath(base);

        size_t    size   = simplified.size();
        MazePoint s      = simplified[0];
        MazePoint e      = simplified[size - 1];
        float     length = sqrtf((s.x - e.x) * (s.x - e.x) + (s.y - e.y) * (s.y - e.y));

        std::vector<DirectX::XMVECTOR> output;
        output.resize(size);

        for (size_t i = 0; i < size; ++i)
        {
                output[i] = XMVectorSet(simplified[i].x - s.x, 0.0f, simplified[i].y, 1.0f);
                output[i] /= length;
                output[i] = XMVectorSetW(output[i], 1.0f);
        }

        return output;
}

std::vector<MazeGenerator::MazePoint> MazeGenerator::SimplifyPath(const std::vector<MazePoint>& base)
{
        std::vector<MazePoint> output;
        size_t                 size = base.size();
        output.reserve(size);
        output.push_back(base[0]);
        bool changedX = false;
        bool changedY = false;
        for (size_t i = 1; i < size - 1; ++i)
        {
                if (base[i - 1].x != base[i + 1].x)
                        changedX = true;
                if (base[i - 1].y != base[i + 1].y)
                        changedY = true;

                if (changedX && changedY)
                {
                        changedX = changedY = false;
                        output.push_back(base[i]);
                }
        }
        output.push_back(base[size - 1]);

        return output;
}

std::vector<MazeGenerator::MazePoint> MazeGenerator::GenerateMaze(unsigned int rows,
                                                                  unsigned int cols,
                                                                  MazePoint    start,
                                                                  MazePoint    end)
{
        std::stack<MazeGenerator::MazePoint> path;


        bool** visited = new bool*[rows];
        for (int i = 0; i < rows; ++i)
                visited[i] = new bool[cols]{};


        visited[start.y][start.x] = true;
        path.push(start);
        MazePoint curr = start;
        while (curr != end)
        {
                curr = GetRandomNeighbor(curr, rows, cols, visited);
                if (curr == InvalidPoint)
                {
                        curr = path.top();
                        path.pop();
                }
                else
                {
                        visited[curr.y][curr.x] = true;
                        path.push(curr);
                }
        }

        std::vector<MazeGenerator::MazePoint> output;
        size_t                                size = path.size();
        output.resize(size);
        while (path.empty() == false)
        {
                output[size - 1] = path.top();
                path.pop();
                --size;
        }

        for (int i = 0; i < rows; ++i)
                delete[] visited[i];
        delete[] visited;

        return output;
}

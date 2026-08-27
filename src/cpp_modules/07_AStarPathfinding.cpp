/**
 * ============================================================================
 * PROMPT 10: 2D Game AI Pathfinding (A* Algorithm)
 * ============================================================================
 * Implements the standard A* graph search algorithm for grid maps:
 * - Manhattan heuristic calculation
 * - Priority queue open list (min-heap)
 * - Optimal waypoint path reconstruction
 */

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

struct Point {
    int x, y;
    bool operator==(const Point& o) const { return x == o.x && y == o.y; }
};

struct Node {
    Point pos;
    float g, h;
    float f() const { return g + h; }
};

struct NodeCompare {
    bool operator()(const Node& a, const Node& b) const { return a.f() > b.f(); }
};

class AStarEngine {
private:
    int width, height;
    std::vector<std::vector<int>> map;

    float Heuristic(Point a, Point b) {
        return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
    }

    bool IsWalkable(Point p) {
        return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height && map[p.y][p.x] == 0;
    }

public:
    AStarEngine(int w, int h, const std::vector<std::vector<int>>& grid)
        : width(w), height(h), map(grid) {}

    std::vector<Point> FindPath(Point start, Point target) {
        std::vector<Point> path;
        if (!IsWalkable(start) || !IsWalkable(target)) return path;

        std::priority_queue<Node, std::vector<Node>, NodeCompare> open;
        std::vector<std::vector<bool>> closed(height, std::vector<bool>(width, false));
        std::vector<std::vector<Point>> parent(height, std::vector<Point>(width, { -1, -1 }));
        std::vector<std::vector<float>> gScore(height, std::vector<float>(width, 1e9f));

        open.push({ start, 0.0f, Heuristic(start, target) });
        gScore[start.y][start.x] = 0.0f;

        const int dx[] = { 0, 1, 0, -1 };
        const int dy[] = { -1, 0, 1, 0 };

        while (!open.empty()) {
            Node curr = open.top();
            open.pop();

            if (curr.pos == target) {
                Point p = target;
                while (!(p == start)) {
                    path.push_back(p);
                    p = parent[p.y][p.x];
                }
                path.push_back(start);
                std::reverse(path.begin(), path.end());
                return path;
            }

            if (closed[curr.pos.y][curr.pos.x]) continue;
            closed[curr.pos.y][curr.pos.x] = true;

            for (int i = 0; i < 4; ++i) {
                Point next = { curr.pos.x + dx[i], curr.pos.y + dy[i] };
                if (IsWalkable(next) && !closed[next.y][next.x]) {
                    float tentativeG = gScore[curr.pos.y][curr.pos.x] + 1.0f;
                    if (tentativeG < gScore[next.y][next.x]) {
                        gScore[next.y][next.x] = tentativeG;
                        parent[next.y][next.x] = curr.pos;
                        open.push({ next, tentativeG, Heuristic(next, target) });
                    }
                }
            }
        }
        return path;
    }
};

int main() {
    std::cout << "=== 2D GAME A* PATHFINDING ALGORITHM ===\n\n";

    std::vector<std::vector<int>> grid = {
        { 0, 0, 0, 0, 0 },
        { 0, 1, 1, 1, 0 },
        { 0, 0, 0, 1, 0 },
        { 1, 1, 0, 0, 0 }
    };

    AStarEngine astar(5, 4, grid);
    Point start = { 0, 0 };
    Point end = { 4, 3 };

    auto path = astar.FindPath(start, end);
    std::cout << "Calculated Path (" << path.size() << " steps):\n";
    for (const auto& pt : path) {
        std::cout << " -> [" << pt.x << ", " << pt.y << "]";
    }
    std::cout << "\n";
    return 0;
}

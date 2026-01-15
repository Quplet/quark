#include "quark.hpp"
#include "spdlog/spdlog.h"
#include <functional>

using namespace quark;

struct Point {
    int x, y;
};

void startup(ECS& ecs) {
    Point p1{0, 0};
    // Point p2{100, 100};

    ecs.create_entity(p1);
    // ecs.create_entity(p2);
}

void test_update(Point& point) {
    point.x++;
    point.y++;

    spdlog::info("P: {}, {}", point.x, point.y);
}

int main() {
    Quark engine{};

    engine.add_system(std::function<void(ECS&)>(startup), SystemType::STARTUP);
    engine.add_system(std::function<void(Point&)>(test_update), SystemType::UPDATE);

    engine.run();

    return 0;
}

#include "quark.hpp"
#include <functional>

using namespace quark;

struct Point {
  int x, y;
};

void test_update(Point& point) {
  // do something
}

int main() {
  auto engine = Quark();
   
  engine.add_system(std::function<void(Point&)>(test_update), SystemType::UPDATE);

  engine.run();
  
  return 0;
}

#pragma once

#include "resource.hpp"
#include <chrono>

namespace quark {

using time_point = std::chrono::time_point<std::chrono::steady_clock>;

class Time : public Resource {
private:
  time_point _last_time;
  float _elapsed_time;

  void _update() override {
    time_point current_time = std::chrono::steady_clock::now();
    this->_elapsed_time = std::chrono::duration<float>(current_time - this->_last_time).count();
    this->_last_time = current_time;
  }

public:
  float delta() { return this->_elapsed_time; }
};
  
}

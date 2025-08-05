#include "quark.hpp"

#include <spdlog/spdlog.h>

namespace quark {

void Quark::run() {
  this->m_running = true;

  spdlog::info("Initializing Qu2Engine...");

  // TODO: Run game loop

  this->_game_loop();
}

void Quark::_add_core_modules() {
  // TODO: Init core modules like Time, renderer, window, audio, etc
}

void Quark::_game_loop() {
  while (true) {
    // draw window
    // render
    // accumulate time
  }
}
  
}

#include "quark.hpp"

#include <memory>
#include <spdlog/spdlog.h>

namespace quark {

Quark::Quark() {
  ECSCore *ecs = new ECSCore();
  this->m_ecs_core = std::unique_ptr<ECSCore>(ecs);

  Config *config = new Config();
  this->m_config = std::unique_ptr<Config>(config);
}

 
void Quark::run() {
  this->m_running = true;

  spdlog::info("Initializing Quark...");

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

    this->m_ecs_core->_ecs_update(SystemType::UPDATE);
  }
}
  
}
